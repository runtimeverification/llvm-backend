module Pattern.Optimiser.Score
  ( computeElementScore
  , minPositiveDouble
  ) where

import Data.Functor.Foldable
       ( Fix (..) )

import Pattern.Type
import Pattern.Var
import Pattern

-- | This computes the final score for a map given the best key.
computeElementScore :: Fix Pattern -> Clause BoundPattern -> [(Fix Pattern, Clause BoundPattern)] -> Double
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
    mapContainsKey _ _ = False
    canonicalizeClause :: Clause BoundPattern -> Clause BoundPattern
    canonicalizeClause (Clause a vars ranges children) =
      let hooks = map getHook vars
          os = map getOccurrence vars
          names = map show os
      in Clause a (zipWith3 VariableBinding names hooks os) ranges children

minPositiveDouble :: Double
minPositiveDouble = encodeFloat 1 $ fst (floatRange (0.0 :: Double)) - floatDigits (0.0 :: Double)

