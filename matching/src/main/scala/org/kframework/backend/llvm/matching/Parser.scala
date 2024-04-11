package org.kframework.backend.llvm.matching

import com.runtimeverification.k.kore._
import com.runtimeverification.k.kore.implementation.{ DefaultBuilders => B }
import java.util
import java.util.Optional
import scala.collection.immutable

case class AxiomInfo(
    priority: Int,
    ordinal: Int,
    rewrite: GeneralizedRewrite,
    sideCondition: Option[Pattern],
    ensures: Option[Pattern],
    source: Optional[Source],
    location: Optional[Location],
    att: Attributes
) {}

object Parser {

  def hasAtt(axiom: AxiomDeclaration, att: String): Boolean =
    hasAtt(axiom.att, att)

  def hasAtt(att: Attributes, attName: String): Boolean =
    getAtt(att, attName).isDefined

  def getAtt(axiom: AxiomDeclaration, att: String): Option[Pattern] =
    getAtt(axiom.att, att)

  def getAtt(att: Attributes, attName: String): Option[Pattern] =
    att.patterns.find(isAtt(attName, _))

  def getStringAtt(att: Attributes, attName: String): Option[String] =
    att.patterns
      .find(isAtt(attName, _))
      .map(_.asInstanceOf[Application].args.head.asInstanceOf[StringLiteral].str)

  def getSymbolAtt(att: Attributes, attName: String): Option[SymbolOrAlias] =
    att.patterns
      .find(isAtt(attName, _))
      .map(_.asInstanceOf[Application].args.head.asInstanceOf[Application].head)

  private def isAtt(att: String, pat: Pattern): Boolean =
    pat match {
      case Application(SymbolOrAlias(x, _), _) => x == att
      case _                                   => false
    }

  class SymLib(
      symbols: immutable.Seq[SymbolOrAlias],
      sorts: immutable.Seq[Sort],
      mod: Definition,
      overloadSeq: immutable.Seq[(SymbolOrAlias, SymbolOrAlias)],
      val heuristics: immutable.Seq[Heuristic]
  ) {
    val sortCache = new util.HashMap[Sort, SortInfo]()

    private val symbolDecls = mod.modules
      .flatMap(_.decls)
      .filter(_.isInstanceOf[SymbolDeclaration])
      .map(_.asInstanceOf[SymbolDeclaration])
      .groupBy(_.symbol.ctr)

    private val sortDecls = mod.modules
      .flatMap(_.decls)
      .filter(_.isInstanceOf[SortDeclaration])
      .map(_.asInstanceOf[SortDeclaration])
      .groupBy(_.sort.asInstanceOf[CompoundSort].ctr)

    private def instantiate(
        s: Sort,
        params: immutable.Seq[Sort],
        args: immutable.Seq[Sort]
    ): Sort = {
      val map = params.lazyZip(args).toMap
      s match {
        case v @ SortVariable(_) => map(v)
        case _                   => s
      }
    }

    def isHooked(symbol: SymbolOrAlias): Boolean =
      symbolDecls(symbol.ctr).head.isInstanceOf[HookSymbolDeclaration]

    private def instantiate(
        s: immutable.Seq[Sort],
        params: immutable.Seq[Sort],
        args: immutable.Seq[Sort]
    ): immutable.Seq[Sort] =
      s.map(instantiate(_, params, args))

    val signatures: Map[SymbolOrAlias, (immutable.Seq[Sort], Sort, Attributes)] =
      symbols.map { symbol =>
        if (symbol.ctr == "\\dv") {
          (symbol, (immutable.Seq(), symbol.params.head, B.Attributes(immutable.Seq())))
        } else {
          (
            symbol,
            (
              instantiate(
                symbolDecls(symbol.ctr).head.argSorts,
                symbolDecls(symbol.ctr).head.symbol.params,
                symbol.params
              ),
              instantiate(
                symbolDecls(symbol.ctr).head.returnSort,
                symbolDecls(symbol.ctr).head.symbol.params,
                symbol.params
              ),
              symbolDecls(symbol.ctr).head.att
            )
          )
        }
      }.toMap

    val constructorsForSort: Map[Sort, immutable.Seq[SymbolOrAlias]] =
      signatures
        .groupBy(_._2._2)
        .view
        .mapValues(_.keys.filter(k => !hasAtt(signatures(k)._3, "function")).to(immutable.Seq))
        .toMap

    private val sortAttData: Map[String, Attributes] =
      sorts
        .filter(_.isInstanceOf[CompoundSort])
        .map(sort =>
          (
            sort.asInstanceOf[CompoundSort].ctr,
            sortDecls(sort.asInstanceOf[CompoundSort].ctr).head.att
          )
        )
        .toMap

    def sortAtt(s: Sort): Attributes =
      sortAttData(s.asInstanceOf[CompoundSort].ctr)

    val functions: immutable.Seq[SymbolOrAlias] =
      signatures
        .filter(s =>
          s._2._3.patterns.exists(isAtt("anywhere", _)) || s._2._3.patterns
            .exists(isAtt("function", _))
        )
        .keys
        .to(immutable.Seq)

    val overloads: Map[SymbolOrAlias, immutable.Seq[SymbolOrAlias]] =
      overloadSeq.groupBy(_._1).view.mapValues(_.map(_._2).to(immutable.Seq)).toMap

    def isSubsorted(less: Sort, greater: Sort): Boolean =
      signatures.contains(B.SymbolOrAlias("inj", immutable.Seq(less, greater)))

    private val hookAtts: Map[String, String] =
      sortAttData.map(t => (t._1.substring(4), getStringAtt(t._2, "hook").getOrElse("")))
  }

