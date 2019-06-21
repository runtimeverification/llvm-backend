package org.kframework.backend.llvm.matching

import org.kframework.parser.kore.SymbolOrAlias
import org.kframework.backend.llvm.matching.pattern._

sealed trait Heuristic {
  def scoreAs[T](p: AsP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreList[T](p: ListP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreLiteral[T](p: LiteralP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreMap[T](p: MapP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreOr[T](p: OrP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreSet[T](p: SetP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreSymbol[T](p: SymbolP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreVariable[T](p: VariableP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreWildcard[T](p: WildcardP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???

  def computeScoreForKey(c: Column, key: Option[Pattern[Option[Occurrence]]]): Double
}

object DefaultHeuristic extends Heuristic {
  override def scoreAs[T](p: AsP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = p.pat.score(this, f, c, key, isEmpty)
  override def scoreList[T](p: ListP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = 1.0
  override def scoreLiteral[T](p: LiteralP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = 1.0
  override def scoreMap[T](p: MapP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = {
    if (p.keys.isEmpty && p.frame.isEmpty) {
      1.0
    } else if (isEmpty) {
      0.0
    } else if (p.keys.isEmpty) {
      p.frame.get.score(this, f, c, key, isEmpty)
    } else if (key.isDefined) {
      if (p.canonicalize(c).keys.contains(key.get)) 1.0 else 0.0
    } else {
      1.0
    }
  }

  override def scoreOr[T](p: OrP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = {
    p.ps.map(_.score(this, f, c, key, isEmpty)).sum
  }

  override def scoreSet[T](p: SetP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = {
    if (p.elements.isEmpty && p.frame.isEmpty) { 
      1.0
    } else if (isEmpty) {
      0.0
    } else if (p.elements.isEmpty) {
      p.frame.get.score(this, f, c, key, isEmpty)
    } else if (key.isDefined) {
      if (p.canonicalize(c).elements.contains(key.get)) 1.0 else 0.0
    } else {
      1.0
    }
  }

  override def scoreSymbol[T](p: SymbolP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = {
    val ncons = f.overloads(p.sym).size + 1.0
    1.0 / ncons
  }

  override def scoreVariable[T](p: VariableP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = 0.0
  override def scoreWildcard[T](p: WildcardP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = 0.0

  def computeScoreForKey(c: Column, key: Option[Pattern[Option[Occurrence]]]): Double = {
    var result = 0.0
    for (i <- c.patterns.indices) {
      if (c.clauses(i).action.priority != c.clauses.head.action.priority)
        return result
      result += c.patterns(i).score(DefaultHeuristic, c.fringe, c.clauses(i), key, c.isEmpty)
    }
    result
  }
}

object AHeuristic extends Heuristic {
  def computeScoreForKey(c: Column, key: Option[Pattern[Option[Occurrence]]]): Double = {
    var result = 0.0
    for (con <- c.signatureForKey(key)) {
      result -= c.fringe.expand(con).size
    }
    result
  }
}

object DHeuristic extends Heuristic {
  def computeScoreForKey(c: Column, key: Option[Pattern[Option[Occurrence]]]): Double = {
    -(c.patterns.count(_.isDefault))
  }
}

object FHeuristic extends Heuristic {
  def scoreAs[T](p: AsP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreList[T](p: ListP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreLiteral[T](p: LiteralP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreMap[T](p: MapP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreOr[T](p: OrP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreSet[T](p: SetP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreSymbol[T](p: SymbolP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreVariable[T](p: VariableP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreWildcard[T](p: WildcardP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???

  def computeScoreForKey(c: Column, key: Option[Pattern[Option[Occurrence]]]): Double = {
    for (i <- c.patterns.indices) {
      if (c.clauses(i).action.priority != c.clauses.head.action.priority)
        return 1.0
      if (c.patterns(i).isWildcard) {
        return 0.0
      }
    }
    1.0
  }
}

object BHeuristic extends Heuristic {
  def computeScoreForKey(c: Column, key: Option[Pattern[Option[Occurrence]]]): Double = {
    val sigma = c.signatureForKey(key)
    if (c.category.hasIncompleteSignature(sigma, c.fringe)) {
      -sigma.size-1
    } else {
      -sigma.size
    }
  }
}

object QHeuristic extends Heuristic {
  def computeScoreForKey(c: Column, key: Option[Pattern[Option[Occurrence]]]): Double = {
    var result = 0
    var priority = c.clauses.head.action.priority
    for (i <- c.patterns.indices) {
      if (c.clauses(i).action.priority != priority) {
        if (result != i) {
          return result
        }
        priority = c.clauses(i).action.priority
      }
      if (!c.patterns(i).isWildcard) {
        result += 1
      }
    }
    result
  }
}
