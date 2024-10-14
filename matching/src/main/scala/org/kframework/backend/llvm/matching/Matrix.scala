package org.kframework.backend.llvm.matching

import com.runtimeverification.k.kore.implementation.{ DefaultBuilders => B }
import com.runtimeverification.k.kore.CompoundSort
import com.runtimeverification.k.kore.Sort
import com.runtimeverification.k.kore.SymbolOrAlias
import com.runtimeverification.k.kore.Variable
import java.util
import java.util.concurrent.ConcurrentHashMap
import java.util.Optional
import org.kframework.backend.llvm.matching.dt._
import org.kframework.backend.llvm.matching.pattern._
import scala.annotation.tailrec
import scala.collection.immutable

trait AbstractColumn {
  def column: Column

  def computeScoreForKey(heuristic: Heuristic, key: Option[Pattern[Option[Occurrence]]]): Double = {
    def withChoice(result: Double): Double =
      if (key.isDefined) {
        val none = computeScoreForKey(heuristic, None)
        if (none > result) {
          none
        } else {
          result
        }
      } else {
        result
      }
    if (column.isWildcard) {
      Double.PositiveInfinity
    } else {
      val result = heuristic.computeScoreForKey(this, key)
      assert(!result.isNaN)
      withChoice(result)
    }
  }
}

case class MatrixColumn(matrix: Matrix, colIx: Int) extends AbstractColumn {
  def column: Column                    = matrix.columns(colIx)
  lazy val score: immutable.Seq[Double] = column.score(this)
}

class Column(
    val fringe: Fringe,
    val patterns: immutable.IndexedSeq[Pattern[String]],
    val clauses: immutable.IndexedSeq[Clause]
) extends AbstractColumn {
  def column: Column = this

  lazy val category: SortCategory = {
    val ps = patterns.map(_.category).filter(_.isDefined)
    if (ps.isEmpty) {
      SymbolS()
    } else {
      ps.head.get
    }
  }

  lazy val score: immutable.Seq[Double] = computeScore

  def score(matrixCol: MatrixColumn): immutable.Seq[Double] = {
    def zeroOrHeuristic(h: Heuristic): Double =
      if (h.needsMatrix) {
        matrixCol.computeScoreForKey(h, bestKey)
      } else {
        0.0
      }
    fringe.symlib.heuristics
      .zip(score)
      .map((hs: (Heuristic, Double)) => hs._2 + zeroOrHeuristic(hs._1))
  }

  def computeScore: immutable.Seq[Double] =
    computeScoreForKey(bestKey)

  def computeScoreForKey(key: Option[Pattern[Option[Occurrence]]]): immutable.Seq[Double] = {
    def zeroOrHeuristic(h: Heuristic): Double =
      if (h.needsMatrix) {
        0.0
      } else {
        computeScoreForKey(h, key)
      }
    fringe.symlib.heuristics.map(zeroOrHeuristic)
  }

  def isValid: Boolean = isValidForKey(bestKey)

  def isValidForKey(key: Option[Pattern[Option[Occurrence]]]): Boolean =
    !fringe.sortInfo.isCollection || key.isDefined || !patterns.exists(_.isChoice)

  lazy val keyVars: immutable.Seq[Set[String]] = {
    val keys = patterns.map(_.mapOrSetKeys)
    keys.map(_.flatMap(_.variables).toSet)
  }
  private lazy val boundVars: immutable.Seq[Set[String]] = patterns.map(_.variables)
  def needed(vars: immutable.Seq[Set[String]]): Boolean = {
    val intersection = vars.lazyZip(boundVars).map(_.intersect(_))
    intersection.exists(_.nonEmpty)
  }

  lazy val isEmpty: Boolean = fringe.sortInfo.isCollection && rawSignature.contains(Empty())

  private lazy val rawSignature: immutable.Seq[Constructor] =
    patterns.zipWithIndex.flatMap(p => p._1.signature(clauses(p._2)))

  def signatureForKey(key: Option[Pattern[Option[Occurrence]]]): List[Constructor] = {
    val bestUsed = key match {
      case None    => rawSignature
      case Some(k) => rawSignature.filter(_.isBest(k))
    }
    val usedInjs = bestUsed.flatMap(fringe.injections)
    val dups     = if (fringe.isExact) bestUsed else bestUsed ++ usedInjs
    val nodups = dups.distinct
      .filter(c =>
        !c.isInstanceOf[SymbolC] || fringe.sortInfo.nonEmptyConstructors
          .contains(c.asInstanceOf[SymbolC].sym)
      )
      .toList
    if (nodups.contains(Empty())) {
      List(Empty())
    } else {
      nodups.filter(_ != Empty())
    }
  }

  def signatureForUsefulness: List[Constructor] =
    signatureForKey(None).filter(!_.isInstanceOf[HasNoKey]).map { c =>
      if (!c.isInstanceOf[HasKey]) {
        c
      } else {
        val hasKey = c.asInstanceOf[HasKey]
        HasKey(hasKey.cat, hasKey.element, None)
      }
    }

  lazy val signature: List[Constructor] =
    signatureForKey(bestKey)

  def isChoice: Boolean = isChoiceForKey(bestKey)

  def isChoiceForKey(key: Option[Pattern[Option[Occurrence]]]): Boolean =
    fringe.sortInfo.isCollection && key.isEmpty

  @tailrec
  private def asListP(p: Pattern[String]): immutable.Seq[ListP[String]] =
    p match {
      case l @ ListP(_, _, _, _, _) => immutable.Seq(l)
      case AsP(_, _, pat)           => asListP(pat)
      case _                        => immutable.Seq()
    }

  def maxListSize: (Int, Int) = {
    val listPs = patterns.flatMap(asListP)
    if (listPs.isEmpty) {
      (0, 0)
    } else {
      val longestHead = listPs.map(_.head.size).max
      val longestTail = listPs.map(_.tail.size).max
      (longestHead, longestTail)
    }
  }

  lazy val validKeys: immutable.Seq[Pattern[Option[Occurrence]]] = {
    val possibleKeys = rawSignature.flatMap {
      case HasKey(_, _, Some(k)) => immutable.Seq(k)
      case _                     => immutable.Seq()
    }
    if (possibleKeys.isEmpty) {
      possibleKeys
    } else {
      possibleKeys.filter(k => isValidForKey(Some(k)))
    }
  }

  lazy val bestKey: Option[Pattern[Option[Occurrence]]] = {
    import Ordering.Implicits._
    if (validKeys.isEmpty) {
      None
    } else {
      val rawBestKey = validKeys.map(k => (k, computeScoreForKey(Some(k)))).maxBy(_._2)
      if (Matching.logging) {
        System.out.println("Best key is " + rawBestKey._1)
      }
      Some(rawBestKey._1)
    }
  }

  def maxPriority: Int = maxPriorityForKey(bestKey)

  def maxPriorityForKey(key: Option[Pattern[Option[Occurrence]]]): Int =
    if (isChoiceForKey(key)) {
      clauses(0).action.priority
    } else {
      Int.MaxValue
    }

  def expand(ix: Constructor, isExact: Boolean): immutable.IndexedSeq[Column] = {
    val fringes = fringe.expand(ix)
    val ps = patterns
      .lazyZip(clauses)
      .toSeq
      .map(t => t._1.expand(ix, isExact, fringes, fringe, t._2, maxPriority))
    val transposed = if (ps.isEmpty) fringes.map(_ => immutable.IndexedSeq()) else ps.transpose
    fringes.lazyZip(transposed).toIndexedSeq.map(t => new Column(t._1, t._2.toIndexedSeq, clauses))
  }

  lazy val isWildcard: Boolean = patterns.forall(_.isWildcard)

  def canEqual(other: Any): Boolean = other.isInstanceOf[Column]

  override def equals(other: Any): Boolean = other match {
    case that: Column =>
      that.canEqual(this) &&
      fringe == that.fringe &&
      patterns == that.patterns
    case _ => false
  }

  override lazy val hashCode: Int = {
    val state = immutable.Seq(patterns)
    state.map(_.hashCode()).foldLeft(0)((a, b) => 31 * a + b)
  }
}

