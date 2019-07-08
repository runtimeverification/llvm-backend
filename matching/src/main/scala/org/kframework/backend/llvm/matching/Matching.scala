package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.dt._
import org.kframework.parser.kore._
import org.kframework.parser.kore.parser.TextToKore

import java.io.File
import java.io.FileWriter

object Matching {
  def writeDecisionTreeToFile(filename: File, heuristic: String, outputFolder: File, iterated: Boolean) {
    val defn = new TextToKore().parse(filename)
    outputFolder.mkdirs()
    val allAxioms = Parser.getAxioms(defn).zipWithIndex
    val axioms = Parser.parseTopAxioms(allAxioms)
    val symlib = Parser.parseSymbols(defn, heuristic)
    val (dt, matrix) = if (axioms.isEmpty) {
      (Failure(), null)
    } else {
      val matrix = Generator.genClauseMatrix(symlib, defn, axioms, Seq(axioms.head.rewrite.sort))
      (matrix.compile, matrix)
    }
    val funcAxioms = Parser.parseFunctionAxioms(allAxioms)
    val sortedFuncAxioms = symlib.functions.map(funcAxioms.getOrElse(_, IndexedSeq()))
    val sorts = symlib.functions.map(symlib.signatures(_)._1)
    val dts = symlib.functions.map(f => {
      if (logging) {
        System.out.println("Compiling " + f)
      }
      Generator.mkDecisionTree(symlib, defn, funcAxioms.getOrElse(f, IndexedSeq()), symlib.signatures(f)._1)
    })
    val path = new File(outputFolder, "dt.yaml")
    dt.serializeToYaml(path)
    if (iterated) {
      val specialDts = axioms.map(a => {
        if (logging) {
          System.out.println("Compiling decision tree for axiom " + a.ordinal)
        }
        Matrix.clearCache
        Generator.mkSpecialDecisionTree(symlib, defn, matrix, a)
      })
      val specialFiles = (axioms, specialDts).zipped.toIterable
      for (pair <- specialFiles) {
        val ordinal = pair._1.ordinal
        val filename = "dt_" + ordinal + ".yaml"
        if (pair._2.isDefined) {
          pair._2.get._1.serializeToYaml(new File(outputFolder, filename), pair._2.get._2)
        }
      }
    }
    val files = (symlib.functions, dts).zipped.toIterable
    val index = new File(outputFolder, "index.txt")
    val writer = new FileWriter(index)
    for (pair <- files) {
      val sym = pair._1.ctr
      val filename = (if (sym.length > 250) sym.substring(0, 250) else sym) + ".yaml"
      pair._2.serializeToYaml(new File(outputFolder, filename))
      writer.write(pair._1.ctr + "\t" + filename + "\n")
    }
    writer.close
  }

  var logging = false

  def main(args: Array[String]): Unit = {
    val file = new File(args(0))
    val outputFolder = new File(args(2))
    logging = args.size > 4
    writeDecisionTreeToFile(file, args(1), outputFolder, args(3) == "on")
  }
}
