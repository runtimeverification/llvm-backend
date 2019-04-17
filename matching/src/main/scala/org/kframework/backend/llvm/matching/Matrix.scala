package org.kframework.backend.llvm.matching

import org.kframework.parser.kore.{Sort,SymbolOrAlias}
import org.kframework.parser.kore.implementation.{DefaultBuilders => B}
import org.kframework.backend.llvm.matching.pattern._
import org.kframework.backend.llvm.matching.dt._

class Column(val fringe: Fringe, val patterns: Seq[Pattern[String]]) {
  def hookAtt: Option[String] = {
    val ps = patterns.map(_.hookAtt).filter(_.isDefined)
    if (ps.isEmpty) {
      None
    } else {
      ps.head
    }
  }

  private def rawScore(clauses: Seq[Clause]): Double = {
    val raw = patterns(0).score(fringe, clauses(0), patterns.tail, clauses.tail)
    assert(!raw.isNaN)
    raw
  }

  def score(matrix: Matrix): Double = {
    val raw = rawScore(matrix.clauses)
    if (raw != 0.0) {
      raw
    } else {
      val unboundMapColumns = matrix.columns.filter(col => col.rawScore(matrix.clauses).isNegInfinity)
      val unboundPatterns = unboundMapColumns.map(_.patterns).transpose
      val keys = unboundPatterns.map(_.flatMap(_.mapOrSetKeys))
      val vars = keys.map(_.flatMap(_.variables)).toSet
      val boundVars = patterns.map(_.variables).toSet
      val intersection = (vars, boundVars).zipped.map(_.intersect(_))
      val needed = intersection.exists(!_.isEmpty)
      if (needed) {
        Double.MinPositiveValue
      } else {
        0.0
      }
    }
  }

  def signature(clauses: Seq[Clause]): Seq[Constructor] = {
    val used = patterns.zipWithIndex.flatMap(p => p._1.signature(clauses(p._2)))
    val bestUsed = bestKey(clauses) match {
      case None => used
      case Some(k) => used.filter(_.isBest(k))
    }
    val usedInjs = bestUsed.flatMap(fringe.injections(_))
    val dups = if (fringe.isExact) bestUsed else bestUsed ++ usedInjs
    val nodups = dups.distinct
    if (nodups.contains(Empty())) {
      Seq(Empty())
    } else {
      nodups.filter(_ != Empty())
    }
  }

  def maxListSize: (Int, Int) = {
    val listPs = patterns.filter(_.isInstanceOf[ListP[String]]).map(_.asInstanceOf[ListP[String]])
    val longestHead = listPs.map(_.head.size).max
    val longestTail = listPs.map(_.tail.size).max
    (longestHead, longestTail)
  }

  def bestKey(clauses: Seq[Clause]): Option[Pattern[Option[Occurrence]]] = {
    for ((pat, clause, i) <- (patterns, clauses, 0 until clauses.size).zipped) {
      if (!pat.isWildcard) {
        return pat.bestKey(fringe, clause, patterns.drop(i), clauses.drop(i))
      }
    }
    None
  }

  def expand(ix: Constructor, clauses: Seq[Clause]): Seq[Column] = {
    val fringes = fringe.expand(ix)
    val ps = (patterns, clauses).zipped.toIterable.map(t => t._1.expand(ix, fringes, fringe, t._2))
    (fringes, ps.transpose).zipped.toSeq.map(t => new Column(t._1, t._2.toSeq))
  }
}

class VariableBinding[T](val name: T, val hook: String, val occurrence: Occurrence) {}

class Fringe(val symlib: Parser.SymLib, val sort: Sort, val occurrence: Occurrence, val isExact: Boolean) { 
  private lazy val constructors = symlib.symbolsForSort.getOrElse(sort, Seq())
  private lazy val rawInjections = constructors.filter(_.ctr == "inj")
  private lazy val injMap = rawInjections.map(b => (b, rawInjections.filter(a => isSubsorted(a.params(0), b.params(0))))).toMap
  private lazy val rawOverloads = constructors.filter(symlib.overloads.contains(_))
  private lazy val overloadMap = rawOverloads.map(s => (s, symlib.overloads(s))).toMap
  private lazy val overloadInjMap = overloadMap.map(e => (e._1, e._2.map(g => B.SymbolOrAlias("inj", Seq(symlib.signatures(g)._2, symlib.signatures(e._1)._2)))))
  private lazy val trueInjMap = injMap ++ overloadInjMap

  lazy val hookAtt = Parser.getStringAtt(symlib.sortAtt(sort), "hook")