case class VariableBinding[T](
    name: T,
    category: SortCategory,
    occurrence: Occurrence,
    pattern: Option[Pattern[String]]
) {
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class Fringe(
    symlib: Parser.SymLib,
    sort: Sort,
    occurrence: Occurrence,
    isExact: Boolean
) {
  val sortInfo: SortInfo = SortInfo(sort, symlib)

  def overloads(sym: SymbolOrAlias): immutable.Seq[SymbolOrAlias] =
    symlib.overloads.getOrElse(sym, immutable.Seq())

  def injections(ix: Constructor): immutable.Seq[Constructor] =
    ix match {
      case SymbolC(sym) =>
        if (symlib.overloads.contains(sym) || sym.ctr == "inj") {
          if (!sortInfo.trueInjMap.contains(sym)) {
            System.err.println(sort)
            System.err.println(sym)
          }
          sortInfo.trueInjMap(sym).map(SymbolC)
        } else {
          immutable.Seq()
        }
      case _ => immutable.Seq()
    }

  def contains(ix: Constructor): Boolean =
    lookup(ix).isDefined

  def expand(ix: Constructor): immutable.Seq[Fringe] =
    lookup(ix).get

  def lookup(ix: Constructor): Option[immutable.Seq[Fringe]] =
    ix.expand(this)

  def inexact: Fringe =
    Fringe(symlib, sort, occurrence, isExact = false)

  override def toString: String   = new util.Formatter().format("%12.12s", sort.toString).toString
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

class SortInfo private (sort: Sort, symlib: Parser.SymLib) {
  val constructors: immutable.Seq[SymbolOrAlias] =
    symlib.constructorsForSort.getOrElse(sort, immutable.Seq())
  lazy val nonEmptyConstructors: Set[SymbolOrAlias] = constructors
    .filter(c =>
      c.ctr != "inj" || SortInfo(c.params.head, symlib).category
        .hasIncompleteSignature(immutable.Seq(), isExact = true, SortInfo(c.params.head, symlib))
    )
    .toSet
  val exactConstructors: immutable.Seq[SymbolOrAlias] = constructors.filter(_.ctr != "inj")
  private val rawInjections                           = constructors.filter(_.ctr == "inj")
  private val injMap = rawInjections
    .map(b => (b, rawInjections.filter(a => symlib.isSubsorted(a.params.head, b.params.head))))
    .toMap
  private val rawOverloads = constructors.filter(symlib.overloads.contains)
  private val overloadMap  = rawOverloads.map(s => (s, symlib.overloads(s))).toMap
  private val overloadInjMap = overloadMap.map(e =>
    (
      e._1,
      e._2.map(g =>
        B.SymbolOrAlias("inj", immutable.Seq(symlib.signatures(g)._2, symlib.signatures(e._1)._2))
      )
    )
  )
  val trueInjMap: Map[SymbolOrAlias, immutable.Seq[SymbolOrAlias]] = injMap ++ overloadInjMap
  val category: SortCategory =
    SortCategory(Parser.getStringAtt(symlib.sortAtt(sort), "hook"), sort, symlib)
  lazy val length: Int = category.length(nonEmptyConstructors.size)
  val exactLength: Int = category.length(exactConstructors.size)
  val isCollection: Boolean =
    category match {
      case MapS() | SetS() | ListS() => true
      case _                         => false
    }
}
object SortInfo {
  def apply(sort: Sort, symlib: Parser.SymLib): SortInfo =
    symlib.sortCache.computeIfAbsent(sort, s => new SortInfo(s, symlib))
}

case class Action(
    ordinal: Int,
    lhsVars: immutable.Seq[Variable],
    rhsVars: immutable.Seq[String],
    scVars: Option[immutable.Seq[String]],
    freshConstants: immutable.Seq[(String, Sort)],
    arity: Int,
    priority: Int,
    source: Optional[Source],
    location: Optional[Location],
    nonlinear: Boolean
) {
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class Clause(
    // the rule to be applied if this row succeeds
    action: Action,
    // the variable bindings made so far while matching this row
    bindings: Vector[VariableBinding[String]],
    // the length of the head and tail of any list patterns
    // with frame variables bound so far in this row
    listRanges: Vector[(Occurrence, Int, Int)],
    // variable bindings to injections that need to be constructed
    // since they do not actually exist in the original subject term
    overloadChildren: Vector[(Constructor, Fringe, VariableBinding[String])],
    specializedVars: Map[Occurrence, (SortCategory, Pattern[Option[Occurrence]])]
) {

  lazy val bindingsMap: Map[String, VariableBinding[String]] =
    bindings.groupBy(_.name).view.mapValues(_.head).toMap
  lazy val boundOccurrences: Set[Occurrence] = bindings.map(_.occurrence).toSet

  def isBound(binding: Any): Boolean =
    binding match {
      case name: String => bindingsMap.contains(name)
      case o: Option[_] => boundOccurrences.contains(o.get.asInstanceOf[Occurrence])
    }

  def canonicalize(name: String): Option[Occurrence] =
    bindingsMap.get(name).map(_.occurrence)
  def canonicalize[T](pat: Pattern[T]): Option[Pattern[Option[Occurrence]]] =
    if (pat.isBound(this)) {
      Some(pat.canonicalize(this))
    } else {
      None
    }

  def addVars(
      ix: Option[Constructor],
      residual: Option[Pattern[String]],
      pat: Pattern[String],
      f: Fringe
  ): Clause =
    Clause(
      action,
      bindings ++ pat.bindings(ix, residual, f.occurrence, f.symlib),
      listRanges ++ pat.listRange(ix, f.occurrence),
      overloadChildren ++ pat.overloadChildren(f, ix, residual, Num(0, f.occurrence)),
      specializedVars
    )

  private def translateVars(
      residuals: immutable.Seq[(Pattern[String], Occurrence)],
      allVars: Vector[VariableBinding[String]],
      symlib: Parser.SymLib
  ): Map[Occurrence, (SortCategory, Pattern[Option[Occurrence]])] = {
    val residualMap = residuals.toMap
    def substituteBy(pat: Pattern[String], category: SortCategory): Pattern[Option[Occurrence]] =
      residualMap.get(pat) match {
        case Some(o) => VariableP(Some(o), category)
        case None =>
          pat match {
            case SymbolP(sym, ps) =>
              SymbolP(
                sym,
                ps.zipWithIndex.map(p =>
                  substituteBy(p._1, SortInfo(symlib.signatures(sym)._1(p._2), symlib).category)
                )
              )
            case LiteralP(lit, sort) => LiteralP(lit, sort)
            case _                   => ???
          }
      }
    allVars
      .filter(_.pattern.isDefined)
      .map(v => v.occurrence -> (v.category, substituteBy(v.pattern.get, v.category)))
      .toMap
  }

  def specializeBy(
      residualMap: immutable.Seq[(Pattern[String], Occurrence)],
      symlib: Parser.SymLib
  ): Clause = {
    val overloadVars = overloadChildren.map(_._3)
    val allVars      = bindings ++ overloadVars
    Clause(
      action,
      allVars,
      listRanges,
      Vector(),
      specializedVars ++ translateVars(residualMap, allVars, symlib)
    )
  }

  override def toString: String   = action.ordinal.toString + "(" + action.priority.toString + ")"
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class Row(patterns: immutable.IndexedSeq[Pattern[String]], clause: Clause) {
  // returns whether the row is done matching
  def isWildcard: Boolean = patterns.forall(_.isWildcard)

  def expand(colIx: Int): immutable.Seq[Row] = {
    val p0s = patterns(colIx).expandOr
    p0s.map(p => Row(patterns.updated(colIx, p), clause))
  }

  def specialize(
      ix: Constructor,
      colIx: Int,
      symlib: Parser.SymLib,
      fringe: immutable.IndexedSeq[Fringe]
  ): Option[Row] =
    Matrix
      .fromRows(symlib, immutable.IndexedSeq(this), fringe, search = false)
      .specialize(ix, colIx, None)
      ._3
      .rows
      .headOption

  def default(
      colIx: Int,
      sigma: immutable.Seq[Constructor],
      symlib: Parser.SymLib,
      fringe: immutable.IndexedSeq[Fringe],
      matrixColumns: immutable.IndexedSeq[Column]
  ): Option[Row] =
    Matrix
      .fromRows(symlib, immutable.IndexedSeq(this), fringe, search = false)
      .trueDefault(colIx, sigma, Some(matrixColumns))
      .rows
      .headOption

  override def toString: String = patterns
    .map(p => new util.Formatter().format("%12.12s", p.toShortString))
    .mkString(" ") + " " + clause.toString
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

class Matrix private (
    val symlib: Parser.SymLib,
    private val rawColumns: immutable.IndexedSeq[Column],
    private val rawRows: immutable.IndexedSeq[Row],
    private val rawClauses: immutable.IndexedSeq[Clause],
    private val rawFringe: immutable.IndexedSeq[Fringe],
    val search: Boolean
) {
  lazy val clauses: immutable.IndexedSeq[Clause] =
    if (rawClauses != null) {
      rawClauses
    } else {
      rawRows.map(_.clause)
    }

  lazy val fringe: immutable.IndexedSeq[Fringe] =
    if (rawFringe != null) {
      rawFringe
    } else {
      rawColumns.map(_.fringe)
    }

  lazy val columns: immutable.IndexedSeq[Column] =
    if (rawColumns != null) {
      rawColumns
    } else if (rawRows.isEmpty) {
      rawFringe.map(f => new Column(f, immutable.IndexedSeq(), immutable.IndexedSeq()))
    } else {
      computeColumns
    }

  private def computeColumns: immutable.IndexedSeq[Column] = {
    val ps = rawRows.map(_.patterns).transpose
    rawFringe.indices.map(col => new Column(rawFringe(col), ps(col), clauses))
  }

  lazy val rows: immutable.IndexedSeq[Row] =
    if (rawRows != null) {
      rawRows
    } else if (rawColumns.isEmpty) {
      rawClauses.map(clause => Row(immutable.IndexedSeq(), clause))
    } else {
      computeRows
    }

  private def computeRows: immutable.IndexedSeq[Row] = {
    val ps = rawColumns.map(_.patterns).transpose
    rawClauses.indices.map(row => Row(ps(row), rawClauses(row)))
  }

  def this(
      symlib: Parser.SymLib,
      cols: immutable.IndexedSeq[(Sort, immutable.IndexedSeq[Pattern[String]])],
      actions: immutable.IndexedSeq[Action]
  ) =
    this(
      symlib,
      cols
        .lazyZip(
          (1 to cols.size).map(i => Fringe(symlib, cols(i - 1)._1, Num(i, Base()), isExact = false))
        )
        .to(immutable.IndexedSeq)
        .map(pair =>
          new Column(
            pair._2,
            pair._1._2,
            actions.map(Clause(_, Vector(), Vector(), Vector(), Map()))
          )
        ),
      null,
      actions.map(Clause(_, Vector(), Vector(), Vector(), Map())),
      null,
      false
    )

  private def isWildcardOrResidual(pat: Pattern[String]): Boolean =
    pat.isWildcard || pat.isResidual(symlib)

  private lazy val matrixColumns: immutable.Seq[MatrixColumn] =
    columns.indices.map(MatrixColumn(this, _))

  private lazy val validCols: immutable.Seq[MatrixColumn] =
    matrixColumns.filter(col =>
      col.column.isValid || columns.forall(c =>
        c == col.column || !c.needed(col.column.keyVars) || col.column.needed(c.keyVars)
      )
    )

  private var specializing: Option[immutable.IndexedSeq[Pattern[String]]] = None

  // compute the column with the best score, choosing the first such column if they are equal
  lazy val bestColIx: Int =
    if (specializing.isDefined) {
      columns.indices.find(i => !isWildcardOrResidual(specializing.get(i))).get
    } else {
      val allBest = if (validCols.nonEmpty) {
        Heuristic.getBest(validCols, matrixColumns)
      } else {
        Heuristic.getBest(matrixColumns, matrixColumns)
      }
      import Ordering.Implicits._
      val best = symlib.heuristics.last.breakTies(allBest)
      if (Matching.logging) {
        System.out.println("Chose column " + best.colIx)
      }
      best.colIx
    }

  lazy val bestCol: Column = columns(bestColIx)

  lazy val sigma: List[Constructor] = bestCol.signature

  def specialize(
      ix: Constructor,
      colIx: Int,
      residual: Option[Pattern[String]]
  ): (String, immutable.Seq[String], Matrix) = {
    val filtered = filterMatrix(
      Some(ix),
      residual,
      (c, p) =>
        p.isSpecialized(ix, residual.isEmpty, columns(colIx).fringe, c, columns(colIx).maxPriority),
      colIx
    )
    val justExpanded = filtered.columns(colIx).expand(ix, residual.isEmpty)
    val expanded = Matrix.fromColumns(
      symlib,
      justExpanded ++ filtered.notBestCol(colIx),
      filtered.clauses,
      search
    )
    (ix.name, justExpanded.map(_.fringe.sortInfo.category.hookAtt), expanded)
  }

  def cases: List[(String, immutable.Seq[String], Matrix)] =
    sigma.map(specialize(_, bestColIx, None))

  lazy val compiledCases: immutable.Seq[(String, immutable.Seq[String], DecisionTree)] = {
    Matrix.remaining += sigma.length
    if (Matching.logging) {
      System.out.println("Signature:")
      System.out.println(sigma.map(_.toString).mkString("\n"))
    }
    val result = cases.map { l =>
      if (Matching.logging) {
        System.out.println("Specializing by " + l._1)
      }
      (l._1, l._2, l._3.compile)
    }
    Matrix.remaining -= sigma.length
    result
  }

  def filterMatrix(
      ix: Option[Constructor],
      residual: Option[Pattern[String]],
      checkPattern: (Clause, Pattern[String]) => Boolean,
      colIx: Int
  ): Matrix = {
    val newRows = rows
      .filter(row => checkPattern(row.clause, row.patterns(colIx)))
      .map(row =>
        Row(row.patterns, row.clause.addVars(ix, residual, row.patterns(colIx), fringe(colIx)))
      )
    Matrix.fromRows(symlib, newRows, fringe, search)
  }

  def defaultConstructor(
      colIx: Int,
      sigma: immutable.Seq[Constructor],
      matrixColumns: Option[immutable.IndexedSeq[Column]]
  ): Option[Constructor] =
    if (sigma.contains(Empty())) Some(NonEmpty())
    else if (sigma.isEmpty) None
    else {
      val col = matrixColumns match {
        case None       => columns(colIx)
        case Some(cols) => cols(colIx)
      }
      lazy val (hd, tl) = col.maxListSize
      sigma.head match {
        case ListC(sym, _) => Some(ListC(sym, hd + tl))
        case _             => None
      }
    }

  def trueDefault(
      colIx: Int,
      sigma: immutable.Seq[Constructor],
      matrixColumns: Option[immutable.IndexedSeq[Column]]
  ): Matrix = {
    val ctr      = defaultConstructor(colIx, sigma, matrixColumns)
    val filtered = filterMatrix(ctr, None, (_, p) => p.isDefault, colIx)
    val expanded = if (ctr.isDefined) {
      if (columns(colIx).fringe.sortInfo.category.isExpandDefault) {
        Matrix.fromColumns(
          symlib,
          filtered.columns(colIx).expand(ctr.get, isExact = true) ++ filtered.notBestCol(colIx),
          filtered.clauses,
          search
        )
      } else {
        Matrix.fromColumns(symlib, filtered.notBestCol(colIx), filtered.clauses, search)
      }
    } else {
      Matrix.fromColumns(symlib, filtered.notBestCol(colIx), filtered.clauses, search)
    }
    expanded
  }

  def default(colIx: Int, sigma: immutable.Seq[Constructor]): Option[Matrix] =
    if (
      columns(colIx).fringe.sortInfo.category.hasIncompleteSignature(sigma, columns(colIx).fringe)
    ) {
      Some(trueDefault(colIx, sigma, None))
    } else {
      None
    }

  lazy val compiledDefault: Option[DecisionTree] = {
    Matrix.remaining += 1
    val result = default(bestColIx, sigma).map(_.compile)
    Matrix.remaining -= 1
    result
  }

  def getLeaf(row: Row, child: DecisionTree): DecisionTree = {
    def makeEquality(
        category: SortCategory,
        os: (Occurrence, Occurrence),
        dt: DecisionTree
    ): DecisionTree =
      Function(
        category.equalityFun,
        Equal(os._1, os._2),
        immutable.Seq((os._1, category.hookAtt), (os._2, category.hookAtt)),
        "BOOL.Bool",
        SwitchLit(
          Equal(os._1, os._2),
          "BOOL.Bool",
          1,
          immutable.Seq(("1", immutable.Seq(), dt), ("0", immutable.Seq(), child)),
          None
        )
      )
    def sortCat(sort: Sort): SortCategory =
      SortCategory(
        Parser.getStringAtt(symlib.sortAtt(sort), "hook").orElse(Some("STRING.String")),
        sort,
        symlib
      )
    // first, add all remaining variable bindings to the clause
    val vars = row.clause.bindings ++ fringe
      .lazyZip(row.patterns)
      .to(immutable.Seq)
      .flatMap(t => t._2.bindings(None, None, t._1.occurrence, symlib))
    val overloadVars = row.clause.overloadChildren.map(_._3)
    val freshVars = row.clause.action.freshConstants.map(t =>
      VariableBinding(t._1, sortCat(t._2), Fresh(t._1), None)
    )
    val allVars = vars ++ overloadVars ++ freshVars
    // then group the bound variables by their name
    val grouped =
      allVars.groupBy(v => v.name).view.mapValues(_.map(v => (v.category, v.occurrence))).toMap
    // compute the variables bound more than once
    val nonlinear      = grouped.filter(_._2.size > 1)
    val nonlinearPairs = nonlinear.view.mapValues(l => l.lazyZip(l.tail)).toMap
    val uniqueLhsVars =
      row.clause.action.lhsVars.filterNot(v => row.clause.action.rhsVars.contains(v.name)).distinct
    val newVars = {
      val lhs = uniqueLhsVars.map(v =>
        grouped
          .get(v.name)
          .map(g => v.name -> (g.head._2, g.head._1.hookAtt))
          .getOrElse(v.name -> (Base(), sortCat(v.sort).hookAtt))
      )
      val rhs =
        try
          row.clause.action.rhsVars.map(v => v -> (grouped(v).head._2, grouped(v).head._1.hookAtt))
        catch {
          case _: NoSuchElementException =>
            throw new MatchingException(
              MatchingException.Type.INTERNAL_ERROR,
              "Could not find binding for variable while compiling pattern matching.",
              row.clause.action.source,
              row.clause.action.location
            )
        }
      (lhs ++ rhs).sortBy(v => v._1).map(v => v._2)
    }
    val atomicLeaf = if (search) {
      SearchLeaf(
        row.clause.action.ordinal,
        newVars,
        Matrix.fromRows(symlib, firstGroup.patch(bestRowIx, Nil, 1), fringe, search = true).compile
      )
    } else {
      Leaf(row.clause.action.ordinal, newVars)
    }
    // check that all occurrences of the same variable are equal
    val sc: DecisionTree = row.clause.action.scVars match {
      // if there is no side condition, continue
      case None       => atomicLeaf
      case Some(cond) =>
        val condVars = cond.map(v => (grouped(v).head._2, grouped(v).head._1.hookAtt))
        val newO     = SC(row.clause.action.ordinal)
        // evaluate the side condition and if it is true, continue, otherwise go to the next row
        Function(
          "side_condition_" + row.clause.action.ordinal,
          newO,
          condVars,
          "BOOL.Bool",
          SwitchLit(
            newO,
            "BOOL.Bool",
            1,
            immutable.Seq(("1", immutable.Seq(), atomicLeaf), ("0", immutable.Seq(), child)),
            None
          )
        )
    }
    val nonlinearLeaf = nonlinearPairs.foldRight[DecisionTree](sc)((e, dt) =>
      e._2.foldRight(dt)((os, dt2) => makeEquality(os._1._1, (os._1._2, os._2._2), dt2))
    )
    // fill out the bindings for list range variables
    val withRanges = row.clause.listRanges.foldRight(nonlinearLeaf) {
      case ((o @ Num(_, o2), hd, tl), dt) =>
        Function(
          "hook_LIST_range_long",
          o,
          immutable.Seq(
            (o2, "LIST.List"),
            (Lit(hd.toString, "MINT.MInt 64"), "MINT.MInt 64"),
            (Lit(tl.toString, "MINT.MInt 64"), "MINT.MInt 64")
          ),
          "LIST.List",
          dt
        )
      case _ => ???
    }
    val withOverloads = row.clause.overloadChildren.foldRight(withRanges) {
      case ((SymbolC(inj), f, v), dt) =>
        MakePattern(
          v.occurrence,
          v.category.hookAtt,
          SymbolP(
            inj,
            immutable.Seq(
              VariableP(
                Some(v.occurrence.asInstanceOf[Inj].rest),
                f.expand(SymbolC(inj)).head.sortInfo.category
              )
            )
          ),
          dt
        )
      case _ => ???
    }
    val withSpecials = row.clause.specializedVars.foldRight(withOverloads) { case ((o, p), dt) =>
      MakePattern(o, p._1.hookAtt, p._2, dt)
    }
    row.clause.action.freshConstants.foldRight(withSpecials) { case ((name, sort), dt) =>
      val sortName = sort.asInstanceOf[CompoundSort].ctr
      val litO     = Lit(sortName, "STRING.String")
      MakePattern(
        litO,
        "STRING.String",
        LiteralP(sortName, StringS()),
        Function(
          "get_fresh_constant",
          Fresh(name),
          immutable
            .Seq((litO, "STRING.String"), (Num(row.clause.action.arity, Base()), "STRING.String")),
          sortCat(sort).hookAtt,
          dt
        )
      )
    }
  }

  def expand: Matrix =
    if (fringe.isEmpty) {
      new Matrix(symlib, rawColumns, rawRows, rawClauses, rawFringe, search)
    } else {
      fringe.indices.foldLeft(this)((accum, colIx) =>
        Matrix.fromRows(symlib, accum.rows.flatMap(_.expand(colIx)), fringe, search)
      )
    }

  lazy val firstGroup: immutable.IndexedSeq[Row] =
    rows.takeWhile(_.clause.action.priority == rows.head.clause.action.priority)

  lazy val bestRowIx: Int = firstGroup.indexWhere(_.isWildcard)

  lazy val bestRow: Row = rows(bestRowIx)

  def compile: DecisionTree = {
    val result = Matrix.cache.get(this)
    if (result == null) {
      val computed = compileInternal
      Matrix.cache.put(this, computed)
      computed
    } else {
      result
    }
  }

  def compileSearch: DecisionTree =
    new Matrix(symlib, rawColumns, rawRows, rawClauses, rawFringe, true).compile

  def compileInternal: DecisionTree = {
    if (Matching.logging) {
      val s = toString
      System.out.println("-- Compile --")
      System.out.println(s)
      System.out.println("remaining: " + Matrix.remaining)
    }
    if (clauses.isEmpty)
      Failure()
    else {
      bestRowIx match {
        case -1 =>
          if (matrixColumns(bestColIx).score.head.isPosInfinity) {
            // decompose this column as it contains only wildcards
            val newClauses = bestCol.clauses
              .lazyZip(bestCol.patterns)
              .toIndexedSeq
              .map(t => t._1.addVars(None, None, t._2, bestCol.fringe))
            Matrix
              .fromColumns(
                symlib,
                notBestCol(bestColIx).map(c => new Column(c.fringe, c.patterns, newClauses)),
                newClauses,
                search
              )
              .compile
          } else {
            // compute the sort category of the best column
            bestCol.category.tree(this)
          }
        case _ =>
          // if there is only one row left, then try to match it and fail the matching if it fails
          // otherwise, if it fails, try to match the remainder of the matrix
          getLeaf(bestRow, notBestRow.compile)
      }
    }
  }

  def necessary(rowIx: Int, colIx: Int): Boolean =
    !columns(colIx).patterns(rowIx).isWildcard || notCol(colIx).rowUseless(rowIx)

  private def useful(r: Row): Boolean = {
    if (Matching.logging) {
      System.out.println("-- Useful --")
      System.out.println("Row: ")
      System.out.println(r)
      System.out.println("Matrix: ")
      System.out.println(this)
    }
    assert(r.patterns.size == columns.size)
    if (fringe.isEmpty) {
      if (clauses.nonEmpty) {
        false
      } else {
        true
      }
    } else {
      val sigma = columns(0).signatureForUsefulness
      if (
        r.patterns(0).isWildcard && columns(0).fringe.sortInfo.category
          .hasIncompleteSignature(sigma, columns(0).fringe)
      ) {
        val matrixDefault = default(0, sigma)
        val rowDefault    = r.default(0, sigma, symlib, fringe, columns)
        matrixDefault.isDefined && rowDefault.isDefined && matrixDefault.get.useful(rowDefault.get)
      } else if (r.patterns(0).isWildcard) {
        for (con <- sigma) {
          if (Matching.logging) {
            System.out.println("Testing constructor " + con)
          }
          val rowSpec = r.specialize(con, 0, symlib, fringe)
          if (rowSpec.isDefined && specialize(con, 0, None)._3.useful(rowSpec.get)) {
            return true
          }
        }
        false
      } else {
        val rowColumn =
          new Column(
            columns(0).fringe,
            immutable.IndexedSeq(r.patterns(0)),
            immutable.IndexedSeq(r.clause)
          )
        val rowSigma = rowColumn.signatureForUsefulness
        for (con <- rowSigma) {
          if (Matching.logging) {
            System.out.println("Testing constructor " + con)
          }
          val rowSpec = r.specialize(con, 0, symlib, fringe)
          if (rowSpec.isDefined && specialize(con, 0, None)._3.useful(rowSpec.get)) {
            return true
          }
        }
        val matrixDefault = default(0, rowSigma)
        val rowDefault    = r.default(0, rowSigma, symlib, fringe, columns)
        matrixDefault.isDefined && rowDefault.isDefined && matrixDefault.get.useful(rowDefault.get)
      }
    }
  }

  def rowUseless(rowIx: Int): Boolean = {
    val filteredRows = rows.take(rowIx) ++ rows
      .drop(rowIx + 1)
      .takeWhile(_.clause.action.priority == rows(rowIx).clause.action.priority)
    val withoutSC =
      filteredRows.filter(r => r.clause.action.scVars.isEmpty && !r.clause.action.nonlinear)
    val matrix = Matrix.fromRows(symlib, withoutSC, fringe, search)
    val row    = rows(rowIx)
    val result = !matrix.useful(row)
    if (Matching.logging && result) {
      System.out.println("Row " + row.clause.action.ordinal + " is useless")
    }
    result
  }

  def nonExhaustive: Option[immutable.IndexedSeq[Pattern[String]]] =
    if (fringe.isEmpty) {
      if (clauses.nonEmpty) {
        None
      } else {
        Some(immutable.IndexedSeq())
      }
    } else {
      val sigma = columns(0).signatureForUsefulness
      if (columns(0).fringe.sortInfo.category.hasIncompleteSignature(sigma, columns(0).fringe)) {
        val matrixDefault = default(0, sigma)
        if (matrixDefault.isEmpty) {
          None
        } else {
          val id = Matrix.id
          if (Matching.logging) {
            System.out.println("-- Exhaustive --")
            System.out.println("Matrix " + id + ": ")
            System.out.println(this)
            Matrix.id += 1
          }
          val child = matrixDefault.get.nonExhaustive
          if (child.isEmpty) {
            None
          } else {
            if (Matching.logging) {
              System.out.println(
                "Submatrix " + id + " is non-exhaustive:\n" + child.get
                  .map(p => new util.Formatter().format("%12.12s", p.toShortString))
                  .mkString(" ")
              )
            }
            val ctr = defaultConstructor(0, sigma, None)
            if (sigma.isEmpty) {
              assert(child.get.size + 1 == fringe.size)
              Some(WildcardP[String]() +: child.get)
            } else if (ctr.isDefined && columns(0).fringe.sortInfo.category.isExpandDefault) {
              val arity = ctr.get.expand(fringe(0)).get.size
              assert(child.get.size - arity + 1 == fringe.size)
              Some(ctr.get.contract(fringe(0), child.get.take(arity)) +: child.get.drop(arity))
            } else {
              assert(child.get.size + 1 == fringe.size)
              Some(columns(0).category.missingConstructor(sigma, columns(0).fringe) +: child.get)
            }
          }
        }
      } else {
        for (con <- sigma) {
          val id = Matrix.id
          if (Matching.logging) {
            System.out.println("Testing constructor " + con)
            System.out.println("-- Exhaustive --")
            System.out.println("Matrix " + id + ": ")
            System.out.println(this)
            Matrix.id += 1
          }
          val child = specialize(con, 0, None)._3.nonExhaustive
          if (child.isDefined) {
            if (Matching.logging) {
              System.out.println(
                "Submatrix " + id + " is non-exhaustive:\n" + child.get
                  .map(p => new util.Formatter().format("%12.12s", p.toShortString))
                  .mkString(" ")
              )
            }
            val arity = con.expand(fringe(0)).get.size
            assert(child.get.size - arity + 1 == fringe.size)
            return Some(con.contract(fringe(0), child.get.take(arity)) +: child.get.drop(arity))
          }
        }
        None
      }
    }

  def checkUsefulness(kem: MatchingException => Unit): Unit =
    for (rowIx <- rows.indices)
      if (rowUseless(rowIx)) {
        if (clauses(rowIx).action.source.isPresent && clauses(rowIx).action.location.isPresent) {
          kem(
            new MatchingException(
              MatchingException.Type.USELESS_RULE,
              "Potentially useless rule detected.",
              clauses(rowIx).action.source,
              clauses(rowIx).action.location
            )
          )
        }
      }

  def checkExhaustiveness(name: SymbolOrAlias, kem: MatchingException => Unit): Unit = {
    Matrix.id = 0
    val id = Matrix.id
    if (Matching.logging) {
      System.out.println("-- Exhaustive --")
      System.out.println("Matrix " + id + ": ")
      System.out.println(this)
      Matrix.id += 1
    }
    val counterexample = nonExhaustive
    if (counterexample.isDefined) {
      if (Matching.logging) {
        System.out.println(
          "Matrix " + id + " is non-exhaustive:\n" + counterexample.get
            .map(p => new util.Formatter().format("%12.12s", p.toShortString))
            .mkString(" ")
        )
      }
      val k          = fringe.zip(counterexample.get).map(t => t._2.toKORE(t._1))
      val func       = B.Application(name, k)
      val attributes = symlib.signatures(name)._3
      val location   = Parser.location(attributes)
      val source     = Parser.source(attributes)

      if (!Parser.hasAtt(attributes, "no-evaluators"))
        kem(
          new MatchingException(
            MatchingException.Type.NON_EXHAUSTIVE_MATCH,
            "Non exhaustive match detected",
            source,
            location,
            func
          )
        )
    }
  }

  def specializeBy(
      ps: immutable.IndexedSeq[Pattern[String]]
  ): (Matrix, immutable.IndexedSeq[Pattern[String]]) = {
    def expandChildren(pat: Pattern[String]): immutable.IndexedSeq[Pattern[String]] =
      pat match {
        case SymbolP(_, ps) => ps.toIndexedSeq
        case LiteralP(_, _) => immutable.IndexedSeq()
        case _              => ???
      }
    def getConstructor(pat: Pattern[String]): Constructor =
      pat match {
        case SymbolP(sym, _)  => SymbolC(sym)
        case LiteralP(lit, _) => LiteralC(lit)
        case _                => ???
      }
    specializing = Some(ps)
    if (Matching.logging) {
      System.out.println(
        "Specializing by:\n" + ps
          .map(p => new util.Formatter().format("%12.12s", p.toShortString))
          .mkString(" ")
      )
      System.out.println(toString)
      System.out.println("remaining: " + Matrix.remaining)
    }
    if (clauses.isEmpty || columns.indices.forall(i => isWildcardOrResidual(ps(i)))) {
      (this, ps)
    } else {
      val residual = ps(bestColIx)
      if (Matching.logging) {
        System.out.println("Chose column " + bestColIx)
      }
      val constructor = getConstructor(residual)
      val specialized = specialize(constructor, bestColIx, Some(residual))
      val args        = expandChildren(residual)
      specialized._3.specializeBy(args ++ ps.patch(bestColIx, Nil, 1))
    }
  }

  def notBestRow: Matrix =
    Matrix.fromRows(symlib, rows.patch(bestRowIx, Nil, 1), fringe, search)

  def notBestCol(colIx: Int): immutable.IndexedSeq[Column] =
    columns.patch(colIx, Nil, 1)

  def notCol(colIx: Int): Matrix =
    Matrix.fromColumns(symlib, notBestCol(colIx), clauses, search)

  def colScoreString: String =
    symlib.heuristics
      .map(h =>
        columns.indices
          .map(c => "%12.2f".format(matrixColumns(c).computeScoreForKey(h, columns(c).bestKey)))
          .mkString(" ")
      )
      .mkString("\n")

  def neededString: String =
    matrixColumns
      .flatMap(col =>
        matrixColumns
          .filter(c =>
            !c.column.isValid && col.colIx != c.colIx && col.column.needed(c.column.keyVars)
          )
          .map((col, _))
      )
      .map(p => "Column " + p._2.colIx + " needs " + p._1.colIx)
      .mkString("\n")

  override def toString: String =
    fringe.map(_.occurrence.toString).mkString("\n") + "\n" + neededString + "\n" + fringe
      .map(_.toString)
      .mkString(" ") + "\n" + columns.indices
      .map(i => validCols.map(_.colIx).contains(i))
      .map(v => "%12.12s".format(v.toString))
      .mkString(" ") + "\n" + colScoreString + "\n" + rows.map(_.toString).mkString("\n") + "\n"

  def canEqual(other: Any): Boolean = other.isInstanceOf[Matrix]

  override def equals(other: Any): Boolean = other match {
    case that: Matrix =>
      that.canEqual(this) &&
      symlib == that.symlib &&
      fringe == that.fringe &&
      rows == that.rows &&
      search == that.search
    case _ => false
  }

  override lazy val hashCode: Int = {
    val state = immutable.Seq(symlib, rows, search)
    state.map(_.hashCode()).foldLeft(0)((a, b) => 31 * a + b)
  }
}

object Matrix {
  var remaining = 0

  def fromRows(
      symlib: Parser.SymLib,
      rows: immutable.IndexedSeq[Row],
      fringe: immutable.IndexedSeq[Fringe],
      search: Boolean
  ): Matrix =
    new Matrix(symlib, null, rows, null, fringe, search)

  def fromColumns(
      symlib: Parser.SymLib,
      cols: immutable.IndexedSeq[Column],
      clauses: immutable.IndexedSeq[Clause],
      search: Boolean
  ): Matrix =
    new Matrix(symlib, cols, null, clauses, null, search)

  private val cache = new ConcurrentHashMap[Matrix, DecisionTree]()

  def clearCache(): Unit =
    cache.clear()

  var id = 0
}
