module Pattern.Parser where

import Control.Comonad.Trans.Cofree
       ( CofreeF (..) )
import Data.Functor.Foldable
       ( para )
import Data.List
       ( find, nub, sortOn )
import Data.List.Index
       ( indexed )
import Data.Maybe
       ( fromMaybe, isJust )
import Data.Proxy
       ( Proxy (..) )
import Data.Text
       ( Text )

import qualified Data.Map as Map

import Kore.AST.Common
       ( And (..), Application (..), Ceil (..), Equals (..), Exists (..),
       Floor (..), Forall (..), Iff (..), Implies (..), In (..), Next (..),
       Not (..), Or (..), Pattern (..), Rewrites (..), SymbolOrAlias (..),
       Variable )
import Kore.AST.Identifier
       ( Id (..) )
import Kore.AST.Kore
       ( CommonKorePattern, KorePattern, UnifiedPattern (..),
       UnifiedSortVariable )
import Kore.AST.MetaOrObject
       ( Meta (..), Object (..) )
import Kore.AST.Sentence
       ( Attributes (..), Definition (..), KoreDefinition, KoreSentenceAxiom,
       Module (..), ModuleName (..), Sentence (..), SentenceAxiom (..),
       applyUnifiedSentence )
import Kore.ASTHelpers
       ( ApplicationSorts (..) )
import Kore.ASTVerifier.DefinitionVerifier
       ( defaultAttributesVerification, verifyAndIndexDefinition )
import Kore.Error
       ( printError )
import Kore.IndexedModule.IndexedModule
       ( VerifiedModule )
import Kore.IndexedModule.MetadataTools
       ( MetadataTools (..), extractMetadataTools )
import Kore.IndexedModule.Resolvers
       ( getHeadApplicationSorts )
import Kore.Parser.Parser
       ( fromKore )
import Kore.Sort
       ( Sort (..) )
import KorePatterns
       ( pattern And_, pattern App_, pattern Bottom_, pattern Equals_,
       pattern Implies_, pattern KoreObjectPattern, pattern Rewrites_,
       pattern Top_ )

import qualified Kore.Builtin as Builtin
import qualified Kore.Domain.Builtin as Domain

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

parseAxiomForSymbols :: KoreSentenceAxiom
                     -> [SymbolOrAlias Object]
parseAxiomForSymbols = parsePatternForSymbols . sentenceAxiomPattern
  where
    parsePatternForSymbols :: CommonKorePattern -> [SymbolOrAlias Object]
    parsePatternForSymbols = para (unifiedPatternRAlgebra (const []) rAlgebra)
    rAlgebra :: Pattern Object Domain.Builtin Variable
                 (CommonKorePattern, [SymbolOrAlias Object])
             -> [SymbolOrAlias Object]
    rAlgebra (AndPattern (And _ (_, p₀) (_, p₁)))         = p₀ ++ p₁
    rAlgebra (ApplicationPattern (Application s ps))      = [s | isConcrete s] ++ mconcat (map snd ps)
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
         -> VerifiedModule Attributes Attributes
         -> [(SymbolOrAlias Object, SymbolOrAlias Object)]
         -> SymLib
mkSymLib symbols sortDecls ixModule overloads =
  let empty = replicate (length sortDecls) []
      (SymLib sorts syms _) = foldl go (SymLib Map.empty (Map.fromList $ zip sortDecls empty) Map.empty) symbols
  in SymLib sorts (Map.map nub syms) (Map.map nub $ foldl mkOverloads Map.empty overloads)
  where
    metaTools = extractMetadataTools ixModule
    go (SymLib dIx rIx oIx) symbol =
      let as = getHeadApplicationSorts ixModule symbol
          att = symAttributes metaTools symbol
          args = applicationSortsOperands as
          result = applicationSortsResult as
      in SymLib { symCs = Map.insert symbol (args, result, att) dIx
             , symSt = Map.insert result (symbol : Map.findWithDefault [] result rIx) rIx
             , symOs = oIx
             }
    mkOverloads oIx (greater,lesser) =
      Map.insert greater (lesser : Map.findWithDefault [] greater oIx) oIx

