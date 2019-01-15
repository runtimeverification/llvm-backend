module Pattern.Map
  ( getMapCs
  , getMapHookName
  , isDefaultMap
  , mightUnifyMap
  , checkMapPatternIndex
  , addMapVarToRow
  ) where

import Data.Functor.Foldable
       ( Fix (..) )

import Kore.AST.Common
       ( SymbolOrAlias (..) )
import Kore.AST.MetaOrObject
       ( Object (..) )

import Pattern.Type
import Pattern.Var

-- | Extracts the constructors from a map pattern. It also returns
-- a pattern for the next keys in the map.
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

-- | This is the hook name for maps. We may be able to remove this
-- one from the public interface
getMapHookName :: Maybe String
getMapHookName = Just "MAP.Map"

-- | This matches the default case for maps. We may be able to remove
-- this one from the public interface
isDefaultMap :: Clause -> Fix Pattern -> Bool
isDefaultMap _ (Fix (MapPattern _ _ (Just _) _ _))  = True
isDefaultMap _ (Fix (MapPattern ks vs Nothing _ _)) = not (null ks) || not (null vs)
isDefaultMap _ _ = error "This should only be called on maps"

-- | Checks if the two patterns passed as arguments are compatible
-- for unification
mightUnifyMap :: Fix BoundPattern -> Fix BoundPattern -> Bool
mightUnifyMap (Fix MapPattern{}) (Fix MapPattern{}) = True
mightUnifyMap (Fix MapPattern{}) _                  = False
mightUnifyMap _ _ = error "First argument must be a map"

checkMapPatternIndex :: (Fix BoundPattern -> Fix BoundPattern -> Bool)
                     -> Constructor
                     -> Metadata
                     -> (Clause, Fix Pattern)
                     -> Bool
checkMapPatternIndex _ Empty _ (_, Fix (MapPattern ks vs _ _ _)) =
  null ks && null vs
checkMapPatternIndex _ HasKey{} _ (_, Fix (MapPattern _ _ (Just _) _ _)) = True
checkMapPatternIndex mu (HasKey _ _ _ (Just p)) _ (c, Fix (MapPattern ks _ Nothing _ _)) = any (mu p) $ map (canonicalizePattern c) ks
checkMapPatternIndex _ (HasNoKey _ (Just p)) _ (c, Fix (MapPattern ks _ _ _ _)) =
  let canonKs = map (canonicalizePattern c) ks
  in p `notElem` canonKs
checkMapPatternIndex _ _ _ _ = error "Third argument must contain a map."

addMapVarToRow :: ( Maybe Constructor -> Occurrence -> Fix Pattern -> [VariableBinding] -> [VariableBinding] )
               -> Maybe Constructor
               -> Occurrence
               -> Fix Pattern
               -> [VariableBinding]
               -> [VariableBinding]
addMapVarToRow f _ o (Fix (MapPattern [] [] (Just p) _ _)) vars =
  f Nothing o p vars
addMapVarToRow _ _ _ (Fix MapPattern{}) vars = vars
addMapVarToRow _ _ _ _ _ = error "Fourth argument must contain a map."
