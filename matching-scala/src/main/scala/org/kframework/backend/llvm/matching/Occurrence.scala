package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.pattern.Pattern

import java.util.ArrayList

sealed trait Occurrence {
  def representation: ArrayList[AnyRef]
}

case class Base() extends Occurrence {
  val representation = new ArrayList[AnyRef]()
}
case class Num(i: Int, rest: Occurrence) extends Occurrence {
  val representation = new ArrayList[AnyRef]()
  representation.add(i.toString)
  representation.addAll(rest.representation)
}
case class Inj(rest: Occurrence) extends Occurrence {
  val representation = new ArrayList[AnyRef]()
  representation.add("-1")
  representation.addAll(rest.representation)
}
case class Lit(name: String, hook: String) extends Occurrence {
  val representation = new ArrayList[AnyRef]()
  representation.add("lit")
  representation.add(name)
  representation.add(hook)
}
case class SC(ordinal: Int) extends Occurrence {
  val representation = new ArrayList[AnyRef]()
  representation.add("side_condition_" + ordinal)
}
case class Equal(o1: Occurrence, o2: Occurrence) extends Occurrence {
  val representation = new ArrayList[AnyRef]()
  representation.add("eq")
  representation.addAll(o1.representation)
  representation.add("and")
  representation.addAll(o2.representation)
}
case class Size(rest: Occurrence) extends Occurrence {
  val representation = new ArrayList[AnyRef]()
  representation.add("size")
  representation.addAll(rest.representation)
}
case class Value(pat: Pattern[Option[Occurrence]], rest: Occurrence) extends Occurrence {
  val representation = new ArrayList[AnyRef]()
  representation.add(pat.toString + "_val")
  representation.addAll(rest.representation)
}
case class Rem(pat: Pattern[Option[Occurrence]], rest: Occurrence) extends Occurrence {
  val representation = new ArrayList[AnyRef]()
  representation.add(pat.toString + "_rem")
  representation.addAll(rest.representation)
}
