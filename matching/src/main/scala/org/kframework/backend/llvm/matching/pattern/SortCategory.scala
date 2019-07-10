package org.kframework.backend.llvm.matching.pattern

import org.kframework.backend.llvm.matching._
import org.kframework.backend.llvm.matching.dt._

sealed trait SortCategory {
  def hookAtt: String
  def hasIncompleteSignature(sigma: Seq[Constructor], f: Fringe): Boolean
  def isExpandDefault: Boolean = false
  def equalityFun: String
  def tree(matrix: Matrix): DecisionTree
  def length(rawLength: Int): Int = rawLength
}

object SortCategory {
  def apply(hookAtt: Option[String]): SortCategory = {
    hookAtt match {
      case None => SymbolS()
      case Some("STRING.String") => StringS()
      case Some("BYTES.Bytes") => BytesS()
      case Some("LIST.List") => ListS()
      case Some("MAP.Map") => MapS()
      case Some("SET.Set") => SetS()
      case Some("FLOAT.Float") => FloatS()
      case Some("INT.Int") => IntS()
      case Some("BOOL.Bool") =>BoolS()
      case Some("KVAR.KVar") => VarS()
      case Some("BUFFER.StringBuffer") => BufferS()
    }
  }
}

case class SymbolS() extends SortCategory {
  def hookAtt = ???
  def hasIncompleteSignature(sigma: Seq[Constructor], f: Fringe): Boolean = sigma.isEmpty || sigma.contains(Empty()) || sigma.size != f.sortInfo.length
  def equalityFun = "hook_KEQUAL_eq"
  // not matching a builtin, therefore construct a regular switch
  // that matches the tag of the block.
  def tree(matrix: Matrix): DecisionTree = Switch(matrix.bestCol.fringe.occurrence, matrix.compiledCases, matrix.compiledDefault)
}
abstract class EqualLiteral() extends SortCategory {
  def hasIncompleteSignature(sigma: Seq[Constructor], f: Fringe): Boolean = true
  // matching a string or int, so compare the value of the token against a list of constants
  // constructs a tree to test the current occurrence against each possible match in turn
  def tree(matrix: Matrix): DecisionTree = {
    tree(matrix, matrix.cases)
  }

