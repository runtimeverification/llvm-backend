package org.kframework.backend.llvm.matching.pattern

import org.kframework.parser.kore
import org.kframework.kore.K
import org.kframework.parser.kore.SymbolOrAlias
import org.kframework.parser.kore.implementation.{DefaultBuilders => B}
import org.kframework.backend.llvm.matching._
import scala.math.min

sealed trait Pattern[T] {
  def signature(clause: Clause): Seq[Constructor]
  def isWildcard: Boolean
  def isDefault: Boolean
  def isSpecialized(ix: Constructor, isExact: Boolean, fringe: Fringe, clause: Clause, maxPriority: Int): Boolean
  def score(h: Heuristic, fringe: Fringe, clause: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]]
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, maxPriority: Int): Seq[Pattern[T]]
  def expandOr: Seq[Pattern[T]]

  def isChoice: Boolean = false
  def mapOrSetKeys: Seq[Pattern[T]] = Seq()
  def listRange(ix: Option[Constructor], o: Occurrence): Seq[(Occurrence, Int, Int)] = Seq()
  def overloadChildren(f: Fringe, ix: Option[Constructor], residual: Option[Pattern[String]], o: Occurrence): Seq[(Constructor, VariableBinding[T])] = Seq()
  def category: Option[SortCategory]
  def variables: Set[T]
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]]
  def decanonicalize: Pattern[String]
  def isBound(clause: Clause): Boolean
  def isResidual(symlib: Parser.SymLib): Boolean
  def toShortString: String
  def toKORE(f: Fringe): kore.Pattern
  def toK(f: Fringe): K = {
    val kore = toKORE(f)
    f.symlib.koreToK(kore)
  }
}

object Pattern {
  def mightUnify[T](p1: Pattern[T], p2: Pattern[T]): Boolean = {
    (p1, p2) match {
      case (WildcardP(), _) | (_, WildcardP()) | (VariableP(_, _), _) | (_, VariableP(_, _)) => true
      case (OrP(ps), _) => ps.exists(Pattern.mightUnify(_, p2))
      case (_, OrP(ps)) => ps.exists(Pattern.mightUnify(p1, _))
      case (AsP(_, _, pat), _) => Pattern.mightUnify(pat, p2)
      case (_, AsP(_, _, pat)) => Pattern.mightUnify(p1, pat)
      case (LiteralP(c1, _), LiteralP(c2, _)) => c1 == c2
      case (SymbolP(c1, ps1), SymbolP(c2, ps2)) => c1 == c2 && (ps1, ps2).zipped.toIterable.forall(t => Pattern.mightUnify(t._1, t._2))
      case (ListP(_, _, _, _, _), ListP(_, _, _, _, _)) => true
      case (MapP(_, _, _, _, _), MapP(_, _, _, _, _)) => true
      case (SetP(_, _, _, _), SetP(_, _, _, _)) => true
      case (LiteralP(_, _) | SymbolP(_, _) | ListP(_, _, _, _, _) | MapP(_, _, _, _, _) | SetP(_, _, _, _), _) => false
    }
  }
}

