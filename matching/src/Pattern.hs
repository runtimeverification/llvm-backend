{-# LANGUAGE DeriveFunctor     #-}
{-# LANGUAGE FlexibleContexts  #-}
{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE TupleSections     #-}
{-# LANGUAGE TypeSynonymInstances #-}
{-# LANGUAGE FlexibleInstances #-}
{-# LANGUAGE UndecidableInstances #-}
{-# LANGUAGE TemplateHaskell   #-}

module Pattern ( PatternMatrix(..)
               , ClauseMatrix(..)
               , Column(..)
               , mkColumn
               , Metadata(..)
               , P(..)
               , Pattern
               , Clause(..)
               , Action(..)
               , Occurrence
               , Index
               , Ignoring(..)
               , Constructor(..)
               , mkClauseMatrix
               , sigma
               , mSpecialize
               , mDefault
               , failure
               , leaf
               , switch
               , switchLiteral
               , function
               , swap
               , simplify
               , DecisionTree(..)
               , Anchor(..)
               , Alias(..)
               , compilePattern
               , shareDt
               , serializeToYaml
               ) where

import           Control.Monad.Free    (Free (..))
import           Data.Bifunctor        (second)
import           Data.Deriving         (deriveOrd1, deriveShow1, deriveEq1, deriveEq2, deriveShow2, deriveOrd2)
import           Data.Function         (on)
import           Data.Functor.Classes  (Show1(..), Eq1(..), Ord1(..), liftEq2, liftCompare2, liftShowsPrec2)
import           Data.Functor.Foldable (Fix (..), cata)
import           Data.List             (transpose,nub,sortBy,maximumBy,elemIndex)
import           Data.List.Index       (indexed)
import           Data.Maybe            (catMaybes,isJust,fromJust,listToMaybe)
import           Data.Ord              (comparing)
import           Data.Semigroup        ((<>))
import           Data.Text             (Text, pack)
import           Data.Traversable      (mapAccumL)
import           Kore.AST.Common       (SymbolOrAlias (..), Id (..), Sort(..))
import           Kore.AST.MetaOrObject (Object (..))
import           Kore.Unparser.Unparse (unparseToString)
import           TextShow              (showt)
import qualified Data.Map.Strict as Map
import qualified Data.Yaml.Builder as Y
import qualified Data.ByteString as B

data Column = Column
              { getMetadata :: !Metadata
              , getScore    :: !Double
              , getTerms    :: ![Fix Pattern]
              }

instance Show Column where
  showsPrec _ (Column _ _ ts) =
    showString "Column " . showList ts

data Metadata = Metadata
                { getLength :: !Integer
                , getInjections :: Constructor -> [Constructor]
                , getSort :: Sort Object
                , getChildren :: Constructor -> Maybe [Metadata]
                }

instance (Show Metadata) where
  show (Metadata _ _ sort _) = show sort

type Occurrence   = [Int]

instance Show1 Pattern where
  liftShowsPrec showP showL prec pat = liftShowsPrec2 showsPrec showList showP showL prec pat
instance Show1 BoundPattern where
  liftShowsPrec showP showL prec pat = liftShowsPrec2 showsPrec showList showP showL prec pat

instance Eq1 BoundPattern where
  liftEq eqT a b = liftEq2 (==) eqT a b

instance Ord1 BoundPattern where
  liftCompare liftT a b = liftCompare2 (compare) liftT a b


data Constructor = Symbol (SymbolOrAlias Object)
                 | Literal String
                 | List 
                   { getElement :: SymbolOrAlias Object
                   , getListLength :: Int
                   }
                 | Empty
                 | NonEmpty (Ignoring Metadata)
                 | HasKey (SymbolOrAlias Object) (Ignoring Metadata) (Fix (P Occurrence))
                 | HasNoKey (Ignoring Metadata) (Fix (P Occurrence))
                 deriving (Show, Eq, Ord)

newtype Ignoring a = Ignoring a
                   deriving (Show)

instance Eq (Ignoring a) where
  _ == _ = True

instance Ord (Ignoring a) where
  _ <= _ = True

type Index       = Int
data P var a   = Pattern Constructor (Maybe String) ![a]
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
                   , original :: a
                   }
                 | As var String a
                 | Wildcard
                 | Variable var String
                 deriving (Show, Eq, Functor)

type Pattern = P String
type BoundPattern = P Occurrence

$(deriveEq2 ''P)
$(deriveShow2 ''P)
$(deriveOrd2 ''P)

newtype PatternMatrix = PatternMatrix [Column]
                        deriving (Show)

data Action       = Action
                    { getRuleNumber :: Int
                    , getRhsVars :: [String]
                    , getSideConditionVars :: Maybe [String]
                    }
                    deriving (Show)

data VariableBinding = VariableBinding
                       { getName :: String
                       , getHook :: String
                       , getOccurrence :: Occurrence
                       }
                       deriving (Show, Eq)

data Clause       = Clause
                    -- the rule to be applied if this row succeeds
                    { getAction :: Action
                    -- the variable bindings made so far while matching this row
                    , getVariableBindings :: [VariableBinding]
                    -- the length of the head and tail of any list patterns
                    -- with frame variables bound so far in this row
                    , getListRanges :: [(Occurrence, Int, Int)]
                    }
                    deriving (Show)

data ClauseMatrix = ClauseMatrix PatternMatrix ![Clause]
                    deriving (Show)
 
-- [ Builders ]

mkClauseMatrix :: [Column]
               -> [Action]
               -> Either Text (ClauseMatrix, [Occurrence])
mkClauseMatrix cs as = do
  validateColumnLength (length as) cs
  pure ((ClauseMatrix (PatternMatrix cs) (map (\a -> Clause a [] []) as)),map (\i -> [i]) [1..length cs]) 
  where
    validateColumnLength :: Int -> [Column] -> Either Text ()
    validateColumnLength as' =
      mapM_ (\c ->
                if length (getTerms c) == as'
                then Right ()
                else Left $ "Wrong column length. Expected " <> showt as' <>
                            " and got " <> showt (length (getTerms c)))

failure :: Fix DecisionTree
failure = Fix Fail

leaf :: Int -> [Occurrence] -> Fix DecisionTree
leaf a os = Fix (Leaf (a, os))

switch :: Occurrence 
       -> [(Text, Fix DecisionTree)]
       -> Maybe (Fix DecisionTree)
       -> Fix DecisionTree
switch o brs def =
  Fix $ Switch o L { getSpecializations = brs
                 , getDefault = def }

switchLiteral :: Occurrence
          -> Int
          -> [(Text, Fix DecisionTree)]
          -> Maybe (Fix DecisionTree)
          -> Fix DecisionTree
switchLiteral o bw brs def =
  Fix $ SwitchLiteral o bw L { getSpecializations = brs
                    , getDefault = def }

function :: Text
         -> Occurrence
         -> [Occurrence]
         -> Hook
         -> Fix DecisionTree
         -> Fix DecisionTree
function name o vars sort child =
  Fix $ Function name o vars sort child

simplify :: Occurrence -> Fix DecisionTree -> Fix DecisionTree
simplify o dt = switch o [] (Just dt)

swap :: Index
     -> Fix DecisionTree
     -> Fix DecisionTree
swap ix tm = Fix (Swap ix tm)

-- [ Matrix ]

sigma :: Column -> [Clause] -> [Constructor]
sigma c cs =
  let used = concat $ zipWith ix cs $ getTerms c
      bestKey = getBestMapKey c cs
      bestUsed = case bestKey of
                   Nothing -> used
                   Just k -> filter (isBest k) used
      inj = nub $ filter isInj bestUsed
      usedInjs = nub $ concatMap (getInjections $ getMetadata c) inj
      dups = bestUsed ++ usedInjs
      nodups = nub dups
  in if elem Empty nodups then [Empty] else filter (not . (== Empty)) nodups
  where
    ix :: Clause -> Fix Pattern -> [Constructor]
    ix _ (Fix (Pattern ix' _ _)) = [ix']
    ix _ (Fix (ListPattern hd Nothing tl s _)) = [List s (length hd + length tl)]
    -- if the list has a frame, then matching lists longer than the current head and tail
    -- is performed via the default case of the switch, which means that we need
    -- to create a switch case per list of lesser length
    ix _ (Fix (ListPattern hd (Just _) tl s _)) = map (List s) [0..(length hd + length tl)]
    ix _ (Fix (MapPattern [] _ Nothing _ _)) = [Empty]
    ix cls (Fix (MapPattern [] _ (Just p) _ _)) = ix cls p
    ix cls (Fix (MapPattern (k:[]) _ _ e _)) = 
      let m = Ignoring $ getMetadata c
      in [HasKey e m $ canonicalizePattern cls k, HasNoKey m $ canonicalizePattern cls k]
    ix cls (Fix (MapPattern (k:k':ks) vs f e o)) = 
      let m = Ignoring $ getMetadata c
      in [HasKey e m $ canonicalizePattern cls k, HasNoKey m $ canonicalizePattern cls k] ++ ix cls (Fix (MapPattern (k':ks) vs f e o))
    ix cls (Fix (As _ _ pat))      = ix cls pat
    ix _ (Fix Wildcard)          = []
    ix _ (Fix (Variable _ _))    = []
    isInj :: Constructor -> Bool
    isInj (Symbol (SymbolOrAlias (Id "inj" _) _)) = True
    isInj _ = False
    isBest :: Fix BoundPattern -> Constructor -> Bool
    isBest k (HasKey _ _ k') = k == k'
    isBest k (HasNoKey _ k') = k == k'
    isBest _ _ = True
   

sigma₁ :: ClauseMatrix -> [Constructor]
sigma₁ (ClauseMatrix (PatternMatrix (c : _)) as) = sigma c as
sigma₁ _                       = []

hook :: PatternMatrix -> Maybe String
hook (PatternMatrix (c : _)) = 
  let s = bw c
  in listToMaybe $ catMaybes $ s
  where
    bw :: Column -> [Maybe String]
    bw = map ix . getTerms
    ix :: Fix Pattern -> Maybe String
    ix (Fix (Pattern _ bw' _)) = bw'
    ix (Fix (ListPattern _ _ _ _ _)) = Just "LIST.List"
    ix (Fix (MapPattern _ _ _ _ _)) = Just "MAP.Map"
    ix (Fix (As _ _ pat))    = ix pat
    ix (Fix Wildcard)        = Nothing
    ix (Fix (Variable _ _))  = Nothing
hook _                       = Nothing

mSpecialize :: Constructor -> (ClauseMatrix, [Occurrence]) -> (Text, (ClauseMatrix, [Occurrence]))
mSpecialize ix (cm@(ClauseMatrix (PatternMatrix (c : _)) _), o : os) = 
   let newOs = expandOccurrence cm o ix <> os
       cm' = filterMatrix (Just ix) (checkPatternIndex ix (getMetadata c)) (cm,o)
       cm'' = expandMatrix ix cm'
   in (getConstructor ix, (cm'', newOs))
   where
     getConstructor :: Constructor -> Text
     getConstructor (Symbol sym) = pack $ unparseToString sym
     getConstructor (Literal str) = pack str
     getConstructor (List _ i) = pack $ show i
     getConstructor Empty = "0"
     getConstructor (NonEmpty _) = error "Invalid map pattern"
     getConstructor (HasKey _ _ _) = "0"
     getConstructor (HasNoKey _ _) = "1"

mSpecialize _ _ = error "must have at least one column"

expandOccurrence :: ClauseMatrix -> Occurrence -> Constructor -> [Occurrence]
expandOccurrence (ClauseMatrix (PatternMatrix (c : _)) _) o ix =
  case ix of
    Empty -> []
    NonEmpty _ -> [o]
    HasKey _ _ _ -> [0 : o, 1 : o, o]
    HasNoKey _ _ -> [o]
    _ -> let (Metadata _ _ _ mtd) = getMetadata c
             a = length $ fromJust $ mtd ix
         in map (\i -> i : o) [0..a-1]
expandOccurrence _ _ _ = error "must have at least one column"

mDefault :: (ClauseMatrix, [Occurrence]) -> Maybe (ClauseMatrix, [Occurrence])
mDefault (cm@(ClauseMatrix pm@(PatternMatrix (c : _)) as),o : os) =
  let (Metadata mtd _ _ _) = getMetadata c
      s₁ = sigma c as
      infiniteLength = case hook pm of
        Nothing -> False
        Just "LIST.List" -> True
        Just "INT.Int" -> True
        Just "STRING.String" -> True
        Just _ -> False
      isMap = case hook pm of
        Nothing -> False
        Just "MAP.Map" -> True
        Just _ -> False
  in  if infiniteLength || null s₁ || elem Empty s₁ || (not isMap && (toInteger $ length s₁) /= mtd)
      then Just ((expandDefault (fromJust $ getDefaultConstructor c as) (filterMatrix (getDefaultConstructor c as) isDefault (cm,o))),expandDefaultOccurrence cm o <> os)
      else Nothing
mDefault _ = Nothing

maxListSize :: Column -> (Int, Int)
maxListSize c =
  let longestHead = maximum $ map getListHeadSize $ getTerms c
      longestTail = maximum $ map getListTailSize $ getTerms c
  in (longestHead, longestTail)
  where
    getListHeadSize :: Fix Pattern -> (Int)
    getListHeadSize (Fix (ListPattern hd _ _ _ _)) = length hd
    getListHeadSize (Fix (Variable _ _)) = -1
    getListHeadSize (Fix Wildcard) = -1
    getListHeadSize _ = error "unsupported list pattern"
    getListTailSize :: Fix Pattern -> (Int)
    getListTailSize (Fix (ListPattern _ _ tl _ _)) = length tl
    getListTailSize (Fix (Variable _ _)) = -1
    getListTailSize (Fix Wildcard) = -1
    getListTailSize _ = error "unsupported list pattern"

getDefaultConstructor :: Column -> [Clause] -> Maybe Constructor
getDefaultConstructor c cs = 
  let s = sigma c cs
  in if elem Empty s then Just $ NonEmpty $ Ignoring $ getMetadata c
  else case s of
         [] -> Nothing
         list:_ -> let (hd,tl) = maxListSize c
                   in case list of 
                        List sym _ -> Just $ List sym $ (hd + tl)
                        _ -> Nothing

expandDefault :: Constructor -> ClauseMatrix -> ClauseMatrix
expandDefault maxList (ClauseMatrix pm@(PatternMatrix (c : cs)) as) =
  case hook pm of
    Just "LIST.List" -> 
      ClauseMatrix (PatternMatrix $ expandColumn maxList c as <> cs) as
    Just "MAP.Map" -> 
      ClauseMatrix (PatternMatrix $ expandColumn maxList c as <> cs) as
    _ -> ClauseMatrix (PatternMatrix cs) as
expandDefault _ _ = error "must have at least one column"

expandDefaultOccurrence :: ClauseMatrix -> Occurrence -> [Occurrence]
expandDefaultOccurrence cm@(ClauseMatrix pm@(PatternMatrix (c : _)) as) o =
  case hook pm of
    Nothing -> []
    Just "LIST.List" ->
      let cons = fromJust $ getDefaultConstructor c as
      in expandOccurrence cm o cons
    Just "MAP.Map" ->
      let cons = fromJust $ getDefaultConstructor c as
      in expandOccurrence cm o cons
    Just _ -> []
expandDefaultOccurrence _ _ = error "must have at least one column"

firstRow :: PatternMatrix -> [Fix Pattern]
firstRow (PatternMatrix cs) =
  map (\(Column _ _ (p : _)) -> p) cs
notFirstRow :: ClauseMatrix -> PatternMatrix
notFirstRow (ClauseMatrix (PatternMatrix cs) cls) =
  PatternMatrix (map (\(Column m _ (_ : ps)) -> mkColumn (tail cls) m ps) cs)

filterByList :: [Bool] -> [a] -> [a]
filterByList (True  : bs) (x : xs) = x : filterByList bs xs
filterByList (False : bs) (_ : xs) = filterByList bs xs
filterByList _ _                   = []

isDefault :: (Clause, Fix Pattern) -> Bool
isDefault (_, Fix (Pattern _ _ _)) = False
isDefault (_, Fix (ListPattern _ Nothing _ _ _)) = False
isDefault (_, Fix (ListPattern _ (Just _) _ _ _)) = True
isDefault (c, Fix (As _ _ pat)) = isDefault (c,pat)
isDefault (_, Fix Wildcard) = True
isDefault (_, Fix (Variable _ _)) = True
isDefault (_, Fix (MapPattern _ _ (Just _) _ _)) = True
isDefault (_, Fix (MapPattern ks vs Nothing _ _)) = length ks > 0 || length vs > 0

mightUnify :: Fix BoundPattern -> Fix BoundPattern -> Bool
mightUnify (Fix Wildcard) _ = True
mightUnify _ (Fix Wildcard) = True
mightUnify _ (Fix (Variable _ _)) = True
mightUnify (Fix (Variable _ _)) _ = True
mightUnify (Fix (As _ _ p)) p' = mightUnify p p'
mightUnify p (Fix (As _ _ p')) = mightUnify p p'
mightUnify (Fix (Pattern c _ ps)) (Fix (Pattern c' _ ps')) = c == c' && (and $ zipWith mightUnify ps ps')
mightUnify (Fix (ListPattern _ _ _ _ _)) (Fix (ListPattern _ _ _ _ _)) = True
mightUnify (Fix (MapPattern _ _ _ _ _)) (Fix (MapPattern _ _ _ _ _)) = True
mightUnify (Fix (Pattern _ _ _)) _ = False
mightUnify (Fix (ListPattern _ _ _ _ _)) _ = False
mightUnify (Fix (MapPattern _ _ _ _ _)) _ = False

checkPatternIndex :: Constructor -> Metadata -> (Clause, Fix Pattern) -> Bool
checkPatternIndex _ _ (_, Fix Wildcard) = True
checkPatternIndex ix m (c, Fix (As _ _ pat)) = checkPatternIndex ix m (c,pat)
checkPatternIndex _ _ (_, Fix (Variable _ _)) = True
checkPatternIndex (List _ len) _ (_, Fix (ListPattern hd Nothing tl _ _)) = len == (length hd + length tl)
checkPatternIndex (List _ len) _ (_, Fix (ListPattern hd (Just _) tl _ _)) = len >= (length hd + length tl)
checkPatternIndex _ _ (_, Fix (ListPattern _ _ _ _ _)) = False
checkPatternIndex (Symbol (SymbolOrAlias (Id "inj" _) [a,c])) (Metadata _ _ _ meta) (cls, Fix (Pattern ix@(Symbol (SymbolOrAlias name@(Id "inj" _) [b,c'])) _ [p])) =
  let m@(Metadata _ _ _ childMeta) = (fromJust $ meta ix) !! 0
      child = Symbol (SymbolOrAlias name [a,b])
  in c == c' && (a == b || ((isJust $ childMeta $ child) && checkPatternIndex child m (cls,p)))
checkPatternIndex ix _ (_, Fix (Pattern ix' _ _)) = ix == ix'
checkPatternIndex Empty _ (_, Fix (MapPattern ks vs _ _ _)) = length ks == 0 && length vs == 0
checkPatternIndex (HasKey _ _ _) _ (_, Fix (MapPattern _ _ (Just _) _ _)) = True
checkPatternIndex (HasKey _ _ p) _ (c, Fix (MapPattern ks _ Nothing _ _)) = any (mightUnify p) $ map (canonicalizePattern c) ks
checkPatternIndex (HasNoKey _ p) _ (c, Fix (MapPattern ks _ _ _ _)) =
  let canonKs = map (canonicalizePattern c) ks
  in not $ elem p canonKs
checkPatternIndex _ _ (_, Fix (MapPattern _ _ _ _ _)) = error "Invalid map pattern"

addVars :: Maybe Constructor -> [Clause] -> [Fix Pattern] -> Occurrence -> [Clause]
addVars ix as c o =
  let rows = zip c as
  in map (\(p, (Clause a vars ranges)) -> (Clause a (addVarToRow ix o p vars) $ addRange ix o p ranges)) rows

addVarToRow :: Maybe Constructor -> Occurrence -> Fix Pattern -> [VariableBinding] -> [VariableBinding]
addVarToRow _ o (Fix (Variable name hookAtt)) vars = VariableBinding name hookAtt o : vars
addVarToRow _ o (Fix (As name hookAtt _)) vars = VariableBinding name hookAtt o : vars
addVarToRow _ _ (Fix Wildcard) vars = vars
addVarToRow (Just (Symbol (SymbolOrAlias (Id "inj" _) [a,_]))) o (Fix (Pattern (Symbol (SymbolOrAlias (Id "inj" _) [b,_])) _ [p])) vars = if a == b then vars else addVarToRow Nothing o p vars
addVarToRow _ _ (Fix (Pattern _ _ _)) vars = vars
addVarToRow _ _ (Fix (ListPattern _ Nothing _ _ _)) vars = vars
addVarToRow (Just (List _ len)) o (Fix (ListPattern _ (Just p) _ _ _)) vars = addVarToRow Nothing (len : o) p vars
addVarToRow _ _ (Fix (ListPattern _ (Just _) _ _ _)) vars = vars
addVarToRow _ o (Fix (MapPattern [] [] (Just p) _ _)) vars = addVarToRow Nothing o p vars
addVarToRow _ _ (Fix (MapPattern _ _ _ _ _)) vars = vars

addRange :: Maybe Constructor -> Occurrence -> Fix Pattern -> [(Occurrence, Int, Int)] -> [(Occurrence, Int, Int)]
addRange (Just (List _ len)) o (Fix (ListPattern hd (Just (Fix (Variable _ _))) tl _ _)) ranges = (len : o, length hd, length tl) : ranges
addRange _ _ _ ranges = ranges

filterMatrix :: Maybe Constructor -> ((Clause, Fix Pattern) -> Bool) -> (ClauseMatrix, Occurrence) -> ClauseMatrix
filterMatrix ix checkPattern ((ClauseMatrix (PatternMatrix cs@(c : _)) as), o) =
  let filteredRows = map checkPattern $ zip as $ getTerms c
      varsAs = addVars ix as (getTerms c) o
      newAs = filterByList filteredRows varsAs
      newCs = map (filterRows newAs filteredRows) cs
  in ClauseMatrix (PatternMatrix newCs) newAs
  where
    filterRows :: [Clause] -> [Bool] -> Column -> Column
    filterRows newCs fr (Column md _ rs) =
      mkColumn newCs md (filterByList fr rs)
filterMatrix _ _ (cmx,_) = cmx

expandMatrix :: Constructor -> ClauseMatrix -> ClauseMatrix
expandMatrix ix (ClauseMatrix (PatternMatrix (c : cs)) as) =
  ClauseMatrix (PatternMatrix (expandColumn ix c as <> cs)) as
expandMatrix _ _ = error "Cannot expand empty matrix."

-- TODO: improve
computeScore :: Metadata -> [(Fix Pattern,Clause)] -> Double
computeScore _ [] = 0.0
computeScore m ((Fix (Pattern ix@(Symbol (SymbolOrAlias (Id "inj" _) _)) _ _),_):tl) = (1.0 / (fromIntegral $ length $ getInjections m ix)) + computeScore m tl
computeScore m ((Fix (Pattern _ _ _),_):tl) = 1.0 + computeScore m tl
computeScore m ((Fix (ListPattern _ _ _ _ _),_):tl) = 1.0 + computeScore m tl
computeScore m ((Fix (As _ _ pat),c):tl) = computeScore m ((pat,c):tl)
computeScore _ ((Fix Wildcard,_):_) = 0.0
computeScore _ ((Fix (Variable _ _),_):_) = 0.0
computeScore m ((Fix (MapPattern [] [] Nothing _ _),_):tl) = 1.0 + computeScore m tl
computeScore m ((Fix (MapPattern [] [] (Just p) _ _),c):tl) = computeScore m ((p,c):tl)
computeScore m ((Fix (MapPattern ks vs _ _ _),c):tl) = snd $ computeMapScore m c ks vs tl

getBestMapKey :: Column -> [Clause] -> Maybe (Fix BoundPattern)
getBestMapKey (Column m _ (Fix (MapPattern (k:ks) vs _ _ _):tl)) cs = Just $ fst $ computeMapScore m (head cs) (k:ks) vs (zip tl $ tail cs)
getBestMapKey _ _ = Nothing

computeMapScore :: Metadata -> Clause -> [Fix Pattern] -> [Fix Pattern] -> [(Fix Pattern,Clause)] -> (Fix BoundPattern, Double)
computeMapScore m c ks vs tl =
  let zipped = zip ks vs
      scores = map (\(k,v) -> (canonicalizePattern c k,computeMapElementScore m c tl (k,v))) zipped
  in maximumBy (comparing snd) scores

computeMapElementScore :: Metadata -> Clause -> [(Fix Pattern,Clause)] -> (Fix Pattern, Fix Pattern) -> Double
computeMapElementScore m c tl (k,v) = computeScore m [(v,c)] * computeElementScore k c tl

canonicalizePattern :: Clause -> Fix Pattern -> Fix (P Occurrence)
canonicalizePattern (Clause _ vars _) (Fix (Variable name hookAtt)) =
  let names = map getName vars
      os = map getOccurrence vars
      oMap = Map.fromList $ zip names os
  in Fix $ Variable (oMap Map.! name) hookAtt
canonicalizePattern c'@(Clause _ vars _) (Fix (As name hookAtt p)) =
  let names = map getName vars
      os = map getOccurrence vars
      oMap = Map.fromList $ zip names os
  in Fix $ As (oMap Map.! name) hookAtt $ canonicalizePattern c' p
canonicalizePattern c' (Fix (Pattern name hookAtt ps)) = Fix (Pattern name hookAtt $ map (canonicalizePattern c') ps)
canonicalizePattern _ (Fix Wildcard) = Fix Wildcard
canonicalizePattern c' (Fix (ListPattern hd f tl' e o)) = 
  Fix (ListPattern (mapPat c' hd) (mapPat c' f) (mapPat c' tl') e $ canonicalizePattern c' o)
canonicalizePattern c' (Fix (MapPattern ks vs f e o)) = 
  Fix (MapPattern (mapPat c' ks) (mapPat c' vs) (mapPat c' f) e $ canonicalizePattern c' o)

mapPat :: Functor f => Clause -> f (Fix Pattern) -> f (Fix BoundPattern)
mapPat c' = fmap (canonicalizePattern c')

computeElementScore :: Fix Pattern -> Clause -> [(Fix Pattern,Clause)] -> Double
computeElementScore k c tl =
  let bound = isBound getName c k
  in if bound then 
    let canonKey = canonicalizePattern c k
        (ps,cs) = unzip tl
        canonCs = map canonicalizeClause cs
        boundedCanonCs = takeWhile (flip (isBound getOccurrence) canonKey) canonCs
        boundedPs = take (length boundedCanonCs) ps
        boundedCs = take (length boundedCanonCs) cs
        canonPs = zipWith canonicalizePattern boundedCs boundedPs
        psWithK = takeWhile (mapContainsKey canonKey) canonPs
    in fromIntegral $ length psWithK
  else -1.0 / 0.0
  where
    mapContainsKey :: Fix BoundPattern -> Fix BoundPattern -> Bool
    mapContainsKey k' (Fix (MapPattern ks _ _ _ _)) = elem k' ks
    mapContainsKey _ _ = False
    canonicalizeClause :: Clause -> Clause
    canonicalizeClause (Clause a vars ranges) =
      let hooks = map getHook vars
          os = map getOccurrence vars
          names = map show os
      in (Clause a (zipWith3 VariableBinding names hooks os) ranges)
    isBound :: Eq a => (VariableBinding -> a) -> Clause -> Fix (P a) -> Bool
    isBound get c' (Fix (Pattern _ _ ps)) = all (isBound get c') ps
    isBound get c' (Fix (ListPattern hd f tl' _ _)) = all (isBound get c') hd && all (isBound get c') tl' && maybe True (isBound get c') f
    isBound get c' (Fix (MapPattern ks vs f _ _)) = all (isBound get c') ks && all (isBound get c') vs && maybe True (isBound get c') f
    isBound get c'@(Clause _ vars _) (Fix (As name _ p)) = 
      isBound get c' p && (elem name $ map get vars )
    isBound _ _ (Fix Wildcard) = False
    isBound get (Clause _ vars _) (Fix (Variable name _)) =
      elem name $ map get vars

mkColumn :: [Clause] -> Metadata -> [Fix Pattern] -> Column
mkColumn cs m ps = Column m (computeScore m (zip ps cs)) ps

expandColumn :: Constructor -> Column -> [Clause] -> [Column]
expandColumn ix (Column m _ ps) cs =
  let metas    = expandMetadata ix m
      expanded = map (expandPattern ix metas) (zip ps cs)
      ps'' = map (expandIfJust ix metas) (zip expanded cs)
  in  zipWith (mkColumn cs) metas (transpose ps'')

expandMetadata :: Constructor -> Metadata -> [Metadata]
expandMetadata ix (Metadata _ _ sort ms) =
  case ms ix of
    Just m -> m
    Nothing -> error $ show (ix,sort)

expandIfJust :: Constructor
             -> [Metadata]
             -> (([Fix Pattern],Maybe Constructor),Clause)
             -> [Fix Pattern]
expandIfJust _ _ ((p,Nothing),_) = p
expandIfJust _ ms (([p],Just ix),c) =
  fst $ expandPattern ix ms (p,c)
expandIfJust _ _ ((_,Just _),_) = error "invalid injection"

except :: Int -> [a] -> [a]
except i as =
  let (hd,tl) = splitAt i as
  in hd ++ (tail tl)

expandPattern :: Constructor
              -> [Metadata]
              -> (Fix Pattern,Clause)
              -> ([Fix Pattern],Maybe Constructor)
expandPattern (List _ len) _ (Fix (ListPattern hd _ tl _ _), _) = (hd ++ (replicate (len - length hd - length tl) (Fix Wildcard)) ++ tl, Nothing)
expandPattern _ _ (Fix (ListPattern _ _ _ _ _), _) = error "invalid list pattern"
expandPattern (Symbol (SymbolOrAlias name [a, _])) _ (Fix (Pattern (Symbol (SymbolOrAlias (Id "inj" _) [b, _])) _ [fixedP]), _) = ([fixedP], if a == b then Nothing else Just (Symbol (SymbolOrAlias name [a,b])))
expandPattern Empty _ (Fix (MapPattern _ _ _ _ _), _) = ([], Nothing)
expandPattern (NonEmpty _) _ (p,_) = ([p], Nothing)
expandPattern (HasKey _ _ p) _ (m@(Fix (MapPattern ks vs f e o)),c) = 
  let canonKs = map (canonicalizePattern c) ks
      hasKey = elemIndex p canonKs
  in case hasKey of
       Just i -> ([vs !! i, Fix (MapPattern (except i ks) (except i vs) f e o), Fix Wildcard],Nothing)
       Nothing -> ([Fix Wildcard, Fix Wildcard, m],Nothing)
expandPattern (HasNoKey _ _) _ (p,_) = ([p], Nothing)
expandPattern _ _ ((Fix (MapPattern _ _ _ _ _)),_) = error "Invalid map pattern"
expandPattern _ _ (Fix (Pattern _ _ fixedPs), _)  = (fixedPs,Nothing)
expandPattern ix ms (Fix (As _ _ pat), c)         = expandPattern ix ms (pat, c)
expandPattern _ ms (Fix Wildcard, _)              = (replicate (length ms) (Fix Wildcard), Nothing)
expandPattern _ ms (Fix (Variable _ _), _)        = (replicate (length ms) (Fix Wildcard), Nothing)

data L a = L
           { getSpecializations :: ![(Text, a)]
           , getDefault         :: !(Maybe a)
           } deriving (Show, Eq, Functor, Ord)

type Hook = Text

data DecisionTree a = Leaf (Int, [Occurrence])
                    | Fail
                    | Switch Occurrence !(L a)
                    | SwitchLiteral Occurrence Int !(L a)
                    | IsNull Occurrence !(L a)
                    | MakePattern Occurrence (Fix BoundPattern) !a
                    | Swap Index !a
                    | Function Text Occurrence [Occurrence] Hook !a  
                    deriving (Show, Eq, Functor, Ord)

newtype Alias = Alias Text
              deriving (Show)
data Anchor a = Anchor (Maybe Text) (DecisionTree a)
              deriving (Show, Eq, Functor, Ord)

$(deriveEq1 ''L)
$(deriveEq1 ''DecisionTree)
$(deriveShow1 ''L)
$(deriveShow1 ''DecisionTree)
$(deriveShow1 ''Anchor)
$(deriveOrd1 ''L)
$(deriveOrd1 ''DecisionTree)

instance Y.ToYaml a => Y.ToYaml (Anchor a) where
    toYaml (Anchor a (Leaf (act, x))) = Y.maybeNamedMapping a [
        "action" Y..= Y.array [Y.toYaml act, Y.toYaml x]
      ]
    toYaml (Anchor a Fail) = Y.maybeNamedString a "fail"
    toYaml (Anchor a (Switch o x)) = Y.maybeNamedMapping a
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just i -> Y.toYaml i
                                    Nothing -> Y.null
                                )
      , "occurrence" Y..= Y.toYaml o
      ]
    toYaml (Anchor a (IsNull o x)) = Y.maybeNamedMapping a
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "occurrence" Y..= Y.toYaml o
      , "isnull" Y..= Y.toYaml ("true" :: Text)
      , "default" Y..= Y.null
      ]
    toYaml (Anchor a (SwitchLiteral o i x)) = Y.maybeNamedMapping a
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just d -> Y.toYaml d
                                    Nothing -> Y.null
                                )
      , "bitwidth" Y..= Y.toYaml i
      , "occurrence" Y..= Y.toYaml o
      ]
    toYaml (Anchor a (Swap i x)) = Y.maybeNamedMapping a
      ["swap" Y..= Y.array [Y.toYaml i, Y.toYaml x]]
    toYaml (Anchor a (Function name o bindings sort x)) = Y.maybeNamedMapping a
      ["function" Y..= Y.toYaml name
      , "sort" Y..= Y.toYaml sort
      , "occurrence" Y..= Y.toYaml o
      , "args" Y..= Y.toYaml bindings
      , "next" Y..= Y.toYaml x
      ]
    toYaml (Anchor a (MakePattern o p x)) = Y.maybeNamedMapping a
      ["pattern" Y..= Y.toYaml p
      , "occurrence" Y..= Y.toYaml o
      , "next" Y..= Y.toYaml x
      ]

instance Y.ToYaml a => Y.ToYaml (BoundPattern a) where
  toYaml Wildcard = error "Unsupported map/set pattern"
  toYaml (Variable o h) = Y.mapping
    ["hook" Y..= Y.toYaml (pack h)
    , "occurrence" Y..= Y.toYaml o
    ]
  toYaml (As _ _ p) = Y.toYaml p
  toYaml (MapPattern _ _ _ _ o) = Y.toYaml o
  toYaml (ListPattern _ _ _ _ o) = Y.toYaml o
  toYaml (Pattern (Literal s) (Just h) []) = Y.mapping
    ["hook" Y..= Y.toYaml (pack h)
    , "literal" Y..= Y.toYaml (pack s)
    ]
  toYaml (Pattern (Symbol s) Nothing ps) = Y.mapping
    ["constructor" Y..= Y.toYaml (pack $ unparseToString s)
    , "args" Y..= Y.array (map Y.toYaml ps)
    ]
  toYaml (Pattern _ _ _) = error "Unsupported map/set pattern"

instance Y.ToYaml Alias where
    toYaml (Alias name) = Y.alias name

instance Y.ToYaml (Fix BoundPattern) where
  toYaml = cata Y.toYaml

serializeToYaml :: (Free Anchor Alias) -> B.ByteString
serializeToYaml = Y.toByteString . Y.toYaml

instance Y.ToYaml (Anchor (Free Anchor Alias)) => Y.ToYaml (Free Anchor Alias) where
    toYaml (Pure a) = Y.toYaml a
    toYaml (Free f) = Y.toYaml f

-- gets the decision tree for handling the leaf of a particular row
getLeaf :: Int -> [Occurrence] -> [Fix Pattern] -> Clause -> (Int -> Fix DecisionTree) -> Fix DecisionTree
getLeaf ix os ps (Clause (Action a rhsVars maybeSideCondition) matchedVars ranges) next =
  let row = zip os ps
      -- first, add all remaining variable bindings to the clause
      vars = nub $ foldr (\(o, p) -> (addVarToRow Nothing o p)) matchedVars row
      -- then group the bound variables by their name
      grouped = foldr (\(VariableBinding name hookAtt o) -> \m -> Map.insert (name,hookAtt) (o : Map.findWithDefault [] (name,hookAtt) m) m) Map.empty vars
      -- compute the variables bound more than once
      nonlinear = Map.filter ((> 1) . length) grouped
      nonlinearPairs = Map.map (\l -> zip l (tail l)) nonlinear
      -- get some random occurrence bound for each variable in the map
      deduped = Map.foldrWithKey (\(name,_) -> \l -> (:) (name,head l)) [] grouped
      -- sort alphabetically by variable name
      sorted = sortBy (compare `on` fst) deduped
      -- filter by the variables used in the rhs
      filtered = filter (flip elem rhsVars . fst) sorted
      (_, newVars) = unzip filtered
      atomicLeaf = leaf a newVars
      -- check that all occurrences of the same variable are equal
      (nonlinearLeaf,ix') = Map.foldrWithKey (\(_, hookAtt) -> flip $ foldr $ makeEquality hookAtt) (atomicLeaf,ix) nonlinearPairs
      sc = case maybeSideCondition of
             -- if there is no side condition, continue
             Nothing -> nonlinearLeaf
                          -- filter by the variables used in the side condition
             Just cond -> let condFiltered = filter (flip elem cond . fst) sorted
                              (_, condVars) = unzip condFiltered
                              newO = [ix', 0]
                          -- evaluate the side condition and if it is true, continue, otherwise go to the next row
                          in function (pack $ "side_condition_" ++ (show a)) newO condVars "BOOL.Bool" (
                                 switchLiteral newO 1 [("1", nonlinearLeaf),
                                                       ("0", (next $ ix'+1))
                                                      ] Nothing)
  -- fill out the bindings for list range variables
  in foldr (\(o, hd, tl) -> Fix . Function "hook_LIST_range_long" o [tail o, [hd, -1], [tl, -1]] "LIST.List") sc ranges
  where
    makeEquality :: String -> (Occurrence, Occurrence) -> (Fix DecisionTree, Int) -> (Fix DecisionTree, Int)
    -- compare the values of the variables with the same name and if they are equal, continue, otherwise go to the next row
    makeEquality hookAtt (o1,o2) (dt,ix') = (function (equalityFun hookAtt) [ix', 0] [o1, o2] "BOOL.Bool" (switchLiteral [ix', 0] 1 [("1", dt), ("0", (next $ ix'+1))] Nothing), ix'+1)

equalityFun :: String -> Text
equalityFun "BOOL.Bool" = "hook_BOOL_eq"
equalityFun "INT.Int" = "hook_INT_eq"
equalityFun "STRING.String" = "hook_KEQUAL_eq"
equalityFun s = error ("unexpected hook: " ++ s)

swapAt :: Int -> Int -> [a] -> [a]
swapAt i j xs = 
  if i == j then xs else
  if i > j then swapAt j i xs else
  let elemI = xs !! i
      elemJ = xs !! j
      left = take i xs
      middle = take (j - i - 1) (drop (i + 1) xs)
      right = drop (j + 1) xs
  in left ++ (elemJ:middle) ++ (elemI:right)

compilePattern :: (ClauseMatrix, [Occurrence]) -> (Fix DecisionTree)
compilePattern firstCm =
  compilePattern' 0 firstCm
  where
    compilePattern' :: Int -> (ClauseMatrix, [Occurrence]) -> (Fix DecisionTree)
    compilePattern' ix (cm@(ClauseMatrix pm@(PatternMatrix cs) ac), os) = 
      case ac of
        [] -> Fix Fail
        hd:tl -> 
          if isWildcardRow pm then
            -- if there is only one row left, then try to match it and fail the matching if it fails
            -- otherwise, if it fails, try to match the remainder of hte matrix
            if length ac == 1 then getLeaf ix os (firstRow pm) hd (const failure) else getLeaf ix os (firstRow pm) hd (flip compilePattern' ((ClauseMatrix (notFirstRow cm) tl), os))
          else 
          -- compute the column with the best score, choosing the first such column if they are equal
          let bestColIx = fst $ maximumBy (comparing (getScore . snd)) $ reverse $ indexed cs
              -- swap the columns and occurrences so that the best column is first
              cs' = swapAt 0 bestColIx cs
              os' = swapAt 0 bestColIx os
              pm' = PatternMatrix cs'
              cm' = (ClauseMatrix pm' ac,os')
              -- compute the hook attribute of the new first column
              hookAtt = hook pm'
              -- c9ompute the signature of the new first column
              s₁ = sigma₁ $ fst cm'
              -- specialize on each constructor in the signature
              ls = map (`mSpecialize` cm') s₁
              -- compute the default matrix if it exists
              d  = mDefault cm'
              dt = case hookAtt of
                -- not matching a builtin, therefore construct a regular switch
                -- that matches the tag of the block.
                Nothing -> Fix $ Switch (head os') L
                    { getSpecializations = map (second (compilePattern' ix)) ls
                    , getDefault = compilePattern' ix <$> d
                    }
                -- matching a bool, so match the integer value of the bool with a bitwidth of 1
                Just "BOOL.Bool" -> Fix $ SwitchLiteral (head os') 1 L
                    { getSpecializations = map (second (compilePattern' ix)) ls
                    , getDefault = compilePattern' ix <$> d
                    }
                -- matching a list, so construct a node to decompose the list into its elements
                Just "LIST.List" -> listPattern ix (head os') ls d s₁ (head cs')
                -- matching a map, so construct a node to decompose the map by one of its elements
                Just "MAP.Map" -> mapPattern ix (head os') ls d s₁ (head cs') ac
                -- matching an mint, so match the integer value of the mint with the specified bitwidth
                Just "MAP.Map" -> mapPattern ix (head os') ls d s₁ (head cs') ac
                Just ('M':'I':'N':'T':'.':'M':'I':'n':'t':' ':bw) -> Fix $ SwitchLiteral (head os') (read bw) L
                    { getSpecializations = map (second (compilePattern' ix)) ls
                    , getDefault = compilePattern' ix <$> d
                    }
                -- matching a string or int, so compare the value of the token against a list of constants
                Just hookName -> equalLiteral ix (head os') hookName ls d
          -- if necessary, generate a swap node
          in if bestColIx == 0 then dt else Fix $ Swap bestColIx dt
    -- returns whether the row is done matching
    isWildcardRow :: PatternMatrix -> Bool
    isWildcardRow = and . map isWildcard . firstRow
    isWildcard :: Fix Pattern -> Bool
    isWildcard (Fix Wildcard) = True
    isWildcard (Fix (Variable _ _)) = True
    isWildcard (Fix (As _ _ pat)) = isWildcard pat
    isWildcard (Fix (Pattern _ _ _)) = False
    isWildcard (Fix (MapPattern [] [] (Just p) _ _)) = isWildcard p
    isWildcard (Fix (MapPattern _ _ _ _ _)) = False
    isWildcard (Fix (ListPattern _ _ _ _)) = False
    -- constructs a tree to test the current occurrence against each possible match in turn
    equalLiteral :: Int -> Occurrence -> String -> [(Text, (ClauseMatrix, [Occurrence]))] -> Maybe (ClauseMatrix, [Occurrence]) -> Fix DecisionTree
    -- if no specializations remain and a default exists, consume the occurrence and continue with the default
    equalLiteral o litO _ [] (Just d) = Fix $ Switch litO L { getSpecializations = [], getDefault = Just $ compilePattern' o d }
    -- if no specializations remain and no default exists, fail the match
    equalLiteral _ _ _ [] (Nothing) = Fix Fail
    -- consume each specialization one at a time and try to match it
    -- if it succeseds, consume the occurrence and continue with the specialized matrix
    -- otherweise, test the next literal
    equalLiteral o litO hookName ((name,spec):tl) d =
      let newO = [o, 0]
          eqO = [o+1,0]
      in Fix $ MakePattern newO (Fix (Pattern (Literal name) (Just hookName) [])) $ 
             Fix $ Function (equalityFun hookName) eqO [litO, newO] "BOOL.Bool" $
                 Fix $ SwitchLiteral eqO 1 $ L [("1", Fix $ Switch litO L 
                                                             { getSpecializations = [], getDefault = Just $ compilePattern' (o+2) spec }),
                                                 ("0", equalLiteral (o+2) litO hookName tl d)
                                                ] Nothing
    -- construct a tree to test the length of the list and bind the elements of the list to their occurrences
    listPattern :: Int -> Occurrence -> [(Text, (ClauseMatrix, [Occurrence]))] -> Maybe (ClauseMatrix, [Occurrence]) -> [Constructor] -> Column -> Fix DecisionTree
    listPattern nextO listO ls d signature firstCol =
      let newO = [nextO, 0]
          (cons,matrices) = unzip ls
          specs = zip cons $ zip signature matrices
          maxList = maxListSize firstCol
      -- test the length of the list against the specializations of the matrix
      -- if it succeeds, bind the occurrences and continue with the specialized matrix
      -- otherwise, try the default case
      in Fix $ Function "hook_LIST_size_long" newO [listO] "MINT.MInt 64" $
           Fix $ SwitchLiteral newO 64 $ L 
             { getSpecializations = map (second (expandListPattern (nextO+1) listO)) specs
             , getDefault = expandListPatternDefault (nextO+1) listO maxList <$> d
             }
    -- get each element of the list specified in the list pattern and bind it to the occurrences,
    -- then compile the remaining matrix
    expandListPattern :: Int -> Occurrence -> (Constructor, (ClauseMatrix, [Occurrence])) -> Fix DecisionTree
    expandListPattern nextO listO ((List _ i),cm') =
      foldl (listGet listO Nothing) (compilePattern' nextO cm') [0..i-1]
    expandListPattern _ _ _ = error "invalid list pattern"
    -- get each element of the list and bind it to the occurrence, then compile the default matrix
    expandListPatternDefault :: Int -> Occurrence -> (Int,Int) -> (ClauseMatrix, [Occurrence]) -> Fix DecisionTree
    expandListPatternDefault nextO listO (hd,tl) cm' =
      foldl (listGet listO $ Just (hd,tl)) (foldl (listGet listO Nothing) (compilePattern' nextO cm') [0..hd-1]) [hd..hd+tl-1]
    -- generate a single list lookup operation to bind one element of the list against its occurrence
    listGet :: Occurrence -> Maybe (Int, Int) -> Fix DecisionTree -> Int -> Fix DecisionTree
    listGet listO l dt o = 
      Fix $ Function "hook_LIST_get_long" (o : listO) 
        [listO, case l of
                  Nothing -> [o, -1]
                  Just (hd,tl) -> [o-tl-hd, -1]
        ] "STRING.String" dt
    mapPattern :: Int -> Occurrence -> [(Text, (ClauseMatrix, [Occurrence]))] -> Maybe (ClauseMatrix, [Occurrence]) -> [Constructor] -> Column -> [Clause] -> Fix DecisionTree
    mapPattern o mapO ls d s c cs =
      let newO = [o, 0]
      -- if Empty is in the signature, test whether the map is empty or not.
      in if elem Empty s then
        Fix $ Function "hook_MAP_size_long" newO [mapO] "MINT.MInt 64" $
          Fix $ SwitchLiteral newO 64 $ L
            { getSpecializations = map (second (compilePattern' (o+1))) ls
            , getDefault = compilePattern' (o+1) <$> d
            }
      else
        -- otherwise, get the best key and test whether the best key is in the map or not
        let key = getBestMapKey c cs
        in case key of
             Nothing -> Fix $ Switch mapO $ L
               { getSpecializations = map (second (compilePattern' (o+1))) ls
               , getDefault = compilePattern' (o+1) <$> d
               }
             Just k -> Fix $ MakePattern newO k $ 
                         Fix $ Function "hook_MAP_lookup_null" (0 : mapO) [mapO, newO] "STRING.String" $
                           Fix $ Function "hook_MAP_remove" (1 : mapO) [mapO, newO] "MAP.Map" $
                             Fix $ IsNull (0 : mapO) $ L
                               { getSpecializations = map (second (compilePattern' (o+1))) ls
                               , getDefault = compilePattern' (o+1) <$> d
                               }

shareDt :: Fix DecisionTree -> Free Anchor Alias
shareDt =
  snd . computeSharing (Map.empty)
  where
    computeSharing :: Map.Map (Fix DecisionTree) Alias -> Fix DecisionTree -> (Map.Map (Fix DecisionTree) Alias, Free Anchor Alias)
    computeSharing m dt = 
      let name = pack . show . length
          mapDefault = mapAccumL computeSharing
          addName m' = Map.insert dt (Alias $ name m') m'
          mapChild = computeSharing m
      in case Map.lookup dt m of
           Just alias -> (m, Pure alias)
           Nothing -> case dt of
                        Fix (Leaf a) -> (addName m, Free (Anchor (Just $ name m) (Leaf a)))
                        Fix Fail -> (m, Free (Anchor Nothing Fail))
                        Fix (Swap i a) -> let (m',child) = mapChild a in (addName m',Free (Anchor (Just $ name m') (Swap i child)))
                        Fix (MakePattern o p a) -> let (m',child) = mapChild a in (addName m', Free (Anchor (Just $ name m') (MakePattern o p child)))
                        Fix (Function n o os s a) -> let (m',child) = mapChild a in (addName m', Free (Anchor (Just $ name m') (Function n o os s child)))
                        Fix (Switch o (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (Switch o (L s' d'))))
                        Fix (IsNull o (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (IsNull o (L s' d'))))
                        Fix (SwitchLiteral o bw (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (SwitchLiteral o bw (L s' d'))))
                      
                        
    mapSpec :: Map.Map (Fix DecisionTree) Alias -> [(Text,Fix DecisionTree)] -> (Map.Map (Fix DecisionTree) Alias,[(Text,Free Anchor Alias)])
    mapSpec m s =
      let (ts,as) = unzip s
          (m',as') = mapAccumL computeSharing m as
      in (m',zip ts as')