  private def rulePriority(axiom: AxiomDeclaration, search: Boolean): Int =
    if (hasAtt(axiom, "owise")) 200
    else if (hasAtt(axiom, "cool") && !search) 150
    else if (hasAtt(axiom, "cool-like") && !search) 100
    else if (hasAtt(axiom, "priority")) getStringAtt(axiom.att, "priority").get.toInt
    else 50

  private val SOURCE   = "org'Stop'kframework'Stop'attributes'Stop'Source"
  private val LOCATION = "org'Stop'kframework'Stop'attributes'Stop'Location"

  def source(att: Attributes): Optional[Source] =
    if (hasAtt(att, SOURCE)) {
      val sourceStr = getStringAtt(att, SOURCE).get
      Optional.of(new Source(sourceStr.substring("Source(".length, sourceStr.length - 1)))
    } else {
      Optional.empty()
    }

  def location(att: Attributes): Optional[Location] =
    if (hasAtt(att, LOCATION)) {
      val locStr   = getStringAtt(att, LOCATION).get
      val splitted = locStr.split("[(,)]")
      Optional.of(
        new Location(splitted(1).toInt, splitted(2).toInt, splitted(3).toInt, splitted(4).toInt)
      )
    } else {
      Optional.empty()
    }

  private def source(axiom: AxiomDeclaration): Optional[Source]     = source(axiom.att)
  private def location(axiom: AxiomDeclaration): Optional[Location] = location(axiom.att)

  private def parseAxiomSentence[T <: GeneralizedRewrite](
      split: Pattern => Option[(Option[SymbolOrAlias], T, Option[Pattern], Option[Pattern])],
      axiom: (AxiomDeclaration, Int),
      simplification: Boolean,
      search: Boolean
  ): immutable.Seq[(Option[SymbolOrAlias], AxiomInfo)] = {
    val splitted = split(axiom._1.pattern)
    if (splitted.isDefined) {
      val s = axiom._1
      if (
        hasAtt(s, "comm") || hasAtt(s, "assoc") || hasAtt(s, "idem") || hasAtt(s, "unit") || hasAtt(
          s,
          "non-executable"
        ) || (hasAtt(s, "simplification") && !simplification)
      ) {
        immutable.Seq()
      } else {
        immutable.Seq(
          (
            splitted.get._1,
            AxiomInfo(
              rulePriority(s, search),
              axiom._2,
              splitted.get._2,
              splitted.get._3,
              splitted.get._4,
              source(s),
              location(s),
              s.att
            )
          )
        )
      }
    } else {
      immutable.Seq()
    }
  }