case class AsP[T](name: T, sort: SortCategory, pat: Pattern[T]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = {
    pat.signature(clause)
  }
  def isWildcard: Boolean = pat.isWildcard
  def isDefault: Boolean = pat.isDefault
  def isSpecialized(ix: Constructor, isExact: Boolean, f: Fringe, c: Clause, m: Int): Boolean = {
    pat.isSpecialized(ix, isExact, f, c, m)
  }
  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreAs(this, f, c, key, isEmpty)
  override def isChoice: Boolean = pat.isChoice
  override def mapOrSetKeys: Seq[Pattern[T]] = pat.mapOrSetKeys
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = {
    Seq(new VariableBinding(name, sort, occurrence, residual)) ++ pat.bindings(ix, residual, occurrence, symlib)
  }
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, m: Int): Seq[Pattern[T]] = {
    pat.expand(ix, isExact, fringes, f, clause, m)
  }
  def expandOr: Seq[AsP[T]] = pat.expandOr.map(AsP(name, sort, _))

  override def overloadChildren(f: Fringe, ix: Option[Constructor], residual: Option[Pattern[String]], o: Occurrence): Seq[(Constructor, VariableBinding[T])] = pat.overloadChildren(f, ix, residual, o)
  def category: Option[SortCategory] = pat.category
  lazy val variables: Set[T] = Set(name) ++ pat.variables
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = AsP(clause.canonicalize(name.toString), sort, pat.canonicalize(clause))
  def decanonicalize: Pattern[String] = AsP("_", sort, pat.decanonicalize)
  def isBound(clause: Clause): Boolean = clause.isBound(name) && pat.isBound(clause)
  def isResidual(symlib: Parser.SymLib) = pat.isResidual(symlib)
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = pat.toShortString + " #as " + name.toString
  def toKORE(f: Fringe): kore.Pattern = {
    B.And(f.sort, pat.toKORE(f), B.Variable(name.toString, f.sort))
  }
}

case class ListP[T](head: Seq[Pattern[T]], frame: Option[Pattern[T]], tail: Seq[Pattern[T]], ctr: SymbolOrAlias, orig: Pattern[T]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = {
    (0 to (head.size + tail.size)).map(ListC(ctr, _))
  }
  def isWildcard = false
  def isDefault: Boolean = frame.isDefined
  def isSpecialized(ix: Constructor, isExact: Boolean, f: Fringe, c: Clause, m: Int): Boolean = {
    ix match {
      case listC: ListC =>
        val len = listC.length
        if (frame.isEmpty) {
          len == head.size + tail.size
        } else {
          // if the list has a frame, then matching lists longer than the current head and tail
          // is performed via the default case of the switch, which means that we need
          // to create a switch case per list of lesser length
          len >= head.size + tail.size
        }
      case _ =>
        false
    }
  }
  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreList(this, f, c, key, isEmpty)
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = {
    if (frame.isEmpty) {
      Seq()
    } else ix.get match {
      case listC: ListC =>
        val len = listC.length
        frame.get.bindings(None, residual, Num(len, occurrence), symlib)
      case _ =>
        Seq()
    }
  }
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, m: Int): Seq[Pattern[T]] = {
    ix match {
      case ListC(_, len) =>
        head ++ (0 until len - head.size - tail.size).map(_ => WildcardP().asInstanceOf[Pattern[T]]) ++ tail
      case _ => ???
    }
  }
  def expandOr: Seq[Pattern[T]] = {
    val withHead = head.indices.foldLeft(Seq(this))((accum, ix) => accum.flatMap(l => l.head(ix).expandOr.map(p => ListP(l.head.updated(ix, p), l.frame, l.tail, ctr, orig))))
    val withTail = tail.indices.foldLeft(withHead)((accum, ix) => accum.flatMap(l => l.tail(ix).expandOr.map(p => ListP(l.head, l.frame, l.tail.updated(ix, p), ctr, orig))))
    if (frame.isDefined) {
      withTail.flatMap(l => l.frame.get.expandOr.map(p => ListP(l.head, Some(p), l.tail, ctr, orig)))
    } else {
      withTail
    }
  }

  override def listRange(ix: Option[Constructor], o: Occurrence): Seq[(Occurrence, Int, Int)] = ix match {
    case Some(ListC(_, len)) => Seq((Num(len, o), head.size, tail.size))
    case _ => Seq()
  }

  def category = Some(ListS())
  lazy val variables: Set[T] = head.flatMap(_.variables).toSet ++ tail.flatMap(_.variables).toSet ++ frame.map(_.variables).getOrElse(Set())
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = ListP(head.map(_.canonicalize(clause)), frame.map(_.canonicalize(clause)), tail.map(_.canonicalize(clause)), ctr, orig.canonicalize(clause))
  def decanonicalize: Pattern[String] = ListP(head.map(_.decanonicalize), frame.map(_.decanonicalize), tail.map(_.decanonicalize), ctr, orig.decanonicalize)
  def isBound(clause: Clause): Boolean = head.forall(_.isBound(clause)) && frame.forall(_.isBound(clause)) && tail.forall(_.isBound(clause))
  def isResidual(symlib: Parser.SymLib) = true
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = "L(" + head.size + " " + frame.isDefined + " " + tail.size + ")"
  def toKORE(f: Fringe): kore.Pattern = orig.toKORE(f)
}

