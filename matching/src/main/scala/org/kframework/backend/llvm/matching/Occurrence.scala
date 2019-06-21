package org.kframework.backend.llvm.matching

import java.util

import org.kframework.backend.llvm.matching.pattern.Pattern
import java.util.ArrayList

sealed trait Occurrence {
  def representation: util.ArrayList[AnyRef]
  def size: Int
}

case class Base() extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  def size = 0
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Num(i: Int, rest: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add(i.toString)
  representation.addAll(rest.representation)
  val size = 1 + rest.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Inj(rest: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("-1")
  representation.addAll(rest.representation)
  val size = 1 + rest.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Lit(name: String, hook: String) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("lit")
  representation.add(name)
  representation.add(hook)
  def size = 1
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Fresh(name: String) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("fresh")
  representation.add(name)
  def size = 1
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class SC(ordinal: Int) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("side_condition_" + ordinal)
  def size = 1
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Equal(o1: Occurrence, o2: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("eq")
  representation.addAll(o1.representation)
  representation.add("and")
  representation.addAll(o2.representation)
  val size = o1.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Size(rest: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("size")
  representation.addAll(rest.representation)
  val size = 1 + rest.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Value(pat: Pattern[Option[Occurrence]], rest: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add(pat.toString + "_val")
  representation.addAll(rest.representation)
  val size = 1 + rest.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Rem(pat: Pattern[Option[Occurrence]], rest: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add(pat.toString + "_rem")
  representation.addAll(rest.representation)
  val size = 1 + rest.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class Choice(rest: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("key")
  representation.addAll(rest.representation)
  val size = 1 + rest.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class ChoiceValue(rest: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("val")
  representation.addAll(rest.representation)
  val size = 1 + rest.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
case class ChoiceRem(rest: Occurrence) extends Occurrence {
  val representation = new util.ArrayList[AnyRef]()
  representation.add("rem")
  representation.addAll(rest.representation)
  val size = 1 + rest.size
  override lazy val hashCode: Int = scala.runtime.ScalaRunTime._hashCode(this)
}
