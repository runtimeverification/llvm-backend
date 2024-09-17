package org.kframework.backend.llvm.matching

import com.runtimeverification.k.kore._
import org.kframework.backend.llvm.matching.dt.DecisionTree
import org.kframework.backend.llvm.matching.pattern.{ Pattern => P }
import org.kframework.backend.llvm.matching.pattern.AsP
import org.kframework.backend.llvm.matching.pattern.ListGetP
import org.kframework.backend.llvm.matching.pattern.ListP
import org.kframework.backend.llvm.matching.pattern.LiteralP
import org.kframework.backend.llvm.matching.pattern.MapP
import org.kframework.backend.llvm.matching.pattern.OrP
import org.kframework.backend.llvm.matching.pattern.SetP
import org.kframework.backend.llvm.matching.pattern.SortCategory
import org.kframework.backend.llvm.matching.pattern.SymbolP
import org.kframework.backend.llvm.matching.pattern.VariableP
import org.kframework.backend.llvm.matching.pattern.WildcardP
import scala.collection.immutable

object Generator {

  sealed trait CollectionCons
  case class Concat()  extends CollectionCons
  case class Unit()    extends CollectionCons
  case class Element() extends CollectionCons

  private def listPattern(
      sym: SymbolOrAlias,
      cons: CollectionCons,
      ps: immutable.Seq[P[String]],
      c: SymbolOrAlias
  ): P[String] =
    (cons, ps) match {
      case (Concat(), immutable.Seq(ListP(hd1, None, tl1, _, o1), ListP(hd2, frame, tl2, _, o2))) =>
        ListP(hd1 ++ tl1 ++ hd2, frame, tl2, c, SymbolP(sym, immutable.Seq(o1, o2)))
      case (Concat(), immutable.Seq(ListP(hd1, frame, tl1, _, o1), ListP(hd2, None, tl2, _, o2))) =>
        ListP(hd1, frame, tl1 ++ hd2 ++ tl2, c, SymbolP(sym, immutable.Seq(o1, o2)))
      case (Concat(), immutable.Seq(ListP(hd, None, tl, _, o), p @ VariableP(_, _))) =>
        ListP(hd ++ tl, Some(p), immutable.Seq(), c, SymbolP(sym, immutable.Seq(o, p)))
      case (Concat(), immutable.Seq(ListP(hd, None, tl, _, o), p @ WildcardP())) =>
        ListP(hd ++ tl, Some(p), immutable.Seq(), c, SymbolP(sym, immutable.Seq(o, p)))
      case (Concat(), immutable.Seq(p @ VariableP(_, _), ListP(hd, None, tl, _, o))) =>
        ListP(immutable.Seq(), Some(p), hd ++ tl, c, SymbolP(sym, immutable.Seq(p, o)))
      case (Concat(), immutable.Seq(p @ WildcardP(), ListP(hd, None, tl, _, o))) =>
        ListP(immutable.Seq(), Some(p), hd ++ tl, c, SymbolP(sym, immutable.Seq(p, o)))
      case (Concat(), immutable.Seq(p1, p2)) =>
        ListP(immutable.Seq(), None, immutable.Seq(), c, SymbolP(sym, immutable.Seq(p1, p2))) /* not
         * valid, but necessary for iterated pattern matching */
      case (Unit(), immutable.Seq()) =>
        ListP(immutable.Seq(), None, immutable.Seq(), c, SymbolP(sym, immutable.Seq()))
      case (Element(), immutable.Seq(p)) =>
        ListP(immutable.Seq(p), None, immutable.Seq(), c, SymbolP(sym, immutable.Seq(p)))
      case _ => ???
    }

  private def listGetPattern(
      sym: SymbolOrAlias,
      ps: immutable.Seq[P[String]],
      c: SymbolOrAlias
  ): P[String] =
    ps match {
      case immutable.Seq(p @ (WildcardP() | VariableP(_, _)), k, v) =>
        ListGetP(immutable.Seq(k), immutable.Seq(v), p, c, SymbolP(sym, immutable.Seq(p, k, v)))
      case immutable.Seq(ListGetP(ks, vs, frame, _, o), k, v) =>
        ListGetP(
          ks ++ immutable.Seq(k),
          vs ++ immutable.Seq(v),
          frame,
          c,
          SymbolP(sym, immutable.Seq(o, k, v))
        )
      case _ => ???
    }

