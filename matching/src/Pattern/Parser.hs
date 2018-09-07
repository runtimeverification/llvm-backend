{-# LANGUAGE GADTs      #-}
{-# LANGUAGE LambdaCase #-}
module Pattern.Parser where

import           Data.Functor.Foldable      (Fix (..), para)
import           Data.List                  (sortBy)
import           Data.List.Index            (indexed)
import qualified Data.Map                   as Map
import           Data.Functor.Impredicative (Rotate31 (..))
import           Data.Ord                   (comparing)
import           Data.Proxy                 (Proxy (..))
import           Data.Tuple.Select          (sel1)
import           Kore.AST.Common            (And (..), Equals (..),
                                             Pattern (..),
                                             Rewrites (..), 
                                             Variable, SymbolOrAlias (..),
                                             Application (..), Sort (..),
                                             And (..), Ceil (..), Equals (..), Exists (..),
                                             Floor (..), Forall (..), Implies (..), Iff (..),
                                             In (..), Next (..), Not (..), Or (..), Id (..))
import           Kore.AST.Kore              (CommonKorePattern,
                                             UnifiedPattern (..), 
                                             UnifiedSortVariable)
import           Kore.AST.MetaOrObject      (Meta (..), Object (..),
                                             Unified (..))
import           Kore.AST.Sentence          (KoreDefinition,
                                             Definition (..), Module (..),
                                             SentenceAxiom (..),
                                             applyUnifiedSentence,
                                             ModuleName (..), Sentence (..),
                                             Attributes (..))
import           Kore.ASTHelpers            (ApplicationSorts (..))
import           Kore.ASTVerifier.DefinitionVerifier
                                            (defaultAttributesVerification, verifyAndIndexDefinition)
import qualified Kore.Builtin               as Builtin
import           Kore.Error                 (printError)
import           Kore.IndexedModule.IndexedModule
                                            (KoreIndexedModule)
import           Kore.IndexedModule.MetadataTools
                                            (extractMetadataTools,
                                             MetadataTools (..))
import           Kore.Parser.Parser         (fromKore)
import           Kore.Step.StepperAttributes
                                            (StepperAttributes (..))

--[ Metadata ]--

data SymLib = SymLib
  { symCs :: Map.Map (SymbolOrAlias Object) ([Sort Object], Sort Object, StepperAttributes)
  , symSt :: Map.Map (Sort Object) [SymbolOrAlias Object]
  } deriving (Show, Eq)

isConcrete :: SymbolOrAlias Object -> Bool
isConcrete (SymbolOrAlias _ params) = all isConcreteSort params
  where
    isConcreteSort :: Sort Object -> Bool
    isConcreteSort (SortActualSort _) = True
    isConcreteSort (SortVariableSort _) = False

parseAxiomForSymbols :: SentenceAxiom UnifiedSortVariable UnifiedPattern Variable
                     -> [SymbolOrAlias Object]
parseAxiomForSymbols = parsePatternForSymbols . sentenceAxiomPattern
  where
    parsePatternForSymbols :: CommonKorePattern -> [SymbolOrAlias Object]
    parsePatternForSymbols = para (unifiedPatternRAlgebra (const []) rAlgebra)
    rAlgebra :: Pattern Object Variable (CommonKorePattern,
                                     [SymbolOrAlias Object])
             -> [SymbolOrAlias Object]
    rAlgebra (AndPattern (And _ (_, p₀) (_, p₁)))         = p₀ ++ p₁
    rAlgebra (ApplicationPattern (Application s ps))      = (if isConcrete s then [s] else []) ++ (mconcat $ map snd ps)
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

mkSymLib :: [SymbolOrAlias Object] 
         -> MetadataTools Object StepperAttributes
         -> SymLib
mkSymLib symbols metaTools = foldl go (SymLib Map.empty Map.empty) symbols
  where
    go (SymLib dIx rIx) symbol =
      let as = (sortTools metaTools) symbol
          att = (symAttributes metaTools) symbol
          args = applicationSortsOperands as
          result = applicationSortsResult as
      in SymLib { symCs = Map.insert symbol (args, result, att) dIx
             , symSt = Map.insert result (symbol : (Map.findWithDefault [] result rIx)) rIx
             }

parseSymbols :: KoreDefinition -> KoreIndexedModule StepperAttributes -> SymLib
parseSymbols def indexedMod =
  let axioms = getAxioms def
      symbols = mconcat (parseAxiomForSymbols <$> axioms)
      metaTools = extractMetadataTools indexedMod
  in mkSymLib symbols metaTools

--[ Patterns ]--
hasAtt :: SentenceAxiom UnifiedSortVariable UnifiedPattern Variable
       -> String
       -> Bool
hasAtt sentence att = 
  let Attributes attr = sentenceAxiomAttributes sentence
  in any isAtt attr
  where
    isAtt :: CommonKorePattern -> Bool
    isAtt (Fix (UnifiedPattern (UnifiedObject object))) =
      case unRotate31 object of
        ApplicationPattern (Application (SymbolOrAlias (Id x _) _) _) -> x == att
        _ -> False
    isAtt _ = False

parseAxiomSentence :: (CommonKorePattern -> Maybe (pattern Object CommonKorePattern, Maybe CommonKorePattern))
                   -> (Int, SentenceAxiom UnifiedSortVariable UnifiedPattern Variable)
                   -> [(Bool,Int, pattern Object CommonKorePattern, Maybe CommonKorePattern)]
parseAxiomSentence split (i,s) = case split (sentenceAxiomPattern s) of
      Just (r,sc) -> if hasAtt s "comm" || hasAtt s "assoc" || hasAtt s "idem" then [] else [(hasAtt s "owise",i,r,sc)]
      Nothing -> []

unifiedPatternRAlgebra :: (Pattern Meta variable (CommonKorePattern, b) -> b)
                       -> (Pattern Object variable (CommonKorePattern, b) -> b)
                       -> (UnifiedPattern variable (CommonKorePattern, b) -> b)
unifiedPatternRAlgebra metaT _ (UnifiedPattern (UnifiedMeta meta)) =
  metaT (unRotate31 meta)
unifiedPatternRAlgebra _ objectT (UnifiedPattern (UnifiedObject object)) =
  objectT (unRotate31 object)

splitFunction :: SymbolOrAlias Object
              -> CommonKorePattern
              -> Maybe (Equals Object CommonKorePattern, Maybe CommonKorePattern)
splitFunction symbol (Fix (UnifiedPattern topPattern)) =
  case topPattern of
    UnifiedObject (Rotate31 (ImpliesPattern (Implies _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (EqualsPattern (Equals _ _ pat _)))))) (Fix (UnifiedPattern (UnifiedObject (Rotate31 (AndPattern (And _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (EqualsPattern eq@(Equals _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (ApplicationPattern (Application s _)))))) _)))))) _))))))))) -> if s == symbol then Just (eq, Just pat) else Nothing
    UnifiedObject (Rotate31 (ImpliesPattern (Implies _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (TopPattern _))))) (Fix (UnifiedPattern (UnifiedObject (Rotate31 (AndPattern (And _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (EqualsPattern eq@(Equals _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (ApplicationPattern (Application s _)))))) _)))))) _))))))))) -> if s == symbol then Just (eq, Nothing) else Nothing
    UnifiedObject (Rotate31 (ImpliesPattern (Implies _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (AndPattern (And _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (EqualsPattern (Equals _ _ pat _)))))))))))) (Fix (UnifiedPattern (UnifiedObject (Rotate31 (AndPattern (And _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (EqualsPattern eq@(Equals _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (ApplicationPattern (Application s _)))))) _)))))) _))))))))) -> if s == symbol then Just (eq, Just pat) else Nothing
    UnifiedObject (Rotate31 (ImpliesPattern (Implies _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (AndPattern (And _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (TopPattern _))))))))))) (Fix (UnifiedPattern (UnifiedObject (Rotate31 (AndPattern (And _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (EqualsPattern eq@(Equals _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (ApplicationPattern (Application s _)))))) _)))))) _))))))))) -> if s == symbol then Just (eq, Nothing) else Nothing
    UnifiedObject (Rotate31 (EqualsPattern eq@(Equals _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (ApplicationPattern (Application s _)))))) _))) -> if s == symbol then Just (eq, Nothing) else Nothing
    _ -> Nothing
        

