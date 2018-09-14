{-# LANGUAGE GADTs      #-}
{-# LANGUAGE FlexibleInstances #-}
module Pattern.Gen where

import qualified Pattern               as P
import           Pattern.Parser        (unifiedPatternRAlgebra,SymLib(..), getTopChildren)
import           Control.Monad.Free    (Free (..))
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

bitwidth :: MetadataTools Object StepperAttributes -> Sort Object -> Int
bitwidth tools sort = 
  let att = sortAttributes tools sort
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
  in map (para (unifiedPatternRAlgebra (error "unsupported: meta level") rAlgebra)) lhs
  where
    rAlgebra :: Pattern Object Variable (CommonKorePattern,
                                         Fix P.Pattern)
             -> Fix P.Pattern
    rAlgebra (ApplicationPattern (Application sym ps)) = Fix $ P.Pattern (Left sym) Nothing (map snd ps)
    rAlgebra (DomainValuePattern (DomainValue sort (Fix (StringLiteralPattern (StringLiteral str))))) =
      Fix $ P.Pattern (case str of
                         "true" -> Right "1"
                         "false" -> Right "0"
                         _ -> Right str)
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

defaultMetadata :: Sort Object -> P.Metadata
defaultMetadata sort = P.Metadata 1 [] sort (const $ Just [])

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
        let tools = extractMetadataTools indexedMod
            bw = bitwidth tools sort
        in Just $ P.Metadata (shiftL 1 bw) [] sort (const $ Just [])
      else
        let metadatas = genMetadatas syms indexedMod
            keys = map Left constructors
            args = map getArgs constructors
            injections = filter isInjection keys
            children = map (map $ (\s -> Map.findWithDefault (defaultMetadata sort) s metadatas)) args
            metaMap = Map.fromList (zip keys children)
        in Just $ P.Metadata (toInteger $ length constructors) injections sort (flip Map.lookup metaMap)
    genMetadata _ _ = Nothing
    getArgs :: SymbolOrAlias Object -> [Sort Object]
    getArgs sym = sel1 $ symbols Map.! sym
    isInjection :: P.Constructor -> Bool
    isInjection (Left (SymbolOrAlias (Id "inj" _) _)) = True
    isInjection _ = False

genClauseMatrix :: KoreRewrite pattern 
               => SymLib
               -> KoreIndexedModule StepperAttributes
               -> [(Int, pattern, Maybe CommonKorePattern)]
               -> [Sort Object]
               -> (P.ClauseMatrix, [P.Occurrence])
genClauseMatrix symlib indexedMod axioms sorts =
  let (indices,rewrites,sideConditions) = unzip3 axioms
      tools = extractMetadataTools indexedMod
      bw = bitwidth tools
      patterns = map (genPattern bw) rewrites
      rhsVars = map (genVars . getRightHandSide) rewrites
      scVars = map (maybe Nothing (Just . genVars)) sideConditions
      actions = zipWith3 P.Action indices rhsVars scVars
      metas = genMetadatas symlib indexedMod
      meta = map (metas Map.!) sorts
      col = zipWith P.Column meta (transpose patterns)
  in case P.mkClauseMatrix col actions of
       Left err -> error (unpack err)
       Right m -> m

mkDecisionTree :: KoreRewrite pattern 
               => SymLib
               -> KoreIndexedModule StepperAttributes
               -> [(Int, pattern, Maybe CommonKorePattern)]
               -> [Sort Object]
               -> Free P.Anchor P.Alias
mkDecisionTree symlib indexedMod axioms sorts =
  let matrix = genClauseMatrix symlib indexedMod axioms sorts
      dt = P.compilePattern matrix
  in P.shareDt dt
