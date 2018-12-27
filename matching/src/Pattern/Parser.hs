{-# LANGUAGE GADTs             #-}
{-# LANGUAGE LambdaCase        #-}
{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE PatternSynonyms   #-}
module Pattern.Parser where

import           Data.Functor.Foldable
                 ( Fix (..), cata, para )
import           Data.Functor.Impredicative
                 ( Rotate31 (..) )
import           Data.List
                 ( find, nub, sortBy )
import           Data.List.Index
                 ( indexed )
import qualified Data.Map as Map
import           Data.Maybe
                 ( isJust )
import           Data.Ord
                 ( comparing )
import           Data.Proxy
                 ( Proxy (..) )
import           Data.Text
                 ( Text )
import           Kore.AST.Common
                 ( And (..), Application (..), Ceil (..), Equals (..),
                 Exists (..), Floor (..), Forall (..), Id (..), Iff (..),
                 Implies (..), In (..), Next (..), Not (..), Or (..),
                 Pattern (..), Rewrites (..), Sort (..), SymbolOrAlias (..),
                 Variable )
import           Kore.AST.Kore
                 ( CommonKorePattern, pattern KoreObjectPattern, KorePattern,
                 UnifiedPattern (..), UnifiedSortVariable )
import           Kore.AST.MetaOrObject
                 ( Meta (..), MetaOrObject (..), Object (..), Unified (..),
                 transformUnified )
import           Kore.AST.Sentence
                 ( Attributes (..), Definition (..), KoreDefinition,
                 Module (..), ModuleName (..), Sentence (..),
                 SentenceAxiom (..), applyUnifiedSentence )
import           Kore.ASTHelpers
                 ( ApplicationSorts (..) )
import           Kore.ASTVerifier.DefinitionVerifier
                 ( defaultAttributesVerification, verifyAndIndexDefinition )
import qualified Kore.Builtin as Builtin
import           Kore.Error
                 ( printError )
import           Kore.IndexedModule.IndexedModule
                 ( KoreIndexedModule )
import           Kore.IndexedModule.MetadataTools
                 ( MetadataTools (..), extractMetadataTools )
import           Kore.Parser.Parser
                 ( fromKore )
import           KorePatterns
                 ( pattern And_, pattern App_, pattern Bottom_,
                 pattern Equals_, pattern Implies_, pattern Rewrites_,
                 pattern Top_ )

--[ Metadata ]--

data SymLib = SymLib
  { symCs :: Map.Map (SymbolOrAlias Object) ([Sort Object], Sort Object, Attributes)
  , symSt :: Map.Map (Sort Object) [SymbolOrAlias Object]
  , symOs :: Map.Map (SymbolOrAlias Object) [SymbolOrAlias Object]
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
    rAlgebra (RewritesPattern (Rewrites _ (_, p₀) (_, p₁))) = p₀ ++ p₁
    rAlgebra _                                            = []

mkSymLib :: [SymbolOrAlias Object]
         -> [Sort Object]
         -> MetadataTools Object Attributes
         -> [(SymbolOrAlias Object, SymbolOrAlias Object)]
         -> SymLib
mkSymLib symbols sortDecls metaTools overloads =
  let empty = replicate (length sortDecls) []
      (SymLib sorts syms _) = foldl go (SymLib Map.empty (Map.fromList $ zip sortDecls empty) Map.empty) symbols
  in SymLib sorts (Map.map nub syms) (Map.map nub $ foldl mkOverloads Map.empty overloads)
  where
    go (SymLib dIx rIx oIx) symbol =
      let as = (symbolOrAliasSorts metaTools) symbol
          att = (symAttributes metaTools) symbol
          args = applicationSortsOperands as
          result = applicationSortsResult as
      in SymLib { symCs = Map.insert symbol (args, result, att) dIx
             , symSt = Map.insert result (symbol : (Map.findWithDefault [] result rIx)) rIx
             , symOs = oIx
             }
    mkOverloads oIx (greater,lesser) =
      Map.insert greater (lesser : (Map.findWithDefault [] greater oIx)) oIx

getOverloads :: [SentenceAxiom UnifiedSortVariable UnifiedPattern Variable]
             -> [(SymbolOrAlias Object, SymbolOrAlias Object)]
getOverloads [] = []
getOverloads (s : tl) =
  case getAtt s "overload" of
    Nothing -> getOverloads tl
    Just (KoreObjectPattern (ApplicationPattern (Application _ [(KoreObjectPattern (ApplicationPattern (Application g _))),(KoreObjectPattern (ApplicationPattern (Application l _)))]))) -> (g,l):(getOverloads tl)
    Just _ -> error "invalid overload attribute"

parseSymbols :: KoreDefinition -> KoreIndexedModule Attributes -> SymLib
parseSymbols def indexedMod =
  let axioms = getAxioms def
      symbols = mconcat (parseAxiomForSymbols <$> axioms)
      allSorts = concatMap symbolOrAliasParams symbols
      metaTools = extractMetadataTools indexedMod
      overloads = getOverloads axioms
  in mkSymLib symbols allSorts metaTools overloads

--[ Patterns ]--
getAtt :: SentenceAxiom UnifiedSortVariable UnifiedPattern Variable
       -> Text
       -> Maybe CommonKorePattern
getAtt sentence att =
  let Attributes attr = sentenceAxiomAttributes sentence
  in find (isAtt att) attr

isAtt :: Text -> CommonKorePattern -> Bool
isAtt att (Fix (UnifiedPattern (UnifiedObject object))) =
  case unRotate31 object of
    ApplicationPattern (Application (SymbolOrAlias (Id x _) _) _) -> x == att
    _ -> False
isAtt _ _ = False


hasAtt :: SentenceAxiom UnifiedSortVariable UnifiedPattern Variable
       -> Text
       -> Bool
hasAtt sentence att =
  isJust (getAtt sentence att)

rulePriority :: SentenceAxiom UnifiedSortVariable UnifiedPattern Variable
             -> Int
rulePriority s =
  if hasAtt s "owise" then 3 else
  if hasAtt s "cool" then 2 else
  if hasAtt s "heat" then 1 else
  0

data AxiomInfo pat = AxiomInfo
                   { getPriority :: Int
                   , getOrdinal :: Int
                   , getRewrite :: pat
                   , getSideCondition :: Maybe CommonKorePattern
                   }

parseAxiomSentence :: (CommonKorePattern -> Maybe (pat, Maybe CommonKorePattern))
                   -> (Int, SentenceAxiom UnifiedSortVariable UnifiedPattern Variable)
                   -> [AxiomInfo pat]
parseAxiomSentence split (i,s) = case split (sentenceAxiomPattern s) of
      Just (r,sc) -> if hasAtt s "comm" || hasAtt s "assoc" || hasAtt s "idem" then [] else [AxiomInfo (rulePriority s) i r sc]
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
splitFunction symbol topPattern =
  case topPattern of
    Implies_ _ (Equals_ _ _ pat _)
               (And_ _ eq@(Equals_ _ _ (App_ s _) _) _) ->
      if s == symbol
      then Just (extract eq, Just pat)
      else Nothing
    Implies_ _ (Top_ _)
               (And_ _ eq@(Equals_ _ _ (App_ s _) _) _) ->
      if s == symbol
      then Just (extract eq, Nothing)
      else Nothing
    Implies_ _ (And_ _ _ (Equals_ _ _ pat _))
               (And_ _ eq@(Equals_ _ _ (App_ s _) _) _) ->
      if s == symbol
      then Just (extract eq, Just pat)
      else Nothing
    Implies_ _ (And_ _ _ (Top_ _))
               (And_ _ eq@(Equals_ _ _ (App_ s _) _) _) ->
      if s == symbol
      then Just (extract eq, Nothing)
      else Nothing
    eq@(Equals_ _ _ (App_ s _) _) ->
      if s == symbol
      then Just (extract eq, Nothing)
      else Nothing
    _ -> Nothing
  where
    extract :: KorePattern v -> Equals Object (KorePattern v)
    extract (KoreObjectPattern (EqualsPattern eq)) = eq
    extract _                                      = undefined -- ^ This is only called after matching equals, so it should never reach this point.


splitTop :: CommonKorePattern
           -> Maybe (Rewrites Object CommonKorePattern, Maybe CommonKorePattern)
splitTop topPattern =
  case topPattern of
    And_ _ (Equals_ _ _ pat _)
           (And_ _ _ rw@(Rewrites_ _ _ _)) ->
      Just (extract rw, Just pat)
    And_ _ (Top_ _)
           (And_ _ _ rw@(Rewrites_ _ _ _)) ->
      Just (extract rw, Nothing)
    Implies_ _ (Bottom_ _) p -> splitTop p
    KoreObjectPattern (ImpliesPattern (Implies _ (KoreObjectPattern (BottomPattern _)) p)) -> splitTop p
    _ -> Nothing
  where
    extract :: KorePattern v -> Rewrites Object (KorePattern v)
    extract (KoreObjectPattern (RewritesPattern rw)) = rw
    extract _ = undefined -- ^ This is only called after matching rewrites, so it should never reach this point.

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

parseTopAxioms :: KoreDefinition -> [AxiomInfo (Rewrites Object CommonKorePattern)]
parseTopAxioms koreDefinition =
  let axioms = getAxioms koreDefinition
      withIndex = indexed axioms
      withOwise = mconcat ((parseAxiomSentence splitTop) <$> withIndex)
  in sortBy (comparing getPriority) withOwise

parseFunctionAxioms :: KoreDefinition -> SymbolOrAlias Object -> [AxiomInfo (Equals Object CommonKorePattern)]
parseFunctionAxioms koreDefinition symbol =
  let axioms = getAxioms koreDefinition
      withIndex = indexed axioms
      withOwise = mconcat ((parseAxiomSentence (splitFunction symbol)) <$> withIndex)
  in sortBy (comparing getPriority) withOwise

parseDefinition :: FilePath -> IO KoreDefinition
parseDefinition fileName = do
  contents  <- readFile fileName
  let result = fromKore fileName contents
  case result of
    Left err         -> error err
    Right definition -> return definition

mainVerify
    :: KoreDefinition
    -> Text
    -> KoreIndexedModule Attributes
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
getPossibleFunction :: (SymbolOrAlias Object, ([Sort Object], Sort Object, Attributes)) -> (SymbolOrAlias Object, Bool)
getPossibleFunction (k, (_,_,Attributes attrs)) =  (k, (isJust $ find (isAtt "function") attrs) || (isJust $ find (isAtt "anywhere") attrs))

-- Return the list of symbols that are actually functions.
getFunctions :: Map.Map (SymbolOrAlias Object) ([Sort Object], Sort Object, Attributes)
    -> [SymbolOrAlias Object]

getFunctions = (fmap fst) . (filter snd) . (fmap getPossibleFunction) . Map.assocs

getTopChildren :: CommonKorePattern -> [CommonKorePattern]
getTopChildren (KoreObjectPattern (ApplicationPattern (Application _ ps))) = ps
getTopChildren (KoreObjectPattern (ApplicationPattern (Application _ ps))) = ps
getTopChildren _ = error "Unexpected pattern on lhs of function"

