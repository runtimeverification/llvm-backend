{-# LANGUAGE LambdaCase #-}
module Pattern.Parser where

import           Data.Map              (Map, findWithDefault, insert)
import qualified Data.Map              as Map (empty)
import           Data.Maybe            (catMaybes)

import           Kore.AST.Common       (Symbol (..), Variable)
import           Kore.AST.Kore         (UnifiedPattern, UnifiedSort,
                                        UnifiedSortVariable)
import           Kore.AST.MetaOrObject (Unified (..), asUnified)
import           Kore.AST.Sentence     (Definition (..), KoreDefinition,
                                        Module (..), Sentence (..),
                                        SentenceSymbol (..), UnifiedSentence,
                                        applyUnifiedSentence)
import           Kore.Parser.Parser    (fromKore)

--[ Metadata ]--

data SymLib = SymLib
  { symCs :: Map (Unified Symbol) ([UnifiedSort], UnifiedSort)
  , symSt :: Map UnifiedSort [(Unified Symbol)]
  } deriving (Show, Eq)

parseSymbolSentence :: UnifiedSentence UnifiedSortVariable UnifiedPattern Variable
                    -> Maybe (Unified Symbol, ([UnifiedSort], UnifiedSort))
parseSymbolSentence = applyUnifiedSentence metaT objectT
  where
    metaT   = \case
      SentenceSymbolSentence s ->
        Just (asUnified $ sentenceSymbolSymbol s,
              (asUnified <$> sentenceSymbolSorts s,
               asUnified $ sentenceSymbolResultSort s))
      _ -> Nothing
    objectT = \case
      SentenceSymbolSentence s ->
        Just (asUnified $ sentenceSymbolSymbol s,
              (asUnified <$> sentenceSymbolSorts s,
               asUnified $ sentenceSymbolResultSort s))
      _ -> Nothing

mkSymLib :: [(Unified Symbol, ([UnifiedSort], UnifiedSort))]
         -> SymLib
mkSymLib = foldl go (SymLib Map.empty Map.empty)
  where
    go (SymLib dIx rIx) (symbol, (args, result)) =
      SymLib { symCs = insert symbol (args, result) dIx
             , symSt = insert result (symbol : (findWithDefault [] result rIx)) rIx
             }

parseSymbols :: KoreDefinition -> SymLib
parseSymbols koreDefinition =
  let modules   = definitionModules koreDefinition
      sentences = mconcat (moduleSentences <$> modules)
      symbols   = catMaybes (parseSymbolSentence <$> sentences)
  in mkSymLib symbols

parseDefinition :: FilePath -> IO KoreDefinition
parseDefinition fileName = do
  contents  <- readFile fileName
  let result = fromKore fileName contents
  case result of
    Left err         -> error err
    Right definition -> return definition
