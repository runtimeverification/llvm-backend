{-# LANGUAGE ScopedTypeVariables #-}
module Pattern.Set
  ( getSetCs 
  , isDefaultSet
  , mightUnifySet
  , checkSetPatternIndex
  , addSetVarToRow
  , getBestSetKey
  , computeSetScore
  , expandSetPattern
  ) where

import Data.Functor.Foldable
       ( Fix (..) )
import Data.List
       ( maximumBy, elemIndex )
import Data.Ord
       ( comparing )

import Pattern.Type
import Pattern.Var
import Pattern.Optimiser.Score
import Pattern.Util
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
      , nextSet p )
    go _ = error "This should only be called on Sets"

-- | Gets the next set value if one is available or returns Nothing
-- if there are no more values
nextSet :: Fix Pattern
        -> Maybe (Fix Pattern)
nextSet (Fix (SetPattern (_ : vs) f e o)) =
  if   null vs
  then Nothing
  else Just (Fix (SetPattern vs f e o))
nextSet _ = error "This should only be called on non-empty Sets"

-- | This matches the default case for sets. We may be able to remove
-- this one from the public interface
isDefaultSet :: Clause BoundPattern -> Fix Pattern -> Bool
isDefaultSet _ (Fix (SetPattern _ (Just _) _ _)) = True
isDefaultSet _ (Fix (SetPattern es Nothing _ _)) = not (null es)
isDefaultSet _ _ = error "This should only be called on sets"

-- | Checks if the two patterns passed as arguments are compatible
-- for unification
mightUnifySet :: Fix BoundPattern -> Fix BoundPattern -> Bool
mightUnifySet (Fix SetPattern{}) (Fix SetPattern{}) = True
mightUnifySet (Fix SetPattern{}) _                  = False
mightUnifySet _ _ = error "First argument must be a set"

checkSetPatternIndex :: (Fix BoundPattern -> Fix BoundPattern -> Bool)
                     -> Constructor BoundPattern
                     -> Metadata BoundPattern
                     -> (Clause BoundPattern, Fix Pattern)
                     -> Bool
checkSetPatternIndex _ Empty _ (_, Fix (SetPattern es _ _ _)) =
  null es
checkSetPatternIndex _ HasKey{} _ (_, Fix (SetPattern _ (Just _) _ _)) = True
checkSetPatternIndex f (HasKey _ _ _ (Just p)) _ (c, Fix (SetPattern es Nothing _ _)) = any (f p) $ map (canonicalizePattern c) es
checkSetPatternIndex _ (HasNoKey _ (Just p)) _ (c, Fix (SetPattern es _ _ _)) =
  let canonEs = map (canonicalizePattern c) es
  in p `notElem` canonEs
checkSetPatternIndex _ (HasKey _ _ _ Nothing) _ _ = error "TODO: set choice"
checkSetPatternIndex _ (HasNoKey _ Nothing) _ _ = error "TODO: set choice"
checkSetPatternIndex _ _ _ _ = error "Third argument must contain a set."


-- | Add variables bound in the pattern to the binding list
addSetVarToRow :: ( Maybe (Constructor BoundPattern) -> Occurrence -> Fix Pattern -> [VariableBinding] -> [VariableBinding] )
               -> Maybe (Constructor BoundPattern)
               -> Occurrence
               -> Fix Pattern
               -> [VariableBinding]
               -> [VariableBinding]
addSetVarToRow f _ o (Fix (SetPattern [] (Just p) _ _)) vars =
  f Nothing o p vars
addSetVarToRow _ _ _ (Fix SetPattern{}) vars = vars
addSetVarToRow _ _ _ _ _ = error "Fourth argument must contain a set."

-- | This function computes the score for a set.
computeSetScore :: (Metadata BoundPattern -> [(Fix Pattern, Clause BoundPattern)] -> Double)
                -> Metadata BoundPattern
                -> [(Fix Pattern, Clause BoundPattern)]
                -> Double
computeSetScore f m ((Fix (SetPattern [] Nothing _ _),_):tl) = 1.0 + f m tl
computeSetScore f m ((Fix (SetPattern [] (Just p) _ _),c):tl) = f m ((p,c):tl)
computeSetScore f m ((Fix (SetPattern es _ _ _),c):tl) = if f m tl == -1.0 / 0.0 then -1.0 / 0.0 else snd $ computeSetScore' c es tl
computeSetScore _ _ _ = error "The first pattern must be a set."

-- | This function selects the best candidate key to use when
-- computing the score for a set.
getBestSetKey :: Column Pattern BoundPattern
              -> [Clause BoundPattern]
              -> Maybe (Fix BoundPattern)
getBestSetKey (Column _ (Fix (SetPattern (k:ks) _ _ _):tl)) cs =
  fst $ computeSetScore' (head cs) (k:ks) (zip tl $ tail cs)
getBestSetKey (Column _ (Fix SetPattern{}:_)) _ = Nothing
getBestSetKey _ _ = error "Column must contain a set pattern."

computeSetScore' :: Clause BoundPattern
                -> [Fix Pattern]
                -> [(Fix Pattern, Clause BoundPattern)]
                -> (Maybe (Fix BoundPattern), Double)
computeSetScore' c es tl =
  let scores = map (\e -> (if isBound getName c e then Just $ canonicalizePattern c e else Nothing,computeElementScore e c tl)) es
  in maximumBy (comparing snd) scores

expandSetPattern :: Constructor BoundPattern
                 -> [Metadata BoundPattern]
                 -> Metadata BoundPattern
                 -> (Fix Pattern,Clause BoundPattern)
                 -> [(Fix Pattern,Maybe (Constructor BoundPattern, Metadata BoundPattern))]
expandSetPattern (HasKey _ _ _ (Just p)) _ _ (m@(Fix (SetPattern es f e o)),c) =
  let canonEs = map (canonicalizePattern c) es
      hasElem = elemIndex p canonEs
  in case hasElem of
       Just i -> [(Fix (SetPattern (except i es) f e o), Nothing), (Fix Wildcard,Nothing)]
       Nothing -> [(Fix Wildcard,Nothing), (m,Nothing)]
expandSetPattern (HasNoKey _ _) _ _ (p,_) = [(p,Nothing)]
expandSetPattern (HasKey _ _ _ Nothing) _ _ _ = error "TODO: set choice"
expandSetPattern _ _ _ (Fix SetPattern{},_) = error "Invalid set pattern."
expandSetPattern _ _ _ _ = error "The expandSetPattern function expects a set parameter as its final argument."
