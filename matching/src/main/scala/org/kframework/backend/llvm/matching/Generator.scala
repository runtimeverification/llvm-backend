package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.dt.DecisionTree
import org.kframework.backend.llvm.matching.pattern.{Pattern => P, SymbolP, LiteralP, VariableP, AsP, OrP, ListP, MapP, SetP, WildcardP, SortCategory}
import org.kframework.parser.kore._
import org.kframework.utils.errorsystem.KException

object Generator {

  sealed trait CollectionCons
  case class Concat() extends CollectionCons
  case class Unit() extends CollectionCons
  case class Element() extends CollectionCons

  private def listPattern(sym: SymbolOrAlias, cons: CollectionCons, ps: Seq[P[String]], c: SymbolOrAlias) : P[String] = {
    (cons, ps) match {
      case (Concat(), Seq(ListP(hd1, None, tl1, _, o1), ListP(hd2, frame, tl2, _, o2))) => ListP(hd1 ++ tl1 ++ hd2, frame, tl2, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(ListP(hd1, frame, tl1, _, o1), ListP(hd2, None, tl2, _, o2))) => ListP(hd1, frame, tl1 ++ hd2 ++ tl2, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(ListP(hd, None, tl, _, o), p @ VariableP(_, _))) => ListP(hd ++ tl, Some(p), Seq(), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(ListP(hd, None, tl, _, o), p @ WildcardP())) => ListP(hd ++ tl, Some(p), Seq(), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(p @ VariableP(_, _), ListP(hd, None, tl, _, o))) => ListP(Seq(), Some(p), hd ++ tl, c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p @ WildcardP(), ListP(hd, None, tl, _, o))) => ListP(Seq(), Some(p), hd ++ tl, c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p1, p2)) => ListP(Seq(), None, Seq(), c, SymbolP(sym, Seq(p1, p2))) // not valid, but necessary for iterated pattern matching
      case (Unit(), Seq()) => ListP(Seq(), None, Seq(), c, SymbolP(sym, Seq()))
      case (Element(), Seq(p)) => ListP(Seq(p), None, Seq(), c, SymbolP(sym, Seq(p)))
    }
  }

  private def mapPattern(sym: SymbolOrAlias, cons: CollectionCons, ps: Seq[P[String]], c: SymbolOrAlias) : P[String] = {
    (cons, ps) match {
      case (Concat(), Seq(MapP(ks1, vs1, None, _, o1), MapP(ks2, vs2, frame, _, o2))) => MapP(ks1 ++ ks2, vs1 ++ vs2, frame, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(MapP(ks1, vs1, frame,_, o1), MapP(ks2, vs2, None, _, o2))) => MapP(ks1 ++ ks2, vs1 ++ vs2, frame, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(MapP(ks, vs, None, _, o), p @ VariableP(_, _))) => MapP(ks, vs, Some(p), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(MapP(ks, vs, None, _, o), p @ WildcardP())) => MapP(ks, vs, Some(p), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(p @ VariableP(_, _), MapP(ks, vs, None, _, o))) => MapP(ks, vs, Some(p), c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p @ WildcardP(), MapP(ks, vs, None, _, o))) => MapP(ks, vs, Some(p), c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p1, p2)) => MapP(Seq(), Seq(), None, c, SymbolP(sym, Seq(p1, p2))) // not valid, but necessary for iterated pattern matching
      case (Unit(), Seq()) => MapP(Seq(), Seq(), None, c, SymbolP(sym, Seq()))
      case (Element(), Seq(k, v)) => MapP(Seq(k), Seq(v), None, c, SymbolP(sym, Seq(k, v)))
    }
  }

  private def setPattern(sym: SymbolOrAlias, cons: CollectionCons, ps: Seq[P[String]], c: SymbolOrAlias) : P[String] = {
    (cons, ps) match {
      case (Concat(), Seq(SetP(ks1, None, _, o1), SetP(ks2, frame, _, o2))) => SetP(ks1 ++ ks2, frame, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(SetP(ks1, frame,_, o1), SetP(ks2, None, _, o2))) => SetP(ks1 ++ ks2, frame, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(SetP(ks, None, _, o), p @ VariableP(_, _))) => SetP(ks, Some(p), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(SetP(ks, None, _, o), p @ WildcardP())) => SetP(ks, Some(p), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(p @ VariableP(_, _), SetP(ks, None, _, o))) => SetP(ks, Some(p), c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p @ WildcardP(), SetP(ks, None, _, o))) => SetP(ks, Some(p), c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p1, p2)) => SetP(Seq(), None, c, SymbolP(sym, Seq(p1, p2))) // not valid, but necessary for iterated pattern matching
      case (Unit(), Seq()) => SetP(Seq(), None, c, SymbolP(sym, Seq()))
      case (Element(), Seq(e)) => SetP(Seq(e), None, c, SymbolP(sym, Seq(e)))
    }
  }

  private def genPatterns(mod: Definition, symlib: Parser.SymLib, lhs: Seq[Pattern]) : List[P[String]] = {
    def getElementSym(sort: Sort): SymbolOrAlias = {
      Parser.getSymbolAtt(symlib.sortAtt(sort), "element").get
    }
    def genPattern(pat: Pattern) : P[String] = {
      pat match {
        case Application(sym, ps) =>
          val att = symlib.signatures(sym)._3
          val hookAtt = Parser.getStringAtt(att, "hook")
          val sort = symlib.signatures(sym)._2
          hookAtt match {
            case Some("LIST.concat") => listPattern(sym, Concat(), ps.map(genPattern), getElementSym(sort))
            case Some("LIST.unit") => listPattern(sym, Unit(), Seq(), getElementSym(sort))
            case Some("LIST.element") => listPattern(sym, Element(), ps.map(genPattern), getElementSym(sort))
            case Some("MAP.concat") => mapPattern(sym, Concat(), ps.map(genPattern), getElementSym(sort))
            case Some("MAP.unit") => mapPattern(sym, Unit(), Seq(), getElementSym(sort))
            case Some("MAP.element") => mapPattern(sym, Element(), ps.map(genPattern), getElementSym(sort))
            case Some("SET.concat") => setPattern(sym, Concat(), ps.map(genPattern), getElementSym(sort))
            case Some("SET.unit") => setPattern(sym, Unit(), Seq(), getElementSym(sort))
            case Some("SET.element") => setPattern(sym, Element(), ps.map(genPattern), getElementSym(sort))
            case _ => SymbolP(sym, ps.map(genPattern))
          }
        case DomainValue(sort, str) =>
          val att = symlib.sortAtt(sort)
          val hookAtt = Parser.getStringAtt(att, "hook")
          LiteralP(if (hookAtt.contains("BOOL.Bool")) {
            str match {
              case "true" => "1"
              case "false" => "0"
              case _ => str
            }
          } else str, SortCategory(hookAtt.orElse(Some("STRING.String"))))
        case Variable(name, sort) =>
          val att = symlib.sortAtt(sort)
          val hookAtt = Parser.getStringAtt(att, "hook")
          VariableP(name, SortCategory(hookAtt.orElse(Some("STRING.String"))))
        case And(_, p, v @ Variable(_, _)) =>
          val _var = genPattern(v).asInstanceOf[VariableP[String]]
          AsP(_var.name, _var.sort, genPattern(p))
        case Or(_, p1, p2) => OrP(genPattern(p1), genPattern(p2))
      }
    }
    lhs.map(genPattern).toList
  }

  private def genVars(pat: Pattern) : Seq[Variable] = {
    pat match {
      case v @ Variable(_, _) => Seq(v)
      case And(_, p1, p2) => genVars(p1) ++ genVars(p2)
      case Application(_, ps) => ps.flatMap(genVars)
      case Ceil(_, _, p) => genVars(p)
      case Equals(_, _, p1, p2) => genVars(p1) ++ genVars(p2)
      case Exists(_, _, p) => genVars(p)
      case Floor(_, _, p) => genVars(p)
      case Forall(_, _, p) => genVars(p)
      case Iff(_, p1, p2) => genVars(p1) ++ genVars(p2)
      case Implies(_, p1, p2) => genVars(p1) ++ genVars(p2)
      case Mem(_, _, p1, p2) => genVars(p1) ++ genVars(p2)
      // case Next(_, p) => genVars(p)
      case Not(_, p) => genVars(p)
      case Or(_, p1, p2) => genVars(p1) ++ genVars(p2)
      case _ => Seq()
    }
  }

  def genClauseMatrix[T](
      symlib: Parser.SymLib,
      mod: Definition,
      axioms: IndexedSeq[AxiomInfo],
      sorts: Seq[Sort]) :
      Matrix = {
    val actions = axioms.map(a => {
      val lhsVars = a.rewrite.getLeftHandSide.flatMap(genVars(_)).map(_.name)
      val rhsVars = genVars(a.rewrite.getRightHandSide)
      val scVars = a.sideCondition.map(genVars(_))
      new Action(a.ordinal, rhsVars.map(_.name).sorted.distinct, scVars.map(_.map(_.name).sorted.distinct), (rhsVars ++ scVars.getOrElse(Seq())).filter(_.name.startsWith("Var'Bang'")).map(v => (v.name, v.sort)), a.rewrite.getLeftHandSide.size, a.priority, a.source, a.location, lhsVars.toSet.size != lhsVars.size)
    })
    val patterns = axioms.map(a => genPatterns(mod, symlib, a.rewrite.getLeftHandSide)).transpose
    val cols = (sorts, if (axioms.isEmpty) sorts.map(_ => IndexedSeq()) else patterns).zipped.toIndexedSeq
    new Matrix(symlib, cols, actions).expand
  }
  
  def mkDecisionTree(symlib: Parser.SymLib, mod: Definition, axioms: IndexedSeq[AxiomInfo], sorts: Seq[Sort], name: SymbolOrAlias, kem: KException => scala.Unit) : DecisionTree = {
    val matrix = genClauseMatrix(symlib, mod, axioms, sorts)
    matrix.checkUsefulness(kem)
    if (!symlib.isHooked(name) && Parser.hasAtt(symlib.signatures(name)._3, "functional")) {
      matrix.checkExhaustiveness(name, kem)
    }
    matrix.compile
  }

  private def isPoorlySpecialized(finalMatrix: Matrix, originalMatrix: Matrix, threshold: (Int, Int)): Boolean = {
    val numerator = originalMatrix.rows.size * threshold._1
    val denominator = finalMatrix.rows.size * threshold._2
    if (Matching.logging) {
      System.out.println(finalMatrix.rows.size + "/" + originalMatrix.rows.size)
    }
    numerator <= denominator
  }

  def mkSpecialDecisionTree(symlib: Parser.SymLib, mod: Definition, matrix: Matrix, axiom: AxiomInfo, threshold: (Int, Int)) : Option[(DecisionTree, Seq[(P[String], Occurrence)])] = {
    val rhs = genPatterns(mod, symlib, Seq(axiom.rewrite.getRightHandSide))
    val (specialized,residuals) = matrix.specializeBy(rhs.toIndexedSeq)
    val residualMap = (residuals, specialized.fringe.map(_.occurrence)).zipped.toSeq
    if (Matching.logging) {
      System.out.println("Residuals: " + residualMap.toList)
    }
    val newClauses = specialized.clauses.map(_.specializeBy(residualMap, symlib))
    val finalMatrix = Matrix.fromColumns(symlib, specialized.columns.map(c => new Column(c.fringe.inexact, c.patterns, newClauses)), newClauses)
    if (isPoorlySpecialized(finalMatrix, matrix, threshold)) {
      None
    } else {
      val dt = finalMatrix.compile
      Some((dt, residualMap))
    }
  }
}
