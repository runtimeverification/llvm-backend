package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.pattern.Pattern
import org.kframework.parser.kore.SymbolOrAlias

sealed trait Constructor {
  def name: String
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean
}

case class Empty() extends Constructor {
  def name = "0"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
}

case class NonEmpty() extends Constructor {
  def name = ???
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
}

case class HasKey(isSet: Boolean, element: SymbolOrAlias, key: Option[Pattern[Option[Occurrence]]]) extends Constructor {
  def name = "1"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = key.isDefined && pat == key.get
}

case class HasNoKey(key: Option[Pattern[Option[Occurrence]]]) extends Constructor {
  def name = "0"
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = key.isDefined && pat == key.get
}

case class ListC(element: SymbolOrAlias, length: Int) extends Constructor {
  def name = length.toString
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
}

case class SymbolC(sym: SymbolOrAlias) extends Constructor {
  def name = sym.toString
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
}

case class LiteralC(literal: String) extends Constructor {
  def name = literal
  def isBest(pat: Pattern[Option[Occurrence]]): Boolean = true
}
