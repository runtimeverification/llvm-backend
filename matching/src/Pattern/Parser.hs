{-# LANGUAGE GADTs      #-}
{-# LANGUAGE LambdaCase #-}
module Pattern.Parser where

import           Data.Functor.Foldable      (Fix (..), para)
import           Data.List.Index            (indexed)
import qualified Data.Map                   as Map
import           Data.Functor.Impredicative (Rotate31 (..))
import           Data.Proxy                 (Proxy (..))
import           Kore.AST.Common            (And (..), Equals (..),
                                             Pattern (..),
                                             Rewrites (..), 
                                             Variable, SymbolOrAlias (..),
                                             Application (..), Sort (..),
                                             And (..), Ceil (..), Equals (..), Exists (..),
                                             Floor (..), Forall (..), Implies (..), Iff (..),
                                             In (..), Next (..), Not (..), Or (..))
import           Kore.AST.Kore              (CommonKorePattern,
                                             UnifiedPattern (..), 
                                             UnifiedSortVariable)
import           Kore.AST.MetaOrObject      (Meta (..), Object (..),
                                             Unified (..))
import           Kore.AST.Sentence          (KoreDefinition,
                                             Definition (..), Module (..),
                                             SentenceAxiom (..),
                                             applyUnifiedSentence,
                                             ModuleName (..), Sentence (..))
import           Kore.ASTHelpers            (ApplicationSorts (..))
import           Kore.ASTVerifier.DefinitionVerifier
                                            (defaultAttributesVerification, verifyAndIndexDefinition)
import qualified Kore.Builtin               as Builtin
import           Kore.Error                 (printError)
import           Kore.IndexedModule.IndexedModule
                                            (KoreIndexedModule)
import           Kore.IndexedModule.MetadataTools
                                            (SortTools, extractMetadataTools,
                                             MetadataTools (..))
import           Kore.Parser.Parser         (fromKore)
import           Kore.Step.StepperAttributes
                                            (StepperAttributes (..))

--[ Metadata ]--

data SymLib = SymLib
  { symCs :: Map.Map (SymbolOrAlias Object) ([Sort Object], Sort Object)
  , symSt :: Map.Map (Sort Object) [SymbolOrAlias Object]
  } deriving (Show, Eq)

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
    rAlgebra (ApplicationPattern (Application s ps))      = s : (mconcat $ map snd ps)
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
         -> SortTools Object
         -> SymLib
mkSymLib symbols decls = foldl go (SymLib Map.empty Map.empty) symbols
  where
    go (SymLib dIx rIx) symbol =
      let as = decls symbol
          args = applicationSortsOperands as
          result = applicationSortsResult as
      in SymLib { symCs = Map.insert symbol (args, result) dIx
             , symSt = Map.insert result (symbol : (Map.findWithDefault [] result rIx)) rIx
             }

parseSymbols :: KoreDefinition -> KoreIndexedModule StepperAttributes -> SymLib
parseSymbols def indexedMod =
  let axioms = getAxioms def
      symbols = mconcat (parseAxiomForSymbols <$> axioms)
      metaTools = extractMetadataTools indexedMod
      symbolDecls = sortTools metaTools
  in mkSymLib symbols symbolDecls

--[ Patterns ]--

parseAxiomSentence :: (Int, SentenceAxiom UnifiedSortVariable UnifiedPattern Variable)
                   -> [(Int, Rewrites Object (Fix (UnifiedPattern Variable)), Maybe CommonKorePattern)]
parseAxiomSentence (i,sentence) = metaT sentence
  where
    metaT :: SentenceAxiom UnifiedSortVariable UnifiedPattern Variable
          -> [(Int,Rewrites Object (Fix (UnifiedPattern Variable)), Maybe CommonKorePattern)]
    metaT s = case splitAxiom (sentenceAxiomPattern s) of
      Just (r,sc) -> [(i,r,sc)]
      Nothing -> []

unifiedPatternRAlgebra :: (Pattern Meta variable (CommonKorePattern, b) -> b)
                       -> (Pattern Object variable (CommonKorePattern, b) -> b)
                       -> (UnifiedPattern variable (CommonKorePattern, b) -> b)
unifiedPatternRAlgebra metaT _ (UnifiedPattern (UnifiedMeta meta)) =
  metaT (unRotate31 meta)
unifiedPatternRAlgebra _ objectT (UnifiedPattern (UnifiedObject object)) =
  objectT (unRotate31 object)

splitAxiom :: CommonKorePattern
           -> Maybe (Rewrites Object CommonKorePattern, Maybe CommonKorePattern)
splitAxiom (Fix (UnifiedPattern topPattern)) =
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

parseAxioms :: KoreDefinition -> [(Int,Rewrites Object CommonKorePattern,Maybe CommonKorePattern)]
parseAxioms koreDefinition =
  let axioms = getAxioms koreDefinition
      withIndex = indexed axioms
  in mconcat (parseAxiomSentence <$> withIndex)

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