  private def tree(matrix: Matrix, ls: Seq[(String, Matrix)]): DecisionTree = {
    val litO = matrix.bestCol.fringe.occurrence
    val defaultMatrix = matrix.default(matrix.bestColIx, matrix.sigma)
    if (defaultMatrix.isDefined && ls.isEmpty) {
      // if no specializations remain and a default exists, consume the occurrence and continue with the default
      Switch(litO, Seq(), Some(defaultMatrix.get.compile))
    } else if (ls.isEmpty) {
      // if no specializations remain and no default exists, fail the match
      Failure()
    } else {
      // consume each specialization one at a time and try to match it
      // if it succeseds, consume the occurrence and continue with the specialized matrix
      // otherweise, test the next literal
      val newO = Lit(ls.head._1, hookAtt)
      val eqO = Equal(litO, newO)
      MakePattern(newO, LiteralP(ls.head._1, this),
        Function(equalityFun, eqO, Seq(litO, newO), "BOOL.Bool",
          SwitchLit(eqO, 1, Seq(("1", Switch(litO, Seq(), Some(ls.head._2.compile))),
                                ("0", tree(matrix, ls.tail))), None)))
    }
  }
}
case class StringS() extends EqualLiteral {
  def hookAtt = "STRING.String"
  def equalityFun = "hook_KEQUAL_eq"
}
case class BytesS() extends EqualLiteral {
  def hookAtt = "BYTES.Bytes"
  def equalityFun = "hook_KEQUAL_eq"
}
case class ListS() extends SortCategory {
  def hookAtt = "LIST.List"
  def hasIncompleteSignature(sigma: Seq[Constructor], f: Fringe): Boolean = true
  override def isExpandDefault = true
  def equalityFun = "hook_LIST_eq"

  // get each element of the list specified in the list pattern and bind it to the occurrences,
  // then compile the remaining matrix
  def expandListPattern(matrix: Matrix, listO: Occurrence, cons: ListC): DecisionTree = {
    (0 until cons.length).foldLeft(matrix.compile)((dt, i) => listGet(listO, None, dt, i))
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

  // get each element of the list and bind it to the occurrence, then compile the default matrix
  def expandListPatternDefault(matrix: Matrix, listO: Occurrence, maxList: (Int, Int)): DecisionTree = {
    val hd = maxList._1
    val tl = maxList._2
    (hd until hd+tl).foldLeft((0 until hd).foldLeft(matrix.compile)((dt, i) => listGet(listO, None, dt, i)))((dt, i) => listGet(listO, Some(maxList), dt, i))
  }

  // matching a list, so construct a node to decompose the list into its elements
  // construct a tree to test the length of the list and bind the elements of the list to their occurrences
  def tree(matrix: Matrix): DecisionTree = {
    val listO = matrix.bestCol.fringe.occurrence
    val newO = Size(listO)
    val maxList = matrix.bestCol.maxListSize
    // test the length of the list against the specializations of the matrix
    // if it succeeds, bind the occurrences and continue with the specialized matrix
    // otherwise, try the default case
    Function("hook_LIST_size_long", newO, Seq(listO), "MINT.MInt 64",
      SwitchLit(newO, 64, matrix.cases.zipWithIndex.map(l => (l._1._1, expandListPattern(l._1._2, listO, matrix.sigma(l._2).asInstanceOf[ListC]))), matrix.default(matrix.bestColIx, matrix.sigma).map(expandListPatternDefault(_, listO, maxList))))
  }
}
case class MapS() extends SortCategory {
  def hookAtt = "MAP.Map"
  def hasIncompleteSignature(sigma: Seq[Constructor], f: Fringe): Boolean = sigma.isEmpty || sigma.contains(Empty())
  override def isExpandDefault = true
  def equalityFun = "hook_MAP_eq"
  // matching a map, so construct a node to decompose the map by one of its elements
  def tree(matrix: Matrix): DecisionTree = {
    val mapO = matrix.bestCol.fringe.occurrence
    val newO = Size(mapO)
    // if Empty is in the signature, test whether the map is empty or not.
    if (matrix.sigma.contains(Empty())) {
      Function("hook_MAP_size_long", newO, Seq(mapO), "MINT.MInt 64",
        SwitchLit(newO, 64, matrix.compiledCases, matrix.compiledDefault))
    } else if (matrix.bestCol.isChoice) {
      val m = matrix.compiledCases.toMap
      MakeIterator("map_iterator", mapO,
        IterNext("map_iterator_next", mapO, Choice(mapO),
          CheckNull(Choice(mapO), Seq(("0", m("0")), ("1", 
            Function("hook_MAP_lookup", ChoiceValue(mapO), Seq(mapO, Choice(mapO)), "STRING.String",
              Function("hook_MAP_remove", ChoiceRem(mapO), Seq(mapO, Choice(mapO)), "MAP.Map",
                m("1"))))), None)))

    } else {
      // otherwise, get the best key and test whether the best key is in the map or not
      val key = matrix.bestCol.bestKey
      key match {
        case None => Switch(mapO, matrix.compiledCases, matrix.compiledDefault)
        case Some(k) =>
          MakePattern(newO, k,
            Function("hook_MAP_lookup_null", Value(k, mapO), Seq(mapO, newO), "STRING.String",
              Function("hook_MAP_remove", Rem(k, mapO), Seq(mapO, newO), "MAP.Map",
                CheckNull(Value(k, mapO), matrix.compiledCases, matrix.compiledDefault))))
      }
    }
  }
}
case class SetS() extends SortCategory {
  def hookAtt = "SET.Set"
  def hasIncompleteSignature(sigma: Seq[Constructor], f: Fringe): Boolean = sigma.isEmpty || sigma.contains(Empty())
  override def isExpandDefault = true
  def equalityFun = "hook_SET_eq"
  // matching a set, so construct a node to decompose the set by one of its elements
  def tree(matrix: Matrix): DecisionTree = {
    val setO = matrix.bestCol.fringe.occurrence
    val newO = Size(setO)
    // if Empty is in the signature, test whether the set is empty or not.
    if (matrix.sigma.contains(Empty())) {
      Function("hook_SET_size_long", newO, Seq(setO), "MINT.MInt 64",
        SwitchLit(newO, 64, matrix.compiledCases, matrix.compiledDefault))
    } else if (matrix.bestCol.isChoice) {
      val m = matrix.compiledCases.toMap
      MakeIterator("set_iterator", setO,
        IterNext("set_iterator_next", setO, Choice(setO),
          CheckNull(Choice(setO), Seq(("0", m("0")), ("1", 
            Function("hook_SET_remove", ChoiceRem(setO), Seq(setO, Choice(setO)), "SET.Set",
              m("1")))), None)))
    } else {
      // otherwise, get the best element and test whether the best element is in the set or not
      val key = matrix.bestCol.bestKey
      key match {
        case None => Switch(setO, matrix.compiledCases, matrix.compiledDefault)
        case Some(k) =>
          MakePattern(newO, k,
            Function("hook_SET_in", Value(k, setO), Seq(newO, setO), "BOOL.Bool",
              Function("hook_SET_remove", Rem(k, setO), Seq(setO, newO), "SET.Set",
                SwitchLit(Value(k, setO), 1, matrix.compiledCases, matrix.compiledDefault))))
      }
    }
  }
}
case class FloatS() extends EqualLiteral {
  def hookAtt = "FLOAT.Float"
  def equalityFun = "hook_FLOAT_trueeq"
}
case class IntS() extends EqualLiteral {
  def hookAtt = "INT.Int"
  def equalityFun = "hook_INT_eq"
}
case class BoolS() extends SortCategory {
  def hookAtt = "BOOL.Bool"
  def hasIncompleteSignature(sigma: Seq[Constructor], f: Fringe): Boolean = sigma.length != 2
  def equalityFun = "hook_BOOL_eq"
  // matching a bool, so match the integer value of the bool with a bitwidth of 1
  def tree(matrix: Matrix): DecisionTree = SwitchLit(matrix.bestCol.fringe.occurrence, 1, matrix.compiledCases, matrix.compiledDefault)
  override def length(rawLength: Int) = 2
}
case class VarS() extends EqualLiteral {
  def hookAtt = "KVAR.KVar"
  def equalityFun = "hook_STRING_eq"
}
case class BufferS() extends EqualLiteral {
  def hookAtt = "BUFFER.StringBuffer"
  def equalityFun = ???
}
case class MIntS(bitwidth: Int) extends SortCategory {
  def hookAtt = "MINT.MInt " + bitwidth
  def hasIncompleteSignature(sigma: Seq[Constructor], f: Fringe): Boolean = sigma.length != (1 << bitwidth)
  // matching an mint, so match the integer value of the mint with the specified bitwidth
  def tree(matrix: Matrix): DecisionTree = SwitchLit(matrix.bestCol.fringe.occurrence, bitwidth, matrix.compiledCases, matrix.compiledDefault)
  def equalityFun = ???
  override def length(rawLength: Int) = 1 << bitwidth
}
