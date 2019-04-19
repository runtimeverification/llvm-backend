package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.pattern.Pattern
import org.kframework.parser.kore.SymbolOrAlias

sealed trait Constructor {
  def name: String
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean
  def expand(f: Fringe): Option[Seq[Fringe]]
}

case class Empty() extends Constructor {
  def name = "0"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[Seq[Fringe]] = Some(Seq())
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class NonEmpty() extends Constructor {
  def name: String = ???
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[Seq[Fringe]] = Some(Seq(f))
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class HasKey(isSet: Boolean, element: SymbolOrAlias, key: Option[Pattern[Option[Occurrence]]]) extends Constructor {
  def name = "1"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = key.isDefined && pat == key.get
  def expand(f: Fringe): Option[Seq[Fringe]] = {
    key match {
      case None => ???
      case Some(k) =>
        val sorts = f.symlib.signatures(element)._1
        if (isSet) {
          Some(Seq(new Fringe(f.symlib, f.sort, Rem(k, f.occurrence), false), f))
        } else {
          Some(Seq(new Fringe(f.symlib, sorts(1), Value(k, f.occurrence), false), new Fringe(f.symlib, f.sort, Rem(k, f.occurrence), false), f))
        }
    }
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class HasNoKey(key: Option[Pattern[Option[Occurrence]]]) extends Constructor {
  def name = "0"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = key.isDefined && pat == key.get
  def expand(f: Fringe): Option[Seq[Fringe]] = Some(Seq(f))
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class ListC(element: SymbolOrAlias, length: Int) extends Constructor {
  def name: String = length.toString
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[Seq[Fringe]] = {
    val sort = f.symlib.signatures(element)._1.head
    Some((0 until length).map(i => new Fringe(f.symlib, sort, Num(i, f.occurrence), false)))
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class SymbolC(sym: SymbolOrAlias) extends Constructor {
  def name: String = sym.toString
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[Seq[Fringe]] = {
    if (f.symlib.signatures(sym)._2 != f.sort) {
      None
    } else {
      val sorts = f.symlib.signatures(sym)._1
      Some(sorts.zipWithIndex.map(t => new Fringe(f.symlib, t._1, Num(t._2, f.occurrence), sym.ctr == "inj")))
    }
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}

case class LiteralC(literal: String) extends Constructor {
  def name: String = literal
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
  def expand(f: Fringe): Option[Seq[Fringe]] = Some(Seq())
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