  private def splitTop(
      topPattern: Pattern
  ): Option[(Option[SymbolOrAlias], Rewrites, Option[Pattern], Option[Pattern])] =
    topPattern match {
      case Rewrites(
            s,
            And(_, (req @ Equals(_, _, _, _)) +: l +: immutable.Seq()),
            And(_, ens +: r +: immutable.Seq())
          ) =>
        Some((None, B.Rewrites(s, l, r), splitPredicate(req), splitPredicate(ens)))
      case Rewrites(
            s,
            And(_, (req @ Top(_)) +: l +: immutable.Seq()),
            And(_, ens +: r +: immutable.Seq())
          ) =>
        Some((None, B.Rewrites(s, l, r), splitPredicate(req), splitPredicate(ens)))
      case Rewrites(
            s,
            And(_, Not(_, _) +: And(_, req +: l +: immutable.Seq()) +: immutable.Seq()),
            And(_, ens +: r +: immutable.Seq())
          ) =>
        Some((None, B.Rewrites(s, l, r), splitPredicate(req), splitPredicate(ens)))
      case Rewrites(s, And(_, l +: req +: immutable.Seq()), And(_, r +: ens +: immutable.Seq())) =>
        Some((None, B.Rewrites(s, l, r), splitPredicate(req), splitPredicate(ens)))
      case Rewrites(s, And(_, l +: req +: immutable.Seq()), r @ Application(_, _)) =>
        Some((None, B.Rewrites(s, l, r), splitPredicate(req), None))
      case _ => None
    }

  private def splitPredicate(pat: Pattern): Option[Pattern] =
    pat match {
      case Top(_)                                                              => None
      case Equals(_, _, pat, DomainValue(CompoundSort("SortBool", _), "true")) => Some(pat)
      case _                                                                   => Some(pat)
    }

  private def getPatterns(pat: Pattern): List[Pattern] =
    pat match {
      case And(_, Mem(_, _, _, pat) +: pats +: immutable.Seq()) => pat :: getPatterns(pats)
      case Top(_)                                               => Nil
    }

  private def splitFunction(
      topPattern: Pattern
  ): Option[(Option[SymbolOrAlias], Equals, Option[Pattern], Option[Pattern])] =
    topPattern match {
      case Implies(
            _,
            And(_, Not(_, _) +: And(_, req +: args +: immutable.Seq()) +: immutable.Seq()),
            Equals(i, o, Application(symbol, _), And(_, rhs +: ens +: immutable.Seq()))
          ) =>
        Some(
          Some(symbol),
          B.Equals(i, o, B.Application(symbol, getPatterns(args)), rhs),
          splitPredicate(req),
          splitPredicate(ens)
        )
      case Implies(
            _,
            And(_, req +: args +: immutable.Seq()),
            Equals(i, o, Application(symbol, _), And(_, rhs +: ens +: immutable.Seq()))
          ) =>
        Some(
          Some(symbol),
          B.Equals(i, o, B.Application(symbol, getPatterns(args)), rhs),
          splitPredicate(req),
          splitPredicate(ens)
        )
      case Implies(
            _,
            req,
            Equals(i, o, app @ Application(symbol, _), And(_, rhs +: ens +: immutable.Seq()))
          ) =>
        Some(Some(symbol), B.Equals(i, o, app, rhs), splitPredicate(req), splitPredicate(ens))
      case Implies(_, req, eq @ Equals(_, _, Application(symbol, _), _)) =>
        Some(Some(symbol), eq, splitPredicate(req), None)
      case eq @ Equals(_, _, Application(symbol, _), _) => Some(Some(symbol), eq, None, None)
      case _                                            => None
    }

  private def getSubstitution(
      pat: Pattern,
      subject: immutable.Seq[Pattern]
  ): Map[String, Pattern] = {
    val pattern = pat.asInstanceOf[Application]
    pattern.args.map(_.asInstanceOf[Variable].name).zip(subject).toMap
  }

