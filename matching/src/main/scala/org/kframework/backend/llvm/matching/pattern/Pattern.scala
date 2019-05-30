package org.kframework.backend.llvm.matching.pattern

import org.kframework.parser.kore.SymbolOrAlias
import org.kframework.parser.kore.implementation.{DefaultBuilders => B}
import org.kframework.backend.llvm.matching._
import scala.math.min

sealed trait Pattern[T] {
  def signature(clause: Clause): Seq[Constructor]
  def isWildcard: Boolean
  def isDefault: Boolean
  def isSpecialized(ix: Constructor, fringe: Fringe, clause: Clause): Boolean
  def score(fringe: Fringe, clause: Clause, key: Option[Pattern[Option[Occurrence]]]): Double
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]]
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]]
  def expandOr: Seq[Pattern[T]]

  def mapOrSetKeys: Seq[Pattern[T]] = Seq()
  def bestKey(f: Fringe, clause: Clause, ps: Seq[Pattern[T]], clauses: Seq[Clause]): Option[Pattern[Option[Occurrence]]] = None
  def listRange(ix: Option[Constructor], o: Occurrence): Seq[(Occurrence, Int, Int)] = Seq()
  def overloadChildren(f: Fringe, ix: Option[Constructor], o: Occurrence): Seq[(Constructor, VariableBinding[T])] = Seq()
  def category: Option[SortCategory]
  def variables: Set[T]
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]]
  def isBound(clause: Clause): Boolean
  def toShortString: String
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
  def isSpecialized(ix: Constructor, f: Fringe, c: Clause): Boolean = {
    pat.isSpecialized(ix, f, c)
  }
  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = pat.score(f, c, key)
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = {
    Seq(new VariableBinding(name, sort, occurrence)) ++ pat.bindings(ix, occurrence)
  }
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = {
    pat.expand(ix, fringes, f, clause)
  }
  def expandOr: Seq[AsP[T]] = pat.expandOr.map(AsP(name, sort, _))

  def category: Option[SortCategory] = pat.category
  def variables: Set[T] = Set(name) ++ pat.variables
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = AsP(clause.canonicalize(name.toString), sort, pat.canonicalize(clause))
  def isBound(clause: Clause): Boolean = clause.isBound(name) && pat.isBound(clause)
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = pat.toShortString + " #as " + name.toString
}

