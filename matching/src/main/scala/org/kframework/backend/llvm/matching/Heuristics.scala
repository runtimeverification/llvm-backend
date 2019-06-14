package org.kframework.backend.llvm.matching

import org.kframework.parser.kore.SymbolOrAlias
import org.kframework.backend.llvm.matching.pattern._

trait Heuristic {
  def scoreAs[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, pat: Pattern[T]): Double
  def scoreList[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double
  def scoreLiteral[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double
  def scoreMap[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, keys: Seq[Pattern[T]], frame: Option[Pattern[T]], canonicalize: Clause => MapP[Option[Occurrence]]): Double
  def scoreOr[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, ps: Seq[Pattern[T]]): Double
  def scoreSet[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, elements: Seq[Pattern[T]], frame: Option[Pattern[T]], canonicalize: Clause => SetP[Option[Occurrence]]): Double
  def scoreSymbol[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, sym: SymbolOrAlias): Double
  def scoreVariable[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double
  def scoreWildcard[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double
}

class FHeuristic() extends Heuristic {
  def scoreAs[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, pat: Pattern[T]): Double = pat.score(this, f, c, key, isEmpty)
  def scoreList[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = 1.0
  def scoreLiteral[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = 1.0
  def scoreMap[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, keys: Seq[Pattern[T]], frame: Option[Pattern[T]], canonicalize: Clause => MapP[Option[Occurrence]]): Double = {
    if (keys.isEmpty && frame.isEmpty) {
      1.0
    } else if (isEmpty) {
      0.0
    } else if (keys.isEmpty) {
      frame.get.score(this, f, c, key, isEmpty)
    } else if (key.isDefined) {
      if (canonicalize(c).keys.contains(key.get)) 1.0 else 0.0
    } else {
      1.0
    }
  }

  def scoreOr[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, ps: Seq[Pattern[T]]): Double = {
    ps.map(_.score(this, f, c, key, isEmpty)).sum
  }

  def scoreSet[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, elements: Seq[Pattern[T]], frame: Option[Pattern[T]], canonicalize: Clause => SetP[Option[Occurrence]]): Double = {
    if (elements.isEmpty && frame.isEmpty) { 
      1.0
    } else if (isEmpty) {
      0.0
    } else if (elements.isEmpty) {
      frame.get.score(this, f, c, key, isEmpty)
    } else if (key.isDefined) {
      if (canonicalize(c).elements.contains(key.get)) 1.0 else 0.0
    } else {
      1.0
    }
  }

  def scoreSymbol[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean, sym: SymbolOrAlias): Double = {
    val ncons = f.overloads(sym).size + 1.0
    1.0 / ncons
  }

  def scoreVariable[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = 0.0
  def scoreWildcard[T](f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = 0.0
}
