package org.kframework.backend.llvm.matching

import org.kframework.parser.kore._
import org.kframework.parser.kore.implementation.{DefaultBuilders => B}
import java.util

case class AxiomInfo(priority: Int, ordinal: Int, rewrite: GeneralizedRewrite, sideCondition: Option[Pattern]) {}

object Parser {

  private def hasAtt(axiom: AxiomDeclaration, att: String): Boolean = {
    getAtt(axiom, att).isDefined
  }

  private def getAtt(axiom: AxiomDeclaration, att: String): Option[Pattern] = {
    axiom.att.patterns.find(isAtt(att, _))
  }

  def getStringAtt(att: Attributes, attName: String): Option[String] = {
    att.patterns.find(isAtt(attName, _)).map(_.asInstanceOf[Application].args.head.asInstanceOf[StringLiteral].str)
  }

  def getSymbolAtt(att: Attributes, attName: String): Option[SymbolOrAlias] = {
    att.patterns.find(isAtt(attName, _)).map(_.asInstanceOf[Application].args.head.asInstanceOf[Application].head)
  }

  private def isAtt(att: String, pat: Pattern): Boolean = {
    pat match {
      case Application(SymbolOrAlias(x, _), _) => x == att
      case _ => false
    }
  }

  class SymLib(symbols: Seq[SymbolOrAlias], sorts: Seq[Sort], mod: Definition, overloadSeq: Seq[(SymbolOrAlias, SymbolOrAlias)], val heuristics: Seq[Heuristic]) {
    val sortCache = new util.HashMap[Sort, SortInfo]()

    private val symbolDecls = mod.modules.flatMap(_.decls).filter(_.isInstanceOf[SymbolDeclaration]).map(_.asInstanceOf[SymbolDeclaration]).groupBy(_.symbol.ctr)

    private val sortDecls = mod.modules.flatMap(_.decls).filter(_.isInstanceOf[SortDeclaration]).map(_.asInstanceOf[SortDeclaration]).groupBy(_.sort.asInstanceOf[CompoundSort].ctr)

    private def instantiate(s: Sort, params: Seq[Sort], args: Seq[Sort]): Sort = {
      val map = (params, args).zipped.toMap
      s match {
        case v @ SortVariable(_) => map(v)
        case _ => s
      }
    }

    private def instantiate(s: Seq[Sort], params: Seq[Sort], args: Seq[Sort]): Seq[Sort] = s.map(instantiate(_, params, args))

    val signatures: Map[SymbolOrAlias, (Seq[Sort], Sort, Attributes)] = {
      symbols.map(symbol => (symbol, (instantiate(symbolDecls(symbol.ctr).head.argSorts, symbolDecls(symbol.ctr).head.symbol.params, symbol.params), instantiate(symbolDecls(symbol.ctr).head.returnSort, symbolDecls(symbol.ctr).head.symbol.params, symbol.params), symbolDecls(symbol.ctr).head.att))).toMap
    }

    val symbolsForSort: Map[Sort, Seq[SymbolOrAlias]] = {
      signatures.groupBy(_._2._2).mapValues(_.keys.toSeq)
    }

    val sortAtt: Map[Sort, Attributes] = {
      sorts.filter(_.isInstanceOf[CompoundSort]).map(sort => (sort, sortDecls(sort.asInstanceOf[CompoundSort].ctr).head.att)).toMap
    }

    val functions: Seq[SymbolOrAlias] = {
      signatures.filter(s => s._2._3.patterns.exists(isAtt("anywhere", _)) || s._2._3.patterns.exists(isAtt("function", _))).keys.toSeq
    }

    val overloads: Map[SymbolOrAlias, Seq[SymbolOrAlias]] = {
      overloadSeq.groupBy(_._1).mapValues(_.map(_._2).toSeq)
    }

    def isSubsorted(less: Sort, greater: Sort): Boolean = {
      signatures.contains(B.SymbolOrAlias("inj",Seq(less,greater)))
    }
  }

  private def rulePriority(axiom: AxiomDeclaration): Int = {
    if (hasAtt(axiom, "owise")) 200
    else if (hasAtt(axiom, "cool")) 150
    else if (hasAtt(axiom, "cool-like")) 100
    else if (hasAtt(axiom, "priority")) getStringAtt(axiom.att, "priority").get.toInt
    else 50
  }

  private def parseAxiomSentence[T <: GeneralizedRewrite](
      split: Pattern => Option[(Option[SymbolOrAlias], T, Option[Pattern])],
      axiom: (AxiomDeclaration, Int)) :
      Seq[(Option[SymbolOrAlias], AxiomInfo)] = {
    val splitted = split(axiom._1.pattern)
    if (splitted.isDefined) {
      val s = axiom._1
      if (hasAtt(s, "comm") || hasAtt(s, "assoc") || hasAtt(s, "idem")) {
        Seq()
      } else {
        Seq((splitted.get._1, AxiomInfo(rulePriority(s), axiom._2, splitted.get._2, splitted.get._3)))
      }
    } else {
      Seq()
    }
  }