case class LiteralP[T](literal: String, sort: SortCategory) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = {
    Seq(LiteralC(literal))
  }
  def isWildcard = false
  def isDefault = false
  def isSpecialized(ix: Constructor, isExact: Boolean, f: Fringe, c: Clause, m: Int): Boolean = {
    ix.isInstanceOf[LiteralC] && f.sortInfo.category.equal(ix.asInstanceOf[LiteralC].literal, literal)
  }

  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreLiteral(this, f, c, key, isEmpty)
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = Seq()
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, m: Int): Seq[Pattern[T]] = Seq()
  def expandOr: Seq[Pattern[T]] = Seq(this)

  def category = Some(sort)
  def variables: Set[T] = Set()
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = LiteralP(literal, sort)
  def decanonicalize: Pattern[String] = LiteralP(literal, sort)
  def isBound(clause: Clause): Boolean = true
  def isResidual(symlib: Parser.SymLib) = false
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = literal
  def toKORE(f: Fringe): kore.Pattern = {
    B.DomainValue(f.sort, literal)
  }
}

case class MapP[T](keys: Seq[Pattern[T]], values: Seq[Pattern[T]], frame: Option[Pattern[T]], ctr: SymbolOrAlias, orig: Pattern[T]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = {
    if (keys.isEmpty && frame.isEmpty) {
      Seq(Empty())
    } else if (keys.isEmpty) {
      frame.get.signature(clause)
    } else if (frame.isEmpty) {
      keys.flatMap(key => Seq(HasKey(isSet = false, ctr, clause.canonicalize(key)), HasNoKey(false, clause.canonicalize(key))))
    } else {
      keys.flatMap(key => Seq(HasKey(isSet = false, ctr, clause.canonicalize(key)), HasNoKey(false, clause.canonicalize(key)))) ++ frame.get.signature(clause)
    }
  }
  def isWildcard: Boolean = keys.isEmpty && values.isEmpty && frame.isDefined && frame.get.isWildcard
  def isDefault: Boolean = frame.isDefined || keys.nonEmpty || values.nonEmpty
  def isSpecialized(ix: Constructor, isExact: Boolean, fringe: Fringe, clause: Clause, maxPriority: Int): Boolean = {
    (ix, frame) match {
      case (Empty(), _) => keys.isEmpty && values.isEmpty
      case (HasKey(_, _, _), Some(_)) => true
      case (HasKey(_, _, Some(p)), None) => keys.map(_.canonicalize(clause)).exists(Pattern.mightUnify(p, _))
      case (HasNoKey(_, Some(p)), _) => !keys.map(_.canonicalize(clause)).contains(p)
      case (HasKey(_, _, None), None) => keys.nonEmpty && clause.action.priority <= maxPriority
      case (HasNoKey(_, None), _) => keys.nonEmpty && clause.action.priority > maxPriority
    }
  }
  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreMap(this, f, c, key, isEmpty)
  override def isChoice: Boolean = keys.nonEmpty
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = {
    if (keys.isEmpty && values.isEmpty && frame.isDefined) {
      frame.get.bindings(None, residual, occurrence, symlib)
    } else {
      Seq()
    }
  }
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, maxPriority: Int): Seq[Pattern[T]] = {
    ix match {
      case Empty() => Seq()
      case HasKey(_, _, Some(p)) =>
        val canonKs = keys.map(_.canonicalize(clause))
        canonKs.indexOf(p) match {
          case -1 => Seq(WildcardP(), WildcardP(), this)
          case i => Seq(values(i), MapP(keys.take(i) ++ keys.takeRight(keys.size - i - 1), values.take(i) ++ values.takeRight(values.size - i - 1), frame, ctr, orig), WildcardP())
        }
      case HasNoKey(_, _) | NonEmpty() => Seq(this)
      case HasKey(_, _, None) =>
        if (keys.isEmpty && frame.isDefined) {
          frame.get.expand(ix, isExact, fringes, f, clause, maxPriority)
        } else {
          Seq(keys.head, values.head, MapP(keys.tail, values.tail, frame, ctr, orig))
        }
    }
  }
  def expandOr: Seq[Pattern[T]] = {
    val withKeys = keys.indices.foldLeft(Seq(this))((accum, ix) => accum.flatMap(m => m.keys(ix).expandOr.map(p => MapP(m.keys.updated(ix, p), m.values, m.frame, ctr, orig))))
    val withValues = values.indices.foldLeft(withKeys)((accum, ix) => accum.flatMap(m => m.values(ix).expandOr.map(p => MapP(m.keys, m.values.updated(ix, p), m.frame, ctr, orig))))
    if (frame.isDefined) {
      withValues.flatMap(m => m.frame.get.expandOr.map(p => MapP(m.keys, m.values, Some(p), ctr, orig)))
    } else {
      withValues
    }
  }

  override def mapOrSetKeys: Seq[Pattern[T]] = keys

  def category = Some(MapS())
  lazy val variables: Set[T] = keys.flatMap(_.variables).toSet ++ values.flatMap(_.variables).toSet ++ frame.map(_.variables).getOrElse(Set())
  def canonicalize(clause: Clause): MapP[Option[Occurrence]] = MapP(keys.map(_.canonicalize(clause)), values.map(_.canonicalize(clause)), frame.map(_.canonicalize(clause)), ctr, orig.canonicalize(clause))
  def decanonicalize: MapP[String] = MapP(keys.map(_.decanonicalize), values.map(_.decanonicalize), frame.map(_.decanonicalize), ctr, orig.decanonicalize)
  def isBound(clause: Clause): Boolean = keys.forall(_.isBound(clause)) && values.forall(_.isBound(clause)) && frame.forall(_.isBound(clause))
  def isResidual(symlib: Parser.SymLib) = true
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = "M(" + keys.size + " " + frame.isDefined + ")"
  def toKORE(f: Fringe): kore.Pattern = orig.toKORE(f)
}

