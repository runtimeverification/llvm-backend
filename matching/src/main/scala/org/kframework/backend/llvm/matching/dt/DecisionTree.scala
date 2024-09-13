package org.kframework.backend.llvm.matching.dt

import java.io.File
import java.io.FileOutputStream
import java.io.OutputStreamWriter
import java.nio.charset.Charset
import java.util
import java.util.concurrent.ConcurrentHashMap
import org.kframework.backend.llvm.matching.pattern._
import org.kframework.backend.llvm.matching.Occurrence
import org.yaml.snakeyaml.Yaml
import scala.collection.immutable

sealed trait DecisionTree {
  def serializeToYaml(file: File): Unit = {
    val writer = new OutputStreamWriter(
      new FileOutputStream(file),
      Charset.forName("UTF-8").newEncoder()
    )
    new Yaml().dump(representation, writer)
    writer.close()
  }

  def serializeToYaml(file: File, residuals: immutable.Seq[(Pattern[String], Occurrence)]): Unit = {
    val writer = new OutputStreamWriter(
      new FileOutputStream(file),
      Charset.forName("UTF-8").newEncoder()
    )
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

case class Failure private () extends DecisionTree {
  val representation              = "fail"
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class Leaf private (ordinal: Int, occurrences: immutable.Seq[(Occurrence, String)])
    extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val action         = new util.ArrayList[AnyRef]()
  representation.put("action", action)
  action.add(ordinal.asInstanceOf[AnyRef])
  private val os = new util.ArrayList[AnyRef]()
  action.add(os)
  for (occurrence <- occurrences) {
    val _var = new util.ArrayList[AnyRef]()
    _var.add(occurrence._1.representation)
    _var.add(occurrence._2)
    os.add(_var)
  }
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class SearchLeaf private (
    ordinal: Int,
    occurrences: immutable.Seq[(Occurrence, String)],
    child: DecisionTree
) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val action         = new util.ArrayList[AnyRef]()
  representation.put("action", action)
  action.add(ordinal.asInstanceOf[AnyRef])
  private val os = new util.ArrayList[AnyRef]()
  action.add(os)
  for (occurrence <- occurrences) {
    val _var = new util.ArrayList[AnyRef]()
    _var.add(occurrence._1.representation)
    _var.add(occurrence._2)
    os.add(_var)
  }
  representation.put("next", child.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class Switch private (
    occurrence: Occurrence,
    hook: String,
    cases: immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
    default: Option[DecisionTree]
) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val specs          = new util.ArrayList[AnyRef]()
  representation.put("sort", hook)
  representation.put("specializations", specs)
  for ((c, bindings, dt) <- cases) {
    val _bindings = new util.ArrayList[AnyRef]()
    val _case     = new util.ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    _case.add(_bindings)
    specs.add(_case)
    for (binding <- bindings)
      _bindings.add(binding)
  }
  representation.put("default", default.map(_.representation).orNull)
  representation.put("occurrence", occurrence.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class SwitchLit private (
    occurrence: Occurrence,
    hook: String,
    bitwidth: Int,
    cases: immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
    default: Option[DecisionTree]
) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val specs          = new util.ArrayList[AnyRef]()
  representation.put("sort", hook)
  representation.put("specializations", specs)
  for ((c, bindings, dt) <- cases) {
    val _bindings = new util.ArrayList[AnyRef]()
    val _case     = new util.ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    _case.add(_bindings)
    specs.add(_case)
    for (binding <- bindings)
      _bindings.add(binding)
  }
  representation.put("bitwidth", bitwidth.asInstanceOf[AnyRef])
  representation.put("default", default.map(_.representation).orNull)
  representation.put("occurrence", occurrence.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class Function private (
    name: String,
    occurrence: Occurrence,
    vars: immutable.Seq[(Occurrence, String)],
    hook: String,
    child: DecisionTree
) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  representation.put("function", name)
  representation.put("sort", hook)
  representation.put("occurrence", occurrence.representation)
  val args = new util.ArrayList[AnyRef]()
  representation.put("args", args)
  for (v <- vars) {
    val _var = new util.ArrayList[AnyRef]()
    _var.add(v._1.representation)
    _var.add(v._2)
    args.add(_var)
  }
  representation.put("next", child.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class CheckNull private (
    occurrence: Occurrence,
    hook: String,
    cases: immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
    default: Option[DecisionTree]
) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  val specs          = new util.ArrayList[AnyRef]()
  representation.put("sort", hook)
  representation.put("specializations", specs)
  for ((c, bindings, dt) <- cases) {
    val _bindings = new util.ArrayList[AnyRef]()
    val _case     = new util.ArrayList[AnyRef]()
    _case.add(c)
    _case.add(dt.representation)
    _case.add(_bindings)
    specs.add(_case)
    for (binding <- bindings)
      _bindings.add(binding)
  }
  representation.put("default", default.map(_.representation).orNull)
  representation.put("isnull", true.asInstanceOf[AnyRef])
  representation.put("occurrence", occurrence.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class MakePattern private (
    occurrence: Occurrence,
    hook: String,
    pattern: Pattern[Option[Occurrence]],
    child: DecisionTree
) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  representation.put("pattern", MakePattern.representPattern(pattern))
  representation.put("occurrence", occurrence.representation)
  representation.put("sort", hook)
  representation.put("next", child.representation)

  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class MakeIterator private (hookName: String, occurrence: Occurrence, child: DecisionTree)
    extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  representation.put("function", hookName)
  representation.put("collection", occurrence.representation)
  representation.put("sort", if (hookName == "set_iterator") "SET.Set" else "MAP.Map")
  representation.put("next", child.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

case class IterNext private (
    hookName: String,
    iterator: Occurrence,
    binding: Occurrence,
    child: DecisionTree
) extends DecisionTree {
  val representation = new util.HashMap[String, AnyRef]()
  representation.put("function", hookName)
  representation.put("iterator", iterator.representation)
  representation.put("sort", "STRING.String")
  representation.put("binding", binding.representation)
  representation.put("next", child.representation)
  override lazy val hashCode: Int = super.hashCode
  override def equals(that: Any): Boolean =
    that.isInstanceOf[AnyRef] && (this eq that.asInstanceOf[AnyRef])
}

object Failure {
  private val instance = new Failure()
  def apply(): Failure = Failure.instance
}

object Leaf {
  private val cache = new ConcurrentHashMap[(Int, immutable.Seq[(Occurrence, String)]), Leaf]()
  def apply(ordinal: Int, occurrences: immutable.Seq[(Occurrence, String)]): Leaf =
    cache.computeIfAbsent((ordinal, occurrences), k => new Leaf(k._1, k._2))
}

object SearchLeaf {
  private val cache =
    new ConcurrentHashMap[(Int, immutable.Seq[(Occurrence, String)], DecisionTree), SearchLeaf]()
  def apply(
      ordinal: Int,
      occurrences: immutable.Seq[(Occurrence, String)],
      child: DecisionTree
  ): SearchLeaf =
    cache.computeIfAbsent((ordinal, occurrences, child), k => new SearchLeaf(k._1, k._2, k._3))
}

object Switch {
  val cache = new ConcurrentHashMap[
    (
        Occurrence,
        String,
        immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
        Option[DecisionTree]
    ),
    Switch
  ]()
  def apply(
      occurrence: Occurrence,
      hook: String,
      cases: immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
      default: Option[DecisionTree]
  ): Switch =
    cache.computeIfAbsent(
      (occurrence, hook, cases, default),
      k => new Switch(k._1, k._2, k._3, k._4)
    )
}

object SwitchLit {
  val cache = new ConcurrentHashMap[
    (
        Occurrence,
        String,
        Int,
        immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
        Option[DecisionTree]
    ),
    SwitchLit
  ]()
  def apply(
      occurrence: Occurrence,
      hook: String,
      bitwidth: Int,
      cases: immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
      default: Option[DecisionTree]
  ): SwitchLit =
    cache.computeIfAbsent(
      (occurrence, hook, bitwidth, cases, default),
      k => new SwitchLit(k._1, k._2, k._3, k._4, k._5)
    )
}

object Function {
  val cache = new ConcurrentHashMap[
    (String, Occurrence, immutable.Seq[(Occurrence, String)], String, DecisionTree),
    Function
  ]()
  def apply(
      name: String,
      occurrence: Occurrence,
      vars: immutable.Seq[(Occurrence, String)],
      hook: String,
      child: DecisionTree
  ): Function =
    cache.computeIfAbsent(
      (name, occurrence, vars, hook, child),
      k => new Function(k._1, k._2, k._3, k._4, k._5)
    )
}

object CheckNull {
  val cache = new ConcurrentHashMap[
    (
        Occurrence,
        String,
        immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
        Option[DecisionTree]
    ),
    CheckNull
  ]()
  def apply(
      occurrence: Occurrence,
      hook: String,
      cases: immutable.Seq[(String, immutable.Seq[String], DecisionTree)],
      default: Option[DecisionTree]
  ): CheckNull =
    cache.computeIfAbsent(
      (occurrence, hook, cases, default),
      k => new CheckNull(k._1, k._2, k._3, k._4)
    )
}

object MakePattern {
  val cache = new ConcurrentHashMap[
    (Occurrence, String, Pattern[Option[Occurrence]], DecisionTree),
    MakePattern
  ]()
  def apply(
      occurrence: Occurrence,
      hook: String,
      pattern: Pattern[Option[Occurrence]],
      child: DecisionTree
  ): MakePattern =
    cache.computeIfAbsent(
      (occurrence, hook, pattern, child),
      k => new MakePattern(k._1, k._2, k._3, k._4)
    )
  def representPattern(pattern: Pattern[Option[Occurrence]]): util.HashMap[String, AnyRef] = {
    val result = new util.HashMap[String, AnyRef]()
    pattern match {
      case OrP(_) | WildcardP() | VariableP(None, _) => ???
      case VariableP(Some(o), h) =>
        result.put("hook", h.hookAtt)
        result.put("occurrence", o.representation)
      case AsP(_, _, p)            => return representPattern(p)
      case MapP(_, _, _, _, o)     => return representPattern(o)
      case SetP(_, _, _, o)        => return representPattern(o)
      case ListP(_, _, _, _, o)    => return representPattern(o)
      case ListGetP(_, _, _, _, o) => return representPattern(o)
      case LiteralP(s, h) =>
        result.put("hook", h.hookAtt)
        if (h.hookAtt == "BYTES.Bytes") {
          result.put("literal", s.getBytes("ISO-8859-1"))
        } else {
          result.put("literal", s)
        }
      case SymbolP(s, ps) =>
        result.put("constructor", s.toString)
        val args = new util.ArrayList[AnyRef]()
        result.put("args", args)
        for (p <- ps)
          args.add(representPattern(p))
    }
    assert(!result.isEmpty)
    result
  }
  def representResidual(pattern: Pattern[String]): util.HashMap[String, AnyRef] = {
    val result = new util.HashMap[String, AnyRef]()
    pattern match {
      case OrP(_) | WildcardP() => ???
      case VariableP(o, h) =>
        result.put("hook", h.hookAtt)
        result.put("occurrence", o)
      case AsP(_, _, p)            => return representResidual(p)
      case MapP(_, _, _, _, o)     => return representResidual(o)
      case SetP(_, _, _, o)        => return representResidual(o)
      case ListP(_, _, _, _, o)    => return representResidual(o)
      case ListGetP(_, _, _, _, o) => return representResidual(o)
      case LiteralP(s, h) =>
        result.put("hook", h.hookAtt)
        result.put("literal", s)
      case SymbolP(s, ps) =>
        result.put("constructor", s.toString)
        val args = new util.ArrayList[AnyRef]()
        result.put("args", args)
        for (p <- ps)
          args.add(representResidual(p))
    }
    assert(!result.isEmpty)
    result
  }
}

object MakeIterator {
  val cache = new ConcurrentHashMap[(String, Occurrence, DecisionTree), MakeIterator]()
  def apply(function: String, occurrence: Occurrence, child: DecisionTree): MakeIterator =
    cache.computeIfAbsent((function, occurrence, child), k => new MakeIterator(k._1, k._2, k._3))
}

object IterNext {
  val cache = new ConcurrentHashMap[(String, Occurrence, Occurrence, DecisionTree), IterNext]()
  def apply(
      function: String,
      iterator: Occurrence,
      binding: Occurrence,
      child: DecisionTree
  ): IterNext =
    cache.computeIfAbsent(
      (function, iterator, binding, child),
      k => new IterNext(k._1, k._2, k._3, k._4)
    )
}
