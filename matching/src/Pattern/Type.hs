{-# LANGUAGE DeriveFunctor     #-}
{-# LANGUAGE FlexibleInstances #-}
{-# LANGUAGE TemplateHaskell   #-}
{-# LANGUAGE OverloadedStrings   #-}

module Pattern.Type
  ( Column (..)
  , Metadata (..)
  , Occurrence (..)
  , Action (..)
  , ClauseMatrix (..)
  , Clause (..)
  , P (..)
  , Pattern
  , BoundPattern
  , PatternMatrix (..)
  , VariableBinding (..)
  , Ignoring (..)
  , Fringe
  , Index
  , Constructor (..)
  ) where

import Data.Deriving
       ( deriveEq2, deriveOrd2, deriveShow2 )
import Data.Functor.Classes
       ( Eq1 (..), Ord1 (..), Show1 (..), liftCompare2, liftEq2,
       liftShowsPrec2 )
import Data.Functor.Foldable
       ( Fix (..), cata )
import Data.Text
       ( Text, pack )
import Kore.AST.Common
       ( Sort (..), SymbolOrAlias (..) )
import Kore.AST.MetaOrObject
       ( Object (..) )
import           Kore.Unparser
                 ( unparseToString )
import qualified Data.Yaml.Builder as Y

data Column = Column
              { getMetadata :: !Metadata
              , getTerms    :: ![Fix Pattern]
              }

instance Show Column where
  showsPrec _ (Column _ ts) =
    showString "Column " . showList ts

data Metadata = Metadata
                { getLength :: !Integer
                , getInjections :: Constructor -> [Constructor]
                , getOverloads :: Constructor -> [Constructor] -- list of overloaded productions less than specified production
                , getSort :: Sort Object
                , getChildren :: Constructor -> Maybe [Metadata]
                }

instance (Show Metadata) where
  show (Metadata _ _ _ sort _) = show sort

data Occurrence = Num Int Occurrence
                | Base
                | Lit Text Text
                | Equal Occurrence Occurrence
                | SC Int
                | Value (Fix BoundPattern) Occurrence
                | Rem (Fix BoundPattern) Occurrence
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

instance Y.ToYaml a => Y.ToYaml (BoundPattern a) where
  toYaml Wildcard = error "Unsupported map/set pattern"
  toYaml (Variable (Just o) h) = Y.mapping
    ["hook" Y..= Y.toYaml (pack h)
    , "occurrence" Y..= Y.toYaml o
    ]
  toYaml (Variable Nothing _) = error "Unsupported map/set pattern"
  toYaml (As _ _ p) = Y.toYaml p
  toYaml (MapPattern _ _ _ _ o) = Y.toYaml o
  toYaml (SetPattern _ _ _ o) = Y.toYaml o
  toYaml (ListPattern _ _ _ _ o) = Y.toYaml o
  toYaml (Pattern (Literal s) (Just h) []) = Y.mapping
    ["hook" Y..= Y.toYaml (pack h)
    , "literal" Y..= Y.toYaml (pack s)
    ]
  toYaml (Pattern (Symbol s) Nothing ps) = Y.mapping
    ["constructor" Y..= Y.toYaml (pack $ unparseToString s)
    , "args" Y..= Y.array (map Y.toYaml ps)
    ]
  toYaml Pattern{} = error "Unsupported map/set pattern"


instance Y.ToYaml (Fix BoundPattern) where
  toYaml = cata Y.toYaml


type Fringe = [(Occurrence, Bool)] -- occurrence and whether to match the exact sort

instance Show1 Pattern where
  liftShowsPrec = liftShowsPrec2 showsPrec showList
instance Show1 BoundPattern where
  liftShowsPrec = liftShowsPrec2 showsPrec showList

instance Eq1 BoundPattern where
  liftEq = liftEq2 (==)

instance Ord1 BoundPattern where
  liftCompare = liftCompare2 compare


data Constructor = Symbol (SymbolOrAlias Object)
                 | Literal String
                 | List
                   { getElement :: SymbolOrAlias Object
                   , getListLength :: Int
                   }
                 | Empty
                 | NonEmpty (Ignoring Metadata)
                 | HasKey Bool (SymbolOrAlias Object) (Ignoring Metadata) (Maybe (Fix BoundPattern))
                 | HasNoKey (Ignoring Metadata) (Maybe (Fix BoundPattern))
                 deriving (Show, Eq, Ord)

newtype Ignoring a = Ignoring a
                   deriving (Show)

instance Eq (Ignoring a) where
  _ == _ = True

instance Ord (Ignoring a) where
  _ <= _ = True

type Index       = Int
data P var a   = Pattern Constructor (Maybe String) ![a]
                 | ListPattern
                   { getHead :: ![a] -- match elements at front of list
                   , getFrame :: !(Maybe a) -- match remainder of list
                   , getTail :: ![a] -- match elements at back of list
                   , element :: SymbolOrAlias Object -- ListItem symbol
                   , original :: a
                   }
                 | MapPattern
                   { getKeys :: ![a]
                   , getValues :: ![a]
                   , getFrame :: !(Maybe a)
                   , element :: SymbolOrAlias Object
                   , original :: !a
                   }
                 | SetPattern
                   { getElements :: ![a]
                   , getFrame :: !(Maybe a)
                   , element :: SymbolOrAlias Object
                   , original :: !a
                   }
                 | As var String a
                 | Wildcard
                 | Variable var String
                 deriving (Show, Eq, Functor)

type Pattern = P String
type BoundPattern = P (Maybe Occurrence)

$(deriveEq2 ''P)
$(deriveShow2 ''P)
$(deriveOrd2 ''P)

newtype PatternMatrix = PatternMatrix [Column]
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

data Clause       = Clause
                    -- the rule to be applied if this row succeeds
                    { getAction :: Action
                    -- the variable bindings made so far while matching this row
                    , getVariableBindings :: [VariableBinding]
                    -- the length of the head and tail of any list patterns
                    -- with frame variables bound so far in this row
                    , getListRanges :: [(Occurrence, Int, Int)]
                    -- variable bindings to injections that need to be constructed
                    -- since they do not actually exist in the original subject term
                    , getOverloadChildren :: [(Constructor, VariableBinding)]
                    }
                    deriving (Show)

data ClauseMatrix = ClauseMatrix PatternMatrix ![Clause]
                    deriving (Show)