case class OrP[T](ps: Seq[Pattern[T]]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = ps.flatMap(_.signature(clause))
  def isWildcard: Boolean = ps.forall(_.isWildcard)
  def isDefault: Boolean = ???
  def isSpecialized(ix: Constructor, isExact: Boolean, f: Fringe, c: Clause, m: Int): Boolean = ???
  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreOr(this, f, c, key, isEmpty)
  override def isChoice: Boolean = ps.exists(_.isChoice)
  override def mapOrSetKeys: Seq[Pattern[T]] = ps.flatMap(_.mapOrSetKeys)
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = ???
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, m: Int): Seq[Pattern[T]] = ???
  def expandOr: Seq[Pattern[T]] = ps.flatMap(_.expandOr)
  def category: Option[SortCategory] = {
    val s = ps.map(_.category).filter(_.isDefined)
    if (s.isEmpty) {
      None
    } else {
      s.head
    }
  }
  lazy val variables: Set[T] = ps.flatMap(_.variables).toSet
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = OrP(ps.map(_.canonicalize(clause)))
  def decanonicalize: Pattern[String] = OrP(ps.map(_.decanonicalize))
  def isBound(clause: Clause): Boolean = ps.forall(_.isBound(clause))
  def isResidual(symlib: Parser.SymLib) = ???
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = ps.map(_.toShortString).mkString(" #Or ")
  def toKORE(f: Fringe): kore.Pattern = {
    ps.map(_.toKORE(f)).reduce((l, r) => B.Or(f.sort, l, r))
  }
}