splitTop :: CommonKorePattern
           -> Maybe (Rewrites Object CommonKorePattern, Maybe CommonKorePattern)
splitTop (Fix (UnifiedPattern topPattern)) =
  case topPattern of
    UnifiedObject (Rotate31 (AndPattern (And _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (EqualsPattern (Equals _ _ pat _)))))) (Fix (UnifiedPattern (UnifiedObject (Rotate31 (AndPattern ((And _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (RewritesPattern (r@(Rewrites _ _ _))))))))))))))))) -> Just (r, Just pat)
    UnifiedObject (Rotate31 (AndPattern (And _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (TopPattern _))))) (Fix (UnifiedPattern (UnifiedObject (Rotate31 (AndPattern (And _ _ (Fix (UnifiedPattern (UnifiedObject (Rotate31 (RewritesPattern (r@(Rewrites _ _ _)))))))))))))))) -> Just (r, Nothing)
    _ -> Nothing

getAxioms :: KoreDefinition -> [SentenceAxiom UnifiedSortVariable UnifiedPattern Variable]
getAxioms koreDefinition =
  let modules   = definitionModules koreDefinition
      sentences = mconcat (moduleSentences <$> modules)
  in mconcat ((applyUnifiedSentence metaT metaT) <$> sentences)
  where
    metaT :: Sentence lvl UnifiedSortVariable UnifiedPattern Variable
          -> [SentenceAxiom UnifiedSortVariable UnifiedPattern Variable]
    metaT = \case
      SentenceAxiomSentence s -> [s]
      _ -> []

