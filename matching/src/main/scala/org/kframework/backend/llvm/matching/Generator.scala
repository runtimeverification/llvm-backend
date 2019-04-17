package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.dt.DecisionTree
import org.kframework.backend.llvm.matching.pattern.{Pattern => P, SymbolP, LiteralP, VariableP, AsP, OrP, ListP, MapP, SetP, WildcardP}
import org.kframework.parser.kore._

object Generator {

  sealed trait CollectionCons
  case class Concat() extends CollectionCons
  case class Unit() extends CollectionCons
  case class Element() extends CollectionCons

  private def listPattern(sym: SymbolOrAlias, cons: CollectionCons, ps: Seq[P[String]], c: SymbolOrAlias) : P[String] = {
    (cons, ps) match {
      case (Concat(), Seq(ListP(hd1, None, tl1, _, o1), ListP(hd2, frame, tl2, _, o2))) => ListP((hd1 ++ tl1 ++ hd2), frame, tl2, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(ListP(hd1, frame, tl1, _, o1), ListP(hd2, None, tl2, _, o2))) => ListP(hd1, frame, (tl1 ++ hd2 ++ tl2), c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(ListP(hd, None, tl, _, o), p @ VariableP(_, _))) => ListP((hd ++ tl), Some(p), Seq(), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(ListP(hd, None, tl, _, o), p @ WildcardP())) => ListP((hd ++ tl), Some(p), Seq(), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(p @ VariableP(_, _), ListP(hd, None, tl, _, o))) => ListP(Seq(), Some(p), (hd ++ tl), c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p @ WildcardP(), ListP(hd, None, tl, _, o))) => ListP(Seq(), Some(p), (hd ++ tl), c, SymbolP(sym, Seq(p, o)))
      case (Unit(), Seq()) => ListP(Seq(), None, Seq(), c, SymbolP(sym, Seq()))
      case (Element(), Seq(p)) => ListP(Seq(p), None, Seq(), c, SymbolP(sym, Seq(p)))
    }
  }

  private def mapPattern(sym: SymbolOrAlias, cons: CollectionCons, ps: Seq[P[String]], c: SymbolOrAlias) : P[String] = {
    (cons, ps) match {
      case (Concat(), Seq(MapP(ks1, vs1, None, _, o1), MapP(ks2, vs2, frame, _, o2))) => MapP((ks1 ++ ks2), (vs1 ++ vs2), frame, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(MapP(ks1, vs1, frame,_, o1), MapP(ks2, vs2, None, _, o2))) => MapP((ks1 ++ ks2), (vs1 ++ vs2), frame, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(MapP(ks, vs, None, _, o), p @ VariableP(_, _))) => MapP(ks, vs, Some(p), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(MapP(ks, vs, None, _, o), p @ WildcardP())) => MapP(ks, vs, Some(p), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(p @ VariableP(_, _), MapP(ks, vs, None, _, o))) => MapP(ks, vs, Some(p), c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p @ WildcardP(), MapP(ks, vs, None, _, o))) => MapP(ks, vs, Some(p), c, SymbolP(sym, Seq(p, o)))
      case (Unit(), Seq()) => MapP(Seq(), Seq(), None, c, SymbolP(sym, Seq()))
      case (Element(), Seq(k, v)) => MapP(Seq(k), Seq(v), None, c, SymbolP(sym, Seq(k, v)))
    }
  }

  private def setPattern(sym: SymbolOrAlias, cons: CollectionCons, ps: Seq[P[String]], c: SymbolOrAlias) : P[String] = {
    (cons, ps) match {
      case (Concat(), Seq(SetP(ks1, None, _, o1), SetP(ks2, frame, _, o2))) => SetP((ks1 ++ ks2), frame, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(SetP(ks1, frame,_, o1), SetP(ks2, None, _, o2))) => SetP((ks1 ++ ks2), frame, c, SymbolP(sym, Seq(o1, o2)))
      case (Concat(), Seq(SetP(ks, None, _, o), p @ VariableP(_, _))) => SetP(ks, Some(p), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(SetP(ks, None, _, o), p @ WildcardP())) => SetP(ks, Some(p), c, SymbolP(sym, Seq(o, p)))
      case (Concat(), Seq(p @ VariableP(_, _), SetP(ks, None, _, o))) => SetP(ks, Some(p), c, SymbolP(sym, Seq(p, o)))
      case (Concat(), Seq(p @ WildcardP(), SetP(ks, None, _, o))) => SetP(ks, Some(p), c, SymbolP(sym, Seq(p, o)))
      case (Unit(), Seq()) => SetP(Seq(), None, c, SymbolP(sym, Seq()))
      case (Element(), Seq(e)) => SetP(Seq(e), None, c, SymbolP(sym, Seq(e)))
    }
  }

  private def genPatterns(mod: Definition, symlib: Parser.SymLib, rewrite: GeneralizedRewrite) : Seq[P[String]] = {
    val lhs = rewrite.getLeftHandSide
    def getSym(hookAtt: String, syms: Seq[SymbolOrAlias]): SymbolOrAlias = {
      syms.filter(isHook(hookAtt, _)).head
    }
    def isHook(hookAttStr: String, sym: SymbolOrAlias): Boolean = {
      val att = symlib.signatures(sym)._3
      val hookAtt = Parser.getStringAtt(att, "hook")
      Some(hookAttStr) == hookAtt
    }
    def genPattern(pat: Pattern) : P[String] = {
      pat match {
        case Application(sym, ps) => {
          val att = symlib.signatures(sym)._3
          val hookAtt = Parser.getStringAtt(att, "hook")
          val sort = symlib.signatures(sym)._2
          hookAtt match {
            case Some("LIST.concat") => listPattern(sym, Concat(), ps.map(genPattern), getSym("LIST.element", symlib.symbolsForSort(sort)))
            case Some("LIST.unit") => listPattern(sym, Unit(), Seq(), getSym("LIST.element", symlib.symbolsForSort(sort)))
            case Some("LIST.element") => listPattern(sym, Element(), ps.map(genPattern), getSym("LIST.element", symlib.symbolsForSort(sort)))
            case Some("MAP.concat") => mapPattern(sym, Concat(), ps.map(genPattern), getSym("MAP.element", symlib.symbolsForSort(sort)))
            case Some("MAP.unit") => mapPattern(sym, Unit(), Seq(), getSym("MAP.element", symlib.symbolsForSort(sort)))
            case Some("MAP.element") => mapPattern(sym, Element(), ps.map(genPattern), getSym("MAP.element", symlib.symbolsForSort(sort)))
            case Some("SET.concat") => setPattern(sym, Concat(), ps.map(genPattern), getSym("SET.element", symlib.symbolsForSort(sort)))
            case Some("SET.unit") => setPattern(sym, Unit(), Seq(), getSym("SET.element", symlib.symbolsForSort(sort)))
            case Some("SET.element") => setPattern(sym, Element(), ps.map(genPattern), getSym("SET.element", symlib.symbolsForSort(sort)))
            case _ => SymbolP(sym, ps.map(genPattern))
          }
        }
        case DomainValue(sort, str) => {
          val att = symlib.sortAtt(sort)
          val hookAtt = Parser.getStringAtt(att, "hook")
          LiteralP(if (hookAtt == Some("BOOL.Bool")) {
            str match {
              case "true" => "1"
              case "false" => "0"
              case _ => str
            }
          } else str, hookAtt.getOrElse("STRING.String"))
        }
        case Variable(name, sort) => {
          val att = symlib.sortAtt(sort)
          val hookAtt = Parser.getStringAtt(att, "hook")
          VariableP(name, hookAtt.getOrElse("STRING.String"))
        }
        case And(_, p, v @ Variable(_, _)) => {
          val _var = genPattern(v).asInstanceOf[VariableP[String]]
          AsP(_var.name, _var.hook, genPattern(p))
        }
        case Or(_, p1, p2) => OrP(genPattern(p1), genPattern(p2))
      }
    }
    lhs.map(genPattern)
  }

  private def genVars(pat: Pattern) : Seq[String] = {
    pat match {
      case Variable(name, _) => Seq(name)
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

  private def genClauseMatrix[T](
      symlib: Parser.SymLib,
      mod: Definition,
      axioms: Seq[AxiomInfo],
      sorts: Seq[Sort]) :
      Matrix = {
    val actions = axioms.map(a => new Action(a.ordinal, genVars(a.rewrite.getRightHandSide), a.sideCondition.map(genVars)))
    val patterns = axioms.map(a => genPatterns(mod, symlib, a.rewrite)).transpose
    val cols = (sorts, patterns).zipped.toSeq
    new Matrix(symlib, cols, actions)
  }
    
  
  def mkDecisionTree[T](symlib: Parser.SymLib, mod: Definition, axioms: Seq[AxiomInfo], sorts: Seq[Sort]) : DecisionTree = {
    val matrix = genClauseMatrix(symlib, mod, axioms, sorts)
    matrix.compile
  }
}
