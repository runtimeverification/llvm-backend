module Pattern.Var
  ( isBound
  , canonicalizePattern
  ) where

import           Data.Functor.Foldable
                 ( Fix (..) )
import qualified Data.Map.Strict as Map

import Pattern.Type

canonicalizePattern :: Clause -> Fix Pattern -> Fix BoundPattern
canonicalizePattern (Clause _ vars _ _) (Fix (Variable name hookAtt)) =
  let names = map getName vars
      os = map getOccurrence vars
      oMap = Map.fromList $ zip names os
  in Fix $ Variable (Map.lookup name oMap) hookAtt
canonicalizePattern c'@(Clause _ vars _ _) (Fix (As name hookAtt p)) =
  let names = map getName vars
      os = map getOccurrence vars
      oMap = Map.fromList $ zip names os
  in Fix $ As (Map.lookup name oMap) hookAtt $ canonicalizePattern c' p
canonicalizePattern c' (Fix (Pattern name hookAtt ps)) = Fix (Pattern name hookAtt $ map (canonicalizePattern c') ps)
canonicalizePattern _ (Fix Wildcard) = Fix Wildcard
canonicalizePattern c' (Fix (ListPattern hd f tl' e o)) =
  Fix (ListPattern (mapPat c' hd) (mapPat c' f) (mapPat c' tl') e $ canonicalizePattern c' o)
canonicalizePattern c' (Fix (MapPattern ks vs f e o)) =
  Fix (MapPattern (mapPat c' ks) (mapPat c' vs) (mapPat c' f) e $ canonicalizePattern c' o)
canonicalizePattern c' (Fix (SetPattern es f e o)) =
  Fix (SetPattern (mapPat c' es) (mapPat c' f) e $ canonicalizePattern c' o)

isBound :: Eq a => (VariableBinding -> a) -> Clause -> Fix (P a) -> Bool
isBound get c' (Fix (Pattern _ _ ps)) = all (isBound get c') ps
isBound get c' (Fix (ListPattern hd f tl' _ _)) = all (isBound get c') hd && all (isBound get c') tl' && maybe True (isBound get c') f
isBound get c' (Fix (MapPattern ks vs f _ _)) = all (isBound get c') ks && all (isBound get c') vs && maybe True (isBound get c') f
isBound get c' (Fix (SetPattern es f _ _)) = all (isBound get c') es && maybe True (isBound get c') f
isBound get c'@(Clause _ vars _ _) (Fix (As name _ p)) =
  isBound get c' p && elem name (map get vars)
isBound _ _ (Fix Wildcard) = False
isBound get (Clause _ vars _ _) (Fix (Variable name _)) =
  elem name $ map get vars

mapPat :: Functor f => Clause -> f (Fix Pattern) -> f (Fix BoundPattern)
mapPat c' = fmap (canonicalizePattern c')

