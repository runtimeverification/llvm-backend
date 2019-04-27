package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.dt._
import org.kframework.parser.kore._
import org.kframework.parser.kore.parser.TextToKore

import java.io.File

object Matching {
  def writeDecisionTreeToFile(filename: File, moduleName: String, outputFolder: File) {
    val defn = new TextToKore().parse(filename)
    outputFolder.mkdirs()
    val allAxioms = Parser.getAxioms(defn).zipWithIndex
    val axioms = Parser.parseTopAxioms(allAxioms)
    val symlib = Parser.parseSymbols(defn)
    val dt = if (axioms.isEmpty) {
      Failure()
    } else {
      Generator.mkDecisionTree(symlib, defn, axioms, Seq(axioms.head.rewrite.sort))
    }
    val funcAxioms = Parser.parseFunctionAxioms(allAxioms)
    val sortedFuncAxioms = symlib.functions.map(funcAxioms.getOrElse(_, IndexedSeq()))
    val sorts = symlib.functions.map(symlib.signatures(_)._1)
    val dts = symlib.functions.map(f => Generator.mkDecisionTree(symlib, defn, funcAxioms.getOrElse(f, IndexedSeq()), symlib.signatures(f)._1))
    val path = new File(outputFolder, "dt.yaml")
    dt.serializeToYaml(path)
    val files = (symlib.functions, dts).zipped.toIterable
    for (pair <- files) {
      pair._2.serializeToYaml(new File(outputFolder, pair._1.ctr + ".yaml"))
    }
  }

  var logging = false

  def main(args: Array[String]): Unit = {
    val file = new File(args(0))
    val outputFolder = new File(args(2))
    logging = args.size > 3
    writeDecisionTreeToFile(file, args(1), outputFolder)
  }
}
