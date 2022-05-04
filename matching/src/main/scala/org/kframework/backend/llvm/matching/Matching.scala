package org.kframework.backend.llvm.matching

import org.kframework.backend.llvm.matching.dt._
import org.kframework.parser.kore._
import org.kframework.parser.kore.parser.TextToKore
import org.kframework.utils.errorsystem.KException

import java.util.Optional
import java.io.File
import java.io.FileWriter

object Matching {
  def writeDecisionTreeToFile(filename: File, heuristic: String, outputFolder: File, threshold: Optional[(Int, Int)], genSingleRuleTrees: Boolean, warn: Boolean, genSearch: Boolean, kem: KException => Unit) {
    val defn = new TextToKore().parse(filename)
    outputFolder.mkdirs()
    val allAxioms = Parser.getAxioms(defn).zipWithIndex
    val axioms = Parser.parseTopAxioms(allAxioms)
    val symlib = Parser.parseSymbols(defn, heuristic)
    val (dt, dtSearch, matrix) = if (axioms.isEmpty) {
      (Failure(), Failure(), null)
    } else {
      val matrix = Generator.genClauseMatrix(symlib, defn, axioms, Seq(axioms.head.rewrite.sort))
      if (warn) {
        matrix.checkUsefulness(kem)
      }
      if (genSearch) {
        (matrix.compile, matrix.compileSearch, matrix)
      } else {
        (matrix.compile, Failure(), matrix)
      }
    }
    if (genSingleRuleTrees) {
      for (axiom <- axioms.par) {
        val matrix = Generator.genClauseMatrix(symlib, defn, IndexedSeq(axiom), Seq(axiom.rewrite.sort))
        val dt = matrix.compile
        val filename = "match_" + axiom.ordinal + ".yaml"
        dt.serializeToYaml(new File(outputFolder, filename))
      }
    }
    val funcAxioms = Parser.parseFunctionAxioms(allAxioms)
    val sortedFuncAxioms = symlib.functions.map(funcAxioms.getOrElse(_, IndexedSeq()))
    val sorts = symlib.functions.map(symlib.signatures(_)._1)
    val dts = symlib.functions.map(f => {
      if (logging) {
        System.out.println("Compiling " + f)
      }
      if (symlib.isHooked(f)) {
        Failure()
      } else {
        Generator.mkDecisionTree(symlib, defn, funcAxioms.getOrElse(f, IndexedSeq()), symlib.signatures(f)._1, f, kem)
      }
    })
    val path = new File(outputFolder, "dt.yaml")
    val pathSearch = new File(outputFolder, "dt-search.yaml")
    dt.serializeToYaml(path)
    dtSearch.serializeToYaml(pathSearch)
    if (threshold.isPresent) {
      axioms.par.foreach(a => {
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
    var idx = 0
    for (pair <- files.par) {
      val sym = pair._1.ctr
      val filename = (if (sym.length > 240) sym.substring(0, 240) + idx else sym) + ".yaml"
      pair._2.serializeToYaml(new File(outputFolder, filename))
      writer.write(pair._1.ctr + "\t" + filename + "\n")
      idx+=1
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
    writeDecisionTreeToFile(file, args(1), outputFolder, getThreshold(args(3)), true, true, true, (e) => ())
  }
}
