module Main where

import qualified Data.ByteString.Char8 as B
import           Data.Map           ((!))
import           Data.Text          (unpack)
import           Kore.AST.Common    (Rewrites (..))
import           Pattern            (Action (..), Column (..), 
                                     mkClauseMatrix, compilePattern,
                                     serializeToYaml, failure)
import           Pattern.Parser     (parseDefinition, parseAxioms, mainVerify, parseSymbols)
import           Pattern.Gen        (bitwidth, genPattern, genVars, genMetadatas)
import           System.Environment (getArgs)

main :: IO ()
main = do
  (filename:moduleName:_) <- getArgs
  def <- parseDefinition filename
  let indexedMod = mainVerify def moduleName
  let axioms = parseAxioms def
  let (indices,rewrites,sideConditions) = unzip3 axioms
  let bw = bitwidth indexedMod
  let patterns = map (genPattern bw) rewrites
  let rhsVars = map (genVars . rewritesSecond) rewrites
  let scVars = map (maybe Nothing (Just . genVars)) sideConditions
  let actions = zipWith3 Action indices rhsVars scVars
  let symlib = parseSymbols def indexedMod
  let metas = genMetadatas symlib indexedMod
  let dt = case rewrites of
             [] -> failure
             r:_ -> let meta = metas ! rewritesSort r
                        col = Column  meta patterns
                        matrix = mkClauseMatrix [col] actions
                    in case matrix of 
                      Left err -> error (unpack err)
                      Right cm -> compilePattern cm
  B.putStrLn $ serializeToYaml dt