  private def mapPattern(
      sym: SymbolOrAlias,
      cons: CollectionCons,
      ps: immutable.Seq[P[String]],
      c: SymbolOrAlias
  ): P[String] =
    (cons, ps) match {
      case (Concat(), immutable.Seq(MapP(ks1, vs1, None, _, o1), MapP(ks2, vs2, frame, _, o2))) =>
        MapP(ks1 ++ ks2, vs1 ++ vs2, frame, c, SymbolP(sym, immutable.Seq(o1, o2)))
      case (Concat(), immutable.Seq(MapP(ks1, vs1, frame, _, o1), MapP(ks2, vs2, None, _, o2))) =>
        MapP(ks1 ++ ks2, vs1 ++ vs2, frame, c, SymbolP(sym, immutable.Seq(o1, o2)))
      case (Concat(), immutable.Seq(MapP(ks, vs, None, _, o), p @ VariableP(_, _))) =>
        MapP(ks, vs, Some(p), c, SymbolP(sym, immutable.Seq(o, p)))
      case (Concat(), immutable.Seq(MapP(ks, vs, None, _, o), p @ WildcardP())) =>
        MapP(ks, vs, Some(p), c, SymbolP(sym, immutable.Seq(o, p)))
      case (Concat(), immutable.Seq(p @ VariableP(_, _), MapP(ks, vs, None, _, o))) =>
        MapP(ks, vs, Some(p), c, SymbolP(sym, immutable.Seq(p, o)))
      case (Concat(), immutable.Seq(p @ WildcardP(), MapP(ks, vs, None, _, o))) =>
        MapP(ks, vs, Some(p), c, SymbolP(sym, immutable.Seq(p, o)))
      case (Concat(), immutable.Seq(p1, p2)) =>
        MapP(immutable.Seq(), immutable.Seq(), None, c, SymbolP(sym, immutable.Seq(p1, p2))) /* not
         * valid, but necessary for iterated pattern matching */
      case (Unit(), immutable.Seq()) =>
        MapP(immutable.Seq(), immutable.Seq(), None, c, SymbolP(sym, immutable.Seq()))
      case (Element(), immutable.Seq(k, v)) =>
        MapP(immutable.Seq(k), immutable.Seq(v), None, c, SymbolP(sym, immutable.Seq(k, v)))
      case _ => ???
    }

  private def setPattern(
      sym: SymbolOrAlias,
      cons: CollectionCons,
      ps: immutable.Seq[P[String]],
      c: SymbolOrAlias
  ): P[String] =
    (cons, ps) match {
      case (Concat(), immutable.Seq(SetP(ks1, None, _, o1), SetP(ks2, frame, _, o2))) =>
        SetP(ks1 ++ ks2, frame, c, SymbolP(sym, immutable.Seq(o1, o2)))
      case (Concat(), immutable.Seq(SetP(ks1, frame, _, o1), SetP(ks2, None, _, o2))) =>
        SetP(ks1 ++ ks2, frame, c, SymbolP(sym, immutable.Seq(o1, o2)))
      case (Concat(), immutable.Seq(SetP(ks, None, _, o), p @ VariableP(_, _))) =>
        SetP(ks, Some(p), c, SymbolP(sym, immutable.Seq(o, p)))
      case (Concat(), immutable.Seq(SetP(ks, None, _, o), p @ WildcardP())) =>
        SetP(ks, Some(p), c, SymbolP(sym, immutable.Seq(o, p)))
      case (Concat(), immutable.Seq(p @ VariableP(_, _), SetP(ks, None, _, o))) =>
        SetP(ks, Some(p), c, SymbolP(sym, immutable.Seq(p, o)))
      case (Concat(), immutable.Seq(p @ WildcardP(), SetP(ks, None, _, o))) =>
        SetP(ks, Some(p), c, SymbolP(sym, immutable.Seq(p, o)))
      case (Concat(), immutable.Seq(p1, p2)) =>
        SetP(immutable.Seq(), None, c, SymbolP(sym, immutable.Seq(p1, p2))) /* not valid, but
         * necessary for iterated pattern matching */
      case (Unit(), immutable.Seq()) =>
        SetP(immutable.Seq(), None, c, SymbolP(sym, immutable.Seq()))
      case (Element(), immutable.Seq(e)) =>
        SetP(immutable.Seq(e), None, c, SymbolP(sym, immutable.Seq(e)))
      case _ => ???
    }