object OrP {
  def apply[T](p1: Pattern[T], p2: Pattern[T]): OrP[T] = {
    (p1, p2) match {
      case (OrP(ps1), OrP(ps2)) => OrP(ps1 ++ ps2)
      case (OrP(ps1), _) => OrP(ps1 ++ Seq(p2))
      case (_, OrP(ps2)) => OrP(Seq(p1) ++ ps2)
      case _ => OrP(Seq(p1, p2))
    }
  }
}

case class SetP[T](elements: Seq[Pattern[T]], frame: Option[Pattern[T]], ctr: SymbolOrAlias, orig: Pattern[T]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = {
    if (elements.isEmpty && frame.isEmpty) {
      Seq(Empty())
    } else if (elements.isEmpty) {
      frame.get.signature(clause)
    } else if (frame.isEmpty) {
      elements.flatMap(elem => Seq(HasKey(isSet = true, ctr, clause.canonicalize(elem)), HasNoKey(true, clause.canonicalize(elem))))
    } else {
      elements.flatMap(elem => Seq(HasKey(isSet = true, ctr, clause.canonicalize(elem)), HasNoKey(true, clause.canonicalize(elem)))) ++ frame.get.signature(clause)
    }
  }
  def isWildcard: Boolean = elements.isEmpty && frame.isDefined && frame.get.isWildcard
  def isDefault: Boolean = frame.isDefined || elements.nonEmpty
  def isSpecialized(ix: Constructor, isExact: Boolean, fringe: Fringe, clause: Clause, maxPriority: Int): Boolean = {
    (ix, frame) match {
      case (Empty(), _) => elements.isEmpty
      case (HasKey(_, _, _), Some(_)) => true
      case (HasKey(_, _, Some(p)), None) => elements.map(_.canonicalize(clause)).exists(Pattern.mightUnify(p, _))
      case (HasNoKey(_, Some(p)), _) => !elements.map(_.canonicalize(clause)).contains(p)
      case (HasKey(_, _, None), None) => elements.nonEmpty && clause.action.priority <= maxPriority
      case (HasNoKey(_, None), _) => elements.nonEmpty && clause.action.priority > maxPriority
    }
  }
  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreSet(this, f, c, key, isEmpty)
  override def isChoice: Boolean = elements.nonEmpty
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = {
    if (elements.isEmpty && frame.isDefined) {
      frame.get.bindings(None, residual, occurrence, symlib)
    } else {
      Seq()
    }
  }
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, maxPriority: Int): Seq[Pattern[T]] = {
    ix match {
      case Empty() => Seq()
      case HasKey(_, _, Some(p)) =>
        val canonEs = elements.map(_.canonicalize(clause))
        canonEs.indexOf(p) match {
          case -1 => Seq(WildcardP(), this)
          case i => Seq(SetP(elements.take(i) ++ elements.takeRight(elements.size - i - 1), frame, ctr, orig), WildcardP())
        }
      case HasNoKey(_, _) | NonEmpty() => Seq(this)
      case HasKey(_, _, None) =>
        if (elements.isEmpty && frame.isDefined) {
          frame.get.expand(ix, isExact, fringes, f, clause, maxPriority)
        } else {
          Seq(elements.head, SetP(elements.tail, frame, ctr, orig))
        }
    }
  }
  def expandOr: Seq[Pattern[T]] = {
    val withElements = elements.indices.foldLeft(Seq(this))((accum, ix) => accum.flatMap(s => s.elements(ix).expandOr.map(p => SetP(s.elements.updated(ix, p), s.frame, ctr, orig))))
    if (frame.isDefined) {
      withElements.flatMap(s => s.frame.get.expandOr.map(p => SetP(s.elements, Some(p), ctr, orig)))
    } else {
      withElements
    }
  }

  override def mapOrSetKeys: Seq[Pattern[T]] = elements

  def category = Some(SetS())
  lazy val variables: Set[T] = elements.flatMap(_.variables).toSet ++ frame.map(_.variables).getOrElse(Set())
  def canonicalize(clause: Clause): SetP[Option[Occurrence]] = SetP(elements.map(_.canonicalize(clause)), frame.map(_.canonicalize(clause)), ctr, orig.canonicalize(clause))
  def decanonicalize: SetP[String] = SetP(elements.map(_.decanonicalize), frame.map(_.decanonicalize), ctr, orig.decanonicalize)
  def isBound(clause: Clause): Boolean = elements.forall(_.isBound(clause)) && frame.forall(_.isBound(clause))
  def isResidual(symlib: Parser.SymLib) = true
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = "S(" + elements.size + " " + frame.isDefined + ")"
  def toKORE(f: Fringe): kore.Pattern = orig.toKORE(f)
}

