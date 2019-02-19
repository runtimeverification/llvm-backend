module Pattern.Type
  ( Column (..)
  , Metadata (..)
  , Occurrence (..)
  , Action (..)
  , ClauseMatrix (..)
  , Clause (..)
  , PatternMatrix (..)
  , VariableBinding (..)
  , Ignoring (..)
  , Fringe
  , Constructor (..)
  , Symbol (..)
  , Literal (..)
  , getPatternConstructor
  , getConstructorPattern
  ) where

import Data.Functor.Classes
       ( Show1 (..) )
import Data.Functor.Foldable
       ( Fix (..) )
import Data.Text
       ( Text, pack )

import Kore.AST.Common
       ( SymbolOrAlias (..) )
import Kore.AST.MetaOrObject
       ( Object (..) )
import Kore.Sort
       ( Sort (..) )

import qualified Data.Yaml.Builder as Y

data Column p bp = Column
                   { getMetadata :: !(Metadata bp)
                   , getTerms    :: ![Fix p]
                   }

instance Show1 p => Show (Column p bp) where
  showsPrec _ (Column _ ts) =
    showString "Column " . showList ts

data Metadata bp = Metadata
                   { getLength :: !Integer
                   , getInjections :: Constructor bp -> [Constructor bp]
                   , getOverloads :: Constructor bp -> [Constructor bp] -- list of overloaded productions less than specified production
                   , getSort :: Sort Object
                   , getChildren :: Constructor bp -> Maybe [Metadata bp]
                   }

instance Show (Metadata bp) where
  show (Metadata _ _ _ sort _) = show sort

data Occurrence = Num Int Occurrence
                | Base
                | Lit Text Text
                | Equal Occurrence Occurrence
                | SC Int
                | Value Text Occurrence
                | Rem Text Occurrence
                | Size Occurrence
                | Inj Occurrence
                deriving (Show, Eq, Ord)

serializeOccurrence :: Occurrence -> [Text]
serializeOccurrence (Num i o) = pack (show i) : serializeOccurrence o
serializeOccurrence Base = []
serializeOccurrence (Lit s1 s2) = ["lit", s1, s2]
serializeOccurrence (Equal o1 o2) = "eq" : (serializeOccurrence o1 ++ ["and"] ++ serializeOccurrence o2)
serializeOccurrence (SC i) = [pack $ "side_condition_" ++ show i]
serializeOccurrence (Value p o) = pack (show p ++ "_val") : serializeOccurrence o
serializeOccurrence (Rem p o) = pack (show p ++ "_rem") : serializeOccurrence o
serializeOccurrence (Size o) = "size" : serializeOccurrence o
serializeOccurrence (Inj o) = "-1" : serializeOccurrence o

instance Y.ToYaml Occurrence where
  toYaml o = Y.toYaml $ serializeOccurrence o

type Fringe = [(Occurrence, Bool)] -- occurrence and whether to match the exact sort

newtype Symbol = Symbol (SymbolOrAlias Object)
                 deriving (Eq, Show, Ord)

newtype Literal = Literal Text
                  deriving (Eq, Show, Ord)

data Constructor bp = SymbolConstructor Symbol
                    | LiteralConstructor Literal
                    | List
                      { getElement :: SymbolOrAlias Object
                      , getListLength :: Int
                      }
                    | Empty
                    | NonEmpty (Ignoring (Metadata bp))
                    | HasKey Bool (SymbolOrAlias Object) (Ignoring (Metadata bp)) (Maybe (Fix bp))
                    | HasNoKey (Ignoring (Metadata bp)) (Maybe (Fix bp))
                    deriving (Show, Eq, Ord)

newtype Ignoring a = Ignoring a
                   deriving (Show)

instance Eq (Ignoring a) where
  _ == _ = True

instance Ord (Ignoring a) where
  _ <= _ = True

getPatternConstructor :: Either Symbol Literal
                      -> Constructor bp
getPatternConstructor = either SymbolConstructor LiteralConstructor

getConstructorPattern :: Constructor bp
                      -> Either Symbol Literal
getConstructorPattern (SymbolConstructor s)  = Left s
getConstructorPattern (LiteralConstructor l) = Right l
getConstructorPattern _ = error "Pattern constructor must be Symbol or Literal"


newtype PatternMatrix p bp = PatternMatrix [Column p bp]
                             deriving (Show)

data Action       = Action
                    { getRuleNumber :: Int
                    , getRhsVars :: [String]
                    , getSideConditionVars :: Maybe [String]
                    }
                    deriving (Show)

data VariableBinding = VariableBinding
                       { getName :: String
                       , getHook :: String
                       , getOccurrence :: Occurrence
                       }
                       deriving (Show, Eq)

data Clause  bp     = Clause
                    -- the rule to be applied if this row succeeds
                    { getAction :: Action
                    -- the variable bindings made so far while matching this row
                    , getVariableBindings :: [VariableBinding]
                    -- the length of the head and tail of any list patterns
                    -- with frame variables bound so far in this row
                    , getListRanges :: [(Occurrence, Int, Int)]
                    -- variable bindings to injections that need to be constructed
                    -- since they do not actually exist in the original subject term
                    , getOverloadChildren :: [(Constructor bp, VariableBinding)]
                    }
                    deriving (Show)

data ClauseMatrix p bp = ClauseMatrix (PatternMatrix p bp) ![Clause bp]
                         deriving (Show)

