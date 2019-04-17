package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.dt._
import org.kframework.parser.kore._
import org.kframework.parser.kore.parser.TextToKore

import java.io.File

object Matching {
  def writeDecisionTreeToFile(filename: File, moduleName: String, outputFolder: File) {
    val defn = new TextToKore().parse(filename)
    outputFolder.mkdirs();
    val axioms = Parser.parseTopAxioms(defn)
    val symlib = Parser.parseSymbols(defn)
    val dt = if (axioms.isEmpty) {
      Failure()
    } else {
      Generator.mkDecisionTree(symlib, defn, axioms, Seq(axioms.head.rewrite.sort))
    }
    val funcAxioms = symlib.functions.map(Parser.parseFunctionAxioms(defn, _))
    val sorts = symlib.functions.map(symlib.signatures(_)._1)
    val dts = (funcAxioms, sorts).zipped.toIterable.map(pair => Generator.mkDecisionTree(symlib, defn, pair._1, pair._2))
    val path = new File(outputFolder, "dt.yaml")
    dt.serializeToYaml(path)
    val files = (symlib.functions, dts).zipped.toIterable
    for (pair <- files) {
      pair._2.serializeToYaml(new File(outputFolder, pair._1.ctr + ".yaml"))
    }
  }

  def main(args: Array[String]): Unit = {
    val file = new File(args(0))
    val outputFolder = new File(args(2))
    writeDecisionTreeToFile(file, args(1), outputFolder)
  }
}
