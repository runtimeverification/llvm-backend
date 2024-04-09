package org.kframework.backend.llvm.matching

import com.runtimeverification.k.kore._
import com.runtimeverification.k.kore.parser.TextToKore
import java.io.File
import java.io.FileWriter
import java.util.Optional
import org.kframework.backend.llvm.matching.dt._
import scala.collection.immutable
import scala.collection.parallel.CollectionConverters._

object Matching {
  def writeDecisionTreeToFile(
      filename: File,
      heuristic: String,
      outputFolder: File,
      threshold: Optional[(Int, Int)],
      genSingleRuleTrees: Boolean,
      warn: Boolean,
      genSearch: Boolean,
      kem: MatchingException => Unit
  ): Unit = {
    val defn = new TextToKore().parse(filename)
    outputFolder.mkdirs()
    val allAxioms    = Parser.getAxioms(defn)
    val axioms       = Parser.parseTopAxioms(allAxioms, search = false)
    val searchAxioms = Parser.parseTopAxioms(allAxioms, search = true)
    val symlib       = Parser.parseSymbols(defn, heuristic)
    val (dt, dtSearch, matrix) = if (axioms.isEmpty) {
      (Failure(), Failure(), null)
    } else {
      val matrix =
        Generator.genClauseMatrix(symlib, defn, axioms, immutable.Seq(axioms.head.rewrite.sort))
      val searchMatrix =
        Generator.genClauseMatrix(
          symlib,
          defn,
          searchAxioms,
          immutable.Seq(searchAxioms.head.rewrite.sort)
        )
      if (warn) {
        searchMatrix.checkUsefulness(kem)
      }
      if (genSearch) {
        (matrix.compile, searchMatrix.compileSearch, matrix)
      } else {
        (matrix.compile, Failure(), matrix)
      }
    }
    if (genSingleRuleTrees) {
      for (axiom <- axioms.par) {
        val matrix =
          Generator.genClauseMatrix(
            symlib,
            defn,
            immutable.IndexedSeq(axiom),
            immutable.Seq(axiom.rewrite.sort)
          )
        val dt       = matrix.compile
        val filename = "match_" + axiom.ordinal + ".yaml"
        dt.serializeToYaml(new File(outputFolder, filename))
      }
    }
    val funcAxioms = Parser.parseFunctionAxioms(allAxioms, simplification = false)
    val dts = symlib.functions.map { f =>
      if (logging) {
        System.out.println("Compiling " + f)
      }
      if (symlib.isHooked(f)) {
        Failure()
      } else {
        Generator.mkDecisionTree(
          symlib,
          defn,
          funcAxioms.getOrElse(f, immutable.IndexedSeq()),
          symlib.signatures(f)._1,
          f,
          kem
        )
      }
    }
    val path       = new File(outputFolder, "dt.yaml")
    val pathSearch = new File(outputFolder, "dt-search.yaml")
    dt.serializeToYaml(path)
    dtSearch.serializeToYaml(pathSearch)
    if (threshold.isPresent) {
      axioms.par.foreach { a =>
        if (logging) {
          System.out.println("Compiling decision tree for axiom " + a.ordinal)
        }
        Matrix.clearCache()
        val dt       = Generator.mkSpecialDecisionTree(symlib, defn, matrix, a, threshold.get)
        val ordinal  = a.ordinal
        val filename = "dt_" + ordinal + ".yaml"
        if (dt.isDefined) {
          dt.get._1.serializeToYaml(new File(outputFolder, filename), dt.get._2)
        }
      }
    }
    val files  = symlib.functions.lazyZip(dts).toSeq
    val index  = new File(outputFolder, "index.txt")
    val writer = new FileWriter(index)
    var idx    = 0
    for (pair <- files.par) {
      val sym      = pair._1.ctr
      val filename = (if (sym.length > 240) sym.substring(0, 240) + idx else sym) + ".yaml"
      pair._2.serializeToYaml(new File(outputFolder, filename))
      writer.write(pair._1.ctr + "\t" + filename + "\n")
      idx += 1
    }
    writer.close()
  }

  var logging = false

  def getThreshold(arg: String): Optional[(Int, Int)] = {
    val (numeratorStr, denominatorStr) = if (arg.indexOf('/') == -1) {
      (arg, "1")
    } else {
      (arg.substring(0, arg.indexOf('/')), arg.substring(arg.indexOf('/') + 1))
    }
    val numerator   = numeratorStr.trim.toInt
    val denominator = denominatorStr.trim.toInt
    if (numerator == 0) {
      Optional.empty[(Int, Int)]
    } else {
      Optional.of((numerator, denominator))
    }
  }

  def main(args: Array[String]): Unit = {
    val file         = new File(args(0))
    val outputFolder = new File(args(2))
    logging = args.length > 4
    writeDecisionTreeToFile(
      file,
      args(1),
      outputFolder,
      getThreshold(args(3)),
      genSingleRuleTrees = true,
      warn = true,
      genSearch = true,
      _ => ()
    )
  }
}