case class SymbolP[T](sym: SymbolOrAlias, ps: Seq[Pattern[T]]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = Seq(SymbolC(sym))
  def isWildcard = false
  def isDefault = false
  def isSpecialized(ix: Constructor, isExact: Boolean, f: Fringe, clause: Clause, m: Int): Boolean = {
    (ix, sym) match {
      case (SymbolC(SymbolOrAlias("inj",Seq(a,c))), SymbolOrAlias("inj",Seq(b,c2))) =>
        lazy val f2 = f.expand(SymbolC(sym)).head
        c == c2 && (a == b || (f.symlib.isSubsorted(b, a) && !isExact) || (f.symlib.isSubsorted(a, b) && f2.sortInfo.category == SymbolS() && ps.head.isSpecialized(SymbolC(B.SymbolOrAlias("inj",Seq(a,b))), isExact, f2, clause, m)))
      case (SymbolC(SymbolOrAlias("inj",_)), _) =>
        val less = f.overloads(sym)
        lazy val f2 = f.expand(ix).head
        less.exists(isValidOverload(f2, isExact, clause, m, f.expand(SymbolC(sym)), _))
      case (SymbolC(ix2), _) => ix2 == sym
    }
  }
  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreSymbol(this, f, c, key, isEmpty)
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = {
    (ix, sym) match {
      case (Some(SymbolC(SymbolOrAlias("inj",Seq(a,_)))), SymbolOrAlias("inj",Seq(b,_))) =>
        if (a == b || !symlib.isSubsorted(a, b)) {
          Seq()
        } else {
          ps.head.bindings(Some(SymbolC(B.SymbolOrAlias("inj",Seq(a,b)))), residual, occurrence, symlib)
        }
      case _ => Seq()
    }
  }
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, m: Int): Seq[Pattern[T]] = {
    (ix, sym) match {
      case (SymbolC(SymbolOrAlias("inj", Seq(a, _))), inj @ SymbolOrAlias("inj", Seq(b, _))) =>
        if (a == b) {
          Seq(ps.head)
        } else if (f.symlib.isSubsorted(a, b)) {
          ps.head.expand(SymbolC(B.SymbolOrAlias("inj", Seq(a, b))), isExact, fringes, f.expand(SymbolC(inj)).head, clause, m)
        } else {
          Seq(SymbolP(B.SymbolOrAlias("inj", Seq(b, a)), Seq(ps.head)))
        }
      case (SymbolC(SymbolOrAlias("inj", _)), _) =>
        val less = f.overloads(sym)
        val f2 = fringes.head
        val fringePs = f.expand(SymbolC(sym))
        val validLess = less.filter(isValidOverload(f2, isExact, clause, m, fringePs, _)) match {
          case Seq(head) => head
        }
        val fringeTs = f2.expand(SymbolC(validLess))
        val newPs = (ps, fringePs, fringeTs).zipped.map({
          case (p, fringeP, fringeT) =>
            if (fringeP.sort == fringeT.sort) {
              p
            } else {
              p.expand(SymbolC(B.SymbolOrAlias("inj", Seq(fringeT.sort, fringeP.sort))), isExact, Seq(fringeT), fringeP, clause, m).head
            }
        })
        Seq(SymbolP(validLess, newPs))
      case _ => ps
    }
  }
  def expandOr: Seq[Pattern[T]] = {
    ps.indices.foldLeft(Seq(this))((accum, ix) => accum.flatMap(s => s.ps(ix).expandOr.map(p => SymbolP(sym, s.ps.updated(ix, p)))))
  }

  // returns true if the specified constructor is an overload of the current pattern and can match it
  private def isValidOverload(f: Fringe, isExact: Boolean, clause: Clause, m: Int, fringePs: Seq[Fringe], less: SymbolOrAlias): Boolean = {
    def isValidChild(p: Pattern[T], fringeP: Fringe, fringeT: Fringe): Boolean = {
      fringeP.sort == fringeT.sort || (fringeP.symlib.isSubsorted(fringeT.sort, fringeP.sort) && fringeP.sortInfo.category == SymbolS() && p.isSpecialized(SymbolC(B.SymbolOrAlias("inj", Seq(fringeT.sort, fringeP.sort))), false, fringeP, clause, m))
    }

    val cons = SymbolC(less)
    if (f.contains(cons)) {
      val fringeTs = f.expand(cons)
      (ps, fringePs, fringeTs).zipped.toIterable.map(t => isValidChild(t._1, t._2, t._3)).forall(identity)
    } else {
      false
    }
  }

  // computes the list of injections that are bound to variables in the current pattern as a result of matching on an overload of the current
  // pattern. these injections are bound to variables despite not existing in the current term, so they need to be tracked so they can be
  // created later
  override def overloadChildren(f: Fringe, ix: Option[Constructor], residual: Option[Pattern[String]], o: Occurrence): Seq[(Constructor, VariableBinding[T])] = {
    def getVar(fringeP: Fringe, fringeT: Fringe, pat: Pattern[T], i: Int): Seq[(Constructor, VariableBinding[T])] = {
      val vars = pat.bindings(None, residual, Inj(Num(i, o)), f.symlib) // compute variable bindings for this pattern
      val child = SymbolC(B.SymbolOrAlias("inj", Seq(fringeT.sort, fringeP.sort)))
      if (fringeP.sort == fringeT.sort) {
        Seq() // exact match, so no bindings
      } else {
        val childOverloads = pat.overloadChildren(fringeP, Some(child), residual, Num(i, o)) // recurse into child term
        vars.map(v => (child, v)) ++ childOverloads
      }
    }
    (ix, sym) match {
      case (Some(SymbolC(SymbolOrAlias("inj", Seq(a, _)))), SymbolOrAlias("inj", Seq(b,_))) =>
        if (a == b || !f.symlib.isSubsorted(a, b)) { // exact match, don't recurse
          Seq()
        } else {
          val fringeB = f.expand(SymbolC(sym)).head
          ps.head.overloadChildren(fringeB, Some(SymbolC(B.SymbolOrAlias("inj", Seq(a, b)))), residual, o)
        }
      case (Some(inj @ SymbolC(SymbolOrAlias("inj", _))), _) =>
        val less = f.overloads(sym)
        val fringePs = f.expand(SymbolC(sym))
        val f2 = f.expand(inj).head // fringe for child of injection

        val child = less.find(c => f2.contains(SymbolC(c))).map(c => f2.expand(SymbolC(c)))
        child match {
          case None => Seq() // no overloads exist
          case Some(fringeTs) => (fringePs, fringeTs, ps).zipped.toSeq.zipWithIndex.flatMap(t => getVar(t._1._1, t._1._2, t._1._3, t._2)) // compute variable bindings
        }
      case _ => Seq()
    }
  }

  def category: None.type = None
  lazy val variables: Set[T] = ps.flatMap(_.variables).toSet
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = SymbolP(sym, ps.map(_.canonicalize(clause)))
  def decanonicalize: Pattern[String] = SymbolP(sym, ps.map(_.decanonicalize))
  def isBound(clause: Clause): Boolean = ps.forall(_.isBound(clause))
  def isResidual(symlib: Parser.SymLib) = {
    symlib.functions.contains(sym) || Parser.getStringAtt(symlib.signatures(sym)._3, "anywhere").isDefined
  }
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = sym.toString
  def toKORE(f: Fringe): kore.Pattern = {
    B.Application(sym, (f.expand(SymbolC(sym)) zip ps).map(t => t._2.toKORE(t._1)))
  }
}

