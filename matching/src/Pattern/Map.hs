module Pattern.Map
  ( getMapCs
  , getMapHookName
  , isDefaultMap
  , mightUnifyMap
  , checkMapPatternIndex
  , addMapVarToRow
  , getBestMapKey
  , computeMapScore
  ) where

import Data.Functor.Foldable
       ( Fix (..) )
import Data.List
       ( maximumBy )
import Data.Ord
       ( comparing )
import Data.Maybe (fromJust)

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
checkMapPatternIndex f (HasKey _ _ _ (Just p)) _ (c, Fix (MapPattern ks _ Nothing _ _)) = any (f p) $ map (canonicalizePattern c) ks
checkMapPatternIndex _ (HasNoKey _ (Just p)) _ (c, Fix (MapPattern ks _ _ _ _)) =
  let canonKs = map (canonicalizePattern c) ks
  in p `notElem` canonKs
checkMapPatternIndex _ _ _ _ = error "Third argument must contain a map."

-- | Add variables bound in the pattern to the binding list
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

computeMapScore :: (Metadata -> [(Fix Pattern, Clause)] -> Double)
                -> Metadata
                -> [(Fix Pattern, Clause)]
                -> Double
computeMapScore f m ((Fix (MapPattern [] [] Nothing _ _),_):tl) = 1.0 + f m tl
computeMapScore f m ((Fix (MapPattern [] [] (Just p) _ _),c):tl) = f m ((p,c):tl)
computeMapScore f m ((Fix (MapPattern ks vs _ e _),c):tl) = if f m tl == -1.0 / 0.0 then -1.0 / 0.0 else snd $ computeMapScore' f m e c ks vs tl
computeMapScore _ _ _ = error "The first pattern must be a map."

getBestMapKey :: (Metadata -> [(Fix Pattern, Clause)] -> Double)
              -> Column
              -> [Clause]
              -> Maybe (Fix BoundPattern)
getBestMapKey f (Column m (Fix (MapPattern (k:ks) vs _ e _):tl)) cs =
  fst $ computeMapScore' f  m e (head cs) (k:ks) vs (zip tl $ tail cs)
getBestMapKey _ (Column _ (Fix MapPattern{}:_)) _ = Nothing
getBestMapKey _ _ _ = error "Column must contain a map pattern."

computeMapScore' :: (Metadata -> [(Fix Pattern, Clause)] -> Double)
                 -> Metadata
                 -> SymbolOrAlias Object -- ^ Map.element
                 -> Clause
                 -> [Fix Pattern]
                 -> [Fix Pattern]
                 -> [(Fix Pattern,Clause)]
                 -> (Maybe (Fix BoundPattern), Double)
computeMapScore' f m e c ks vs tl =
  let zipped = zip ks vs
      scores = map (\(k,v) -> (if isBound getName c k then Just $ canonicalizePattern c k else Nothing, computeMapElementScore f m e c tl (k,v))) zipped
  in maximumBy (comparing snd) scores


computeMapElementScore :: (Metadata -> [(Fix Pattern, Clause)] -> Double)
                       -> Metadata
                       -> SymbolOrAlias Object
                       -> Clause
                       -> [(Fix Pattern,Clause)]
                       -> (Fix Pattern, Fix Pattern)
                       -> Double
computeMapElementScore f m e c tl (k,v) =
  let score = computeElementScore k c tl
  in if score == -1.0 / 0.0 then score else
  let finalScore = score * f (head $ fromJust $ getChildren m (HasKey False e (Ignoring m) Nothing)) [(v,c)]
  in if finalScore == 0.0 then minPositiveDouble else finalScore

computeElementScore :: Fix Pattern -> Clause -> [(Fix Pattern,Clause)] -> Double
computeElementScore k c tl =
  let bound = isBound getName c k
  in if bound then
    let canonKey = canonicalizePattern c k
        (ps,cs) = unzip tl
        canonCs = map canonicalizeClause cs
        boundedCanonCs = takeWhile (flip (isBound (Just . getOccurrence)) canonKey) canonCs
        boundedPs = take (length boundedCanonCs) ps
        boundedCs = take (length boundedCanonCs) cs
        canonPs = zipWith canonicalizePattern boundedCs boundedPs
        psWithK = takeWhile (mapContainsKey canonKey) canonPs
    in fromIntegral $ length psWithK
  else -1.0 / 0.0
  where
    mapContainsKey :: Fix BoundPattern -> Fix BoundPattern -> Bool
    mapContainsKey k' (Fix (MapPattern ks _ _ _ _)) = k' `elem` ks
    mapContainsKey _ _ = False
    canonicalizeClause :: Clause -> Clause
    canonicalizeClause (Clause a vars ranges children) =
      let hooks = map getHook vars
          os = map getOccurrence vars
          names = map show os
      in Clause a (zipWith3 VariableBinding names hooks os) ranges children

minPositiveDouble :: Double
minPositiveDouble = encodeFloat 1 $ fst (floatRange (0.0 :: Double)) - floatDigits (0.0 :: Double)