getOverloads :: [KoreSentenceAxiom]
             -> [(SymbolOrAlias Object, SymbolOrAlias Object)]
getOverloads [] = []
getOverloads (s : tl) =
  case getAtt s "overload" of
    Nothing -> getOverloads tl
    Just (KoreObjectPattern (ApplicationPattern (Application _ [KoreObjectPattern (ApplicationPattern (Application g _)),KoreObjectPattern (ApplicationPattern (Application l _))]))) -> (g,l) : getOverloads tl
    Just _ -> error "invalid overload attribute"

parseSymbols :: KoreDefinition
             -> VerifiedModule Attributes Attributes
             -> SymLib
parseSymbols def indexedMod =
  let axioms = getAxioms def
      symbols = mconcat (parseAxiomForSymbols <$> axioms)
      allSorts = concatMap symbolOrAliasParams symbols
      overloads = getOverloads axioms
  in mkSymLib symbols allSorts indexedMod overloads

--[ Patterns ]--
getAtt :: KoreSentenceAxiom
       -> Text
       -> Maybe CommonKorePattern
getAtt sentence att =
  let Attributes attr = sentenceAxiomAttributes sentence
  in find (isAtt att) attr

isAtt :: Text -> CommonKorePattern -> Bool
isAtt att (KoreObjectPattern pat) =
  case pat of
    ApplicationPattern (Application (SymbolOrAlias (Id x _) _) _) -> x == att
    _ -> False
isAtt _ _ = False

hasAtt :: KoreSentenceAxiom
       -> Text
       -> Bool
hasAtt sentence att =
  isJust (getAtt sentence att)

rulePriority :: KoreSentenceAxiom
             -> Int
rulePriority s
  | hasAtt s "owise" = 3
  | hasAtt s "cool"  = 2
  | hasAtt s "heat"  = 1
  | otherwise        = 0

data AxiomInfo pat = AxiomInfo
                   { getPriority :: Int
                   , getOrdinal :: Int
                   , getRewrite :: pat
                   , getSideCondition :: Maybe CommonKorePattern
                   }

parseAxiomSentence :: (CommonKorePattern -> Maybe (pat, Maybe CommonKorePattern))
                   -> (Int, KoreSentenceAxiom)
                   -> [AxiomInfo pat]
parseAxiomSentence split (i,s) = case split (sentenceAxiomPattern s) of
      Just (r,sc) -> if hasAtt s "comm" || hasAtt s "assoc" || hasAtt s "idem" then [] else [AxiomInfo (rulePriority s) i r sc]
      Nothing -> []

unifiedPatternRAlgebra :: (Pattern Meta Domain.Builtin Variable
                           (CommonKorePattern, b) -> b)
                       -> (Pattern Object Domain.Builtin Variable
                           (CommonKorePattern, b) -> b)
                       -> CofreeF
                          (UnifiedPattern Domain.Builtin Variable)
                          annotation
                          (CommonKorePattern, b)
                       -> b
unifiedPatternRAlgebra metaT _   (_ :< UnifiedMetaPattern   pat) = metaT   pat
unifiedPatternRAlgebra _ objectT (_ :< UnifiedObjectPattern pat) = objectT pat

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
    extract :: Functor d => KorePattern d v a -> Equals Object (KorePattern d v a)
    extract (KoreObjectPattern (EqualsPattern eq)) = eq
    extract _                                      = undefined -- ^ This is only called after matching equals, so it should never reach this point.


splitTop :: CommonKorePattern
           -> Maybe (Rewrites Object CommonKorePattern, Maybe CommonKorePattern)