  lazy val length: Int = {
    hookAtt match {
      case Some("BOOL.Bool") => 2
      case Some("MINT.MInt") => ???
      case _ => constructors.size
    }
  }

  def overloads(sym: SymbolOrAlias): Seq[SymbolOrAlias] = {
    symlib.overloads.getOrElse(sym, Seq())
  }

  def isSubsorted(less: Sort, greater: Sort): Boolean = {
    symlib.signatures.contains(B.SymbolOrAlias("inj",Seq(less,greater)))
  }

  def injections(ix: Constructor): Seq[Constructor] = {
    ix match {
      case SymbolC(sym) => {
        if (symlib.overloads.contains(sym) ||sym.ctr == "inj") {
          trueInjMap(sym).map(SymbolC(_))
        } else {
          Seq()
        }
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
    ix match {
      case SymbolC(sym) => {
        if (symlib.signatures(sym)._2 != sort) {
          None
        } else {
          val sorts = symlib.signatures(sym)._1
          Some(sorts.zipWithIndex.map(t => new Fringe(symlib, t._1, Num(t._2, occurrence), sym.ctr == "inj")))
        }
      }
      case LiteralC(_) => Some(Seq())
      case ListC(sym, len) => {
        val sort = symlib.signatures(sym)._1(0)
        Some((0 until len).map(i => new Fringe(symlib, sort, Num(i, occurrence), false)))
      }
      case Empty() => Some(Seq())
      case NonEmpty() => Some(Seq(new Fringe(symlib, sort, occurrence, isExact)))
      case HasKey(_, _, None) => ???
      case HasKey(isSet, sym, Some(key)) => {
        val sorts = symlib.signatures(sym)._1
        if (isSet) {
          Some(Seq(new Fringe(symlib, sort, Rem(key, occurrence), false), this))
        } else {
          Some(Seq(new Fringe(symlib, sorts(1), Value(key, occurrence), false), new Fringe(symlib, sort, Rem(key, occurrence), false), this))
        }
    }
      case HasNoKey(key) => {
        Some(Seq(this))
      }
    }
  }
}

class Action(val ordinal: Int, val rhsVars: Seq[String], val scVars: Option[Seq[String]]) {}

class Clause(
  // the rule to be applied if this row succeeds
  val action: Action,
  // the variable bindings made so far while matching this row
  val bindings: Seq[VariableBinding[String]],
  // the length of the head and tail of any list patterns
  // with frame variables bound so far in this row
  val listRanges: Seq[(Occurrence, Int, Int)],
  // variable bindings to injections that need to be constructed
  // since they do not actually exist in the original subject term
  val overloadChildren: Seq[(Constructor, VariableBinding[String])]) {

  lazy val bindingsMap = bindings.groupBy(_.name).mapValues(_(0))
  lazy val boundOccurrences = bindings.map(_.occurrence).toSet

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
}

class Row(val patterns: Seq[Pattern[String]], val clause: Clause) {
  // returns whether the row is done matching
  def isWildcard: Boolean = patterns.forall(_.isWildcard)

  def expand(colIx: Int): Seq[Row] = {
    val p0s = patterns(colIx).expandOr
    p0s.map(p => new Row(patterns.take(colIx) ++ Seq(p) ++ patterns.takeRight(patterns.size - colIx - 1), clause))
  }
}

class Matrix private(val symlib: Parser.SymLib, private val rawColumns: Seq[Column], private val rawRows: Seq[Row], private val rawClauses: Seq[Clause], private val rawFringe: Seq[Fringe], expanded: Boolean) {

  lazy val columns: Seq[Column] = {
    if (rawColumns != null) {
      rawColumns
    } else if (rows.isEmpty) {
      fringe.map(f => new Column(f, Seq()))
    } else {
      val ps = rows.map(_.patterns).transpose
      (fringe, ps).zipped.toSeq.map(col => new Column(col._1, col._2))
    }
  }

  lazy val rows: Seq[Row] = {
    if (rawRows != null) {
      rawRows
    } else if (columns.isEmpty) {
      clauses.map(clause => new Row(Seq(), clause))
    } else {
      val ps = columns.map(_.patterns).transpose
      (ps, clauses).zipped.toSeq.map(row => new Row(row._1, row._2))
    }
  }

  lazy val clauses: Seq[Clause] = {
    if (rawClauses != null) {
      rawClauses
    } else {
      rows.map(_.clause)
    }
  }

  lazy val fringe: Seq[Fringe] = {
    if (rawFringe != null) {
      rawFringe
    } else {
      columns.map(_.fringe)
    }
  }

  def this(symlib: Parser.SymLib, cols: Seq[Tuple2[Sort, Seq[Pattern[String]]]], actions: Seq[Action]) {
    this(symlib, (cols, (1 to cols.size).map(i => new Fringe(symlib, cols(i-1)._1, Num(i, Base()), false)).toSeq).zipped.toSeq.map(pair => new Column(pair._2, pair._1._2)), null, actions.map(new Clause(_, Seq(), Seq(), Seq())), null, false)
  }

  // compute the column with the best score, choosing the first such column if they are equal
  lazy val bestColIx: Int = {
    columns.zipWithIndex.maxBy(_._1.score(this))._2
  }

  def bestCol: Column = columns(bestColIx)

  lazy val sigma: Seq[Constructor] = bestCol.signature(clauses)

  def specialize(ix: Constructor): (String, Matrix) = {
    val filtered = filterMatrix(Some(ix), (c, p) => p.isSpecialized(ix, bestCol.fringe, c))
    val expanded = Matrix.fromColumns(symlib, filtered.columns(bestColIx).expand(ix, filtered.clauses) ++ filtered.notBestCol(bestColIx), filtered.clauses)
    (ix.name, expanded)
  }

  def cases: Seq[(String, Matrix)] = sigma.map(specialize(_))

  def compiledCases: Seq[(String, DecisionTree)] = cases.map(l => (l._1, l._2.compile))

  def filterMatrix(ix: Option[Constructor], checkPattern: (Clause, Pattern[String]) => Boolean): Matrix = {
    val newRows = rows.filter(row => checkPattern(row.clause, row.patterns(bestColIx))).map(row => new Row(row.patterns, row.clause.addVars(ix, row.patterns(bestColIx), columns(bestColIx).fringe)))
    Matrix.fromRows(symlib, newRows, fringe, false)
  }

  def hasIncompleteSignature: Boolean = {
    val infiniteLength = bestCol.hookAtt match {
      case None => false
      case Some("LIST.List") => true
      case Some("INT.Int") => true
      case Some("STRING.String") => true
      case Some(_) => false
    }
    val isMapOrSet = bestCol.hookAtt match {
      case None => false
      case Some("MAP.Map") => true
      case Some("SET.Set") => true
      case Some(_) => false
    }
    infiniteLength || sigma.isEmpty || sigma.contains(Empty()) || (!isMapOrSet && sigma.size != bestCol.fringe.length)
  }

  lazy val default: Option[Matrix] = {
    if (hasIncompleteSignature) {
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
      val filtered = filterMatrix(defaultConstructor, (c, p) => p.isDefault)
      val expanded = if (defaultConstructor.isDefined) {
        bestCol.hookAtt match {
          case Some("LIST.List") | Some("MAP.Map") | Some("SET.Set") =>
            Matrix.fromColumns(symlib, filtered.columns(bestColIx).expand(defaultConstructor.get, filtered.clauses) ++ filtered.notBestCol(bestColIx), filtered.clauses)
          case _ => Matrix.fromColumns(symlib, filtered.notBestCol(bestColIx), filtered.clauses)
        }
      } else {
        Matrix.fromColumns(symlib, filtered.notBestCol(bestColIx), filtered.clauses)
      }
      Some(expanded)
    } else {
      None
    }
  }

  def compiledDefault: Option[DecisionTree] = default.map(_.compile)

  def getLeaf(row: Row, child: DecisionTree): DecisionTree = {
    def makeEquality(hookAtt: String, os: (Occurrence, Occurrence), dt: DecisionTree): DecisionTree = {
      Function(equalityFun(hookAtt), Equal(os._1, os._2), Seq(os._1, os._2), "BOOL.Bool",
        SwitchLit(Equal(os._1, os._2), 1, Seq(("1", dt), ("0", child)), None))
    }
    // first, add all remaining variable bindings to the clause
    val vars = row.clause.bindings ++ columns.flatMap(col => col.patterns(0).bindings(None, col.fringe.occurrence))
    val overloadVars = row.clause.overloadChildren.map(_._2)
    val allVars = vars ++ overloadVars
    // then group the bound variables by their name
    val grouped = allVars.groupBy(v => (v.name, v.hook)).mapValues(_.map(_.occurrence))
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
      case Some(cond) => {
        // filter by the variables used in the side condition
        val condFiltered = sorted.filter(t => cond.contains(t._1))
        val condVars = condFiltered.map(_._2)
        val newO = SC(row.clause.action.ordinal)
        // evaluate the side condition and if it is true, continue, otherwise go to the next row
        Function("side_condition_" + row.clause.action.ordinal, newO, condVars, "BOOL.Bool",
          SwitchLit(newO, 1, Seq(("1", nonlinearLeaf), ("0", child)), None))
      }
    }
    // fill out the bindings for list range variables
    val withRanges = row.clause.listRanges.foldRight(sc)({
      case ((o @ Num(_, o2), hd, tl), dt) => Function("hook_LIST_range_long", o, Seq(o2, Lit(hd.toString, "MINT.MInt 64"), Lit(tl.toString, "MINT.MInt 64")), "LIST.List", dt)
    })
    row.clause.overloadChildren.foldRight(withRanges)({
      case ((SymbolC(inj), v),dt) => MakePattern(v.occurrence, SymbolP(inj, Seq(VariableP(Some(v.occurrence.asInstanceOf[Inj].rest), v.hook))), dt)
    })
  }

  def equalityFun(hook: String): String = {
    hook match {
      case "BOOL.Bool" => "hook_BOOL_eq"
      case "FLOAT.Float" => "hook_FLOAT_trueeq"
      case "INT.Int" => "hook_INT_eq"
      case "STRING.String" => "hook_KEQUAL_eq"
      case "MAP.Map" => "hook_MAP_eq"
      case "KVAR.KVar" =>"hook_STRING_eq"
    }
  }

  // constructs a tree to test the current occurrence against each possible match in turn
  def equalLiteral(hookName: String, ls: Seq[(String, Matrix)]): DecisionTree = {
    val litO = bestCol.fringe.occurrence
    if (default.isDefined && ls.isEmpty) {
      // if no specializations remain and a default exists, consume the occurrence and continue with the default
      Switch(litO, Seq(), Some(default.get.compile))
    } else if (ls.isEmpty) {
      // if no specializations remain and no default exists, fail the match
      Failure()
    } else {
      // consume each specialization one at a time and try to match it
      // if it succeseds, consume the occurrence and continue with the specialized matrix
      // otherweise, test the next literal
      val newO = Lit(ls.head._1, hookName)
      val eqO = Equal(litO, newO)
      MakePattern(newO, LiteralP(ls.head._1, hookName),
        Function(equalityFun(hookName), eqO, Seq(litO, newO), "BOOL.Bool",
          SwitchLit(eqO, 1, Seq(("1", Switch(litO, Seq(), Some(ls.head._2.compile))),
                                ("0", equalLiteral(hookName, ls.tail))), None)))
    }
  }

  // generate a single list lookup operation to bind one element of the list against its occurrence
  def listGet(listO: Occurrence, len: Option[(Int, Int)], dt: DecisionTree, i: Int): DecisionTree = {
    if (dt.isInstanceOf[Failure]) {
      dt
    } else {
      Function("hook_LIST_get_long", Num(i, listO), Seq(listO, len match {
        case None => Lit(i.toString, "MINT.MInt 64")
        case Some((hd, tl)) => Lit((i-tl-hd).toString, "MINT.MInt 64")
      }), "STRING.String", dt)
    }
  }

  // get each element of the list specified in the list pattern and bind it to the occurrences,
  // then compile the remaining matrix
  def expandListPattern(listO: Occurrence, cons: ListC): DecisionTree = {
    (0 until cons.length).foldLeft(compile)((dt, i) => listGet(listO, None, dt, i))
  }

  // get each element of the list and bind it to the occurrence, then compile the default matrix
  def expandListPatternDefault(listO: Occurrence, maxList: (Int, Int)): DecisionTree = {
    val hd = maxList._1
    val tl = maxList._2
    (hd until hd+tl).foldLeft((0 until hd).foldLeft(compile)((dt, i) => listGet(listO, None, dt, i)))((dt, i) => listGet(listO, Some(maxList), dt, i))
  }

  // construct a tree to test the length of the list and bind the elements of the list to their occurrences
  def listPattern: DecisionTree = {
    val listO = bestCol.fringe.occurrence
    val newO = Size(listO)
    val maxList = bestCol.maxListSize
    // test the length of the list against the specializations of the matrix
    // if it succeeds, bind the occurrences and continue with the specialized matrix
    // otherwise, try the default case
    Function("hook_LIST_size_long", newO, Seq(listO), "MINT.MInt 64",
      SwitchLit(newO, 64, cases.zipWithIndex.map(l => (l._1._1, l._1._2.expandListPattern(listO, sigma(l._2).asInstanceOf[ListC]))), default.map(_.expandListPatternDefault(listO, maxList))))
  }

  def mapPattern: DecisionTree = {
    val mapO = bestCol.fringe.occurrence
    val newO = Size(mapO)
    // if Empty is in the signature, test whether the map is empty or not.
    if (sigma.contains(Empty())) {
      Function("hook_MAP_size_long", newO, Seq(mapO), "MINT.MInt 64",
        SwitchLit(newO, 64, compiledCases, compiledDefault))
    } else {
      // otherwise, get the best key and test whether the best key is in the map or not
      val key = bestCol.bestKey(clauses)
      key match {
        case None => Switch(mapO, compiledCases, compiledDefault)
        case Some(k) => {
          MakePattern(newO, k,
            Function("hook_MAP_lookup_null", Value(k, mapO), Seq(mapO, newO), "STRING.String",
              Function("hook_MAP_remove", Rem(k, mapO), Seq(mapO, newO), "MAP.Map",
                CheckNull(Value(k, mapO), compiledCases, compiledDefault))))
        }
      }
    }
  }

  def setPattern: DecisionTree = {
    val setO = bestCol.fringe.occurrence
    val newO = Size(setO)
    // if Empty is in the signature, test whether the set is empty or not.
    if (sigma.contains(Empty())) {
      Function("hook_SET_size_long", newO, Seq(setO), "MINT.MInt 64",
        SwitchLit(newO, 64, compiledCases, compiledDefault))
    } else {
      // otherwise, get the best element and test whether the best element is in the set or not
      val key = bestCol.bestKey(clauses)
      key match {
        case None => Switch(setO, compiledCases, compiledDefault)
        case Some(k) => {
          MakePattern(newO, k,
            Function("hook_SET_in", Value(k, setO), Seq(newO, setO), "BOOL.Bool",
              Function("hook_SET_remove", Rem(k, setO), Seq(setO, newO), "SET.Set",
                SwitchLit(Value(k, setO), 1, compiledCases, compiledDefault))))
        }
      }
    }
  }

  def expand: Matrix = {
    if (columns.isEmpty) {
      new Matrix(symlib, rawColumns, rawRows, rawClauses, rawFringe, true)
    } else {
      Matrix.fromRows(symlib, rows.flatMap(_.expand(bestColIx)), fringe, true)
    }
  }

  def compile: DecisionTree = {
    if (!expanded) {
      return expand.compile
    }
    if (rows.isEmpty)
      Failure()
    else {
      if (rows(0).isWildcard) {
        // if there is only one row left, then try to match it and fail the matching if it fails
        // otherwise, if it fails, try to match the remainder of hte matrix
        getLeaf(rows(0), notFirstRow.compile)
      } else {
        // compute the hook attribute of the best column
        bestCol.hookAtt match {
          // not matching a builtin, therefore construct a regular switch
          // that matches the tag of the block.
          case None => Switch(bestCol.fringe.occurrence, compiledCases, compiledDefault)
          // matching a bool, so match the integer value of the bool with a bitwidth of 1
          case Some("BOOL.Bool") => SwitchLit(bestCol.fringe.occurrence, 1, compiledCases, compiledDefault)
          // matching a list, so construct a node to decompose the list into its elements
          case Some("LIST.List") => listPattern
          // matching a map, so construct a node to decompose the map by one of its elements
          case Some("MAP.Map") => mapPattern
          // matching a set, so construct a node to decompose the set by one of its elements
          case Some("SET.Set") => setPattern
          // matching an mint, so match the integer value of the mint with the specified bitwidth
          case Some(s) if s.startsWith("MINT.MInt ") => SwitchLit(bestCol.fringe.occurrence, s.substring(10).toInt, compiledCases, compiledDefault)
          // matching a string or int, so compare the value of the token against a list of constants
          case Some(hookName) => equalLiteral(hookName, cases)
        }
      }
    }
  }

  def notFirstRow: Matrix = {
    Matrix.fromRows(symlib, rows.tail, fringe, false)
  }

  def notBestCol(colIx: Int): Seq[Column] = {
    columns.take(colIx) ++ columns.takeRight(columns.size - colIx - 1)
  }
}

object Matrix {
  def fromRows(symlib: Parser.SymLib, rows: Seq[Row], fringe: Seq[Fringe], expanded: Boolean): Matrix = {
    new Matrix(symlib, null, rows, null, fringe, expanded)
  }

  def fromColumns(symlib: Parser.SymLib, cols: Seq[Column], clauses: Seq[Clause]): Matrix = {
    new Matrix(symlib, cols, null, clauses, null, false)
  }
}