parseTopAxioms :: KoreDefinition -> [(Int,Rewrites Object CommonKorePattern,Maybe CommonKorePattern)]
parseTopAxioms koreDefinition =
  let axioms = getAxioms koreDefinition
      withIndex = indexed axioms
  in map (\(_,a,b,c) -> (a,b,c)) $ mconcat ((parseAxiomSentence splitTop) <$> withIndex)

parseFunctionAxioms :: KoreDefinition -> SymbolOrAlias Object -> [(Int,Equals Object CommonKorePattern,Maybe CommonKorePattern)]
parseFunctionAxioms koreDefinition symbol =
  let axioms = getAxioms koreDefinition
      withIndex = indexed axioms
      withOwise = mconcat ((parseAxiomSentence (splitFunction symbol)) <$> withIndex)
      sorted = sortBy (comparing sel1) withOwise
  in map (\(_,a,b,c) -> (a,b,c)) sorted

parseDefinition :: FilePath -> IO KoreDefinition
parseDefinition fileName = do
  contents  <- readFile fileName
  let result = fromKore fileName contents
  case result of
    Left err         -> error err
    Right definition -> return definition

mainVerify
    :: KoreDefinition
    -> String
    -> KoreIndexedModule StepperAttributes
mainVerify definition mainModuleName =
    let attributesVerification = defaultAttributesVerification Proxy
        verifyResult = verifyAndIndexDefinition
                attributesVerification
                Builtin.koreVerifiers
                definition
    in case verifyResult of
        Left err1            -> error (printError err1)
        Right indexedModules -> case Map.lookup (ModuleName mainModuleName) indexedModules of
                                  Nothing -> error "Could not find main module"
                                  Just m -> m

-- Return the function symbol and whether or not the symbol is actually a function.
getPossibleFunction :: (SymbolOrAlias Object, ([Sort Object], Sort Object, StepperAttributes)) -> (SymbolOrAlias Object, Bool)
getPossibleFunction (k, (_,_,attrs)) =  (k, isFunction attrs)

-- Return the list of symbols that are actually functions.
getFunctions :: Map.Map (SymbolOrAlias Object) ([Sort Object], Sort Object, StepperAttributes)
    -> [SymbolOrAlias Object]

getFunctions = (fmap fst) . (filter snd) . (fmap getPossibleFunction) . Map.assocs

getTopChildren :: CommonKorePattern -> [CommonKorePattern]
getTopChildren (Fix (UnifiedPattern (UnifiedObject (Rotate31 (ApplicationPattern (Application _ ps)))))) = ps
getTopChildren (Fix (UnifiedPattern (UnifiedMeta (Rotate31 (ApplicationPattern (Application _ ps)))))) = ps
getTopChildren _ = error "Unexpected pattern on lhs of function"