  private def substitute(pat: Pattern, subst: Map[String, Pattern]): Pattern =
    pat match {
      case Variable(name, _)       => subst.getOrElse(name, pat)
      case Application(head, args) => B.Application(head, args.map(substitute(_, subst)))
      case And(s, args)            => B.And(s, args.map(substitute(_, subst)))
      case Or(s, args)             => B.Or(s, args.map(substitute(_, subst)))
      case Not(s, p)               => B.Not(s, substitute(p, subst))
      case Implies(s, l, r)        => B.Implies(s, substitute(l, subst), substitute(r, subst))
      case Iff(s, l, r)            => B.Iff(s, substitute(l, subst), substitute(r, subst))
      case Exists(s, v, p)         => B.Exists(s, v, substitute(p, subst - v.name))
      case Forall(s, v, p)         => B.Forall(s, v, substitute(p, subst - v.name))
      case Ceil(s1, s2, p)         => B.Ceil(s1, s2, substitute(p, subst))
      case Floor(s1, s2, p)        => B.Floor(s1, s2, substitute(p, subst))
      case Rewrites(s, l, r)       => B.Rewrites(s, substitute(l, subst), substitute(r, subst))
      case Equals(s1, s2, l, r)    => B.Equals(s1, s2, substitute(l, subst), substitute(r, subst))
      case Mem(s1, s2, l, r)       => B.Mem(s1, s2, substitute(l, subst), substitute(r, subst))
      case _                       => pat
    }

