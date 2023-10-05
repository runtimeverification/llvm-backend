package org.kframework.backend.llvm.matching

import org.kframework.attributes.{Source, Location}
import org.kframework.parser.kore._
import org.kframework.parser.kore.parser.KoreToK
import org.kframework.parser.kore.implementation.{DefaultBuilders => B}
import java.util
import java.util.Optional

case class AxiomInfo(priority: Int, ordinal: Int, rewrite: GeneralizedRewrite, sideCondition: Option[Pattern], ensures: Option[Pattern], source: Optional[Source], location: Optional[Location], att: Attributes) {}

object Parser {

  def hasAtt(axiom: AxiomDeclaration, att: String): Boolean = {
    hasAtt(axiom.att, att)
  }

  def hasAtt(att: Attributes, attName: String): Boolean = {
    getAtt(att, attName).isDefined
  }

  def getAtt(axiom: AxiomDeclaration, att: String): Option[Pattern] = {
    getAtt(axiom.att, att)
  }

  def getAtt(att: Attributes, attName: String): Option[Pattern] = {
    att.patterns.find(isAtt(attName, _))
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

    def isHooked(symbol: SymbolOrAlias): Boolean = {
      return symbolDecls(symbol.ctr).head.isInstanceOf[HookSymbolDeclaration]
    }

    private def instantiate(s: Seq[Sort], params: Seq[Sort], args: Seq[Sort]): Seq[Sort] = s.map(instantiate(_, params, args))

    val signatures: Map[SymbolOrAlias, (Seq[Sort], Sort, Attributes)] = {
      symbols.map(symbol => {
        if (symbol.ctr == "\\dv") {
          (symbol, (Seq(), symbol.params(0), B.Attributes(Seq())))
        } else {
          (symbol, (instantiate(symbolDecls(symbol.ctr).head.argSorts, symbolDecls(symbol.ctr).head.symbol.params, symbol.params), instantiate(symbolDecls(symbol.ctr).head.returnSort, symbolDecls(symbol.ctr).head.symbol.params, symbol.params), symbolDecls(symbol.ctr).head.att))
        }
      }).toMap
    }

    val constructorsForSort: Map[Sort, Seq[SymbolOrAlias]] = {
      signatures.groupBy(_._2._2).mapValues(_.keys.filter(k => !hasAtt(signatures(k)._3, "function")).toSeq)
    }

    private val sortAttData: Map[String, Attributes] = {
      sorts.filter(_.isInstanceOf[CompoundSort]).map(sort => (sort.asInstanceOf[CompoundSort].ctr, sortDecls(sort.asInstanceOf[CompoundSort].ctr).head.att)).toMap
    }

    def sortAtt(s: Sort): Attributes = {
      sortAttData(s.asInstanceOf[CompoundSort].ctr)
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

    private val hookAtts: Map[String, String] = sortAttData.map(t => (t._1.substring(4), getStringAtt(t._2, "hook").getOrElse(""))).toMap

    val koreToK = new KoreToK(hookAtts)
  }

  private def rulePriority(axiom: AxiomDeclaration, search: Boolean): Int = {
    if (hasAtt(axiom, "owise")) 200
    else if (hasAtt(axiom, "cool") && !search) 150
    else if (hasAtt(axiom, "cool-like") && !search) 100
    else if (hasAtt(axiom, "priority")) getStringAtt(axiom.att, "priority").get.toInt
    else 50
  }

  private val SOURCE = "org'Stop'kframework'Stop'attributes'Stop'Source"
  private val LOCATION = "org'Stop'kframework'Stop'attributes'Stop'Location"

  def source(att: Attributes): Optional[Source] = {
    if (hasAtt(att, SOURCE)) {
      val sourceStr = getStringAtt(att, SOURCE).get
      return Optional.of(Source(sourceStr.substring("Source(".length, sourceStr.length - 1)))
    } else {
      Optional.empty()
    }
  }

  def location(att: Attributes): Optional[Location] = {
    if (hasAtt(att, LOCATION)) {
      val locStr = getStringAtt(att, LOCATION).get
      val splitted = locStr.split("[(,)]")
      return Optional.of(Location(splitted(1).toInt, splitted(2).toInt, splitted(3).toInt, splitted(4).toInt))
    } else {
      Optional.empty()
    }
  }

  private def source(axiom: AxiomDeclaration): Optional[Source] = source(axiom.att)
  private def location(axiom: AxiomDeclaration): Optional[Location] = location(axiom.att)

  private def parseAxiomSentence[T <: GeneralizedRewrite](
      split: Pattern => Option[(Option[SymbolOrAlias], T, Option[Pattern], Option[Pattern])],
      axiom: (AxiomDeclaration, Int),
      simplification: Boolean,
      search: Boolean) :
      Seq[(Option[SymbolOrAlias], AxiomInfo)] = {
    val splitted = split(axiom._1.pattern)
    if (splitted.isDefined) {
      val s = axiom._1
      if (hasAtt(s, "comm") || hasAtt(s, "assoc") || hasAtt(s, "idem") || hasAtt(s, "unit") || hasAtt(s, "non-executable") || (hasAtt(s, "simplification") && !simplification)) {
        Seq()
      } else {
        Seq((splitted.get._1, AxiomInfo(rulePriority(s, search), axiom._2, splitted.get._2, splitted.get._3, splitted.get._4, source(s), location(s), s.att)))
      }
    } else {
      Seq()
    }
  }

  private def splitTop(topPattern: Pattern): Option[(Option[SymbolOrAlias], Rewrites, Option[Pattern], Option[Pattern])] = {
    topPattern match {
      case Rewrites(s, And(_, (req @ Equals(_, _, _, _)) +: l +: Seq()), And(_, ens +: r +: Seq())) => Some((None, B.Rewrites(s, l, r), splitPredicate(req), splitPredicate(ens)))
      case Rewrites(s, And(_, (req @ Top(_)) +: l +: Seq()), And(_, ens +: r +: Seq())) => Some((None, B.Rewrites(s, l, r), splitPredicate(req), splitPredicate(ens)))
      case Rewrites(s, And(_, Not(_, _) +: And(_, req +: l +: Seq()) +: Seq()), And(_, ens +: r +: Seq())) => Some((None, B.Rewrites(s, l, r), splitPredicate(req), splitPredicate(ens)))
      case Rewrites(s, And(_, l +: req +: Seq()), And(_, r +: ens +: Seq())) => Some((None, B.Rewrites(s, l, r), splitPredicate(req), splitPredicate(ens)))
      case _ => None
    }
  }

  private def splitPredicate(pat: Pattern): Option[Pattern] = {
    pat match {
      case Top(_) => None
      case Equals(_, _, pat, _) => Some(pat)
    }
  }

  private def getPatterns(pat: Pattern): List[Pattern] = {
    pat match {
      case And(_, Mem(_, _, _, pat) +: pats +: Seq()) => pat :: getPatterns(pats)
      case Top(_) => Nil
    }
  }

  private def splitFunction(topPattern: Pattern): Option[(Option[SymbolOrAlias], Equals, Option[Pattern], Option[Pattern])] = {
    topPattern match {
      case Implies(_, And(_, Not(_, _) +: And (_, req +: args +: Seq()) +: Seq()), Equals(i, o, Application(symbol, _), And(_, rhs +: ens +: Seq()))) => Some(Some(symbol), B.Equals(i, o, B.Application(symbol, getPatterns(args)), rhs), splitPredicate(req), splitPredicate(ens))
      case Implies(_, And(_, req +: args +: Seq()), Equals(i, o, Application(symbol, _), And(_, rhs +: ens +: Seq()))) => Some(Some(symbol), B.Equals(i, o, B.Application(symbol, getPatterns(args)), rhs), splitPredicate(req), splitPredicate(ens))
      case Implies(_, req, Equals(i, o, app @ Application(symbol, _), And(_, rhs +: ens +: Seq()))) => Some(Some(symbol), B.Equals(i, o, app, rhs), splitPredicate(req), splitPredicate(ens))
      case Implies(_, req, eq @ Equals(_, _, Application(symbol, _), _)) => Some(Some(symbol), eq, splitPredicate(req), None)
      case eq @ Equals(_, _, Application(symbol, _), _) => Some(Some(symbol), eq, None, None)
      case _ => None
    }
  }

  private def getSubstitution(pat: Pattern, subject: Seq[Pattern]): Map[String, Pattern] = {
    val pattern = pat.asInstanceOf[Application]
    (pattern.args.map(_.asInstanceOf[Variable].name) zip subject).toMap
  }

  private def substitute(pat: Pattern, subst: Map[String, Pattern]): Pattern = {
    pat match {
      case Variable(name, _) => subst.getOrElse(name, pat)
      case Application(head, args) => B.Application(head, args.map(substitute(_, subst)))
      case And(s, args)=> B.And(s, args.map(substitute(_, subst)))
      case Or(s, args) => B.Or(s, args.map(substitute(_, subst)))
      case Not(s, p) => B.Not(s, substitute(p, subst))
      case Implies(s, l, r) => B.Implies(s, substitute(l, subst), substitute(r, subst))
      case Iff(s, l, r) => B.Iff(s, substitute(l, subst), substitute(r, subst))
      case Exists(s, v, p) => B.Exists(s, v, substitute(p, subst - v.name))
      case Forall(s, v, p) => B.Forall(s, v, substitute(p, subst - v.name))
      case Ceil(s1, s2, p) => B.Ceil(s1, s2, substitute(p, subst))
      case Floor(s1, s2, p) => B.Floor(s1, s2, substitute(p, subst))
      case Rewrites(s, l, r) => B.Rewrites(s, substitute(l, subst), substitute(r, subst))
      case Equals(s1, s2, l, r) => B.Equals(s1, s2, substitute(l, subst), substitute(r, subst))
      case Mem(s1, s2, l, r) => B.Mem(s1, s2, substitute(l, subst), substitute(r, subst))
      case _ => pat
    }
  }

  private def expandAliases(pat: Pattern, aliases: Map[String, AliasDeclaration]): Pattern = {
    pat match {
      case Application(head, args) =>
        if (aliases.contains(head.ctr)) {
          val alias = aliases(head.ctr)
          val subst = getSubstitution(alias.leftPattern, args)
          expandAliases(substitute(alias.rightPattern, subst), aliases)
        } else if (args.isEmpty) {
          pat
        } else {
          B.Application(head, args.map(expandAliases(_, aliases)))
        }
      case And(s, args) => B.And(s, args.map(expandAliases(_, aliases)))
      case Or(s, args) => B.Or(s, args.map(expandAliases(_, aliases)))
      case Not(s, p) => B.Not(s, expandAliases(p, aliases))
      case Implies(s, l, r) => B.Implies(s, expandAliases(l, aliases), expandAliases(r, aliases))
      case Iff(s, l, r) => B.Iff(s, expandAliases(l, aliases), expandAliases(r, aliases))
      case Exists(s, v, p) => B.Exists(s, v, expandAliases(p, aliases))
      case Forall(s, v, p) => B.Forall(s, v, expandAliases(p, aliases))
      case Ceil(s1, s2, p) => B.Ceil(s1, s2, expandAliases(p, aliases))
      case Floor(s1, s2, p) => B.Floor(s1, s2, expandAliases(p, aliases))
      case Rewrites(s, l, r) => B.Rewrites(s, expandAliases(l, aliases), expandAliases(r, aliases))
      case Equals(s1, s2, l, r) => B.Equals(s1, s2, expandAliases(l, aliases), expandAliases(r, aliases))
      case Mem(s1, s2, l, r) => B.Mem(s1, s2, expandAliases(l, aliases), expandAliases(r, aliases))
      case _ => pat
    }
  }

  def expandAliases(axiom: AxiomDeclaration, aliases: Map[String, AliasDeclaration]) : AxiomDeclaration = { 
    B.AxiomDeclaration(axiom.params, expandAliases(axiom.pattern, aliases), axiom.att).asInstanceOf[AxiomDeclaration]
  }

  def getAxioms(defn: Definition) : Seq[AxiomDeclaration] = {
    val aliases = defn.modules.flatMap(_.decls).filter(_.isInstanceOf[AliasDeclaration]).map(_.asInstanceOf[AliasDeclaration]).map(al => (al.alias.ctr, al)).toMap
    defn.modules.flatMap(_.decls).filter(_.isInstanceOf[AxiomDeclaration]).map(_.asInstanceOf[AxiomDeclaration]).map(expandAliases(_, aliases))
  }

  def getSorts(defn: Definition): Seq[Sort] = {
    defn.modules.flatMap(_.decls).filter(_.isInstanceOf[SortDeclaration]).map(_.asInstanceOf[SortDeclaration].sort)
  }

  def parseTopAxioms(axioms: Seq[AxiomDeclaration], search: Boolean) : IndexedSeq[AxiomInfo] = {
    val withOwise = axioms.zipWithIndex.flatMap(parseAxiomSentence(splitTop, _, false, search))
    withOwise.map(_._2).sortWith(_.priority < _.priority).toIndexedSeq
  }

  def parseFunctionAxioms(axioms: Seq[AxiomDeclaration], simplification: Boolean) : Map[SymbolOrAlias, IndexedSeq[AxiomInfo]] = {
    val withOwise = axioms.zipWithIndex.flatMap(parseAxiomSentence(a => splitFunction(a), _, simplification, true))
    withOwise.sortWith(_._2.priority < _._2.priority).toIndexedSeq.filter(_._1.isDefined).map(t => (t._1.get, t._2)).groupBy(_._1).mapValues(_.map(_._2))
  }

  private def isConcrete(symbol: SymbolOrAlias) : Boolean = {
    symbol.params.forall(_.isInstanceOf[CompoundSort])
  }

  private def parsePatternForSymbols(pat: Pattern): Seq[SymbolOrAlias] = {
    pat match {
      case And(_, ps) => ps.flatMap(parsePatternForSymbols)
      case Application(s, ps) => Seq(s).filter(isConcrete) ++ ps.flatMap(parsePatternForSymbols)
      case DomainValue(sort, _) => Seq(B.SymbolOrAlias("\\dv", Seq(sort)))
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
      case Or(_, ps) => ps.flatMap(parsePatternForSymbols)
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

  val heuristicMap: Map[Char, Heuristic] = {
    import scala.reflect.runtime.universe

    val heuristicType = universe.typeOf[Heuristic]
    val heuristicClass = heuristicType.typeSymbol.asClass
    val pseudoHeuristicType = universe.typeOf[PseudoHeuristic]
    val pseudoHeuristicClass = pseudoHeuristicType.typeSymbol.asClass
    val runtimeMirror = universe.runtimeMirror(getClass.getClassLoader)
    val classes = heuristicClass.knownDirectSubclasses.filter(!_.asClass.isTrait) ++ pseudoHeuristicClass.knownDirectSubclasses
    classes.map(c => {
        val name = c.annotations.head.tree.children.tail.head.children.tail.collect({ case universe.Literal(universe.Constant(id: Char)) => id }).head
        val symbol = c.asClass.module.asModule
        val moduleMirror = runtimeMirror.reflectModule(symbol)
        val obj = moduleMirror.instance.asInstanceOf[Heuristic]
        name -> obj
      }).toMap
  }

  def parseHeuristic(heuristic: Char): Heuristic = {
    heuristicMap(heuristic)
  }

  def parseHeuristics(heuristics: String): Seq[Heuristic] = {
    heuristics.toList.map(parseHeuristic(_))
  }

  def parseSymbols(defn: Definition, heuristics: String) : SymLib = {
    val axioms = getAxioms(defn)
    val symbols = axioms.flatMap(a => parsePatternForSymbols(a.pattern))
    val allSorts = getSorts(defn)
    val overloads = getOverloads(axioms)
    new SymLib(symbols, allSorts, defn, overloads, parseHeuristics(heuristics))
  }
}