  private def genPatterns(
      mod: Definition,
      symlib: Parser.SymLib,
      lhs: immutable.Seq[Pattern]
  ): List[P[String]] = {
    def getElementSym(sort: Sort): SymbolOrAlias =
      Parser.getSymbolAtt(symlib.sortAtt(sort), "element").get
    def getUpdateSym(sort: Sort): SymbolOrAlias =
      Parser.getSymbolAtt(symlib.sortAtt(sort), "update").get
    def genPattern(pat: Pattern): P[String] =
      pat match {
        case Application(sym, ps) =>
          val att     = symlib.signatures(sym)._3
          val hookAtt = Parser.getStringAtt(att, "hook")
          val sort    = symlib.signatures(sym)._2
          hookAtt match {
            case Some("LIST.concat") =>
              listPattern(sym, Concat(), ps.map(genPattern), getElementSym(sort))
            case Some("LIST.unit") => listPattern(sym, Unit(), immutable.Seq(), getElementSym(sort))
            case Some("LIST.element") =>
              listPattern(sym, Element(), ps.map(genPattern), getElementSym(sort))
            case Some("LIST.update") =>
              listGetPattern(sym, ps.map(genPattern), getUpdateSym(sort))
            case Some("MAP.concat") =>
              mapPattern(sym, Concat(), ps.map(genPattern), getElementSym(sort))
            case Some("MAP.unit") => mapPattern(sym, Unit(), immutable.Seq(), getElementSym(sort))
            case Some("MAP.element") =>
              mapPattern(sym, Element(), ps.map(genPattern), getElementSym(sort))
            case Some("SET.concat") =>
              setPattern(sym, Concat(), ps.map(genPattern), getElementSym(sort))
            case Some("SET.unit") => setPattern(sym, Unit(), immutable.Seq(), getElementSym(sort))
            case Some("SET.element") =>
              setPattern(sym, Element(), ps.map(genPattern), getElementSym(sort))
            case _ => SymbolP(sym, ps.map(genPattern))
          }
        case DomainValue(sort, str) =>
          val att     = symlib.sortAtt(sort)
          val hookAtt = Parser.getStringAtt(att, "hook")
          LiteralP(
            if (hookAtt.contains("BOOL.Bool")) {
              str match {
                case "true"  => "1"
                case "false" => "0"
                case _       => str
              }
            } else str,
            SortCategory(hookAtt.orElse(Some("STRING.String")), sort, symlib)
          )
        case Variable(name, sort) =>
          val att     = symlib.sortAtt(sort)
          val hookAtt = Parser.getStringAtt(att, "hook")
          VariableP(name, SortCategory(hookAtt.orElse(Some("STRING.String")), sort, symlib))
        case And(_, p +: (v @ Variable(_, _)) +: immutable.Seq()) =>
          val _var = genPattern(v).asInstanceOf[VariableP[String]]
          AsP(_var.name, _var.sort, genPattern(p))
        case Or(_, args) => args.map(genPattern).reduce(OrP(_, _))
      }
    lhs.map(genPattern).toList
  }

  private def genVars(pat: Pattern): immutable.Seq[Variable] =
    pat match {
      case v @ Variable(_, _)   => immutable.Seq(v)
      case And(_, ps)           => ps.flatMap(genVars)
      case Application(_, ps)   => ps.flatMap(genVars)
      case Ceil(_, _, p)        => genVars(p)
      case Equals(_, _, p1, p2) => genVars(p1) ++ genVars(p2)
      case Exists(_, _, p)      => genVars(p)
      case Floor(_, _, p)       => genVars(p)
      case Forall(_, _, p)      => genVars(p)
      case Iff(_, p1, p2)       => genVars(p1) ++ genVars(p2)
      case Implies(_, p1, p2)   => genVars(p1) ++ genVars(p2)
      case Mem(_, _, p1, p2)    => genVars(p1) ++ genVars(p2)
      // case Next(_, p) => genVars(p)
      case Not(_, p) => genVars(p)
      case Or(_, ps) => ps.flatMap(genVars)
      case _         => immutable.Seq()
    }

