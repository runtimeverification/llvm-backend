package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.dt._
import org.kframework.parser.kore._
import org.kframework.parser.kore.parser.TextToKore
import org.kframework.utils.errorsystem.KException

import java.util.Optional
import java.io.File
import java.io.FileWriter

object Matching {
  def writeDecisionTreeToFile(filename: File, heuristic: String, outputFolder: File, threshold: Optional[(Int, Int)], kem: KException => Unit) {
    val defn = new TextToKore().parse(filename)
    outputFolder.mkdirs()
    val allAxioms = Parser.getAxioms(defn).zipWithIndex
    val axioms = Parser.parseTopAxioms(allAxioms)
    val symlib = Parser.parseSymbols(defn, heuristic)
    val (dt, matrix) = if (axioms.isEmpty) {
      (Failure(), null)
    } else {
      val matrix = Generator.genClauseMatrix(symlib, defn, axioms, Seq(axioms.head.rewrite.sort))
      matrix.checkUsefulness(kem)
      (matrix.compile, matrix)
    }
    val funcAxioms = Parser.parseFunctionAxioms(allAxioms)
    val sortedFuncAxioms = symlib.functions.map(funcAxioms.getOrElse(_, IndexedSeq()))
    val sorts = symlib.functions.map(symlib.signatures(_)._1)
    val dts = symlib.functions.map(f => {
      if (logging) {
        System.out.println("Compiling " + f)
      }
      Generator.mkDecisionTree(symlib, defn, funcAxioms.getOrElse(f, IndexedSeq()), symlib.signatures(f)._1, f, kem)
    })
    val path = new File(outputFolder, "dt.yaml")
    dt.serializeToYaml(path)
    if (threshold.isPresent) {
      axioms.foreach(a => {
        if (logging) {
          System.out.println("Compiling decision tree for axiom " + a.ordinal)
        }
        Matrix.clearCache
        val dt = Generator.mkSpecialDecisionTree(symlib, defn, matrix, a, threshold.get)
        val ordinal = a.ordinal
        val filename = "dt_" + ordinal + ".yaml"
        if (dt.isDefined) {
          dt.get._1.serializeToYaml(new File(outputFolder, filename), dt.get._2)
        }
      })
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

  def getThreshold(arg: String): Optional[(Int, Int)] = {
    val (numeratorStr, denominatorStr) = if (arg.indexOf('/') == -1) {
      (arg, "1")
    } else {
      (arg.substring(0, arg.indexOf('/')), arg.substring(arg.indexOf('/') + 1))
    }
    val numerator = numeratorStr.trim.toInt
    val denominator = denominatorStr.trim.toInt
    if (numerator == 0) {
      Optional.empty[(Int, Int)]
    } else {
      Optional.of((numerator, denominator))
    }
  }

  def main(args: Array[String]): Unit = {
    val file = new File(args(0))
    val outputFolder = new File(args(2))
    logging = args.size > 4
    writeDecisionTreeToFile(file, args(1), outputFolder, getThreshold(args(3)), (e) => ())
  }
}