splitTop topPattern =
  case topPattern of
    And_ _ (Equals_ _ _ pat _)
           (And_ _ _ rw@Rewrites_{}) ->
      Just (extract rw, Just pat)
    And_ _ (Top_ _)
           (And_ _ _ rw@Rewrites_{}) ->
      Just (extract rw, Nothing)
    Rewrites_ s (And_ _ (Equals_ _ _ pat _) l)
           (And_ _ _ r) ->
      Just (Rewrites s l r, Just pat)
    Rewrites_ s (And_ _ (Top_ _) l)
           (And_ _ _ r) ->
      Just (Rewrites s l r, Nothing)
    Implies_ _ (Bottom_ _) p -> splitTop p
    KoreObjectPattern (ImpliesPattern (Implies _ (KoreObjectPattern (BottomPattern _)) p)) -> splitTop p
    _ -> Nothing
  where
    extract :: Functor d => KorePattern d v a -> Rewrites Object (KorePattern d v a)
    extract (KoreObjectPattern (RewritesPattern rw)) = rw
    extract _ = undefined -- ^ This is only called after matching rewrites, so it should never reach this point.

getAxioms :: KoreDefinition -> [KoreSentenceAxiom]
getAxioms koreDefinition =
  let modules   = definitionModules koreDefinition
      sentences = mconcat (moduleSentences <$> modules)
  in mconcat (applyUnifiedSentence metaT metaT <$> sentences)
  where
    metaT :: Sentence lvl UnifiedSortVariable CommonKorePattern
          -> [KoreSentenceAxiom]
    metaT = \case
      SentenceAxiomSentence s -> [s]
      _ -> []

parseTopAxioms :: KoreDefinition -> [AxiomInfo (Rewrites Object CommonKorePattern)]
parseTopAxioms koreDefinition =
  let axioms = getAxioms koreDefinition
      withIndex = indexed axioms
      withOwise = mconcat (parseAxiomSentence splitTop <$> withIndex)
  in sortOn getPriority withOwise

parseFunctionAxioms :: KoreDefinition -> SymbolOrAlias Object -> [AxiomInfo (Equals Object CommonKorePattern)]
parseFunctionAxioms koreDefinition symbol =
  let axioms = getAxioms koreDefinition
      withIndex = indexed axioms
      withOwise = mconcat (parseAxiomSentence (splitFunction symbol) <$> withIndex)
  in sortOn getPriority withOwise

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
    -> VerifiedModule Attributes Attributes
mainVerify definition mainModuleName =
    let attributesVerification = defaultAttributesVerification Proxy Proxy
        verifyResult = verifyAndIndexDefinition
                attributesVerification
                Builtin.koreVerifiers
                definition
    in case verifyResult of
        Left err1            -> error (printError err1)
        Right indexedModules -> fromMaybe (error "Could not find main module")
                                (Map.lookup (ModuleName mainModuleName) indexedModules)

-- Return the function symbol and whether or not the symbol is actually a function.
getPossibleFunction :: (SymbolOrAlias Object, ([Sort Object], Sort Object, Attributes)) -> (SymbolOrAlias Object, Bool)
getPossibleFunction (k, (_,_,Attributes attrs)) =  (k, isJust (find (isAtt "function") attrs) || isJust (find (isAtt "anywhere") attrs))

-- Return the list of symbols that are actually functions.
getFunctions :: Map.Map (SymbolOrAlias Object) ([Sort Object], Sort Object, Attributes)
    -> [SymbolOrAlias Object]

getFunctions = fmap fst . filter snd . fmap getPossibleFunction . Map.assocs

getTopChildren :: CommonKorePattern -> [CommonKorePattern]
getTopChildren (KoreObjectPattern (ApplicationPattern (Application _ ps))) = ps
getTopChildren (KoreObjectPattern (ApplicationPattern (Application _ ps))) = ps
getTopChildren _ = error "Unexpected pattern on lhs of function"

