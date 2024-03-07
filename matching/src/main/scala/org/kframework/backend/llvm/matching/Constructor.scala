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
      case _      => ???
    }
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class NonEmpty() extends Constructor {
  def name: String                                      = ???
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[immutable.Seq[Fringe]]  = Some(immutable.Seq(f))
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] = children(0)
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class HasKey(isSet: Boolean, element: SymbolOrAlias, key: Option[Pattern[Option[Occurrence]]])
    extends Constructor {
  def name                                              = "1"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = key.isDefined && pat == key.get
  def expand(f: Fringe): Option[immutable.Seq[Fringe]] = {
    val sorts = f.symlib.signatures(element)._1
    key match {
      case None =>
        if (isSet) {
          Some(
            immutable.Seq(
              new Fringe(f.symlib, sorts(0), Choice(f.occurrence), false),
              new Fringe(f.symlib, f.sort, ChoiceRem(f.occurrence), false)
            )
          )
        } else {
          Some(
            immutable.Seq(
              new Fringe(f.symlib, sorts(0), Choice(f.occurrence), false),
              new Fringe(f.symlib, sorts(1), ChoiceValue(f.occurrence), false),
              new Fringe(f.symlib, f.sort, ChoiceRem(f.occurrence), false)
            )
          )
        }
      case Some(k) =>
        if (isSet) {
          Some(immutable.Seq(new Fringe(f.symlib, f.sort, Rem(k, f.occurrence), false), f))
        } else {
          Some(
            immutable.Seq(
              new Fringe(f.symlib, sorts(1), Value(k, f.occurrence), false),
              new Fringe(f.symlib, f.sort, Rem(k, f.occurrence), false),
              f
            )
          )
        }
    }
  }
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] = {
    val child                  = children.last
    var key: Pattern[String]   = null
    var value: Pattern[String] = null
    assert((isSet && children.size == 2) || (!isSet && children.size == 3))
    if (this.key.isEmpty) {
      if (isSet) {
        key = children(0)
      } else {
        key = children(0)
        value = children(1)
      }
    } else {
      if (isSet) {
        key = this.key.get.decanonicalize
      } else {
        key = this.key.get.decanonicalize
        value = children(0)
      }
    }
    def element(k: Pattern[String], v: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(k, v))
    def setElement(k: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(k))
    def concat(m1: Pattern[String], m2: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "concat").get, immutable.Seq(m1, m2))
    child match {
      case MapP(keys, values, frame, ctr, orig) =>
        MapP(key +: keys, value +: values, frame, ctr, orig)
      case SetP(elems, frame, ctr, orig) =>
        SetP(key +: elems, frame, ctr, orig)
      case WildcardP() | VariableP(_, _) =>
        if (isSet) {
          SetP(
            immutable.Seq(key),
            Some(child),
            Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
            concat(setElement(key), child)
          )
        } else {
          MapP(
            immutable.Seq(key),
            immutable.Seq(value),
            Some(child),
            Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
            concat(element(key, value), child)
          )
        }
      case _ => ???
    }
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class HasNoKey(isSet: Boolean, key: Option[Pattern[Option[Occurrence]]]) extends Constructor {
  def name                                              = "0"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = key.isDefined && pat == key.get
  def expand(f: Fringe): Option[immutable.Seq[Fringe]]  = Some(immutable.Seq(f))
  def contract(f: Fringe, children: immutable.Seq[Pattern[String]]): Pattern[String] = {
    val child = children(0)
    def element(k: Pattern[String], v: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(k, v))
    def setElement(k: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get, immutable.Seq(k))
    val unit: Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "unit").get, immutable.Seq())
    def concat(m1: Pattern[String], m2: Pattern[String]): Pattern[String] =
      SymbolP(Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "concat").get, immutable.Seq(m1, m2))
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
        if (isSet) {
          SetP(
            immutable.Seq(wildcard),
            Some(child),
            Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
            concat(setElement(wildcard), child)
          )
        } else {
          MapP(
            immutable.Seq(wildcard),
            immutable.Seq(wildcard),
            Some(child),
            Parser.getSymbolAtt(f.symlib.sortAtt(f.sort), "element").get,
            concat(element(wildcard, wildcard), child)
          )
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
    Some((0 until length).map(i => new Fringe(f.symlib, sort, Num(i, f.occurrence), false)))
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
          new Fringe(f.symlib, t._1, Num(t._2, f.occurrence), sym.ctr == "inj")
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
