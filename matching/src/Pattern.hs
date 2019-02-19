module Pattern
  ( P(..)
  , BoundPattern
  , Pattern
  ) where

import Data.Deriving
       ( deriveEq2, deriveOrd2, deriveShow2 )
import Data.Functor.Classes
       ( Eq1 (..), Ord1 (..), Show1 (..), liftCompare2, liftEq2,
       liftShowsPrec2 )
import Data.Functor.Foldable
       ( Fix (..), cata )
import Data.Text
       ( pack )

import Kore.Unparser
       ( unparseToString )
import Kore.AST.Common
       ( SymbolOrAlias (..) )
import Kore.AST.MetaOrObject
       ( Object (..) )

import qualified Data.Yaml.Builder as Y

import Pattern.Type

data P var a   = Pattern (Either Symbol Literal) (Maybe String) ![a]
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

instance Show1 Pattern where
  liftShowsPrec = liftShowsPrec2 showsPrec showList
instance Show1 BoundPattern where
  liftShowsPrec = liftShowsPrec2 showsPrec showList
instance Eq1 BoundPattern where
  liftEq = liftEq2 (==)
instance Ord1 BoundPattern where
  liftCompare = liftCompare2 compare
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
  toYaml (Pattern (Right (Literal s)) (Just h) []) = Y.mapping
    ["hook" Y..= Y.toYaml (pack h)
    , "literal" Y..= Y.toYaml s
    ]
  toYaml (Pattern (Left (Symbol s)) Nothing ps) = Y.mapping
    ["constructor" Y..= Y.toYaml (pack $ unparseToString s)
    , "args" Y..= Y.array (map Y.toYaml ps)
    ]
  toYaml Pattern{} = error "Unsupported map/set pattern"


instance Y.ToYaml (Fix BoundPattern) where
  toYaml = cata Y.toYaml
