package org.kframework.backend.llvm.matching.dt

import org.kframework.backend.llvm.matching.Occurrence
import org.kframework.backend.llvm.matching.pattern._

import java.io.File
import java.io.FileWriter
import java.util.Map
import java.util.HashMap
import java.util.ArrayList
import com.esotericsoftware.yamlbeans.YamlWriter

sealed trait DecisionTree {
  def serializeToYaml(file: File): Unit = {
    val writer = new YamlWriter(new FileWriter(file))
    writer.write(representation)
    writer.close
  }

  def representation: AnyRef
}

case class Failure private() extends DecisionTree {
  val representation = "fail"
}

case class Leaf private(ordinal: Int, occurrences: Seq[Occurrence]) extends DecisionTree {
  val representation = new HashMap[String, AnyRef]()
  val action = new ArrayList[AnyRef]()
  representation.put("action", action)
  action.add(ordinal.asInstanceOf[AnyRef])
  private val os = new ArrayList[AnyRef]()
  action.add(os)
  for (occurrence <- occurrences) {
    os.add(occurrence.representation)
  }
}

case class Switch private(occurrence: Occurrence, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]) extends DecisionTree {
  val representation = new HashMap[String, AnyRef]()
  val specs = new ArrayList[AnyRef]()
  representation.put("specializations", specs)
  for ((c, dt) <- cases) {
    val _case = new ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    specs.add(_case)
  }
  representation.put("default", default.map(_.representation).getOrElse(null))
  representation.put("occurrence", occurrence.representation)
}

case class SwitchLit private(occurrence: Occurrence, bitwidth: Int, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]) extends DecisionTree {
  val representation = new HashMap[String, AnyRef]()
  val specs = new ArrayList[AnyRef]()
  representation.put("specializations", specs)
  for ((c, dt) <- cases) {
    val _case = new ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    specs.add(_case)
  }
  representation.put("bitwidth", bitwidth.asInstanceOf[AnyRef])
  representation.put("default", default.map(_.representation).getOrElse(null))
  representation.put("occurrence", occurrence.representation)
}

case class Function private(name: String, occurrence: Occurrence, vars: Seq[Occurrence], hook: String, child: DecisionTree) extends DecisionTree {
  val representation = new HashMap[String, AnyRef]()
  representation.put("function", name)
  representation.put("sort", hook)
  representation.put("occurrence", occurrence.representation)
  val args = new ArrayList[AnyRef]()
  representation.put("args", args)
  for (v <- vars) {
    args.add(v.representation)
  }
  representation.put("next", child.representation)
}

case class CheckNull private(occurrence: Occurrence, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]) extends DecisionTree {
  val representation = new HashMap[String, AnyRef]()
  val specs = new ArrayList[AnyRef]()
  representation.put("specializations", specs)
  for ((c, dt) <- cases) {
    val _case = new ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    specs.add(_case)
  }
  representation.put("default", default.map(_.representation).getOrElse(null))
  representation.put("isnull", true.asInstanceOf[AnyRef])
  representation.put("occurrence", occurrence.representation)
}

case class MakePattern private(occurrence: Occurrence, pattern: Pattern[Option[Occurrence]], child: DecisionTree) extends DecisionTree {
  val representation = new HashMap[String, AnyRef]()
  representation.put("pattern", representPattern(pattern))
  representation.put("occurrence", occurrence.representation)
  representation.put("next", child.representation)

  def representPattern(pattern: Pattern[Option[Occurrence]]): HashMap[String, AnyRef] = {
    val result = new HashMap[String, AnyRef]()
    pattern match {
      case OrP(_) | WildcardP() | VariableP(None, _) => ???
      case VariableP(Some(o), h) => {
        result.put("hook", h)
        result.put("occurrence", o.representation)
      }
      case AsP(_, _, p) => representPattern(p)
      case MapP(_, _, _, _, o) => representPattern(o)
      case SetP(_, _, _, o) => representPattern(o)
      case ListP(_, _, _, _, o) => representPattern(o)
      case LiteralP(s, h) => {
        result.put("hook", h)
        result.put("literal", s)
      }
      case SymbolP(s, ps) => {
        result.put("constructor", s.toString)
        val args = new ArrayList[AnyRef]()
        result.put("args", args)
        for (p <- ps) {
          args.add(representPattern(p))
        }
      }
    }
    result
  }
}

object Failure {
  private val instance = new Failure()
  def apply() = instance
}

object Leaf {
  private val cache = new HashMap[(Int, Seq[Occurrence]), Leaf]()
  def apply(ordinal: Int, occurrences: Seq[Occurrence]): Leaf = {
    cache.computeIfAbsent((ordinal, occurrences), k => new Leaf(k._1, k._2))
  }
}

object Switch {
  val cache = new HashMap[(Occurrence, Seq[(String, DecisionTree)], Option[DecisionTree]), Switch]()
  def apply(occurrence: Occurrence, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]): Switch = {
    cache.computeIfAbsent((occurrence, cases, default), k => new Switch(k._1, k._2, k._3))
  }
}

object SwitchLit {
  val cache = new HashMap[(Occurrence, Int, Seq[(String, DecisionTree)], Option[DecisionTree]), SwitchLit]()
  def apply(occurrence: Occurrence, bitwidth: Int, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]): SwitchLit = {
    cache.computeIfAbsent((occurrence, bitwidth, cases, default), k => new SwitchLit(k._1, k._2, k._3, k._4))
  }
}

object Function {
  val cache = new HashMap[(String, Occurrence, Seq[Occurrence], String, DecisionTree), Function]()
  def apply(name: String, occurrence: Occurrence, vars: Seq[Occurrence], hook: String, child: DecisionTree): Function = {
    cache.computeIfAbsent((name, occurrence, vars, hook, child), k => new Function(k._1, k._2, k._3, k._4, k._5))
  }
}

object CheckNull {
  val cache = new HashMap[(Occurrence, Seq[(String, DecisionTree)], Option[DecisionTree]), CheckNull]()
  def apply(occurrence: Occurrence, cases: Seq[(String, DecisionTree)], default: Option[DecisionTree]): CheckNull = {
    cache.computeIfAbsent((occurrence, cases, default), k => new CheckNull(k._1, k._2, k._3))
  }
}

object MakePattern {
  val cache = new HashMap[(Occurrence, Pattern[Option[Occurrence]], DecisionTree), MakePattern]()
  def apply(occurrence: Occurrence, pattern: Pattern[Option[Occurrence]], child: DecisionTree): MakePattern = {
    cache.computeIfAbsent((occurrence, pattern, child), k => new MakePattern(k._1, k._2, k._3))
  }
}
