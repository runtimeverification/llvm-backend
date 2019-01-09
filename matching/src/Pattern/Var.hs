{-# LANGUAGE ScopedTypeVariables #-}

module Pattern.Var
  ( isBound
  , canonicalizePattern
  , lookupCanonicalName
  ) where

import           Data.Functor.Foldable
                 ( Fix (..), cata )
import qualified Data.Map.Strict as Map
import           Data.Maybe
                 ( fromMaybe )

import Pattern.Type

-- | Checks to see if the variable name has been bound in the clause.
lookupCanonicalName :: Clause
                    -> Fix Pattern
                    -> Maybe (Fix BoundPattern)
lookupCanonicalName cls p =
  if   isBound getName cls p
  then Just (canonicalizePattern cls p)
  else Nothing

-- | Looks up all variables from the pattern in the clause and replaces
-- them with the occurrences it finds.
canonicalizePattern :: Clause
                    -> Fix Pattern
                    -> Fix BoundPattern
canonicalizePattern clause = cata go
  where
    bs :: Map.Map String Occurrence
    bs = Map.fromList
         $   (\b -> (getName b, getOccurrence b))
         <$> getVariableBindings clause
    go :: Pattern (Fix BoundPattern) -> Fix BoundPattern
    go (Variable name hook)      = Fix $ Variable (Map.lookup name bs) hook
    go (As name hook p)          = Fix $ As (Map.lookup name bs) hook p
    go (Pattern name hook ps)    = Fix $ Pattern name hook ps
    go (ListPattern hd f tl e o) = Fix $ ListPattern hd f tl e o
    go (MapPattern  ks vs f e o) = Fix $ MapPattern  ks vs f e o
    go (SetPattern     vs f e o) = Fix $ SetPattern     vs f e o
    go Wildcard                  = Fix Wildcard

-- | Checks if all variables in the pattern are bound.
isBound :: forall a. Eq a
        => (VariableBinding -> a)
        -> Clause
        -> Fix (P a)
        -> Bool
isBound get (Clause _ vars _ _) = cata go
  where
    go :: P a Bool -> Bool
    go (Pattern _ _ ps)          = and ps
    go (ListPattern hd f tl _ _) = and hd && and tl && fromMaybe True f
    go (MapPattern  ks vs f _ _) = and ks && and vs && fromMaybe True f
    go (SetPattern  vs    f _ _) = and vs && fromMaybe True f
    go (As name _ p)             = p && name `elem` map get vars
    go (Variable name _) = name `elem` map get vars
    go Wildcard = False