case class ListP[T](head: Seq[Pattern[T]], frame: Option[Pattern[T]], tail: Seq[Pattern[T]], ctr: SymbolOrAlias, orig: Pattern[T]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = {
    (0 to (head.size + tail.size)).map(ListC(ctr, _))
  }
  def isWildcard = false
  def isDefault: Boolean = frame.isDefined
  def isSpecialized(ix: Constructor, f: Fringe, c: Clause): Boolean = {
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
  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = 1.0
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = {
    if (frame.isEmpty) {
      Seq()
    } else ix.get match {
      case listC: ListC =>
        val len = listC.length
        frame.get.bindings(None, Num(len, occurrence))
      case _ =>
        Seq()
    }
  }
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = {
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
  def variables: Set[T] = head.flatMap(_.variables).toSet ++ tail.flatMap(_.variables).toSet ++ frame.map(_.variables).getOrElse(Set())
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = ListP(head.map(_.canonicalize(clause)), frame.map(_.canonicalize(clause)), tail.map(_.canonicalize(clause)), ctr, orig.canonicalize(clause))
  def isBound(clause: Clause): Boolean = head.forall(_.isBound(clause)) && frame.forall(_.isBound(clause)) && tail.forall(_.isBound(clause))
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = "L(" + head.size + " " + frame.isDefined + " " + tail.size + ")"
}

case class LiteralP[T](literal: String, sort: SortCategory) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = {
    Seq(LiteralC(literal))
  }
  def isWildcard = false
  def isDefault = false
  def isSpecialized(ix: Constructor, f: Fringe, c: Clause): Boolean = {
    ix.isInstanceOf[LiteralC] && ix.asInstanceOf[LiteralC].literal == literal
  }

  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = 1.0
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = Seq()
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = Seq()
  def expandOr: Seq[Pattern[T]] = Seq(this)

  def category = Some(sort)
  def variables: Set[T] = Set()
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = LiteralP(literal, sort)
  def isBound(clause: Clause): Boolean = true
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = literal
}

case class MapP[T](keys: Seq[Pattern[T]], values: Seq[Pattern[T]], frame: Option[Pattern[T]], ctr: SymbolOrAlias, orig: Pattern[T]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = {
    if (keys.isEmpty && frame.isEmpty) {
      Seq(Empty())
    } else if (keys.isEmpty) {
      frame.get.signature(clause)
    } else if (frame.isEmpty) {
      keys.flatMap(key => Seq(HasKey(isSet = false, ctr, clause.canonicalize(key)), HasNoKey(clause.canonicalize(key))))
    } else {
      keys.flatMap(key => Seq(HasKey(isSet = false, ctr, clause.canonicalize(key)), HasNoKey(clause.canonicalize(key)))) ++ frame.get.signature(clause)
    }
  }
  def isWildcard: Boolean = keys.isEmpty && values.isEmpty && frame.isDefined && frame.get.isWildcard
  def isDefault: Boolean = frame.isDefined || keys.nonEmpty || values.nonEmpty
  def isSpecialized(ix: Constructor, fringe: Fringe, clause: Clause): Boolean = {
    (ix, frame) match {
      case (Empty(), _) => keys.isEmpty && values.isEmpty
      case (HasKey(_, _, _), Some(_)) => true
      case (HasKey(_, _, Some(p)), None) => keys.map(_.canonicalize(clause)).exists(Pattern.mightUnify(p, _))
      case (HasNoKey(Some(p)), _) => !keys.map(_.canonicalize(clause)).contains(p)
      case (HasKey(_, _, None), None) => keys.nonEmpty
      case (HasNoKey(None), _) => false
    }
  }
  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = {
    if (keys.isEmpty && frame.isEmpty) {
      1.0
    } else if (keys.isEmpty) {
      frame.get.score(f, c, key)
    } else if (key.isDefined) {
      if (canonicalize(c).keys.contains(key.get)) 1.0 else 0.0
    } else {
      Double.NegativeInfinity
    }
  }
      
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = {
    if (keys.isEmpty && values.isEmpty && frame.isDefined) {
      frame.get.bindings(None, occurrence)
    } else {
      Seq()
    }
  }
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = {
    ix match {
      case Empty() => Seq()
      case HasKey(_, _, Some(p)) =>
        val canonKs = keys.map(_.canonicalize(clause))
        canonKs.indexOf(p) match {
          case -1 => Seq(WildcardP(), WildcardP(), this)
          case i => Seq(values(i), MapP(keys.take(i) ++ keys.takeRight(keys.size - i - 1), values.take(i) ++ values.takeRight(values.size - i - 1), frame, ctr, orig), WildcardP())
        }
      case HasNoKey(_) | NonEmpty() => Seq(this)
      case HasKey(_, _, None) =>
        Seq(keys.head, values.head, MapP(keys.tail, values.tail, frame, ctr, orig))
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
  override def bestKey(f: Fringe, clause: Clause, ps: Seq[Pattern[T]], clauses: Seq[Clause]): Option[Pattern[Option[Occurrence]]] = {
    if (keys.isEmpty) {
      None
    } else {
      computeScore(f, clause, ps, clauses)._1
    }
  }

  private def computeScore(f: Fringe, clause: Clause, ps: Seq[Pattern[T]], clauses: Seq[Clause]): (Option[Pattern[Option[Occurrence]]], Double) = {
    (keys, values).zipped.toIterable.map(t => (clause.canonicalize(t._1), elementScore(f, clause, ps, clauses, t._1, t._2))).maxBy(_._2)
  }
  private def elementScore(f: Fringe, clause: Clause, ps: Seq[Pattern[T]], clauses: Seq[Clause], key: Pattern[T], value: Pattern[T]): Double = {
    val rawScore = CollectionP.elementScore(key, clause, ps, clauses)
    if (rawScore.isNegInfinity) {
      rawScore
    } else {
      val finalScore = rawScore * value.score(f.expand(HasKey(isSet = false, ctr, Some(key.canonicalize(clause)))).head, clause, None)
      if (finalScore == 0.0) {
        Double.MinPositiveValue
      } else {
        finalScore
      }
    }
  }

  def category = Some(MapS())
  def variables: Set[T] = keys.flatMap(_.variables).toSet ++ values.flatMap(_.variables).toSet ++ frame.map(_.variables).getOrElse(Set())
  def canonicalize(clause: Clause): MapP[Option[Occurrence]] = MapP(keys.map(_.canonicalize(clause)), values.map(_.canonicalize(clause)), frame.map(_.canonicalize(clause)), ctr, orig.canonicalize(clause))
  def isBound(clause: Clause): Boolean = keys.forall(_.isBound(clause)) && values.forall(_.isBound(clause)) && frame.forall(_.isBound(clause))
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = "M(" + keys.size + " " + frame.isDefined + ")"
}

case class OrP[T](ps: Seq[Pattern[T]]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = ps.flatMap(_.signature(clause))
  def isWildcard: Boolean = ps.forall(_.isWildcard)
  def isDefault: Boolean = ???
  def isSpecialized(ix: Constructor, f: Fringe, c: Clause): Boolean = ???
  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = {
    ps.map(_.score(f, c, key)).sum
  }
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = ???
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = ???
  def expandOr: Seq[Pattern[T]] = ps.flatMap(_.expandOr)
  def category: Option[SortCategory] = {
    val s = ps.map(_.category).filter(_.isDefined)
    if (s.isEmpty) {
      None
    } else {
      s.head
    }
  }
  def variables: Set[T] = ps.flatMap(_.variables).toSet
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = OrP(ps.map(_.canonicalize(clause)))
  def isBound(clause: Clause): Boolean = ps.forall(_.isBound(clause))
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = ps.map(_.toShortString).mkString(" #Or ")
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
      elements.flatMap(elem => Seq(HasKey(isSet = true, ctr, clause.canonicalize(elem)), HasNoKey(clause.canonicalize(elem))))
    } else {
      elements.flatMap(elem => Seq(HasKey(isSet = true, ctr, clause.canonicalize(elem)), HasNoKey(clause.canonicalize(elem)))) ++ frame.get.signature(clause)
    }
  }
  def isWildcard: Boolean = elements.isEmpty && frame.isDefined && frame.get.isWildcard
  def isDefault: Boolean = frame.isDefined || elements.nonEmpty
  def isSpecialized(ix: Constructor, fringe: Fringe, clause: Clause): Boolean = {
    (ix, frame) match {
      case (Empty(), _) => elements.isEmpty
      case (HasKey(_, _, _), Some(_)) => true
      case (HasKey(_, _, Some(p)), None) => elements.map(_.canonicalize(clause)).exists(Pattern.mightUnify(p, _))
      case (HasNoKey(Some(p)), _) => !elements.map(_.canonicalize(clause)).contains(p)
      case (HasKey(_, _, None), None) => elements.nonEmpty
      case (HasNoKey(None), _) => false
    }
  }
  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = {
    if (elements.isEmpty && frame.isEmpty) {
      1.0
    } else if (elements.isEmpty) {
      frame.get.score(f, c, key)
    } else if (key.isDefined) {
      if (canonicalize(c).elements.contains(key.get)) 1.0 else 0.0
    } else {
      Double.NegativeInfinity
    }
  }
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = {
    if (elements.isEmpty && frame.isDefined) {
      frame.get.bindings(None, occurrence)
    } else {
      Seq()
    }
  }
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = {
    ix match {
      case Empty() => Seq()
      case HasKey(_, _, Some(p)) =>
        val canonEs = elements.map(_.canonicalize(clause))
        canonEs.indexOf(p) match {
          case -1 => Seq(WildcardP(), this)
          case i => Seq(SetP(elements.take(i) ++ elements.takeRight(elements.size - i - 1), frame, ctr, orig), WildcardP())
        }
      case HasNoKey(_) | NonEmpty() => Seq(this)
      case HasKey(_, _, None) =>
        Seq(elements.head, SetP(elements.tail, frame, ctr, orig))
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
  override def bestKey(f: Fringe, clause: Clause, ps: Seq[Pattern[T]], clauses: Seq[Clause]): Option[Pattern[Option[Occurrence]]] = {
    if (elements.isEmpty) {
      None
    } else {
      computeScore(clause, ps, clauses)._1
    }
  }

  private def computeScore(clause: Clause, ps: Seq[Pattern[T]], clauses: Seq[Clause]): (Option[Pattern[Option[Occurrence]]], Double) = {
    elements.map(e => (clause.canonicalize(e), CollectionP.elementScore(e, clause, ps, clauses))).maxBy(_._2)
  }

  def category = Some(SetS())
  def variables: Set[T] = elements.flatMap(_.variables).toSet ++ frame.map(_.variables).getOrElse(Set())
  def canonicalize(clause: Clause): SetP[Option[Occurrence]] = SetP(elements.map(_.canonicalize(clause)), frame.map(_.canonicalize(clause)), ctr, orig.canonicalize(clause))
  def isBound(clause: Clause): Boolean = elements.forall(_.isBound(clause)) && frame.forall(_.isBound(clause))
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = "S(" + elements.size + " " + frame.isDefined + ")"
}

private[pattern] object CollectionP {
  def elementScore[T](e: Pattern[T], clause: Clause, ps: Seq[Pattern[T]], clauses: Seq[Clause]): Double = {
    if (e.isBound(clause)) {
      val canonE = e.canonicalize(clause)
      val boundedCs = clauses.takeWhile(c => canonE.variables.forall(v => v.isDefined && c.boundOccurrences(v.get)))
      val boundedPs = ps.take(boundedCs.size)
      val canonPs = (boundedPs, boundedCs).zipped.map({ case (p, c) => p.canonicalize(c)})
      val psWithK = canonPs.takeWhile(mapContainsKey(canonE, _))
      psWithK.size
    } else {
      Double.NegativeInfinity
    }
  }

  def mapContainsKey(e: Pattern[Option[Occurrence]], map: Pattern[Option[Occurrence]]): Boolean = {
    map match {
      case MapP(keys, _, _, _, _) => keys.contains(e)
      case SetP(keys, _, _, _) => keys.contains(e)
      case _ => false
    }
  }
}

case class SymbolP[T](sym: SymbolOrAlias, ps: Seq[Pattern[T]]) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = Seq(SymbolC(sym))
  def isWildcard = false
  def isDefault = false
  def isSpecialized(ix: Constructor, f: Fringe, clause: Clause): Boolean = {
    (ix, sym) match {
      case (SymbolC(SymbolOrAlias("inj",Seq(a,c))), SymbolOrAlias("inj",Seq(b,c2))) =>
        lazy val f2 = f.expand(SymbolC(sym)).head
        c == c2 && (a == b || (f.symlib.isSubsorted(a, b) && ps.head.isSpecialized(SymbolC(B.SymbolOrAlias("inj",Seq(a,b))), f2, clause)))
      case (SymbolC(SymbolOrAlias("inj",_)), _) =>
        val less = f.overloads(sym)
        lazy val f2 = f.expand(ix).head
        less.exists(isValidOverload(f2, clause, f.expand(SymbolC(sym)), _))
      case (SymbolC(ix2), _) => ix2 == sym
    }
  }
  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = {
    val ncons = f.overloads(sym).size + 1.0
    1.0 / ncons
  }
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = {
    (ix, sym) match {
      case (Some(SymbolC(SymbolOrAlias("inj",Seq(a,_)))), SymbolOrAlias("inj",Seq(b,_))) =>
        if (a == b) {
          Seq()
        } else {
          ps.head.bindings(None, occurrence)
        }
      case _ => Seq()
    }
  }
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = {
    (ix, sym) match {
      case (SymbolC(SymbolOrAlias("inj", Seq(a, _))), inj @ SymbolOrAlias("inj", Seq(b, _))) =>
        if (a == b) {
          Seq(ps.head)
        } else {
          ps.head.expand(SymbolC(B.SymbolOrAlias("inj", Seq(a, b))), fringes, f.expand(SymbolC(inj)).head, clause)
        }
      case (SymbolC(SymbolOrAlias("inj", _)), _) =>
        val less = f.overloads(sym)
        val f2 = fringes.head
        val fringePs = f.expand(SymbolC(sym))
        val validLess = less.filter(isValidOverload(f2, clause, fringePs, _)) match {
          case Seq(head) => head
        }
        val fringeTs = f2.expand(SymbolC(validLess))
        val newPs = (ps, fringePs, fringeTs).zipped.map({
          case (p, fringeP, fringeT) =>
            if (fringeP.sort == fringeT.sort) {
              p
            } else {
              p.expand(SymbolC(B.SymbolOrAlias("inj", Seq(fringeT.sort, fringeP.sort))), Seq(fringeT), fringeP, clause).head
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
  private def isValidOverload(f: Fringe, clause: Clause, fringePs: Seq[Fringe], less: SymbolOrAlias): Boolean = {
    def isValidChild(p: Pattern[T], fringeP: Fringe, fringeT: Fringe): Boolean = {
      fringeP.sort == fringeT.sort || (fringeP.symlib.isSubsorted(fringeT.sort, fringeP.sort) && p.isSpecialized(SymbolC(B.SymbolOrAlias("inj", Seq(fringeT.sort, fringeP.sort))), fringeP, clause))
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
  override def overloadChildren(f: Fringe, ix: Option[Constructor], o: Occurrence): Seq[(Constructor, VariableBinding[T])] = {
    def getVar(fringeP: Fringe, fringeT: Fringe, pat: Pattern[T], i: Int): Seq[(Constructor, VariableBinding[T])] = {
      val vars = pat.bindings(None, Inj(Num(i, o))) // compute variable bindings for this pattern
      val child = SymbolC(B.SymbolOrAlias("inj", Seq(fringeT.sort, fringeP.sort)))
      val childOverloads = pat.overloadChildren(fringeP, Some(child), Num(i, o)) // recurse into child term
      if (fringeP.sort == fringeT.sort) {
        Seq() // exact match, so no bindings
      } else {
        vars.map(v => (child, v)) ++ childOverloads
      }
    }
    (ix, sym) match {
      case (Some(SymbolC(SymbolOrAlias("inj", Seq(a, _)))), SymbolOrAlias("inj", Seq(b,_))) =>
        if (a == b) { // exact match, don't recurse
          Seq()
        } else { // flexible injection, so recurse into child
          val fringeB = f.expand(SymbolC(sym)).head
          ps.head.overloadChildren(fringeB, Some(SymbolC(B.SymbolOrAlias("inj", Seq(a, b)))), o)
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
  def isBound(clause: Clause): Boolean = ps.forall(_.isBound(clause))
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = sym.toString
}

case class VariableP[T](name: T, sort: SortCategory) extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = Seq()
  def isWildcard = true
  def isDefault = true
  def isSpecialized(ix: Constructor, fringe: Fringe, clause: Clause): Boolean = true
  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = 0.0
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = {
    Seq(new VariableBinding(name, sort, occurrence))
  }
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = {
    fringes.map(_ => WildcardP().asInstanceOf[Pattern[T]])
  }
  def expandOr: Seq[Pattern[T]] = Seq(this)

  def category: None.type = None
  val variables: Set[T] = Set(name)
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = VariableP(clause.canonicalize(name.toString), sort)
  def isBound(clause: Clause): Boolean = clause.isBound(name)
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = name.toString
}

case class WildcardP[T]() extends Pattern[T] {
  def signature(clause: Clause): Seq[Constructor] = Seq()
  def isWildcard = true
  def isDefault = true
  def isSpecialized(ix: Constructor, fringe: Fringe, clause: Clause): Boolean = true

  def score(f: Fringe, c: Clause, key: Option[Pattern[Option[Occurrence]]]): Double = 0.0
  def bindings(ix: Option[Constructor], occurrence: Occurrence): Seq[VariableBinding[T]] = Seq()
  def expand(ix: Constructor, fringes: Seq[Fringe], f: Fringe, clause: Clause): Seq[Pattern[T]] = {
    fringes.map(_ => WildcardP().asInstanceOf[Pattern[T]])
  }
  def expandOr: Seq[Pattern[T]] = Seq(this)

  def category: None.type = None
  def variables: Set[T] = Set()
  def canonicalize(clause: Clause): Pattern[Option[Occurrence]] = WildcardP()
  def isBound(clause: Clause): Boolean = true
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
  def toShortString: String = "_"
}
