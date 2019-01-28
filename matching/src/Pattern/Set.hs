{-# LANGUAGE ScopedTypeVariables #-}
module Pattern.Set
  ( getSetCs ) where

import Data.Functor.Foldable
       ( Fix (..) )

import Pattern.Type
import Pattern.Var
import Pattern

-- | Extracts the constructors from a set pattern. It also returns
-- a pattern for the next values in the set.
getSetCs :: Column Pattern BoundPattern
         -> Clause BoundPattern
         -> Fix Pattern
         -> ([Constructor BoundPattern], Maybe (Fix Pattern))
getSetCs c cls = go
  where
    metadata :: Ignoring (Metadata BoundPattern)
    metadata  = Ignoring (getMetadata c)
    value :: Fix Pattern -> Maybe (Fix BoundPattern)
    value  = lookupCanonicalName cls
    go :: Fix Pattern -> ([Constructor BoundPattern], Maybe (Fix Pattern))
    go (Fix (SetPattern [] Nothing _ _))  = ([Empty], Nothing)
    go (Fix (SetPattern [] (Just next) _ _)) = ([], Just next)
    go p@(Fix (SetPattern (k : _) _ e _)) =
      ( [ HasKey True e metadata (value k)
        , HasNoKey metadata (value k)]
      , nextMap p )
    go _ = error "This should only be called on Sets"

-- | Gets the next set value if one is available or returns Nothing
-- if there are no more values
nextMap :: Fix Pattern
        -> Maybe (Fix Pattern)
nextMap (Fix (SetPattern (_ : vs) f e o)) =
  if   null vs
  then Nothing
  else Just (Fix (SetPattern vs f e o))
nextMap _ = error "This should only be called on non-empty Sets"
