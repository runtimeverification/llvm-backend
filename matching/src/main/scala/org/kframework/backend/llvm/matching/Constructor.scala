package org.kframework.backend.llvm.matching

import com.runtimeverification.k.kore.SymbolOrAlias
import org.kframework.backend.llvm.matching.pattern._
import scala.collection.immutable

sealed trait Constructor {
  def name: String
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean
  def expand(f: Fringe): Option[immutable.Seq[Fringe]]
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String]
}

case class Empty() extends Constructor {
  def name                                              = "0"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[immutable.Seq[Fringe]]  = Some(immutable.Seq())
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] = {
    val symbol = Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "unit").get
    f.sortInfo.category match {
      case SetS() => SetP(immutable.Seq(), None, symbol, SymbolP(symbol, immutable.Seq()))
      case MapS() =>
        MapP(immutable.Seq(), immutable.Seq(), None, symbol, SymbolP(symbol, immutable.Seq()))
      case _ => ???
    }
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class NonEmpty() extends Constructor {
  def name: String                                      = ???
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[immutable.Seq[Fringe]]  = Some(immutable.Seq(f))
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] = children.head
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class HasKey(
    cat: SortCategory,
    element: SymbolOrAlias,
    key: Option[Pattern[Option[Occurrence]]]
) extends Constructor {
  def name                                              = "1"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = key.isDefined && pat == key.get
  def expand(f: Fringe): Option[immutable.Seq[Fringe]] = {
    val sorts = f.symlib.signatures(element)._1
    key match {
      case None =>
        cat match {
          case SetS() =>
            Some(
              immutable.Seq(
                Fringe(f.symlib, sorts.head, Choice(f.occurrence), isExact = false),
                Fringe(f.symlib, f.sort, ChoiceRem(f.occurrence), isExact = false)
              )
            )
          case MapS() =>
            Some(
              immutable.Seq(
                Fringe(f.symlib, sorts.head, Choice(f.occurrence), isExact = false),
                Fringe(f.symlib, sorts(1), ChoiceValue(f.occurrence), isExact = false),
                Fringe(f.symlib, f.sort, ChoiceRem(f.occurrence), isExact = false)
              )
            )
          case ListS() =>
            Some(
              immutable.Seq(
                Fringe(f.symlib, sorts(1), Choice(f.occurrence), isExact = false),
                Fringe(f.symlib, sorts(2), ChoiceValue(f.occurrence), isExact = false),
                Fringe(f.symlib, f.sort, ChoiceRem(f.occurrence), isExact = false)
              )
            )
          case _ => ???
        }
      case Some(k) =>
        cat match {
          case SetS() =>
            Some(immutable.Seq(Fringe(f.symlib, f.sort, Rem(k, f.occurrence), isExact = false), f))
          case MapS() =>
            Some(
              immutable.Seq(
                Fringe(f.symlib, sorts(1), Value(k, f.occurrence), isExact = false),
                Fringe(f.symlib, f.sort, Rem(k, f.occurrence), isExact = false),
                f
              )
            )
          case ListS() =>
            Some(
              immutable.Seq(
                Fringe(f.symlib, sorts(2), Value(k, f.occurrence), isExact = false),
                Fringe(f.symlib, f.sort, f.occurrence, isExact = false),
                f
              )
            )
          case _ => ???
        }
    }
  }
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] = {
    val child                  = children.last
    var key: Pattern[String]   = null
    var value: Pattern[String] = null
    assert((cat == SetS() && children.size == 2) || (cat != SetS() && children.size == 3))
    if (this.key.isEmpty) {
      cat match {
        case SetS() =>
          key = children.head
        case MapS() =>
          key = children.head
          value = children(1)
        case _ => ???
      }
    } else {
      cat match {
        case SetS() =>
          key = this.key.get.decanonicalize
        case ListS() | MapS() =>
          key = this.key.get.decanonicalize
          value = children.head
        case _ => ???
      }
    }
    def element(k: Pattern[String], v: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(k, v))
    def setElement(k: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(k))
    def concat(m1: Pattern[String], m2: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "concat").get, immutable.Seq(m1, m2))
    def update(m1: Pattern[String], m2: Pattern[String], m3: Pattern[String]): Pattern[String] =
      SymbolP(
        Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "update").get,
        immutable.Seq(m1, m2, m3)
      )
    child match {
      case MapP(keys, values, frame, ctr, orig) =>
        MapP(key +: keys, value +: values, frame, ctr, orig)
      case ListGetP(keys, values, frame, ctr, orig) =>
        ListGetP(key +: keys, value +: values, frame, ctr, orig)
      case SetP(elems, frame, ctr, orig) =>
        SetP(key +: elems, frame, ctr, orig)
      case WildcardP() | VariableP(_, _) =>
        cat match {
          case SetS() =>
            SetP(
              immutable.Seq(key),
              Some(child),
              Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
              concat(setElement(key), child)
            )
          case MapS() =>
            MapP(
              immutable.Seq(key),
              immutable.Seq(value),
              Some(child),
              Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
              concat(element(key, value), child)
            )
          case ListS() =>
            ListGetP(
              immutable.Seq(key),
              immutable.Seq(value),
              child,
              Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "update").get,
              update(child, key, value)
            )
          case _ => ???
        }
      case _ => ???
    }
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class HasNoKey(cat: SortCategory, key: Option[Pattern[Option[Occurrence]]])
    extends Constructor {
  def name                                              = "0"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = key.isDefined && pat == key.get
  def expand(f: Fringe): Option[immutable.Seq[Fringe]]  = Some(immutable.Seq(f))
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] = {
    val child = children.head
    def element(k: Pattern[String], v: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(k, v))
    def setElement(k: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(k))
    val unit: Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "unit").get, immutable.Seq())
    def concat(m1: Pattern[String], m2: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "concat").get, immutable.Seq(m1, m2))
    def update(m1: Pattern[String], m2: Pattern[String], m3: Pattern[String]): Pattern[String] =
      SymbolP(
        Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "update").get,
        immutable.Seq(m1, m2, m3)
      )
    def wildcard = WildcardP[String]()
    child match {
      case MapP(keys, values, frame, ctr, orig) =>
        MapP(
          wildcard +: keys,
          wildcard +: values,
          frame,
          ctr,
          concat(element(wildcard, wildcard), orig)
        )
      case SetP(elems, frame, ctr, orig) =>
        SetP(wildcard +: elems, frame, ctr, concat(setElement(wildcard), orig))
      case WildcardP() | VariableP(_, _) =>
        cat match {
          case SetS() =>
            SetP(
              immutable.Seq(wildcard),
              Some(child),
              Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
              concat(setElement(wildcard), child)
            )
          case MapS() =>
            MapP(
              immutable.Seq(wildcard),
              immutable.Seq(wildcard),
              Some(child),
              Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
              concat(element(wildcard, wildcard), child)
            )
          case ListS() =>
            ListGetP(
              immutable.Seq(wildcard),
              immutable.Seq(wildcard),
              child,
              Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "update").get,
              update(child, wildcard, wildcard)
            )
          case _ => ???
        }
      case _ => ???
    }
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class ListC(element: SymbolOrAlias, length: Int) extends Constructor {
  def name: String                                      = length.toString
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[immutable.Seq[Fringe]] = {
    val sort = f.symlib.signatures(element)._1.head
    Some((0 until length).map(i => Fringe(f.symlib, sort, Num(i, f.occurrence), isExact = false)))
  }
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] = {
    def element(v: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(v))
    val unit: Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "unit").get, immutable.Seq())
    def concat(m1: Pattern[String], m2: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "concat").get, immutable.Seq(m1, m2))
    ListP(
      children,
      None,
      immutable.Seq(),
      Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
      children.map(element).fold(unit)(concat)
    )
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class SymbolC(sym: SymbolOrAlias) extends Constructor {
  def name: String                                      = sym.toString
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[immutable.Seq[Fringe]] =
    if (f.symlib.signatures(sym)._2 != f.sort) {
      None
    } else {
      val sorts = f.symlib.signatures(sym)._1
      Some(
        sorts.zipWithIndex.map(t =>
          Fringe(f.symlib, t._1, Num(t._2, f.occurrence), sym.ctr == "inj")
        )
      )
    }
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] =
    SymbolP(sym, children)
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class LiteralC(literal: String) extends Constructor {
  def name: String                                      = literal
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[immutable.Seq[Fringe]]  = Some(immutable.Seq())
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] =
    LiteralP(literal, f.sortInfo.category)
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