  private def splitTop(topPattern: Pattern): Option[(Option[SymbolOrAlias], Rewrites, Option[Pattern])] = {
    topPattern match {
      case And(_, Equals(_, _, pat, _), And(_, _, rw @ Rewrites(_, _, _))) => Some((None, rw, Some(pat)))
      case And(_, Top(_), And(_, _, rw @ Rewrites(_, _, _))) => Some((None, rw, None))
      case Rewrites(s, And(_, Equals(_, _, pat, _), l), And(_, _, r)) => Some((None, B.Rewrites(s, l, r), Some(pat)))
      case Rewrites(s, And(_, Top(_), l), And(_, _, r)) => Some((None, B.Rewrites(s, l, r), None))
      case Implies(_, Bottom(_), p) => splitTop(p)
      case _ => None
    }
  }

  private def splitFunction(topPattern: Pattern): Option[(Option[SymbolOrAlias], Equals, Option[Pattern])] = {
    topPattern match {
      case Implies(_, Equals(_, _, pat, _), And(_, eq @ Equals(_, _, Application(symbol, _), _), _)) => Some(Some(symbol), eq, Some(pat))
      case Implies(_, Top(_), And(_, eq @ Equals(_, _, Application(symbol, _), _), _)) => Some(Some(symbol), eq, None)
      case Implies(_, And(_, _, Equals(_, _, pat, _)), And(_, eq @ Equals(_, _, Application(symbol, _), _), _)) => Some(Some(symbol), eq, Some(pat))
      case Implies(_, And(_, _, Top(_)), And(_, eq @ Equals(_, _, Application(symbol, _), _), _)) => Some(Some(symbol), eq, None)
      case eq @ Equals(_, _, Application(symbol, _), _) => Some(Some(symbol), eq, None)
      case _ => None
    }
  }

  def getAxioms(defn: Definition) : Seq[AxiomDeclaration] = {
    defn.modules.flatMap(_.decls).filter(_.isInstanceOf[AxiomDeclaration]).map(_.asInstanceOf[AxiomDeclaration])
  }

  def parseTopAxioms(axioms: Seq[(AxiomDeclaration, Int)]) : IndexedSeq[AxiomInfo] = {
    val withOwise = axioms.flatMap(parseAxiomSentence(splitTop, _))
    withOwise.map(_._2).sortWith(_.priority < _.priority).toIndexedSeq
  }

  def parseFunctionAxioms(axioms: Seq[(AxiomDeclaration, Int)]) : Map[SymbolOrAlias, IndexedSeq[AxiomInfo]] = {
    val withOwise = axioms.flatMap(parseAxiomSentence(a => splitFunction(a), _))
    withOwise.sortWith(_._2.priority < _._2.priority).toIndexedSeq.filter(_._1.isDefined).map(t => (t._1.get, t._2)).groupBy(_._1).mapValues(_.map(_._2))
  }

  private def isConcrete(symbol: SymbolOrAlias) : Boolean = {
    symbol.params.forall(_.isInstanceOf[CompoundSort])
  }

  private def parsePatternForSymbols(pat: Pattern): Seq[SymbolOrAlias] = {
    pat match {
      case And(_, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case Application(s, ps) => Seq(s).filter(isConcrete) ++ ps.flatMap(parsePatternForSymbols)
      case Ceil(_, _, p) => parsePatternForSymbols(p)
      case Equals(_, _, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case Exists(_, _, p) => parsePatternForSymbols(p)
      case Floor(_, _, p) => parsePatternForSymbols(p)
      case Forall(_, _, p) => parsePatternForSymbols(p)
      case Iff(_, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case Implies(_, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case Mem(_, _, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
//      case Next(_, p) => parsePatternForSymbols(p)
      case Not(_, p) => parsePatternForSymbols(p)
      case Or(_, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case Rewrites(_, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case _ => Seq()
    }
  }

  private def getOverloads(axioms: Seq[AxiomDeclaration]): Seq[(SymbolOrAlias, SymbolOrAlias)] = {
    if (axioms.isEmpty) {
      Seq()
    }
    axioms.filter(hasAtt(_, "overload")).map(getAtt(_, "overload") match {
      case Some(Application(_, args)) =>
        assert(args.size == 2)
        (args.head, args(1)) match {
          case (Application(g, _), Application(l, _)) => (g, l)
        }
    })
  }

  def parseHeuristic(heuristic: Char): Heuristic = {
    heuristic match {
      case 'f' => FHeuristic
      case 'd' => DHeuristic
      case 'b' => BHeuristic
      case 'a' => AHeuristic
      case 'r' => RHeuristic
      case 'q' => QHeuristic
      case '_' => DefaultHeuristic
      case 'N' => NPseudoHeuristic
      case 'L' => LPseudoHeuristic
      case 'R' => RPseudoHeuristic
    }
  }

  def parseHeuristics(heuristics: String): Seq[Heuristic] = {
    heuristics.toList.map(parseHeuristic(_))
  }

  def parseSymbols(defn: Definition, heuristics: String) : SymLib = {
    val axioms = getAxioms(defn)
    val symbols = axioms.flatMap(a => parsePatternForSymbols(a.pattern))
    val allSorts = symbols.flatMap(_.params)
    val overloads = getOverloads(axioms)
    new SymLib(symbols, allSorts, defn, overloads, parseHeuristics(heuristics))
  }
}
