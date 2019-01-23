module Pattern.Class ( IsPattern(..)
                     , HasMetadata(..)
                     ) where

import Data.Functor.Foldable (Fix(..))
import Data.Proxy (Proxy(..))

import Pattern.Type

class IsPattern a where
  toPattern :: a -> Fix Pattern

class HasMetadata a where
  getMetadata :: Proxy a -> Metadata BoundPattern
