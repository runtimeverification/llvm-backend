package org.kframework.backend.llvm.matching

import org.kframework.parser.kore._
import org.kframework.parser.kore.implementation.{DefaultBuilders => B}

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

  private def isAtt(att: String, pat: Pattern): Boolean = {
    pat match {
      case Application(SymbolOrAlias(x, _), _) => x == att
      case _ => false
    }
  }

  class SymLib(symbols: Seq[SymbolOrAlias], sorts: Seq[Sort], mod: Definition, overloadSeq: Seq[(SymbolOrAlias, SymbolOrAlias)]) {
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
    if (hasAtt(axiom, "owise")) 3
    else if (hasAtt(axiom, "cool")) 2
    else if (hasAtt(axiom, "heat")) 1
    else 0
  }

  private def parseAxiomSentence[T <: GeneralizedRewrite](
      split: Pattern => Option[(T, Option[Pattern])],
      axiom: (AxiomDeclaration, Int)) :
      Seq[AxiomInfo] = {
    val splitted = split(axiom._1.pattern)
    if (splitted.isDefined) {
      val s = axiom._1
      if (hasAtt(s, "comm") || hasAtt(s, "assoc") || hasAtt(s, "idem")) {
        Seq()
      } else {
        Seq(AxiomInfo(rulePriority(s), axiom._2, splitted.get._1, splitted.get._2))
      }
    } else {
      Seq()
    }
  }

  private def splitTop(topPattern: Pattern): Option[(Rewrites, Option[Pattern])] = {
    topPattern match {
      case And(_, Equals(_, _, pat, _), And(_, _, rw @ Rewrites(_, _, _))) => Some((rw, Some(pat)))
      case And(_, Top(_), And(_, _, rw @ Rewrites(_, _, _))) => Some((rw, None))
      case Rewrites(s, And(_, Equals(_, _, pat, _), l), And(_, _, r)) => Some((B.Rewrites(s, l, r), Some(pat)))
      case Rewrites(s, And(_, Top(_), l), And(_, _, r)) => Some((B.Rewrites(s, l, r), None))
      case Implies(_, Bottom(_), p) => splitTop(p)
      case _ => None
    }
  }

  private def splitFunction(symbol: SymbolOrAlias, topPattern: Pattern): Option[(Equals, Option[Pattern])] = {
    topPattern match {
      case Implies(_, Equals(_, _, pat, _), And(_, eq @ Equals(_, _, Application(`symbol`, _), _), _)) => Some(eq, Some(pat))
      case Implies(_, Top(_), And(_, eq @ Equals(_, _, Application(`symbol`, _), _), _)) => Some(eq, None)
      case Implies(_, And(_, _, Equals(_, _, pat, _)), And(_, eq @ Equals(_, _, Application(`symbol`, _), _), _)) => Some(eq, Some(pat))
      case Implies(_, And(_, _, Top(_)), And(_, eq @ Equals(_, _, Application(`symbol`, _), _), _)) => Some(eq, None)
      case eq @ Equals(_, _, Application(`symbol`, _), _) => Some(eq, None)
      case _ => None
    }
  }

  private def getAxioms(defn: Definition) : Seq[AxiomDeclaration] = {
    defn.modules.flatMap(_.decls).filter(_.isInstanceOf[AxiomDeclaration]).map(_.asInstanceOf[AxiomDeclaration])
  }

  def parseTopAxioms(defn: Definition) : IndexedSeq[AxiomInfo] = {
    val axioms = getAxioms(defn).zipWithIndex
    val withOwise = axioms.flatMap(parseAxiomSentence(splitTop, _))
    withOwise.sortWith(_.priority < _.priority).toIndexedSeq
  }

  def parseFunctionAxioms(defn: Definition, symbol: SymbolOrAlias) : IndexedSeq[AxiomInfo] = {
    val axioms = getAxioms(defn).zipWithIndex
    val withOwise = axioms.flatMap(parseAxiomSentence(a => splitFunction(symbol, a), _))
    withOwise.sortWith(_.priority < _.priority).toIndexedSeq
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

  def parseSymbols(defn: Definition) : SymLib = {
    val axioms = getAxioms(defn)
    val symbols = axioms.flatMap(a => parsePatternForSymbols(a.pattern))
    val allSorts = symbols.flatMap(_.params)
    val overloads = getOverloads(axioms)
    new SymLib(symbols, allSorts, defn, overloads)
  }
}
