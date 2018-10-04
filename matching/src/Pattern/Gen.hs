{-# LANGUAGE GADTs      #-}
{-# LANGUAGE FlexibleInstances #-}
module Pattern.Gen where

import qualified Pattern               as P
import           Pattern.Parser        (unifiedPatternRAlgebra,SymLib(..), getTopChildren,
                                        AxiomInfo(..))
import           Control.Monad.Free    (Free (..))
import           Data.Bits             (shiftL)
import           Data.Functor.Foldable (Fix (..), para)
import           Data.List             (transpose)
import qualified Data.Map              as Map
import           Data.Maybe            (maybe, isJust, fromJust)
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
import           Kore.ASTHelpers       (ApplicationSorts (..))
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

data ListCons = Concat | Unit | Element

genPattern :: KoreRewrite pattern => MetadataTools Object StepperAttributes -> SymLib -> pattern -> [Fix P.Pattern]
genPattern tools (SymLib _ sorts) rewrite =
  let lhs = getLeftHandSide rewrite
  in map (para (unifiedPatternRAlgebra (error "unsupported: meta level") rAlgebra)) lhs
  where
    rAlgebra :: Pattern Object Variable (CommonKorePattern,
                                         Fix P.Pattern)
             -> Fix P.Pattern
    rAlgebra (ApplicationPattern (Application sym ps)) =
      let att = getHook $ hook $ symAttributes tools sym
          sort = applicationSortsResult $ sortTools tools sym
      in case att of
        Just "LIST.concat" -> listPattern Concat (map snd ps) (getListElement (sorts Map.! sort))
        Just "LIST.unit" -> listPattern Unit [] (getListElement (sorts Map.! sort))
        Just "LIST.element" -> listPattern Element (map snd ps) (getListElement (sorts Map.! sort))
        Just _ -> Fix $ P.Pattern (P.Symbol sym) Nothing (map snd ps)
        Nothing -> Fix $ P.Pattern (P.Symbol sym) Nothing (map snd ps)
    rAlgebra (DomainValuePattern (DomainValue sort (Fix (StringLiteralPattern (StringLiteral str))))) =
      let att = getHook $ hook $ sortAttributes tools sort
      in Fix $ P.Pattern (if att == Just "BOOL.Bool" then case str of
                           "true" -> P.Literal "1"
                           "false" -> P.Literal "0"
                           _ -> P.Literal str else P.Literal str)
          (if att == Nothing then Just "STRING.String" else att) []
    rAlgebra (VariablePattern (Variable (Id name _) sort)) =
      let att = getHook $ hook $ sortAttributes tools sort
      in Fix $ P.Variable name $ maybe "STRING.String" id att
    rAlgebra (AndPattern (And _ p (_,Fix (P.Variable name hookAtt)))) = Fix $ P.As name hookAtt $ snd p
    rAlgebra pat = error $ show pat
    listPattern :: ListCons
                -> [Fix P.Pattern]
                -> SymbolOrAlias Object
                -> Fix P.Pattern
    listPattern Concat [Fix (P.ListPattern hd Nothing tl _), Fix (P.ListPattern hd' frame tl' _)] c =
      Fix (P.ListPattern (hd ++ tl ++ hd') frame tl' c)
    listPattern Concat [Fix (P.ListPattern hd frame tl _), Fix (P.ListPattern hd' Nothing tl' _)] c =
      Fix (P.ListPattern hd frame (tl ++ hd' ++ tl') c)
    listPattern Concat [Fix (P.ListPattern hd Nothing tl _), p@(Fix (P.Variable _ _))] c =
      Fix (P.ListPattern (hd ++ tl) (Just p) [] c)
    listPattern Concat [Fix (P.ListPattern hd Nothing tl _), p@(Fix P.Wildcard)] c =
      Fix (P.ListPattern (hd ++ tl) (Just p) [] c)
    listPattern Concat [p@(Fix (P.Variable _ _)), Fix (P.ListPattern hd Nothing tl _)] c =
      Fix (P.ListPattern [] (Just p) (hd ++ tl) c)
    listPattern Concat [p@(Fix P.Wildcard), Fix (P.ListPattern hd Nothing tl _)] c =
      Fix (P.ListPattern [] (Just p) (hd ++ tl) c)
    listPattern Unit [] c = Fix (P.ListPattern [] Nothing [] c)
    listPattern Element [p] c = Fix (P.ListPattern [p] Nothing [] c)
    listPattern Concat [_, Fix (P.ListPattern _ (Just _) _ _)] _ = error "unsupported list pattern"
    listPattern Concat [Fix (P.ListPattern _ (Just _) _ _), _] _ = error "unsupported list pattern"
    listPattern Concat [Fix (P.As _ _ _), _] _ = error "unsupported list pattern"
    listPattern Concat [_, Fix (P.As _ _ _)] _ = error "unsupported list pattern"
    listPattern Concat [Fix (P.Pattern _ _ _), _] _ = error "unsupported list pattern"
    listPattern Concat [_, Fix (P.Pattern _ _ _)] _ = error "unsupported list pattern"
    listPattern Concat [Fix P.Wildcard, _] _ = error "unsupported list pattern"
    listPattern Concat [Fix (P.Variable _ _), _] _ = error "unsupported list pattern"
    listPattern Concat [] _ = error "unsupported list pattern"
    listPattern Concat (_:[]) _ = error "unsupported list pattern"
    listPattern Concat (_:_:_:_) _ = error "unsupported list pattern"
    listPattern Unit (_:_) _ = error "unsupported list pattern"
    listPattern Element [] _ = error "unsupported list pattern"
    listPattern Element (_:_:_) _ = error "unsupported list pattern"
    getListElement :: [SymbolOrAlias Object] -> SymbolOrAlias Object
    getListElement syms = head $ filter isListElement syms
    isListElement :: SymbolOrAlias Object -> Bool
    isListElement sym =
      let att = getHook $ hook $ symAttributes tools sym
      in att == Just "LIST.element"

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

metaLookup :: (P.Constructor -> Maybe [P.Metadata]) -> P.Constructor -> Maybe [P.Metadata]
metaLookup f c@(P.Symbol _) = f c
metaLookup _ (P.Literal _) = Just []
metaLookup f (P.List c i) = Just $ replicate i $ head $ fromJust $ f $ P.Symbol c

defaultMetadata :: Sort Object -> P.Metadata
defaultMetadata sort = P.Metadata 1 (const []) sort $ metaLookup $ const Nothing

genMetadatas :: SymLib -> KoreIndexedModule StepperAttributes -> Map.Map (Sort Object) P.Metadata
genMetadatas syms@(SymLib symbols sorts) indexedMod =
  Map.mapMaybeWithKey genMetadata sorts
  where
    genMetadata :: Sort Object -> [SymbolOrAlias Object] -> Maybe P.Metadata
    genMetadata sort@(SortActualSort _) constructors =
      let att = sortAttributes (extractMetadataTools indexedMod) sort
          hookAtt = getHook $ hook att
          isInt = case hookAtt of
            Just "BOOL.Bool" -> True
            Just "MINT.MInt" -> True
            _                -> False
      in if isInt then
        let tools = extractMetadataTools indexedMod
            bw = bitwidth tools sort
        in Just $ P.Metadata (shiftL 1 bw) (const []) sort (metaLookup $ const Nothing)
      else
        let metadatas = genMetadatas syms indexedMod
            keys = map P.Symbol constructors
            args = map getArgs constructors
            injections = filter isInjection keys
            usedInjs = map (\c -> filter (isSubsort metadatas c) injections) injections
            children = map (map $ (\s -> Map.findWithDefault (defaultMetadata s) s metadatas)) args
            metaMap = Map.fromList (zip keys children)
            injMap = Map.fromList (zip injections usedInjs)
        in Just $ P.Metadata (toInteger $ length constructors) (injMap Map.!) sort $ metaLookup $ flip Map.lookup metaMap
    genMetadata _ _ = Nothing
    getArgs :: SymbolOrAlias Object -> [Sort Object]
    getArgs sym = sel1 $ symbols Map.! sym
    isInjection :: P.Constructor -> Bool
    isInjection (P.Symbol (SymbolOrAlias (Id "inj" _) _)) = True
    isInjection _ = False
    isSubsort :: Map.Map (Sort Object) P.Metadata -> P.Constructor -> P.Constructor -> Bool
    isSubsort metas (P.Symbol (SymbolOrAlias name [b,_])) (P.Symbol (SymbolOrAlias _ [a,_])) =
      let (P.Metadata _ _ _ childMeta) = Map.findWithDefault (defaultMetadata b) b metas
          child = P.Symbol (SymbolOrAlias name [a,b])
      in isJust $ childMeta $ child
    isSubsort _ _ _ = error "invalid injection"


genClauseMatrix :: KoreRewrite pattern 
               => SymLib
               -> KoreIndexedModule StepperAttributes
               -> [AxiomInfo pattern]
               -> [Sort Object]
               -> (P.ClauseMatrix, [P.Occurrence])
genClauseMatrix symlib indexedMod axioms sorts =
  let indices = map getOrdinal axioms
      rewrites = map getRewrite axioms
      sideConditions = map getSideCondition axioms
      tools = extractMetadataTools indexedMod
      patterns = map (genPattern tools symlib) rewrites
      rhsVars = map (genVars . getRightHandSide) rewrites
      scVars = map (maybe Nothing (Just . genVars)) sideConditions
      actions = zipWith3 P.Action indices rhsVars scVars
      metas = genMetadatas symlib indexedMod
      meta = map (metas Map.!) sorts
      col = zipWith P.mkColumn meta (transpose patterns)
  in case P.mkClauseMatrix col actions of
       Left err -> error (unpack err)
       Right m -> m

mkDecisionTree :: KoreRewrite pattern 
               => SymLib
               -> KoreIndexedModule StepperAttributes
               -> [AxiomInfo pattern]
               -> [Sort Object]
               -> Free P.Anchor P.Alias
mkDecisionTree symlib indexedMod axioms sorts =
  let matrix = genClauseMatrix symlib indexedMod axioms sorts
      dt = P.compilePattern matrix
  in P.shareDt dt