  private def expandAliases(pat: Pattern, aliases: Map[String, AliasDeclaration]): Pattern =
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
      case And(s, args)      => B.And(s, args.map(expandAliases(_, aliases)))
      case Or(s, args)       => B.Or(s, args.map(expandAliases(_, aliases)))
      case Not(s, p)         => B.Not(s, expandAliases(p, aliases))
      case Implies(s, l, r)  => B.Implies(s, expandAliases(l, aliases), expandAliases(r, aliases))
      case Iff(s, l, r)      => B.Iff(s, expandAliases(l, aliases), expandAliases(r, aliases))
      case Exists(s, v, p)   => B.Exists(s, v, expandAliases(p, aliases))
      case Forall(s, v, p)   => B.Forall(s, v, expandAliases(p, aliases))
      case Ceil(s1, s2, p)   => B.Ceil(s1, s2, expandAliases(p, aliases))
      case Floor(s1, s2, p)  => B.Floor(s1, s2, expandAliases(p, aliases))
      case Rewrites(s, l, r) => B.Rewrites(s, expandAliases(l, aliases), expandAliases(r, aliases))
      case Equals(s1, s2, l, r) =>
        B.Equals(s1, s2, expandAliases(l, aliases), expandAliases(r, aliases))
      case Mem(s1, s2, l, r) => B.Mem(s1, s2, expandAliases(l, aliases), expandAliases(r, aliases))
      case _                 => pat
    }

  def expandAliases(
      axiom: AxiomDeclaration,
      aliases: Map[String, AliasDeclaration]
  ): AxiomDeclaration =
    B.AxiomDeclaration(axiom.params, expandAliases(axiom.pattern, aliases), axiom.att)
      .asInstanceOf[AxiomDeclaration]

  def getAxioms(defn: Definition): immutable.Seq[AxiomDeclaration] = {
    val aliases = defn.modules
      .flatMap(_.decls)
      .filter(_.isInstanceOf[AliasDeclaration])
      .map(_.asInstanceOf[AliasDeclaration])
      .map(al => (al.alias.ctr, al))
      .toMap
    defn.modules
      .flatMap(_.decls)
      .filter(_.isInstanceOf[AxiomDeclaration])
      .map(_.asInstanceOf[AxiomDeclaration])
      .map(expandAliases(_, aliases))
  }

  def getSorts(defn: Definition): immutable.Seq[Sort] =
    defn.modules
      .flatMap(_.decls)
      .filter(_.isInstanceOf[SortDeclaration])
      .map(_.asInstanceOf[SortDeclaration].sort)

  def parseTopAxioms(
      axioms: immutable.Seq[AxiomDeclaration],
      search: Boolean
  ): immutable.IndexedSeq[AxiomInfo] = {
    val withOwise = axioms.zipWithIndex.flatMap(
      parseAxiomSentence(splitTop, _, simplification = false, search = search)
    )
    withOwise.map(_._2).sortWith(_.priority < _.priority).toIndexedSeq
  }

  def parseFunctionAxioms(
      axioms: immutable.Seq[AxiomDeclaration],
      simplification: Boolean
  ): Map[SymbolOrAlias, immutable.IndexedSeq[AxiomInfo]] = {
    val withOwise = axioms.zipWithIndex.flatMap(
      parseAxiomSentence(a => splitFunction(a), _, simplification, search = true)
    )
    withOwise
      .sortWith(_._2.priority < _._2.priority)
      .toIndexedSeq
      .filter(_._1.isDefined)
      .map(t => (t._1.get, t._2))
      .groupBy(_._1)
      .view
      .mapValues(_.map(_._2))
      .toMap
  }

  private def isConcrete(symbol: SymbolOrAlias): Boolean =
    symbol.params.forall(_.isInstanceOf[CompoundSort])

  private def parsePatternForSymbols(pat: Pattern): immutable.Seq[SymbolOrAlias] =
    pat match {
      case And(_, ps) => ps.flatMap(parsePatternForSymbols)
      case Application(s, ps) =>
        immutable.Seq(s).filter(isConcrete) ++ ps.flatMap(parsePatternForSymbols)
      case DomainValue(sort, _) => immutable.Seq(B.SymbolOrAlias("\\dv", immutable.Seq(sort)))
      case Ceil(_, _, p)        => parsePatternForSymbols(p)
      case Equals(_, _, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case Exists(_, _, p)      => parsePatternForSymbols(p)
      case Floor(_, _, p)       => parsePatternForSymbols(p)
      case Forall(_, _, p)      => parsePatternForSymbols(p)
      case Iff(_, p1, p2)       => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case Implies(_, p1, p2)   => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case Mem(_, _, p1, p2)    => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
//      case Next(_, p) => parsePatternForSymbols(p)
      case Not(_, p)           => parsePatternForSymbols(p)
      case Or(_, ps)           => ps.flatMap(parsePatternForSymbols)
      case Rewrites(_, p1, p2) => parsePatternForSymbols(p1) ++ parsePatternForSymbols(p2)
      case _                   => immutable.Seq()
    }

  private def getOverloads(
      axioms: immutable.Seq[AxiomDeclaration]
  ): immutable.Seq[(SymbolOrAlias, SymbolOrAlias)] = {
    if (axioms.isEmpty) {
      immutable.Seq()
    }
    axioms
      .filter(hasAtt(_, "symbol-overload"))
      .map(getAtt(_, "symbol-overload") match {
        case Some(Application(_, args)) =>
          assert(args.size == 2)
          (args.head, args(1)) match {
            case (Application(g, _), Application(l, _)) => (g, l)
          }
        case _ => ???
      })
  }

  val heuristicMap: Map[Char, Heuristic] = {
    import scala.reflect.runtime.universe
    import universe.LiteralTag

    val heuristicType        = universe.typeOf[Heuristic]
    val heuristicClass       = heuristicType.typeSymbol.asClass
    val pseudoHeuristicType  = universe.typeOf[PseudoHeuristic]
    val pseudoHeuristicClass = pseudoHeuristicType.typeSymbol.asClass
    val runtimeMirror        = universe.runtimeMirror(getClass.getClassLoader)
    val classes = heuristicClass.knownDirectSubclasses.filter(
      !_.asClass.isTrait
    ) ++ pseudoHeuristicClass.knownDirectSubclasses
    classes.map { c =>
      val name = c.annotations.head.tree.children.tail.head.children.tail.collect {
        case universe.Literal(universe.Constant(id: Char)) => id
      }.head
      val symbol       = c.asClass.module.asModule
      val moduleMirror = runtimeMirror.reflectModule(symbol)
      val obj          = moduleMirror.instance.asInstanceOf[Heuristic]
      name -> obj
    }.toMap
  }

  def parseHeuristic(heuristic: Char): Heuristic =
    heuristicMap(heuristic)

  def parseHeuristics(heuristics: String): immutable.Seq[Heuristic] =
    heuristics.toList.map(parseHeuristic)

  def parseSymbols(defn: Definition, heuristics: String): SymLib = {
    val axioms    = getAxioms(defn)
    val symbols   = axioms.flatMap(a => parsePatternForSymbols(a.pattern))
    val allSorts  = getSorts(defn)
    val overloads = getOverloads(axioms)
    new SymLib(symbols, allSorts, defn, overloads, parseHeuristics(heuristics))
  }
}
