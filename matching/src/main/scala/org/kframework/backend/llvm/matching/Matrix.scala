package org.kframework.backend.llvm.matching

import org.kframework.parser.kore.{Sort,SymbolOrAlias}
import org.kframework.parser.kore.implementation.{DefaultBuilders => B}
import org.kframework.backend.llvm.matching.pattern._
import org.kframework.backend.llvm.matching.dt._
import java.util
import java.util.concurrent.ConcurrentHashMap

class Column(val fringe: Fringe, val patterns: IndexedSeq[Pattern[String]], val clauses: IndexedSeq[Clause]) {
  lazy val category: SortCategory = {
    val ps = patterns.map(_.category).filter(_.isDefined)
    if (ps.isEmpty) {
      SymbolS()
    } else {
      ps.head.get
    }
  }

  lazy val score: Double = computeScore
  
  def computeScore: Double = {
    if (isWildcard) {
      Double.PositiveInfinity
    } else {
      var result = 0.0
      for (i <- patterns.indices) {
        if (clauses(i).action.priority != clauses.head.action.priority)
          return result
        result += patterns(i).score(fringe, clauses(i), bestKey)
        if (result == Double.NegativeInfinity) {
          return result
        }
      }
      assert(!result.isNaN)
      result
    }
  }

  private lazy val boundVars: Seq[Set[String]] = patterns.map(_.variables)
  def needed(vars: Seq[Set[String]]): Boolean = {
    if (score < 0) {
      false
    } else {
      val intersection = (vars, boundVars).zipped.map(_.intersect(_))
      intersection.exists(_.nonEmpty)
    }
  }

