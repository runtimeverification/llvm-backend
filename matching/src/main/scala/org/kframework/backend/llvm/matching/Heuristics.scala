package org.kframework.backend.llvm.matching

import org.kframework.parser.kore.SymbolOrAlias
import org.kframework.backend.llvm.matching.pattern._

sealed trait Heuristic {
  val needsMatrix: Boolean

  def scoreAs[T](p: AsP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreList[T](p: ListP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreLiteral[T](p: LiteralP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreMap[T](p: MapP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreOr[T](p: OrP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreSet[T](p: SetP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreSymbol[T](p: SymbolP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreVariable[T](p: VariableP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???
  def scoreWildcard[T](p: WildcardP[T], f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = ???

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double

  def breakTies(cols: Seq[MatrixColumn]): MatrixColumn = RPseudoHeuristic.breakTies(cols)
}

object Heuristic {
  def getBest(cols: Seq[MatrixColumn], allCols: Seq[MatrixColumn]): Seq[MatrixColumn] = {
    var result: List[MatrixColumn] = Nil
    var best = cols(0).score
    for (col <- cols) {
      import Ordering.Implicits._
      val bestInvalid = allCols.filter(c => !c.column.isValid && col.column.needed(c.column.keyVars)).sortBy(_.score).headOption
      val colBest = if (bestInvalid.isDefined && bestInvalid.get.score > col.score) {
        bestInvalid.get.score
      } else {
        col.score
      }
      if (colBest > best) {
        best = colBest
        result = col :: Nil
      } else if (colBest == best) {
        result = col :: result
      }
    }
    result
  }
}

@NamedHeuristic(name='_')
object DefaultHeuristic extends Heuristic {
  val needsMatrix: Boolean = false

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

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    var result = 0.0
    for (i <- c.column.patterns.indices) {
      if (c.column.clauses(i).action.priority != c.column.clauses.head.action.priority)
        return result
      result += c.column.patterns(i).score(DefaultHeuristic, c.column.fringe, c.column.clauses(i), key, c.column.isEmpty)
    }
    result
  }
}

@NamedHeuristic(name='f')
object FHeuristic extends Heuristic {
  val needsMatrix: Boolean = false

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    for (i <- c.column.patterns.indices) {
      if (c.column.clauses(i).action.priority != c.column.clauses.head.action.priority)
        return 1.0
      if (c.column.patterns(i).isWildcard) {
        return 0.0
      }
    }
    1.0
  }
}

@NamedHeuristic(name='d')
object DHeuristic extends Heuristic {
  val needsMatrix: Boolean = false

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    -(c.column.patterns.count(_.isDefault))
  }
}

@NamedHeuristic(name='b')
object BHeuristic extends Heuristic {
  val needsMatrix: Boolean = false

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    val sigma = c.column.signatureForKey(key)
    if (c.column.category.hasIncompleteSignature(sigma, c.column.fringe)) {
      -sigma.size-1
    } else {
      -sigma.size
    }
  }
}

@NamedHeuristic(name='a')
object AHeuristic extends Heuristic {
  val needsMatrix: Boolean = false

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    var result = 0.0
    for (con <- c.column.signatureForKey(key)) {
      result -= c.column.fringe.expand(con).size
    }
    result
  }
}

@NamedHeuristic(name='l')
object LHeuristic extends Heuristic {
  val needsMatrix: Boolean = true

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    var result = 0.0
    val matrixColumn = c.asInstanceOf[MatrixColumn]
    val matrix = matrixColumn.matrix
    val colIx = matrixColumn.colIx

    val sigma = matrix.columns(colIx).signatureForKey(key)
    for (con <- sigma) {
      val spec = matrix.specialize(con, colIx, None)._2
      if (spec.bestRowIx != -1) {
        result += 1.0
      }
    }

    val defaultMatrix = matrix.default(colIx, sigma)
    if (defaultMatrix.isDefined) {
      if (defaultMatrix.get.bestRowIx != -1) {
        result += 1.0
      }
    }
    result
  }
}

@NamedHeuristic(name='r')
object RHeuristic extends Heuristic {
  val needsMatrix: Boolean = false

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    var result = 0.0
    val signature = c.column.signatureForKey(key)
    for (con <- signature) {
      for (i <- c.column.patterns.indices) {
        if (c.column.patterns(i).isSpecialized(con, false, c.column.fringe, c.column.clauses(i), c.column.maxPriority)) {
          result += 1.0
        }
      }
    }

    if (c.column.category.hasIncompleteSignature(signature, c.column.fringe)) {
      for (i <- c.column.patterns.indices) {
        if (c.column.patterns(i).isDefault) {
          result += 1.0
        }
      }
    }

    -result
  }
}

@NamedHeuristic(name='n')
object NHeuristic extends Heuristic {
  val needsMatrix: Boolean = true

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    val matrixColumn = c.asInstanceOf[MatrixColumn]
    val matrix = matrixColumn.matrix
    val colIx = matrixColumn.colIx
    matrix.rows.indices.count(matrix.necessary(_, colIx))
  }
}

@NamedHeuristic(name='p')
object PHeuristic extends Heuristic {
  val needsMatrix: Boolean = true

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    val matrixColumn = c.asInstanceOf[MatrixColumn]
    val matrix = matrixColumn.matrix
    val colIx = matrixColumn.colIx
    var result = 0
    var priority = c.column.clauses.head.action.priority
    for (i <- c.column.patterns.indices) {
      if (c.column.clauses(i).action.priority != priority) {
        if (result != i) {
          return result
        }
        priority = c.column.clauses(i).action.priority
      }
      if (matrix.necessary(i, colIx)) {
        result += 1
      }
    }
    result
  }
}

@NamedHeuristic(name='q')
object QHeuristic extends Heuristic {
  val needsMatrix: Boolean = false

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = {
    var result = 0
    var priority = c.column.clauses.head.action.priority
    for (i <- c.column.patterns.indices) {
      if (c.column.clauses(i).action.priority != priority) {
        if (result != i) {
          return result
        }
        priority = c.column.clauses(i).action.priority
      }
      if (!c.column.patterns(i).isWildcard) {
        result += 1
      }
    }
    result
  }
}

sealed trait PseudoHeuristic extends Heuristic {
  val needsMatrix: Boolean = false

  def computeScoreForKey(c: AbstractColumn, key: Option[Pattern[Option[Occurrence]]]): Double = 0.0
}

@NamedHeuristic(name='N')
object NPseudoHeuristic extends PseudoHeuristic {
  override def breakTies(cols: Seq[MatrixColumn]): MatrixColumn = {
    cols(0)
  }
}

@NamedHeuristic(name='L')
object LPseudoHeuristic extends PseudoHeuristic {
  override def breakTies(cols: Seq[MatrixColumn]): MatrixColumn = {
    cols.minBy(_.column.fringe.occurrence.size)
  }
}

@NamedHeuristic(name='R')
object RPseudoHeuristic extends PseudoHeuristic {
  override def breakTies(cols: Seq[MatrixColumn]): MatrixColumn = {
    cols.reverse.minBy(_.column.fringe.occurrence.size)
  }
}
