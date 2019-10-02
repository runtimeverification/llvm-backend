package org.kframework.backend.llvm.matching

import org.kframework.kore.KORE.{KApply,KList}
import org.kframework.unparser.ToKast
import org.kframework.attributes.{Location,Source}
import org.kframework.parser.kore.{Sort,CompoundSort,SymbolOrAlias}
import org.kframework.parser.kore.implementation.{DefaultBuilders => B}
import org.kframework.utils.errorsystem.KException
import org.kframework.backend.llvm.matching.pattern._
import org.kframework.backend.llvm.matching.dt._
import java.util
import java.util.concurrent.ConcurrentHashMap

trait AbstractColumn {
  def column: Column

  def computeScoreForKey(heuristic: Heuristic, key: Option[Pattern[Option[Occurrence]]]): Double = {
    def withChoice(result: Double): Double = {
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

case class MatrixColumn(val matrix: Matrix, colIx: Int) extends AbstractColumn {
  def column: Column = matrix.columns(colIx)
  lazy val score: Seq[Double] = column.score(this)
}

class Column(val fringe: Fringe, val patterns: IndexedSeq[Pattern[String]], val clauses: IndexedSeq[Clause]) extends AbstractColumn {
  def column: Column = this

  lazy val category: SortCategory = {
    val ps = patterns.map(_.category).filter(_.isDefined)
    if (ps.isEmpty) {
      SymbolS()
    } else {
      ps.head.get
    }
  }

  lazy val score: Seq[Double] = computeScore

  def score(matrixCol: MatrixColumn) : Seq[Double] = {
    def zeroOrHeuristic(h: Heuristic): Double = {
      if (h.needsMatrix) {
        matrixCol.computeScoreForKey(h, bestKey)
      } else {
        0.0
      }
    }
    fringe.symlib.heuristics.zip(score).map((hs: (Heuristic, Double)) => hs._2 + zeroOrHeuristic(hs._1))
  }

  def computeScore: Seq[Double] = {
    computeScoreForKey(bestKey)
  }

  def computeScoreForKey(key: Option[Pattern[Option[Occurrence]]]): Seq[Double] = {
    def zeroOrHeuristic(h: Heuristic): Double = {
      if (h.needsMatrix) {
        return 0.0
      } else {
        return computeScoreForKey(h, key)
      }
    }
    fringe.symlib.heuristics.map(zeroOrHeuristic(_))
  }

  def isValid: Boolean = isValidForKey(bestKey)

  def isValidForKey(key: Option[Pattern[Option[Occurrence]]]): Boolean = {
    !fringe.sortInfo.isCollection || key.isDefined || !patterns.exists(_.isChoice)
  }

  lazy val keyVars: Seq[Set[String]] = {
    val keys = patterns.map(_.mapOrSetKeys)
    keys.map(_.flatMap(_.variables).toSet)
  }
  private lazy val boundVars: Seq[Set[String]] = patterns.map(_.variables)
  def needed(vars: Seq[Set[String]]): Boolean = {
    val intersection = (vars, boundVars).zipped.map(_.intersect(_))
    intersection.exists(_.nonEmpty)
  }

  lazy val isEmpty = fringe.sortInfo.isCollection && rawSignature.contains(Empty())

  private lazy val rawSignature: Seq[Constructor] = {
    patterns.zipWithIndex.flatMap(p => p._1.signature(clauses(p._2)))
  }

  def signatureForKey(key: Option[Pattern[Option[Occurrence]]]): List[Constructor] = {
    val bestUsed = key match {
      case None => rawSignature
      case Some(k) => rawSignature.filter(_.isBest(k))
    }
    val usedInjs = bestUsed.flatMap(fringe.injections)
    val dups = if (fringe.isExact) bestUsed else bestUsed ++ usedInjs
    val nodups = dups.distinct.toList
    if (nodups.contains(Empty())) {
      List(Empty())
    } else {
      nodups.filter(_ != Empty())
    }
  }

  def signatureForUsefulness: List[Constructor] = {
    signatureForKey(None).filter(!_.isInstanceOf[HasNoKey]).map(c => {
      if (!c.isInstanceOf[HasKey]) {
        c
      } else {
        val hasKey = c.asInstanceOf[HasKey]
        HasKey(hasKey.isSet, hasKey.element, None)
      }
    })
  }

  lazy val signature: List[Constructor] = {
    signatureForKey(bestKey)
  }

  def isChoice: Boolean = fringe.sortInfo.isCollection && bestKey == None

  private def asListP(p: Pattern[String]): Seq[ListP[String]] = {
    p match {
      case l@ListP(_, _, _, _, _) => Seq(l)
      case AsP(_, _, pat) => asListP(pat)
      case _ => Seq()
    }
  }

  def maxListSize: (Int, Int) = {
    val listPs = patterns.flatMap(asListP(_))
    if (listPs.isEmpty) {
      (0, 0)
    } else {
      val longestHead = listPs.map(_.head.size).max
      val longestTail = listPs.map(_.tail.size).max
      (longestHead, longestTail)
    }
  }

  lazy val validKeys: Seq[Pattern[Option[Occurrence]]] = {
    val possibleKeys = rawSignature.flatMap({
      case HasKey(_, _, Some(k)) => Seq(k)
      case _ => Seq()
    })
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

  def maxPriority: Int = {
    if (isChoice) {
      clauses(0).action.priority
    } else {
      Int.MaxValue
    }
  }

  def expand(ix: Constructor, isExact: Boolean): IndexedSeq[Column] = {
    val fringes = fringe.expand(ix)
    val ps = (patterns, clauses).zipped.toIterable.map(t => t._1.expand(ix, isExact, fringes, fringe, t._2, maxPriority))
    val transposed = if (ps.isEmpty) fringes.map(_ => IndexedSeq()) else ps.transpose
    (fringes, transposed).zipped.toIndexedSeq.map(t => new Column(t._1, t._2.toIndexedSeq, clauses))
  }

  lazy val isWildcard: Boolean = patterns.forall(_.isWildcard)

  def canEqual(other: Any): Boolean = other.isInstanceOf[Column]

  override def equals(other: Any): Boolean = other match {
    case that: Column =>
      (that canEqual this) &&
        fringe == that.fringe &&
        patterns == that.patterns
    case _ => false
  }

  override lazy val hashCode: Int = {
    val state = Seq(patterns)
    state.map(_.hashCode()).foldLeft(0)((a, b) => 31 * a + b)
  }
}

case class VariableBinding[T](val name: T, val category: SortCategory, val occurrence: Occurrence, val pattern: Option[Pattern[String]]) {
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class Fringe(val symlib: Parser.SymLib, val sort: Sort, val occurrence: Occurrence, val isExact: Boolean) {
  val sortInfo = SortInfo(sort, symlib)

  def overloads(sym: SymbolOrAlias): Seq[SymbolOrAlias] = {
    symlib.overloads.getOrElse(sym, Seq())
  }

  def injections(ix: Constructor): Seq[Constructor] = {
    ix match {
      case SymbolC(sym) =>
        if (symlib.overloads.contains(sym) ||sym.ctr == "inj") {
          if (!sortInfo.trueInjMap.contains(sym)) {
            System.err.println(sort)
            System.err.println(sym)
          }
          sortInfo.trueInjMap(sym).map(SymbolC)
        } else {
          Seq()
        }
      case _ => Seq()
    }
  }

  def contains(ix: Constructor): Boolean = {
    lookup(ix).isDefined
  }

  def expand(ix: Constructor): Seq[Fringe] = {
    lookup(ix).get
  }

  def lookup(ix: Constructor): Option[Seq[Fringe]] = {
    ix.expand(this)
  }

  def inexact: Fringe = {
    Fringe(symlib, sort, occurrence, false)
  }

  override def toString: String = new util.Formatter().format("%12.12s", sort.toString).toString
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

class SortInfo private(sort: Sort, symlib: Parser.SymLib) {
  val constructors = symlib.constructorsForSort.getOrElse(sort, Seq())
  val exactConstructors = constructors.filter(_.ctr != "inj")
  private val rawInjections = constructors.filter(_.ctr == "inj")
  private val injMap = rawInjections.map(b => (b, rawInjections.filter(a => symlib.isSubsorted(a.params.head, b.params.head)))).toMap
  private val rawOverloads = constructors.filter(symlib.overloads.contains)
  private val overloadMap = rawOverloads.map(s => (s, symlib.overloads(s))).toMap
  private val overloadInjMap = overloadMap.map(e => (e._1, e._2.map(g => B.SymbolOrAlias("inj", Seq(symlib.signatures(g)._2, symlib.signatures(e._1)._2)))))
  val trueInjMap = injMap ++ overloadInjMap
  val category: SortCategory = SortCategory(Parser.getStringAtt(symlib.sortAtt(sort), "hook"))
  val length: Int = category.length(constructors.size)
  val exactLength: Int = category.length(exactConstructors.size)
  val isCollection: Boolean = {
    category match {
      case MapS() | SetS() => true
      case _ => false
    }
  }
}
object SortInfo {
  def apply(sort: Sort, symlib: Parser.SymLib): SortInfo = {
    symlib.sortCache.computeIfAbsent(sort, s => new SortInfo(s, symlib))
  }
}

case class Action(val ordinal: Int, val rhsVars: Seq[String], val scVars: Option[Seq[String]], val freshConstants: Seq[(String, Sort)], val arity: Int, val priority: Int, source: Option[Source], location: Option[Location], nonlinear: Boolean) {
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class Clause(
  // the rule to be applied if this row succeeds
  val action: Action,
  // the variable bindings made so far while matching this row
  val bindings: Vector[VariableBinding[String]],
  // the length of the head and tail of any list patterns
  // with frame variables bound so far in this row
  val listRanges: Vector[(Occurrence, Int, Int)],
  // variable bindings to injections that need to be constructed
  // since they do not actually exist in the original subject term
  val overloadChildren: Vector[(Constructor, VariableBinding[String])],
  val specializedVars: Map[Occurrence, Pattern[Option[Occurrence]]]) {

  lazy val bindingsMap: Map[String, VariableBinding[String]] = bindings.groupBy(_.name).mapValues(_.head)
  lazy val boundOccurrences: Set[Occurrence] = bindings.map(_.occurrence).toSet

  def isBound(binding: Any) = {
    binding match {
      case name: String => bindingsMap.contains(name)
      case o: Option[_] => boundOccurrences.contains(o.get.asInstanceOf[Occurrence])
    }
  }

  def canonicalize(name: String): Option[Occurrence] = {
    bindingsMap.get(name).map(_.occurrence)
  }
  def canonicalize[T](pat: Pattern[T]): Option[Pattern[Option[Occurrence]]] = {
    if (pat.isBound(this)) {
      Some(pat.canonicalize(this))
    } else {
      None
    }
  }

  def addVars(ix: Option[Constructor], residual: Option[Pattern[String]], pat: Pattern[String], f: Fringe): Clause = {
    new Clause(action, bindings ++ pat.bindings(ix, residual, f.occurrence, f.symlib), listRanges ++ pat.listRange(ix, f.occurrence), overloadChildren ++ pat.overloadChildren(f, ix, residual, Num(0, f.occurrence)), specializedVars)
  }

  private def translateVars(residuals: Seq[(Pattern[String], Occurrence)], allVars: Vector[VariableBinding[String]], symlib: Parser.SymLib): Map[Occurrence, Pattern[Option[Occurrence]]] = {
    val residualMap = residuals.toMap
    def substituteBy(pat: Pattern[String], category: SortCategory): Pattern[Option[Occurrence]] = {
      residualMap.get(pat) match {
        case Some(o) => VariableP(Some(o), category)
        case None => pat match {
          case SymbolP(sym, ps) => SymbolP(sym, ps.zipWithIndex.map(p => substituteBy(p._1, SortInfo(symlib.signatures(sym)._1(p._2), symlib).category)))
          case LiteralP(lit, sort) => LiteralP(lit, sort)
          case _ => ???
        }
      }
    }
    allVars.filter(_.pattern.isDefined).map(v => v.occurrence -> substituteBy(v.pattern.get, v.category)).toMap
  }

  def specializeBy(residualMap: Seq[(Pattern[String], Occurrence)], symlib: Parser.SymLib): Clause = {
    val overloadVars = overloadChildren.map(_._2)
    val allVars = bindings ++ overloadVars
    Clause(action, allVars, listRanges, Vector(), specializedVars ++ translateVars(residualMap, allVars, symlib))
  }

  override def toString: String = action.ordinal.toString + "(" + action.priority.toString + ")"
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class Row(val patterns: IndexedSeq[Pattern[String]], val clause: Clause) {
  // returns whether the row is done matching
  def isWildcard: Boolean = patterns.forall(_.isWildcard)

  def expand(colIx: Int): Seq[Row] = {
    val p0s = patterns(colIx).expandOr
    p0s.map(p => new Row(patterns.updated(colIx, p), clause))
  }

  def specialize(ix: Constructor, colIx: Int, symlib: Parser.SymLib, fringe: IndexedSeq[Fringe]): Option[Row] = {
    Matrix.fromRows(symlib, IndexedSeq(this), fringe).specialize(ix, colIx, None)._2.rows.headOption
  }

  def default(colIx: Int, sigma: Seq[Constructor], symlib: Parser.SymLib, fringe: IndexedSeq[Fringe], matrixColumns: IndexedSeq[Column]): Option[Row] = {
    Matrix.fromRows(symlib, IndexedSeq(this), fringe).trueDefault(colIx, sigma, Some(matrixColumns)).rows.headOption
  }

  override def toString: String = patterns.map(p => new util.Formatter().format("%12.12s", p.toShortString)).mkString(" ") + " " + clause.toString
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

class Matrix private(val symlib: Parser.SymLib, private val rawColumns: IndexedSeq[Column], private val rawRows: IndexedSeq[Row], private val rawClauses: IndexedSeq[Clause], private val rawFringe: IndexedSeq[Fringe]) {
  lazy val clauses: IndexedSeq[Clause] = {
    if (rawClauses != null) {
      rawClauses
    } else {
      rawRows.map(_.clause)
    }
  }

  lazy val fringe: IndexedSeq[Fringe] = {
    if (rawFringe != null) {
      rawFringe
    } else {
      rawColumns.map(_.fringe)
    }
  }

  lazy val columns: IndexedSeq[Column] = {
    if (rawColumns != null) {
      rawColumns
    } else if (rawRows.isEmpty) {
      rawFringe.map(f => new Column(f, IndexedSeq(), IndexedSeq()))
    } else {
      computeColumns
    }
  }

  private def computeColumns: IndexedSeq[Column] = {
    val ps = rawRows.map(_.patterns).transpose
    rawFringe.indices.map(col => new Column(rawFringe(col), ps(col), clauses))
  }

  lazy val rows: IndexedSeq[Row] = {
    if (rawRows != null) {
      rawRows
    } else if (rawColumns.isEmpty) {
      rawClauses.map(clause => new Row(IndexedSeq(), clause))
    } else {
      computeRows
    }
  }

  private def computeRows: IndexedSeq[Row] = {
    val ps = rawColumns.map(_.patterns).transpose
    rawClauses.indices.map(row => new Row(ps(row), rawClauses(row)))
  }

  def this(symlib: Parser.SymLib, cols: IndexedSeq[(Sort, IndexedSeq[Pattern[String]])], actions: IndexedSeq[Action]) {
    this(symlib, (cols, (1 to cols.size).map(i => new Fringe(symlib, cols(i - 1)._1, Num(i, Base()), false))).zipped.toIndexedSeq.map(pair => new Column(pair._2, pair._1._2, actions.map(new Clause(_, Vector(), Vector(), Vector(), Map())))), null, actions.map(new Clause(_, Vector(), Vector(), Vector(), Map())), null)
  }

  private def isWildcardOrResidual(pat: Pattern[String]): Boolean = {
    pat.isWildcard || pat.isResidual(symlib)
  }

  private lazy val matrixColumns: Seq[MatrixColumn] = {
    columns.indices.map(MatrixColumn(this, _))
  }

  private lazy val validCols: Seq[MatrixColumn] = {
    matrixColumns.filter(col => col.column.isValid || columns.forall(c => c == col.column || !c.needed(col.column.keyVars)))
  }

  private var specializing: Option[IndexedSeq[Pattern[String]]] = None

  // compute the column with the best score, choosing the first such column if they are equal
  lazy val bestColIx: Int = {
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
  }

  lazy val bestCol: Column = columns(bestColIx)

  lazy val sigma: List[Constructor] = bestCol.signature

  def specialize(ix: Constructor, colIx: Int, residual: Option[Pattern[String]]): (String, Matrix) = {
    val filtered = filterMatrix(Some(ix), residual, (c, p) => p.isSpecialized(ix, residual.isEmpty, columns(colIx).fringe, c, columns(colIx).maxPriority), colIx)
    val expanded = Matrix.fromColumns(symlib, filtered.columns(colIx).expand(ix, residual.isEmpty) ++ filtered.notBestCol(colIx), filtered.clauses)
    (ix.name, expanded)
  }

  def cases: List[(String, Matrix)] = sigma.map(specialize(_, bestColIx, None))

  lazy val compiledCases: Seq[(String, DecisionTree)] = {
    Matrix.remaining += sigma.length
    val result = cases.map(l => {
      if (Matching.logging) {
        System.out.println("Specializing by " + l._1);
      }
      (l._1, l._2.compile)
    })
    Matrix.remaining -= sigma.length
    result
  }

  def filterMatrix(ix: Option[Constructor], residual: Option[Pattern[String]], checkPattern: (Clause, Pattern[String]) => Boolean, colIx: Int): Matrix = {
    val newRows = rows.filter(row => checkPattern(row.clause, row.patterns(colIx))).map(row => new Row(row.patterns, row.clause.addVars(ix, residual, row.patterns(colIx), fringe(colIx))))
    Matrix.fromRows(symlib, newRows, fringe)
  }

  def defaultConstructor(colIx: Int, sigma: Seq[Constructor], matrixColumns: Option[IndexedSeq[Column]]): Option[Constructor] = {
    if (sigma.contains(Empty())) Some(NonEmpty())
    else if (sigma.isEmpty) None
    else {
      val col = matrixColumns match {
        case None => columns(colIx)
        case Some(cols) => cols(colIx)
      }
      lazy val (hd, tl) = col.maxListSize
      sigma.head match {
        case ListC(sym,_) => Some(ListC(sym, hd + tl))
        case _ => None
      }
    }
  }

  def trueDefault(colIx: Int, sigma: Seq[Constructor], matrixColumns: Option[IndexedSeq[Column]]): Matrix = {
    val ctr = defaultConstructor(colIx, sigma, matrixColumns)
    val filtered = filterMatrix(ctr, None, (_, p) => p.isDefault, colIx)
    val expanded = if (ctr.isDefined) {
      if (columns(colIx).fringe.sortInfo.category.isExpandDefault) {
        Matrix.fromColumns(symlib, filtered.columns(colIx).expand(ctr.get, true) ++ filtered.notBestCol(colIx), filtered.clauses)
      } else {
        Matrix.fromColumns(symlib, filtered.notBestCol(colIx), filtered.clauses)
      }
    } else {
      Matrix.fromColumns(symlib, filtered.notBestCol(colIx), filtered.clauses)
    }
    expanded
  }

  def default(colIx: Int, sigma: Seq[Constructor]): Option[Matrix] = {
    if (columns(colIx).category.hasIncompleteSignature(sigma, columns(colIx).fringe)) {
      Some(trueDefault(colIx, sigma, None))
    } else {
      None
    }
  }

  lazy val compiledDefault: Option[DecisionTree] = {
    Matrix.remaining += 1
    val result = default(bestColIx, sigma).map(_.compile)
    Matrix.remaining -= 1
    result
  }

  def getLeaf(row: Row, child: DecisionTree): DecisionTree = {
    def makeEquality(category: SortCategory, os: (Occurrence, Occurrence), dt: DecisionTree): DecisionTree = {
      Function(category.equalityFun, Equal(os._1, os._2), Seq(os._1, os._2), "BOOL.Bool",
        SwitchLit(Equal(os._1, os._2), 1, Seq(("1", dt), ("0", child)), None))
    }
    def sortCat(sort: Sort): SortCategory = {
      SortCategory(Parser.getStringAtt(symlib.sortAtt(sort), "hook").orElse(Some("STRING.String")))
    }
    // first, add all remaining variable bindings to the clause
    val vars = row.clause.bindings ++ (fringe, row.patterns).zipped.toSeq.flatMap(t => t._2.bindings(None, None, t._1.occurrence, symlib))
    val overloadVars = row.clause.overloadChildren.map(_._2)
    val freshVars = row.clause.action.freshConstants.map(t => VariableBinding(t._1, sortCat(t._2), Fresh(t._1), None))
    val allVars = vars ++ overloadVars ++ freshVars
    // then group the bound variables by their name
    val grouped = allVars.groupBy(v => v.name).mapValues(_.map(v => (v.category, v.occurrence)))
    // compute the variables bound more than once
    val nonlinear = grouped.filter(_._2.size > 1)
    val nonlinearPairs = nonlinear.mapValues(l => (l, l.tail).zipped)
    val newVars = row.clause.action.rhsVars.map(v => grouped(v).head._2)
    val atomicLeaf = Leaf(row.clause.action.ordinal, newVars)
    // check that all occurrences of the same variable are equal
    val nonlinearLeaf = nonlinearPairs.foldRight[DecisionTree](atomicLeaf)((e, dt) => e._2.foldRight(dt)((os,dt2) => makeEquality(os._1._1, (os._1._2, os._2._2), dt2)))
    val sc = row.clause.action.scVars match {
      // if there is no side condition, continue
      case None => nonlinearLeaf
      case Some(cond) =>
        val condVars = cond.map(v => grouped(v).head._2)
        val newO = SC(row.clause.action.ordinal)
        // evaluate the side condition and if it is true, continue, otherwise go to the next row
        Function("side_condition_" + row.clause.action.ordinal, newO, condVars, "BOOL.Bool",
          SwitchLit(newO, 1, Seq(("1", nonlinearLeaf), ("0", child)), None))
    }
    // fill out the bindings for list range variables
    val withRanges = row.clause.listRanges.foldRight(sc)({
      case ((o @ Num(_, o2), hd, tl), dt) => Function("hook_LIST_range_long", o, Seq(o2, Lit(hd.toString, "MINT.MInt 64"), Lit(tl.toString, "MINT.MInt 64")), "LIST.List", dt)
    })
    val withOverloads = row.clause.overloadChildren.foldRight(withRanges)({
      case ((SymbolC(inj), v),dt) => MakePattern(v.occurrence, SymbolP(inj, Seq(VariableP(Some(v.occurrence.asInstanceOf[Inj].rest), v.category))), dt)
    })
    val withSpecials = row.clause.specializedVars.foldRight(withOverloads)({
      case ((o, p),dt) => MakePattern(o, p, dt)
    })
    row.clause.action.freshConstants.foldRight(withSpecials)({
      case ((name, sort),dt) => 
        val sortName = sort.asInstanceOf[CompoundSort].ctr
        val litO = Lit(sortName, "STRING.String")
        MakePattern(litO, LiteralP(sortName, StringS()),
          Function("get_fresh_constant", Fresh(name), Seq(litO, Num(row.clause.action.arity, Base())), sortCat(sort).hookAtt, dt))
    })
  }

  def expand: Matrix = {
    if (fringe.isEmpty) {
      new Matrix(symlib, rawColumns, rawRows, rawClauses, rawFringe)
    } else {
      fringe.indices.foldLeft(this)((accum, colIx) => Matrix.fromRows(symlib, accum.rows.flatMap(_.expand(colIx)), fringe))
    }
  }

  lazy val firstGroup = rows.takeWhile(_.clause.action.priority == rows.head.clause.action.priority)

  lazy val bestRowIx = firstGroup.indexWhere(_.isWildcard)

  lazy val bestRow = rows(bestRowIx)

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

  def compileInternal: DecisionTree = {
    if (Matching.logging) {
      System.out.println("-- Compile --")
      System.out.println(toString)
      System.out.println("remaining: " + Matrix.remaining)
    }
    if (clauses.isEmpty)
      Failure()
    else {
      bestRowIx match {
        case -1 => 
          if (matrixColumns(bestColIx).score(0).isPosInfinity) {
            // decompose this column as it contains only wildcards
            val newClauses = (bestCol.clauses, bestCol.patterns).zipped.toIndexedSeq.map(t => t._1.addVars(None, None, t._2, bestCol.fringe))
            Matrix.fromColumns(symlib, notBestCol(bestColIx).map(c => new Column(c.fringe, c.patterns, newClauses)), newClauses).compile
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

  def necessary(rowIx: Int, colIx: Int): Boolean = {
    !columns(colIx).patterns(rowIx).isWildcard || !notCol(colIx).rowUseless(rowIx)
  }

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
      if (r.patterns(0).isWildcard && columns(0).category.hasIncompleteSignature(sigma, columns(0).fringe)) {
        val matrixDefault = default(0, sigma)
        val rowDefault = r.default(0, sigma, symlib, fringe, columns)
        matrixDefault.isDefined && rowDefault.isDefined && matrixDefault.get.useful(rowDefault.get)
      } else if (r.patterns(0).isWildcard) {
        for (con <- sigma) {
          if (Matching.logging) {
            System.out.println("Testing constructor " + con);
          }
          val rowSpec = r.specialize(con, 0, symlib, fringe)
          if (rowSpec.isDefined && specialize(con, 0, None)._2.useful(rowSpec.get)) {
            return true
          }
        }
        false
      } else {
        val rowColumn = new Column(columns(0).fringe, IndexedSeq(r.patterns(0)), IndexedSeq(r.clause))
        val rowSigma = rowColumn.signatureForUsefulness
        for (con <- rowSigma) {
          if (Matching.logging) {
            System.out.println("Testing constructor " + con);
          }
          val rowSpec = r.specialize(con, 0, symlib, fringe)
          if (rowSpec.isDefined && specialize(con, 0, None)._2.useful(rowSpec.get)) {
            return true
          }
        }
        val matrixDefault = default(0, rowSigma)
        val rowDefault = r.default(0, rowSigma, symlib, fringe, columns)
        matrixDefault.isDefined && rowDefault.isDefined && matrixDefault.get.useful(rowDefault.get)
      }
    }
  }

  def rowUseless(rowIx: Int): Boolean = {
    val filteredRows = rows.take(rowIx) ++ rows.drop(rowIx + 1).takeWhile(_.clause.action.priority == rows(rowIx).clause.action.priority)
    val withoutSC = filteredRows.filter(r => r.clause.action.scVars.isEmpty && !r.clause.action.nonlinear)
    val matrix = Matrix.fromRows(symlib, withoutSC, fringe)
    val row = rows(rowIx)
    val result = !matrix.useful(row)
    if (Matching.logging && result) {
      System.out.println("Row " + row.clause.action.ordinal + " is useless");
    }
    result
  }

  def nonExhaustive: Option[IndexedSeq[Pattern[String]]] = {
    if (fringe.isEmpty) {
      if (clauses.nonEmpty) {
        None
      } else {
        Some(IndexedSeq())
      }
    } else {
      val sigma = columns(0).signatureForUsefulness
      if (columns(0).category.hasIncompleteSignature(sigma, columns(0).fringe)) {
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
              System.out.println("Submatrix " + id + " is non-exhaustive:\n" + child.get.map(p => new util.Formatter().format("%12.12s", p.toShortString)).mkString(" "))
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
            System.out.println("Testing constructor " + con);
            System.out.println("-- Exhaustive --")
            System.out.println("Matrix " + id + ": ")
            System.out.println(this)
            Matrix.id += 1
          }
          val child = specialize(con, 0, None)._2.nonExhaustive
          if (child.isDefined) {
            if (Matching.logging) {
              System.out.println("Submatrix " + id + " is non-exhaustive:\n" + child.get.map(p => new util.Formatter().format("%12.12s", p.toShortString)).mkString(" "))
            }
            val arity = con.expand(fringe(0)).get.size
            assert(child.get.size - arity + 1 == fringe.size)
            return Some(con.contract(fringe(0), child.get.take(arity)) +: child.get.drop(arity))
          }
        }
        None
      }
    }
  }

  def checkUsefulness(kem: KException => Unit): Unit = {
    for (rowIx <- rows.indices) {
      if (rowUseless(rowIx)) {
        if (clauses(rowIx).action.source.isDefined && clauses(rowIx).action.location.isDefined) {
          kem(new KException(KException.ExceptionType.WARNING, KException.KExceptionGroup.COMPILER, "Potentially useless rule detected.", clauses(rowIx).action.source.get, clauses(rowIx).action.location.get))
        }
      }
    }
  }

  def checkExhaustiveness(name: SymbolOrAlias, kem: KException => Unit): Unit = {
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
        System.out.println("Matrix " + id + " is non-exhaustive:\n" + counterexample.get.map(p => new util.Formatter().format("%12.12s", p.toShortString)).mkString(" "))
      }
      val k = (fringe zip counterexample.get).map(t => t._2.toK(t._1))
      val func = KApply(symlib.koreToK(name), KList(k))
      kem(new KException(KException.ExceptionType.WARNING, KException.KExceptionGroup.COMPILER, "Non exhaustive match detected: " ++ ToKast(func)))
    }
  }


  def specializeBy(ps: IndexedSeq[Pattern[String]]): (Matrix, IndexedSeq[Pattern[String]]) = {
    def expandChildren(pat: Pattern[String]): IndexedSeq[Pattern[String]] = {
      pat match {
        case SymbolP(_, ps) => ps.toIndexedSeq
        case LiteralP(_, _) => IndexedSeq()
        case _ => ???
      }
    }
    def getConstructor(pat: Pattern[String]): Constructor = {
      pat match {
        case SymbolP(sym, _) => SymbolC(sym)
        case LiteralP(lit, _) => LiteralC(lit)
        case _ => ???
      }
    }
    specializing = Some(ps)
    if (Matching.logging) {
      System.out.println("Specializing by:\n" + ps.map(p => new util.Formatter().format("%12.12s", p.toShortString)).mkString(" "))
      System.out.println(toString)
      System.out.println("remaining: " + Matrix.remaining)
    }
    if (clauses.isEmpty || columns.indices.forall(i => isWildcardOrResidual(ps(i)))) {
      (this, ps)
    } else {
      val residual = ps(bestColIx)
      if (Matching.logging) {
        System.out.println("Chose column " + bestColIx);
      }
      val constructor = getConstructor(residual)
      val specialized = specialize(constructor, bestColIx, Some(residual))
      val args = expandChildren(residual)
      specialized._2.specializeBy(args ++ ps.patch(bestColIx, Nil, 1))
    }
  }

  def notBestRow: Matrix = {
    Matrix.fromRows(symlib, rows.patch(bestRowIx, Nil, 1), fringe)
  }

  def notBestCol(colIx: Int): IndexedSeq[Column] = {
    columns.patch(colIx, Nil, 1)
  }

  def notCol(colIx: Int): Matrix = {
    Matrix.fromColumns(symlib, notBestCol(colIx), clauses)
  }

  def colScoreString: String = {
    symlib.heuristics.map(h => columns.indices.map(c => "%12.2f".format(matrixColumns(c).computeScoreForKey(h, columns(c).bestKey))).mkString(" ")).mkString("\n")
  }

  def neededString: String = {
    matrixColumns.flatMap(col => matrixColumns.filter(c => !c.column.isValid && col.colIx != c.colIx && col.column.needed(c.column.keyVars)).map((col, _))).map(p => "Column " + p._2.colIx + " needs " + p._1.colIx).mkString("\n")
  }

  override def toString: String = fringe.map(_.occurrence.toString).mkString("\n") + "\n" + neededString + "\n" + fringe.map(_.toString).mkString(" ") + "\n" + columns.indices.map(i => validCols.map(_.colIx).contains(i)).map(v => "%12.12s".format(v.toString)).mkString(" ") + "\n" + colScoreString + "\n" + rows.map(_.toString).mkString("\n") + "\n"

  def canEqual(other: Any): Boolean = other.isInstanceOf[Matrix]

  override def equals(other: Any): Boolean = other match {
    case that: Matrix =>
      (that canEqual this) &&
        symlib == that.symlib &&
        fringe == that.fringe &&
        rows == that.rows
    case _ => false
  }

  override lazy val hashCode: Int = {
    val state = Seq(symlib, rows)
    state.map(_.hashCode()).foldLeft(0)((a, b) => 31 * a + b)
  }
}

object Matrix {
  var remaining = 0

  def fromRows(symlib: Parser.SymLib, rows: IndexedSeq[Row], fringe: IndexedSeq[Fringe]): Matrix = {
    new Matrix(symlib, null, rows, null, fringe)
  }

  def fromColumns(symlib: Parser.SymLib, cols: IndexedSeq[Column], clauses: IndexedSeq[Clause]): Matrix = {
    new Matrix(symlib, cols, null, clauses, null)
  }

  private val cache = new ConcurrentHashMap[Matrix, DecisionTree]()

  def clearCache: Unit = {
    cache.clear
  }

  var id = 0
}