  lazy val signature: List[Constructor] = {
    val used = patterns.zipWithIndex.flatMap(p => p._1.signature(clauses(p._2)))
    val bestUsed = bestKey match {
      case None => used
      case Some(k) => used.filter(_.isBest(k))
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

  def isChoice: Boolean = fringe.sortInfo.isCollection && bestKey == None

  def maxListSize: (Int, Int) = {
    val listPs = patterns.filter(_.isInstanceOf[ListP[String]]).map(_.asInstanceOf[ListP[String]])
    if (listPs.isEmpty) {
      (0, 0)
    } else {
      val longestHead = listPs.map(_.head.size).max
      val longestTail = listPs.map(_.tail.size).max
      (longestHead, longestTail)
    }
  }

  lazy val bestKey: Option[Pattern[Option[Occurrence]]] = {
    if (!fringe.sortInfo.isCollection) None
    else {
      def go: Option[Pattern[Option[Occurrence]]] = {
        for ((pat, clause, i) <- (patterns, clauses, clauses.indices).zipped) {
          if (!pat.isWildcard) {
            return pat.bestKey(fringe, clause, patterns.drop(i), clauses.drop(i))
          }
        }
        None
      }
      go
    }
  }

  def expand(ix: Constructor): IndexedSeq[Column] = {
    val fringes = fringe.expand(ix)
    val ps = (patterns, clauses).zipped.toIterable.map(t => t._1.expand(ix, fringes, fringe, t._2))
    (fringes, ps.transpose).zipped.toIndexedSeq.map(t => new Column(t._1, t._2.toIndexedSeq, clauses))
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
    val state = Seq(fringe, patterns)
    state.map(_.hashCode()).foldLeft(0)((a, b) => 31 * a + b)
  }
}

case class VariableBinding[T](val name: T, val category: SortCategory, val occurrence: Occurrence) {
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

  override def toString: String = new util.Formatter().format("%12.12s", sort.toString).toString
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

class SortInfo private(sort: Sort, symlib: Parser.SymLib) {
  val constructors = symlib.symbolsForSort.getOrElse(sort, Seq())
  private val rawInjections = constructors.filter(_.ctr == "inj")
  private val injMap = rawInjections.map(b => (b, rawInjections.filter(a => symlib.isSubsorted(a.params.head, b.params.head)))).toMap
  private val rawOverloads = constructors.filter(symlib.overloads.contains)
  private val overloadMap = rawOverloads.map(s => (s, symlib.overloads(s))).toMap
  private val overloadInjMap = overloadMap.map(e => (e._1, e._2.map(g => B.SymbolOrAlias("inj", Seq(symlib.signatures(g)._2, symlib.signatures(e._1)._2)))))
  val trueInjMap = injMap ++ overloadInjMap
  val category: SortCategory = SortCategory(Parser.getStringAtt(symlib.sortAtt(sort), "hook"))
  val length: Int = category.length(constructors.size)
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

case class Action(val ordinal: Int, val rhsVars: Seq[String], val scVars: Option[Seq[String]], val priority: Int) {
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
  val overloadChildren: Vector[(Constructor, VariableBinding[String])]) {

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

  def addVars(ix: Option[Constructor], pat: Pattern[String], f: Fringe): Clause = {
    new Clause(action, bindings ++ pat.bindings(ix, f.occurrence), listRanges ++ pat.listRange(ix, f.occurrence), overloadChildren ++ pat.overloadChildren(f, ix, Num(0, f.occurrence)))
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
    this(symlib, (cols, (1 to cols.size).map(i => new Fringe(symlib, cols(i - 1)._1, Num(i, Base()), false))).zipped.toIndexedSeq.map(pair => new Column(pair._2, pair._1._2, actions.map(new Clause(_, Vector(), Vector(), Vector())))), null, actions.map(new Clause(_, Vector(), Vector(), Vector())), null)
  }

  // compute the column with the best score, choosing the first such column if they are equal
  lazy val bestColIx: Int = {
    val best = columns.zipWithIndex.maxBy(_._1.score)
    if (best._1.score == 0.0) {
      val unboundMapColumns = columns.filter(col => col.score.isNegInfinity)
      val unboundPatterns = unboundMapColumns.map(_.patterns).transpose
      val keys = unboundPatterns.map(_.flatMap(_.mapOrSetKeys))
      val vars = keys.map(_.flatMap(_.variables).toSet)
      columns.zipWithIndex.find(_._1.needed(vars)).getOrElse(columns.head, 0)._2
    } else {
      best._2
    }
  }

  lazy val bestCol: Column = columns(bestColIx)

  lazy val sigma: List[Constructor] = bestCol.signature

  def specialize(ix: Constructor): (String, Matrix) = {
    val filtered = filterMatrix(Some(ix), (c, p) => p.isSpecialized(ix, bestCol.fringe, c))
    val expanded = Matrix.fromColumns(symlib, filtered.columns(bestColIx).expand(ix) ++ filtered.notBestCol(bestColIx), filtered.clauses)
    (ix.name, expanded)
  }

  def cases: List[(String, Matrix)] = sigma.map(specialize)

  lazy val compiledCases: Seq[(String, DecisionTree)] = {
    Matrix.remaining += sigma.length
    val result = cases.map(l => (l._1, l._2.compile))
    Matrix.remaining -= sigma.length
    result
  }

  def filterMatrix(ix: Option[Constructor], checkPattern: (Clause, Pattern[String]) => Boolean): Matrix = {
    val newRows = rows.filter(row => checkPattern(row.clause, row.patterns(bestColIx))).map(row => new Row(row.patterns, row.clause.addVars(ix, row.patterns(bestColIx), fringe(bestColIx))))
    Matrix.fromRows(symlib, newRows, fringe)
  }

  lazy val default: Option[Matrix] = {
    if (bestCol.category.hasIncompleteSignature(sigma, bestCol.fringe)) {
      val defaultConstructor = {
        if (sigma.contains(Empty())) Some(NonEmpty())
        else if (sigma.isEmpty) None
        else {
          lazy val (hd, tl) = bestCol.maxListSize
          sigma.head match {
            case ListC(sym,_) => Some(ListC(sym, hd + tl))
            case _ => None
          }
        }
      }
      val filtered = filterMatrix(defaultConstructor, (_, p) => p.isDefault)
      val expanded = if (defaultConstructor.isDefined) {
        if (bestCol.category.isExpandDefault) {
          Matrix.fromColumns(symlib, filtered.columns(bestColIx).expand(defaultConstructor.get) ++ filtered.notBestCol(bestColIx), filtered.clauses)
        } else {
          Matrix.fromColumns(symlib, filtered.notBestCol(bestColIx), filtered.clauses)
        }
      } else {
        Matrix.fromColumns(symlib, filtered.notBestCol(bestColIx), filtered.clauses)
      }
      Some(expanded)
    } else {
      None
    }
  }

  lazy val compiledDefault: Option[DecisionTree] = {
    Matrix.remaining += 1
    val result = default.map(_.compile)
    Matrix.remaining -= 1
    result
  }

  def getLeaf(row: Row, child: DecisionTree): DecisionTree = {
    def makeEquality(category: SortCategory, os: (Occurrence, Occurrence), dt: DecisionTree): DecisionTree = {
      Function(category.equalityFun, Equal(os._1, os._2), Seq(os._1, os._2), "BOOL.Bool",
        SwitchLit(Equal(os._1, os._2), 1, Seq(("1", dt), ("0", child)), None))
    }
    // first, add all remaining variable bindings to the clause
    val vars = row.clause.bindings ++ (fringe, row.patterns).zipped.toSeq.flatMap(t => t._2.bindings(None, t._1.occurrence))
    val overloadVars = row.clause.overloadChildren.map(_._2)
    val allVars = vars ++ overloadVars
    // then group the bound variables by their name
    val grouped = allVars.groupBy(v => (v.name, v.category)).mapValues(_.map(_.occurrence))
    // compute the variables bound more than once
    val nonlinear = grouped.filter(_._2.size > 1)
    val nonlinearPairs = nonlinear.mapValues(l => (l, l.tail).zipped)
    // get some random occurrence bound for each variable in the map
    val deduped = grouped.toSeq.map(e => (e._1._1, e._2.head))
    // sort alphabetically by variable name
    val sorted = deduped.sortWith(_._1 < _._1)
    // filter by the variables used in the rhs
    val filtered = sorted.filter(v => row.clause.action.rhsVars.contains(v._1))
    val newVars = filtered.map(_._2)
    val atomicLeaf = Leaf(row.clause.action.ordinal, newVars)
    // check that all occurrences of the same variable are equal
    val nonlinearLeaf = nonlinearPairs.foldRight[DecisionTree](atomicLeaf)((e, dt) => e._2.foldRight(dt)((os,dt2) => makeEquality(e._1._2, os, dt2)))
    val sc = row.clause.action.scVars match {
      // if there is no side condition, continue
      case None => nonlinearLeaf
      case Some(cond) =>
        // filter by the variables used in the side condition
        val condFiltered = sorted.filter(t => cond.contains(t._1))
        val condVars = condFiltered.map(_._2)
        val newO = SC(row.clause.action.ordinal)
        // evaluate the side condition and if it is true, continue, otherwise go to the next row
        Function("side_condition_" + row.clause.action.ordinal, newO, condVars, "BOOL.Bool",
          SwitchLit(newO, 1, Seq(("1", nonlinearLeaf), ("0", child)), None))
    }
    // fill out the bindings for list range variables
    val withRanges = row.clause.listRanges.foldRight(sc)({
      case ((o @ Num(_, o2), hd, tl), dt) => Function("hook_LIST_range_long", o, Seq(o2, Lit(hd.toString, "MINT.MInt 64"), Lit(tl.toString, "MINT.MInt 64")), "LIST.List", dt)
    })
    row.clause.overloadChildren.foldRight(withRanges)({
      case ((SymbolC(inj), v),dt) => MakePattern(v.occurrence, SymbolP(inj, Seq(VariableP(Some(v.occurrence.asInstanceOf[Inj].rest), v.category))), dt)
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
      System.out.println(toString)
      System.out.println("remaining: " + Matrix.remaining)
    }
    if (clauses.isEmpty)
      Failure()
    else {
      bestRowIx match {
        case -1 => 
          if (bestCol.score.isPosInfinity) {
            // decompose this column as it contains only wildcards
            val newClauses = (bestCol.clauses, bestCol.patterns).zipped.toIndexedSeq.map(t => t._1.addVars(None, t._2, bestCol.fringe))
            Matrix.fromColumns(symlib, notBestCol(bestColIx).map(c => new Column(c.fringe, c.patterns, newClauses)), newClauses).compile
          } else {
            // compute the sort category of the best column
            bestCol.category.tree(this)
          }
        case _ =>
          // if there is only one row left, then try to match it and fail the matching if it fails
          // otherwise, if it fails, try to match the remainder of hte matrix
          getLeaf(bestRow, notBestRow.compile)
      }
    }
  }

  def notBestRow: Matrix = {
    Matrix.fromRows(symlib, rows.patch(bestRowIx, Nil, 1), fringe)
  }

  def notBestCol(colIx: Int): IndexedSeq[Column] = {
    columns.patch(colIx, Nil, 1)
  }

  override def toString: String = fringe.map(_.toString).mkString(" ") + "\n" + rows.map(_.toString).mkString("\n") + "\n"

  def canEqual(other: Any): Boolean = other.isInstanceOf[Matrix]

  override def equals(other: Any): Boolean = other match {
    case that: Matrix =>
      (that canEqual this) &&
        symlib == that.symlib &&
        columns == that.columns &&
        rows == that.rows
    case _ => false
  }

  override lazy val hashCode: Int = {
    val state = Seq(symlib, columns, rows)
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
}
