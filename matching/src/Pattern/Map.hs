module Pattern.Map
  ( getMapCs
  ) where

import Data.Functor.Foldable
       ( Fix (..) )

import Pattern.Type
import Pattern.Var

-- | Extracts the constructors from a map pattern. It also returns
-- a sort a pattern for the next keys in the map.
getMapCs :: Column
         -> Clause
         -> Fix Pattern
         -> ([Constructor], Maybe (Fix Pattern))
getMapCs c cls = go
  where
    metadata :: Ignoring Metadata
    metadata  = Ignoring (getMetadata c)
    key :: Fix Pattern -> Maybe (Fix BoundPattern)
    key  = lookupCanonicalName cls
    go :: Fix Pattern -> ([Constructor], Maybe (Fix Pattern))
    go (Fix (MapPattern [] _ Nothing _ _))  = ([Empty], Nothing)
    go (Fix (MapPattern [] _ (Just next) _ _)) = ([], Just next)
    go p@(Fix (MapPattern (k : _) _ _ e _)) =
      ( [ HasKey False e metadata (key k)
        , HasNoKey metadata (key k)]
      , nextMap p )
    go _ = error "This should only be called on Maps"

-- | Gets the next map key if one is available or returns Nothing
-- if there are no more keys
nextMap :: Fix Pattern
        -> Maybe (Fix Pattern)
nextMap (Fix (MapPattern (_ : ks) vs f e o)) =
  if   null ks
  then Nothing
  else Just (Fix (MapPattern ks vs f e o))
nextMap _ = error "This should only be called on non-empty Maps"

lookupCanonicalName :: Clause
                    -> Fix Pattern
                    -> Maybe (Fix BoundPattern)
lookupCanonicalName cls p =
  if   isBound getName cls p
  then Just (canonicalizePattern cls p)
  else Nothing