  def genClauseMatrix(
      symlib: Parser.SymLib,
      mod: Definition,
      axioms: immutable.IndexedSeq[AxiomInfo],
      sorts: immutable.Seq[Sort]
  ): Matrix = {
    val actions = axioms.map { a =>
      val lhsVars     = a.rewrite.getLeftHandSide.flatMap(genVars)
      val lhsVarNames = lhsVars.map(_.name)
      val rhsVars     = genVars(a.rewrite.getRightHandSide)
      val scVars      = a.sideCondition.map(genVars)
      Action(
        a.ordinal,
        lhsVars,
        rhsVars.map(_.name).sorted.distinct,
        scVars.map(_.map(_.name).sorted.distinct),
        (rhsVars ++ scVars.getOrElse(immutable.Seq()))
          .filter(_.name.startsWith("Var'Bang"))
          .map(v => (v.name, v.sort)),
        a.rewrite.getLeftHandSide.size,
        a.priority,
        a.source,
        a.location,
        lhsVarNames.toSet.size != lhsVarNames.size
      )
    }
    val patterns = axioms.map(a => genPatterns(mod, symlib, a.rewrite.getLeftHandSide)).transpose
    val cols =
      sorts
        .lazyZip(
          if (axioms.isEmpty) sorts.map(_ => immutable.IndexedSeq()) else patterns
        )
        .toIndexedSeq
    new Matrix(symlib, cols, actions).expand
  }

  def mkDecisionTree(
      symlib: Parser.SymLib,
      mod: Definition,
      axioms: immutable.IndexedSeq[AxiomInfo],
      sorts: immutable.Seq[Sort],
      name: SymbolOrAlias,
      kem: MatchingException => scala.Unit
  ): DecisionTree = {
    val matrix = genClauseMatrix(symlib, mod, axioms, sorts)
    matrix.checkUsefulness(kem)
    if (
      !symlib.isHooked(name) && (Parser.hasAtt(symlib.signatures(name)._3, "functional") || Parser
        .hasAtt(symlib.signatures(name)._3, "total")) && Parser.hasAtt(
        symlib.signatures(name)._3,
        "function"
      )
    ) {
      matrix.checkExhaustiveness(name, kem)
    }
    matrix.compile
  }

  private def isPoorlySpecialized(
      finalMatrix: Matrix,
      originalMatrix: Matrix,
      threshold: (Int, Int)
  ): Boolean = {
    val numerator   = originalMatrix.rows.size * threshold._1
    val denominator = finalMatrix.rows.size * threshold._2
    if (Matching.logging) {
      System.out.println(finalMatrix.rows.size.toString + "/" + originalMatrix.rows.size.toString)
    }
    numerator <= denominator
  }

  def mkSpecialDecisionTree(
      symlib: Parser.SymLib,
      mod: Definition,
      matrix: Matrix,
      axiom: AxiomInfo,
      threshold: (Int, Int)
  ): Option[(DecisionTree, immutable.Seq[(P[String], Occurrence)])] = {
    val rhs = genPatterns(mod, symlib, immutable.Seq(axiom.rewrite.getRightHandSide))
    val (specialized, residuals) = matrix.specializeBy(rhs.toIndexedSeq)
    val residualMap = residuals.lazyZip(specialized.fringe.map(_.occurrence)).to(immutable.Seq)
    if (Matching.logging) {
      System.out.println("Residuals: " + residualMap.toList)
    }
    val newClauses = specialized.clauses.map(_.specializeBy(residualMap, symlib))
    val finalMatrix = Matrix.fromColumns(
      symlib,
      specialized.columns.map(c => new Column(c.fringe.inexact, c.patterns, newClauses)),
      newClauses,
      search = false
    )
    if (isPoorlySpecialized(finalMatrix, matrix, threshold)) {
      None
    } else {
      val dt = finalMatrix.compile
      Some((dt, residualMap))
    }
  }
}