case class VariableP[T](name: T, sort: SortCategory) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = Seq()
  def isWildcard = true
  def isDefault = true
  def isSpecialized(ix: Constructor, ixExact: Boolean, fringe: Fringe, clause: Clause, maxPriority: Int): Boolean = {
    ix match {
      case HasKey(_, _, None) => clause.action.priority <= maxPriority
      case _ => true
    }
  }
  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreVariable(this, f, c, key, isEmpty)
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = {
    Seq(new VariableBinding(name, sort, occurrence, residual))
  }
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, m: Int): Seq[Pattern[T]] = {
    fringes.map(_ => WildcardP().asInstanceOf[Pattern[T]])
  }
  def expandOr: Seq[Pattern[T]] = Seq(this)

  def category: None.type = None
  lazy val variables: Set[T] = Set(name)
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = {
    val subst = clause.canonicalize(name.toString).flatMap(clause.specializedVars.get(_))
    if (subst.isDefined) {
      subst.get
    } else {
      VariableP(clause.canonicalize(name.toString), sort)
    }
  }
  def decanonicalize: Pattern[String] = WildcardP()
  def isBound(clause: Clause): Boolean = clause.isBound(name)
  def isResidual(symlib: Parser.SymLib) = false
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = name.toString
  def toKORE(f: Fringe): kore.Pattern = {
    B.Variable(name.toString, f.sort)
  }
}

