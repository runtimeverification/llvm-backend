{-# LANGUAGE GADTs      #-}
{-# LANGUAGE FlexibleInstances #-}
module Pattern.Gen where

import qualified Pattern               as P (Pattern (..),Metadata (..), DecisionTree, Action (..), Column(..),
                                             mkClauseMatrix, compilePattern)
import           Pattern.Parser        (unifiedPatternRAlgebra,SymLib(..), getTopChildren)
import           Data.Bits             (shiftL)
import           Data.Functor.Foldable (Fix (..), para)
import           Data.List             (transpose)
import qualified Data.Map              as Map
import           Data.Text             (unpack)
import           Data.Tuple.Select     (sel1)
import           Kore.AST.Common       (Rewrites (..), Sort (..),
                                        Variable (..), Application (..),
                                        DomainValue (..), StringLiteral (..),
                                        And (..), Ceil (..), Equals (..), Exists (..),
                                        Floor (..), Forall (..), Implies (..), Iff (..),
                                        In (..), Next (..), Not (..), Or (..),
                                        Pattern (..), Id (..), SymbolOrAlias (..))
import           Kore.AST.Kore         (CommonKorePattern)
import           Kore.AST.MetaOrObject (Object (..))
import           Kore.Builtin.Hook     (Hook (..))
import           Kore.IndexedModule.IndexedModule
                                       (KoreIndexedModule)
import           Kore.IndexedModule.MetadataTools
                                       (MetadataTools (..), extractMetadataTools)
import           Kore.Step.StepperAttributes
                                       (StepperAttributes (..))
import           Kore.Unparser.Unparse (unparseToString)

bitwidth :: KoreIndexedModule StepperAttributes -> Sort Object -> Int
bitwidth mainModule sort = 
  let tools = extractMetadataTools mainModule
      att = sortAttributes tools sort
      hookAtt = hook att
  in case getHook hookAtt of
      Just "BOOL.Bool" -> 1
      _ -> error "Unsupported pattern type"

class KoreRewrite a where
  getLeftHandSide :: a -> [CommonKorePattern]
  getRightHandSide :: a -> CommonKorePattern

instance KoreRewrite (Equals lvl CommonKorePattern) where
  getLeftHandSide = getTopChildren . equalsFirst
  getRightHandSide = equalsSecond

instance KoreRewrite (Rewrites lvl CommonKorePattern) where
  getLeftHandSide = (: []) . rewritesFirst
  getRightHandSide = rewritesSecond

genPattern :: KoreRewrite pattern => (Sort Object -> Int) -> pattern -> [Fix P.Pattern]
genPattern getBitwidth rewrite =
  let lhs = getLeftHandSide rewrite
  in map (para (unifiedPatternRAlgebra rAlgebra rAlgebra)) lhs
  where
    rAlgebra :: Pattern lvl Variable (CommonKorePattern,
                                     Fix P.Pattern)
             -> Fix P.Pattern
    rAlgebra (ApplicationPattern (Application sym ps)) = Fix $ P.Pattern (unparseToString sym) Nothing (map snd ps)
    rAlgebra (DomainValuePattern (DomainValue sort (Fix (StringLiteralPattern (StringLiteral str))))) =
      Fix $ P.Pattern (case str of
                         "true" -> "1"
                         "false" -> "0"
                         _ -> str)
        (Just $ getBitwidth sort) []
    rAlgebra (VariablePattern (Variable (Id name _) _)) = Fix $ P.Variable name
    rAlgebra _ = error "Unsupported pattern type"

genVars :: CommonKorePattern -> [String]
genVars = para (unifiedPatternRAlgebra rAlgebra rAlgebra)
  where
    rAlgebra :: Pattern lvl Variable (CommonKorePattern,
                                     [String])
             -> [String]
    rAlgebra (VariablePattern (Variable (Id name _) _)) = [name]
    rAlgebra (AndPattern (And _ (_, p₀) (_, p₁)))         = p₀ ++ p₁
    rAlgebra (ApplicationPattern (Application _ ps))      = mconcat $ map snd ps
    rAlgebra (CeilPattern (Ceil _ _ (_, p)))              = p
    rAlgebra (EqualsPattern (Equals _ _ (_, p₀) (_, p₁))) = p₀ ++ p₁
    rAlgebra (ExistsPattern (Exists _ _ (_, p)))          = p
    rAlgebra (FloorPattern (Floor _ _ (_, p)))            = p
    rAlgebra (ForallPattern (Forall _ _ (_, p)))          = p
    rAlgebra (IffPattern (Iff _ (_, p₀) (_, p₁)))         = p₀ ++ p₁
    rAlgebra (ImpliesPattern (Implies _ (_, p₀) (_, p₁))) = p₀ ++ p₁
    rAlgebra (InPattern (In _ _ (_, p₀) (_, p₁)))         = p₀ ++ p₁
    rAlgebra (NextPattern (Next _ (_, p)))                = p
    rAlgebra (NotPattern (Not _ (_, p)))                  = p
    rAlgebra (OrPattern (Or _ (_, p₀) (_, p₁)))           = p₀ ++ p₁
    rAlgebra _                                            = []

defaultMetadata :: P.Metadata
defaultMetadata = P.Metadata 1 (const [])

genMetadatas :: SymLib -> KoreIndexedModule StepperAttributes -> Map.Map (Sort Object) P.Metadata
genMetadatas syms@(SymLib symbols sorts) indexedMod =
  Map.mapMaybeWithKey genMetadata sorts
  where
    genMetadata :: Sort Object -> [SymbolOrAlias Object] -> Maybe P.Metadata
    genMetadata sort@(SortActualSort _) constructors =
      let att = sortAttributes (extractMetadataTools indexedMod) sort
          hookAtt = getHook $ hook att
          isToken = case hookAtt of
            Just "BOOL.Bool" -> True
            Just "MINT.MInt" -> True
            _                -> False
      in if isToken then
        let bw = bitwidth indexedMod sort
        in Just $ P.Metadata (shiftL 1 bw) (const [])
      else
        let metadatas = genMetadatas syms indexedMod
            args = map getArgs constructors
            children = map (map $ (\s -> Map.findWithDefault defaultMetadata s metadatas)) args
            names = map unparseToString constructors
            metaMap = Map.fromList (zip names children)
        in Just $ P.Metadata (toInteger $ length constructors) ((Map.!) metaMap)
    genMetadata _ _ = Nothing
    getArgs :: SymbolOrAlias Object -> [Sort Object]
    getArgs sym = sel1 $ symbols Map.! sym

mkDecisionTree :: KoreRewrite pattern 
               => SymLib
               -> KoreIndexedModule StepperAttributes
               -> [(Int, pattern, Maybe CommonKorePattern)]
               -> [Sort Object]
               -> Fix P.DecisionTree
mkDecisionTree symlib indexedMod axioms sorts =
  let (indices,rewrites,sideConditions) = unzip3 axioms
      bw = bitwidth indexedMod
      patterns = map (genPattern bw) rewrites
      rhsVars = map (genVars . getRightHandSide) rewrites
      scVars = map (maybe Nothing (Just . genVars)) sideConditions
      actions = zipWith3 P.Action indices rhsVars scVars
      metas = genMetadatas symlib indexedMod
      meta = map (metas Map.!) sorts
      col = zipWith P.Column meta (transpose patterns)
      matrix = P.mkClauseMatrix col actions
  in case matrix of 
       Left err -> error (unpack err)
       Right cm -> P.compilePattern cm


