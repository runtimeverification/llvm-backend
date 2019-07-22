package org.kframework.backend.llvm.matching.dt

import org.kframework.backend.llvm.matching.Occurrence
import org.kframework.backend.llvm.matching.pattern._
import java.io.File
import java.io.FileWriter
import java.util
import java.util.concurrent.ConcurrentHashMap

import org.yaml.snakeyaml.Yaml

sealed trait DecisionTree {
  def serializeToYaml(file: File): Unit = {
    val writer = new FileWriter(file)
    new Yaml().dump(representation, writer)
    writer.close()
  }

  def serializeToYaml(file: File, residuals: Seq[(Pattern[String], Occurrence)]): Unit = {
    val writer = new FileWriter(file)
    val residualRepr = new util.ArrayList[AnyRef]()
    for (entry <- residuals) {
      val pair = new util.ArrayList[AnyRef]()
      pair.add(MakePattern.representResidual(entry._1))
      pair.add(entry._2.representation)
      residualRepr.add(pair)
    }
    val bothRepr = new util.ArrayList[AnyRef]()
    bothRepr.add(representation)
    bothRepr.add(residualRepr)
    new Yaml().dump(bothRepr, writer)
    writer.close()
  }

  def representation: AnyRef
}

case class Failure private() extends DecisionTree {
  val representation = "fail"
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class Leaf private(ordinal: Int, occurrences: Seq[Occurrence]) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val action = new util.ArrayList[AnyRef]()
  representation.put("action", action)
  action.add(ordinal.asInstanceOf[AnyRef])
  private val os = new util.ArrayList[AnyRef]()
  action.add(os)
  for (occurrence <- occurrences) {
    os.add(occurrence.representation)
  }
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class Switch private(occurrence: Occurrence, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val specs = new util.ArrayList[AnyRef]()
  representation.put("specializations", specs)
  for ((c, dt) <- cases) {
    val _case = new util.ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    specs.add(_case)
  }
  representation.put("default", default.map(_.representation).orNull)
  representation.put("occurrence", occurrence.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class SwitchLit private(occurrence: Occurrence, bitwidth: Int, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val specs = new util.ArrayList[AnyRef]()
  representation.put("specializations", specs)
  for ((c, dt) <- cases) {
    val _case = new util.ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    specs.add(_case)
  }
  representation.put("bitwidth", bitwidth.asInstanceOf[AnyRef])
  representation.put("default", default.map(_.representation).orNull)
  representation.put("occurrence", occurrence.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class Function private(name: String, occurrence: Occurrence, vars: Seq[Occurrence], hook: String, child: DecisionTree) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  representation.put("function", name)
  representation.put("sort", hook)
  representation.put("occurrence", occurrence.representation)
  val args = new util.ArrayList[AnyRef]()
  representation.put("args", args)
  for (v <- vars) {
    args.add(v.representation)
  }
  representation.put("next", child.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class CheckNull private(occurrence: Occurrence, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val specs = new util.ArrayList[AnyRef]()
  representation.put("specializations", specs)
  for ((c, dt) <- cases) {
    val _case = new util.ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    specs.add(_case)
  }
  representation.put("default", default.map(_.representation).orNull)
  representation.put("isnull", true.asInstanceOf[AnyRef])
  representation.put("occurrence", occurrence.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class MakePattern private(occurrence: Occurrence, pattern: Pattern[Option[Occurrence]], child: DecisionTree) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  representation.put("pattern", MakePattern.representPattern(pattern))
  representation.put("occurrence", occurrence.representation)
  representation.put("next", child.representation)

  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class MakeIterator private(hookName: String, occurrence: Occurrence, child: DecisionTree) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  representation.put("function", hookName)
  representation.put("collection", occurrence.representation)
  representation.put("next", child.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class IterNext private(hookName: String, iterator: Occurrence, binding: Occurrence, child: DecisionTree) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  representation.put("function", hookName)
  representation.put("iterator", iterator.representation)
  representation.put("binding", binding.representation)
  representation.put("next", child.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean = that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

object Failure {
  private val instance = new Failure()
  def apply(): Failure = Failure.instance
}

object Leaf {
  private val cache = new ConcurrentHashMap[(Int, Seq[Occurrence]), Leaf]()
  def apply(ordinal: Int, occurrences: Seq[Occurrence]): Leaf = {
    cache.computeIfAbsent((ordinal, occurrences), k => new Leaf(k._1, k._2))
  }
}

object Switch {
  val cache = new ConcurrentHashMap[(Occurrence, Seq[(String, DecisionTree)], Option[DecisionTree]), Switch]()
  def apply(occurrence: Occurrence, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]): Switch = {
    cache.computeIfAbsent((occurrence, cases, default), k => new Switch(k._1, k._2, k._3))
  }
}

object SwitchLit {
  val cache = new ConcurrentHashMap[(Occurrence, Int, Seq[(String, DecisionTree)], Option[DecisionTree]), SwitchLit]()
  def apply(occurrence: Occurrence, bitwidth: Int, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]): SwitchLit = {
    cache.computeIfAbsent((occurrence, bitwidth, cases, default), k => new SwitchLit(k._1, k._2, k._3, k._4))
  }
}

object Function {
  val cache = new ConcurrentHashMap[(String, Occurrence, Seq[Occurrence], String, DecisionTree), Function]()
  def apply(name: String, occurrence: Occurrence, vars: Seq[Occurrence], hook: String, child: DecisionTree): Function = {
    cache.computeIfAbsent((name, occurrence, vars, hook, child), k => new Function(k._1, k._2, k._3, k._4, k._5))
  }
}

object CheckNull {
  val cache = new ConcurrentHashMap[(Occurrence, Seq[(String, DecisionTree)], Option[DecisionTree]), CheckNull]()
  def apply(occurrence: Occurrence, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]): CheckNull = {
    cache.computeIfAbsent((occurrence, cases, default), k => new CheckNull(k._1, k._2, k._3))
  }
}

object MakePattern {
  val cache = new ConcurrentHashMap[(Occurrence, Pattern[Option[Occurrence]], DecisionTree), MakePattern]()
  def apply(occurrence: Occurrence, pattern: Pattern[Option[Occurrence]], child: DecisionTree): MakePattern = {
    cache.computeIfAbsent((occurrence, pattern, child), k => new MakePattern(k._1, k._2, k._3))
  }
  def representPattern(pattern: Pattern[Option[Occurrence]]): util.HashMap[String, AnyRef] = {
    val result = new util.HashMap[String, AnyRef]()
    pattern match {
      case OrP(_) | WildcardP() | VariableP(None, _) => ???
      case VariableP(Some(o), h) => {
        result.put("hook", h.hookAtt)
        result.put("occurrence", o.representation)
      }
      case AsP(_, _, p) => return representPattern(p)
      case MapP(_, _, _, _, o) => return representPattern(o)
      case SetP(_, _, _, o) => return representPattern(o)
      case ListP(_, _, _, _, o) => return representPattern(o)
      case LiteralP(s, h) => {
        result.put("hook", h.hookAtt)
        result.put("literal", s)
      }
      case SymbolP(s, ps) => {
        result.put("constructor", s.toString)
        val args = new util.ArrayList[AnyRef]()
        result.put("args", args)
        for (p <- ps) {
          args.add(representPattern(p))
        }
      }
    }
    assert(!result.isEmpty())
    result
  }
  def representResidual(pattern: Pattern[String]): util.HashMap[String, AnyRef] = {
    val result = new util.HashMap[String, AnyRef]()
    pattern match {
      case OrP(_) | WildcardP() => ???
      case VariableP(o, h) => {
        result.put("hook", h.hookAtt)
        result.put("occurrence", o)
      }
      case AsP(_, _, p) => return representResidual(p)
      case MapP(_, _, _, _, o) => return representResidual(o)
      case SetP(_, _, _, o) => return representResidual(o)
      case ListP(_, _, _, _, o) => return representResidual(o)
      case LiteralP(s, h) => {
        result.put("hook", h.hookAtt)
        result.put("literal", s)
      }
      case SymbolP(s, ps) => {
        result.put("constructor", s.toString)
        val args = new util.ArrayList[AnyRef]()
        result.put("args", args)
        for (p <- ps) {
          args.add(representResidual(p))
        }
      }
    }
    assert(!result.isEmpty())
    result
  }
}

object MakeIterator {
  val cache = new ConcurrentHashMap[(String, Occurrence, DecisionTree), MakeIterator]()
  def apply(function: String, occurrence: Occurrence, child: DecisionTree): MakeIterator = {
    cache.computeIfAbsent((function, occurrence, child), k => new MakeIterator(k._1, k._2, k._3))
  }
}

object IterNext {
  val cache = new ConcurrentHashMap[(String, Occurrence, Occurrence, DecisionTree), IterNext]()
  def apply(function: String, iterator: Occurrence, binding: Occurrence, child: DecisionTree): IterNext = {
    cache.computeIfAbsent((function, iterator, binding, child), k => new IterNext(k._1, k._2, k._3, k._4))
  }
}