case class WildcardP[T]() extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = Seq()
  def isWildcard = true
  def isDefault = true
  def isSpecialized(ix: Constructor, isExact: Boolean, fringe: Fringe, clause: Clause, maxPriority: Int): Boolean = {
    ix match {
      case HasKey(_, _, None) => clause.action.priority <= maxPriority
      case _ => true
    }
  }

  def score(h: Heuristic, f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]], isEmpty: Boolean): Double = h.scoreWildcard(this, f, c, key, isEmpty)
  def bindings(ix: Option[Constructor], residual: Option[Pattern[String]], occurrence: Occurrence, symlib: Parser.SymLib): Seq[VariableBinding[T]] = Seq()
  def expand(ix: Constructor, isExact: Boolean, fringes: Seq[Fringe], f: Fringe, clause: Clause, m: Int): Seq[Pattern[T]] = {
    fringes.map(_ => WildcardP().asInstanceOf[Pattern[T]])
  }
  def expandOr: Seq[Pattern[T]] = Seq(this)

  def category: None.type = None
  def variables: Set[T] = Set()
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = WildcardP()
  def decanonicalize: Pattern[String] = WildcardP()
  def isBound(clause: Clause): Boolean = true
  def isResidual(symlib: Parser.SymLib) = false
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = "_"
  def toKORE(f: Fringe): kore.Pattern = {
    B.Variable("_", f.sort)
  }
}
