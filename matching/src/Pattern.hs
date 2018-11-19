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
               , Metadata(..)
               , P(..)
               , Pattern
               , Clause(..)
               , Action(..)
               , Occurrence
               , Fringe
               , Index
               , Ignoring(..)
               , Constructor(..)
               , mkClauseMatrix
               , sigma
               , mSpecialize
               , mDefault
               , getRealScore
               , swapAt
               , failure
               , leaf
               , switch
               , switchLiteral
               , function
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
import           Data.List             (transpose,nub,sortBy,maximumBy,elemIndex,intersect,zipWith4)
import           Data.List.Index       (indexed)
import           Data.Maybe            (catMaybes,isJust,fromJust,listToMaybe)
import           Data.Ord              (comparing)
import           Data.Semigroup        ((<>))
import           Data.Text             (Text, pack, unpack)
import           Data.Traversable      (mapAccumL)
import           Kore.AST.Common       (SymbolOrAlias (..), Id (..), Sort(..), AstLocation(..))
import           Kore.AST.MetaOrObject (Object (..))
import           Kore.Unparser         (unparseToString)
import           TextShow              (showt)
import qualified Data.Map.Strict as Map
import qualified Data.Yaml.Builder as Y
import qualified Data.ByteString as B

data Column = Column
              { getMetadata :: !Metadata
              , getTerms    :: ![Fix Pattern]
              }

instance Show Column where
  showsPrec _ (Column _ ts) =
    showString "Column " . showList ts

data Metadata = Metadata
                { getLength :: !Integer
                , getInjections :: Constructor -> [Constructor]
                , getOverloads :: Constructor -> [Constructor]
                , getSort :: Sort Object
                , getChildren :: Constructor -> Maybe [Metadata]
                }

instance (Show Metadata) where
  show (Metadata _ _ _ sort _) = show sort

type Occurrence   = [Text]
type Fringe = [(Occurrence, Bool)]

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
                 | HasKey Bool (SymbolOrAlias Object) (Ignoring Metadata) (Maybe (Fix BoundPattern))
                 | HasNoKey (Ignoring Metadata) (Maybe (Fix BoundPattern))
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
                   , original :: !a
                   }
                 | SetPattern
                   { getElements :: ![a]
                   , getFrame :: !(Maybe a)
                   , element :: SymbolOrAlias Object
                   , original :: !a
                   }
                 | As var String a
                 | Wildcard
                 | Variable var String
                 deriving (Show, Eq, Functor)

type Pattern = P String
type BoundPattern = P (Maybe Occurrence)

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
                    , getOverloadChildren :: [(Constructor, VariableBinding)]
                    }
                    deriving (Show)

data ClauseMatrix = ClauseMatrix PatternMatrix ![Clause]
                    deriving (Show)
 
-- [ Builders ]

mkClauseMatrix :: [Column]
               -> [Action]
               -> Either Text (ClauseMatrix, Fringe)
mkClauseMatrix cs as = do
  validateColumnLength (length as) cs
  pure ((ClauseMatrix (PatternMatrix cs) (map (\a -> Clause a [] [] []) as)),map (\i -> ([pack $ show i], False)) [1..length cs]) 
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

-- [ Matrix ]

sigma :: Bool -> Column -> [Clause] -> [Constructor]
sigma exact c cs =
  let used = concat $ zipWith ix cs $ getTerms c
      bestKey = getBestKey c cs
      bestUsed = case bestKey of
                   Nothing -> used
                   Just k -> filter (isBest k) used
      usedInjs = nub $ concatMap (getInjections $ getMetadata c) bestUsed
      dups = if exact then bestUsed else bestUsed ++ usedInjs
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
          bound = isBound getName cls k
          canonK = if bound then Just $ canonicalizePattern cls k else Nothing
      in [HasKey False e m canonK, HasNoKey m canonK]
    ix cls (Fix (MapPattern (k:k':ks) vs f e o)) = 
      let m = Ignoring $ getMetadata c
          bound = isBound getName cls k
          canonK = if bound then Just $ canonicalizePattern cls k else Nothing
      in [HasKey False e m canonK, HasNoKey m canonK] ++ ix cls (Fix (MapPattern (k':ks) vs f e o))
    ix _ (Fix (SetPattern [] Nothing _ _)) = [Empty]
    ix cls (Fix (SetPattern [] (Just p) _ _)) = ix cls p
    ix cls (Fix (SetPattern (k:[]) _ e _)) = 
      let m = Ignoring $ getMetadata c
          bound = isBound getName cls k
          canonK = if bound then Just $ canonicalizePattern cls k else Nothing
      in [HasKey True e m canonK, HasNoKey m canonK]
    ix cls (Fix (SetPattern (k:k':ks) f e o)) = 
      let m = Ignoring $ getMetadata c
          bound = isBound getName cls k
          canonK = if bound then Just $ canonicalizePattern cls k else Nothing
      in [HasKey True e m canonK, HasNoKey m canonK] ++ ix cls (Fix (SetPattern (k':ks) f e o))
    ix cls (Fix (As _ _ pat))      = ix cls pat
    ix _ (Fix Wildcard)          = []
    ix _ (Fix (Variable _ _))    = []
    isBest :: Fix BoundPattern -> Constructor -> Bool
    isBest k (HasKey _ _ _ (Just k')) = k == k'
    isBest k (HasNoKey _ (Just k')) = k == k'
    isBest _ (HasKey _ _ _ Nothing) = False
    isBest _ (HasNoKey _ Nothing) = False
    isBest _ _ = True
   

sigma₁ :: Bool -> ClauseMatrix -> [Constructor]
sigma₁ exact (ClauseMatrix (PatternMatrix (c : _)) as) = sigma exact c as
sigma₁ _ _                       = []

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
    ix (Fix (SetPattern _ _ _ _)) = Just "SET.Set"
    ix (Fix (As _ _ pat))    = ix pat
    ix (Fix Wildcard)        = Nothing
    ix (Fix (Variable _ _))  = Nothing
hook _                       = Nothing

mSpecialize :: Constructor -> (ClauseMatrix, Fringe) -> (Text, (ClauseMatrix, Fringe))
mSpecialize ix (cm@(ClauseMatrix (PatternMatrix (c : _)) _), o : os) = 
   let newOs = expandOccurrence cm o ix <> os
       cm' = filterMatrix (Just ix) (checkPatternIndex ix (getMetadata c)) (cm,fst o)
       cm'' = expandMatrix ix cm'
   in (getConstructor ix, (cm'', newOs))
   where
     getConstructor :: Constructor -> Text
     getConstructor (Symbol sym) = pack $ unparseToString sym
     getConstructor (Literal str) = pack str
     getConstructor (List _ i) = pack $ show i
     getConstructor Empty = "0"
     getConstructor (NonEmpty _) = error "Invalid map pattern"
     getConstructor (HasKey _ _ _ _) = "1"
     getConstructor (HasNoKey _ _) = "0"

mSpecialize _ _ = error "must have at least one column"

expandOccurrence :: ClauseMatrix -> (Occurrence,Bool) -> Constructor -> Fringe
expandOccurrence (ClauseMatrix (PatternMatrix (c : _)) _) o ix =
  case ix of
    Empty -> []
    NonEmpty _ -> [o]
    HasKey isSet _ _ (Just k) -> if isSet then [(pack (show k ++ "_rem") : fst o, False), o] else [(pack (show k ++ "_val") : fst o, False), (pack (show k ++ "_rem") : fst o, False), o]
    HasKey _ _ _ Nothing -> error "Invalid map/set pattern"
    HasNoKey _ _ -> [o]
    _ -> let (Metadata _ _ _ _ mtd) = getMetadata c
             a = length $ fromJust $ mtd ix
             os = map (\i -> (pack $ show i) : fst o) [0..a-1]
             isExact = isInj ix
         in zip os $ replicate (length os) isExact
  where
    isInj :: Constructor -> Bool
    isInj (Symbol (SymbolOrAlias (Id "inj" _) _)) = True
    isInj _ = False
expandOccurrence _ _ _ = error "must have at least one column"

mDefault :: [Constructor] -> (ClauseMatrix, Fringe) -> Maybe (ClauseMatrix, Fringe)
mDefault s₁ (cm@(ClauseMatrix pm@(PatternMatrix (c : _)) _),o : os) =
  let (Metadata mtd _ _ _ _) = getMetadata c
      infiniteLength = case hook pm of
        Nothing -> False
        Just "LIST.List" -> True
        Just "INT.Int" -> True
        Just "STRING.String" -> True
        Just _ -> False
      isMapOrSet = case hook pm of
        Nothing -> False
        Just "MAP.Map" -> True
        Just "SET.Set" -> True
        Just _ -> False
  in  if infiniteLength || null s₁ || elem Empty s₁ || (not isMapOrSet && (toInteger $ length s₁) /= mtd)
      then Just ((expandDefault (getDefaultConstructor c s₁) (hook pm) (filterMatrix (getDefaultConstructor c s₁) isDefault (cm,fst o))),expandDefaultOccurrence cm o s₁ <> os)
      else Nothing
mDefault _ _ = Nothing

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

getDefaultConstructor :: Column -> [Constructor] -> Maybe Constructor
getDefaultConstructor c s = 
  if elem Empty s then Just $ NonEmpty $ Ignoring $ getMetadata c
  else case s of
         [] -> Nothing
         list:_ -> let (hd,tl) = maxListSize c
                   in case list of 
                        List sym _ -> Just $ List sym $ (hd + tl)
                        _ -> Nothing

expandDefault :: Maybe Constructor -> Maybe String -> ClauseMatrix -> ClauseMatrix
expandDefault (Just ix) hookAtt (ClauseMatrix (PatternMatrix (c : cs)) as) =
  case hookAtt of
    Just "LIST.List" -> 
      ClauseMatrix (PatternMatrix $ expandColumn ix c as <> cs) as
    Just "MAP.Map" -> 
      ClauseMatrix (PatternMatrix $ expandColumn ix c as <> cs) as
    Just "SET.Set" -> 
      ClauseMatrix (PatternMatrix $ expandColumn ix c as <> cs) as
    _ -> ClauseMatrix (PatternMatrix cs) as
expandDefault Nothing _ (ClauseMatrix (PatternMatrix (_ : cs)) as) = ClauseMatrix (PatternMatrix cs) as
expandDefault _ _ _ = error "must have at least one column"

expandDefaultOccurrence :: ClauseMatrix -> (Occurrence,Bool) -> [Constructor] -> Fringe
expandDefaultOccurrence cm@(ClauseMatrix pm@(PatternMatrix (c : _)) _) o s =
  case hook pm of
    Nothing -> []
    Just "LIST.List" ->
      let cons = fromJust $ getDefaultConstructor c s
      in expandOccurrence cm o cons
    Just "MAP.Map" ->
      case getDefaultConstructor c s of
        Nothing -> []
        Just cons -> expandOccurrence cm o cons
    Just "SET.Set" ->
      case getDefaultConstructor c s of
        Nothing -> []
        Just cons -> expandOccurrence cm o cons
    Just _ -> []
expandDefaultOccurrence _ _ _ = error "must have at least one column"

firstRow :: PatternMatrix -> [Fix Pattern]
firstRow (PatternMatrix cs) =
  map (\(Column _ (p : _)) -> p) cs
notFirstRow :: PatternMatrix -> PatternMatrix
notFirstRow (PatternMatrix cs) =
  PatternMatrix (map (\(Column m (_ : ps)) -> Column m ps) cs)

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
isDefault (_, Fix (SetPattern _ (Just _) _ _)) = True
isDefault (_, Fix (SetPattern es Nothing _ _)) = length es > 0

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
mightUnify (Fix (SetPattern _ _ _ _)) (Fix (SetPattern _ _ _ _)) = True
mightUnify (Fix (Pattern _ _ _)) _ = False
mightUnify (Fix (ListPattern _ _ _ _ _)) _ = False
mightUnify (Fix (MapPattern _ _ _ _ _)) _ = False
mightUnify (Fix (SetPattern _ _ _ _)) _ = False

isValidOverload :: [Fix Pattern] -> Metadata -> Clause -> [Metadata] -> Constructor -> Bool
isValidOverload ps (Metadata _ _ _ _ childMeta) cls metaPs less =
  case childMeta less of
    Nothing -> False
    Just metaTs -> 
      let items = zipWith3 isValidChild ps metaPs metaTs
      in and items
  where
    isValidChild :: Fix Pattern -> Metadata -> Metadata -> Bool
    isValidChild p metaP@(Metadata _ _ _ _ m) metaT =
      let sortP = getSort metaP
          sortT = getSort metaT
          child = Symbol (SymbolOrAlias (Id "inj" AstLocationNone) [sortT, sortP])
      in sortP == sortT || ((isJust $ m $ child) && checkPatternIndex child metaP (cls,p))

checkPatternIndex :: Constructor -> Metadata -> (Clause, Fix Pattern) -> Bool
checkPatternIndex _ _ (_, Fix Wildcard) = True
checkPatternIndex ix m (c, Fix (As _ _ pat)) = checkPatternIndex ix m (c,pat)
checkPatternIndex _ _ (_, Fix (Variable _ _)) = True
checkPatternIndex (List _ len) _ (_, Fix (ListPattern hd Nothing tl _ _)) = len == (length hd + length tl)
checkPatternIndex (List _ len) _ (_, Fix (ListPattern hd (Just _) tl _ _)) = len >= (length hd + length tl)
checkPatternIndex _ _ (_, Fix (ListPattern _ _ _ _ _)) = False
checkPatternIndex (Symbol (SymbolOrAlias (Id "inj" _) [a,c])) (Metadata _ _ _ _ meta) (cls, Fix (Pattern ix@(Symbol (SymbolOrAlias name@(Id "inj" _) [b,c'])) _ [p])) =
  let m@(Metadata _ _ _ _ childMeta) = (fromJust $ meta ix) !! 0
      child = Symbol (SymbolOrAlias name [a,b])
  in c == c' && (a == b || ((isJust $ childMeta $ child) && checkPatternIndex child m (cls,p)))
checkPatternIndex inj@(Symbol (SymbolOrAlias (Id "inj" _) _)) (Metadata _ _ overloads _ meta) (cls, Fix (Pattern ix _ ps)) =
  let less = overloads ix
      childMeta = (fromJust $ meta inj) !! 0
  in any (isValidOverload ps childMeta cls $ fromJust $ meta ix) less
  
checkPatternIndex ix _ (_, Fix (Pattern ix' _ _)) = ix == ix'
checkPatternIndex Empty _ (_, Fix (MapPattern ks vs _ _ _)) = length ks == 0 && length vs == 0
checkPatternIndex Empty _ (_, Fix (SetPattern es _ _ _)) = length es == 0
checkPatternIndex (HasKey _ _ _ _) _ (_, Fix (MapPattern _ _ (Just _) _ _)) = True
checkPatternIndex (HasKey _ _ _ _) _ (_, Fix (SetPattern _ (Just _) _ _)) = True
checkPatternIndex (HasKey _ _ _ (Just p)) _ (c, Fix (MapPattern ks _ Nothing _ _)) = any (mightUnify p) $ map (canonicalizePattern c) ks
checkPatternIndex (HasKey _ _ _ (Just p)) _ (c, Fix (SetPattern es Nothing _ _)) = any (mightUnify p) $ map (canonicalizePattern c) es
checkPatternIndex (HasKey _ _ _ Nothing) _ _ = error "TODO: map/set choice"
checkPatternIndex (HasNoKey _ (Just p)) _ (c, Fix (MapPattern ks _ _ _ _)) =
  let canonKs = map (canonicalizePattern c) ks
  in not $ elem p canonKs
checkPatternIndex (HasNoKey _ (Just p)) _ (c, Fix (SetPattern es _ _ _)) =
  let canonEs = map (canonicalizePattern c) es
  in not $ elem p canonEs
checkPatternIndex (HasNoKey _ Nothing) _ _ = error "TODO: map/set choice"
checkPatternIndex _ _ (_, Fix (MapPattern _ _ _ _ _)) = error "Invalid map pattern"
checkPatternIndex _ _ (_, Fix (SetPattern _ _ _ _)) = error "Invalid map pattern"

addVars :: Maybe Constructor -> [Clause] -> Column -> Occurrence -> [Clause]
addVars ix as (Column m ps) o =
  let rows = zip ps as
  in map (\(p, (Clause a vars ranges children)) -> (Clause a (addVarToRow ix o p vars) (addRange ix o p ranges) $ addOverloads m ix ("0" : o) p children)) rows

addVarToRow :: Maybe Constructor -> Occurrence -> Fix Pattern -> [VariableBinding] -> [VariableBinding]
addVarToRow _ o (Fix (Variable name hookAtt)) vars = VariableBinding name hookAtt o : vars
addVarToRow ix o (Fix (As name hookAtt p)) vars = VariableBinding name hookAtt o : addVarToRow ix o p vars
addVarToRow _ _ (Fix Wildcard) vars = vars
addVarToRow (Just (Symbol (SymbolOrAlias (Id "inj" _) [a,_]))) o (Fix (Pattern (Symbol (SymbolOrAlias (Id "inj" _) [b,_])) _ [p])) vars = if a == b then vars else addVarToRow Nothing o p vars
addVarToRow _ _ (Fix (Pattern _ _ _)) vars = vars
addVarToRow _ _ (Fix (ListPattern _ Nothing _ _ _)) vars = vars
addVarToRow (Just (List _ len)) o (Fix (ListPattern _ (Just p) _ _ _)) vars = addVarToRow Nothing ((pack $ show len) : o) p vars
addVarToRow _ _ (Fix (ListPattern _ (Just _) _ _ _)) vars = vars
addVarToRow _ o (Fix (MapPattern [] [] (Just p) _ _)) vars = addVarToRow Nothing o p vars
addVarToRow _ _ (Fix (MapPattern _ _ _ _ _)) vars = vars
addVarToRow _ o (Fix (SetPattern [] (Just p) _ _)) vars = addVarToRow Nothing o p vars
addVarToRow _ _ (Fix (SetPattern _ _ _ _)) vars = vars

addRange :: Maybe Constructor -> Occurrence -> Fix Pattern -> [(Occurrence, Int, Int)] -> [(Occurrence, Int, Int)]
addRange (Just (List _ len)) o (Fix (ListPattern hd (Just (Fix (Variable _ _))) tl _ _)) ranges = ((pack $ show len) : o, length hd, length tl) : ranges
addRange _ _ _ ranges = ranges

addOverloads :: Metadata -> Maybe Constructor -> Occurrence -> Fix Pattern -> [(Constructor, VariableBinding)] -> [(Constructor, VariableBinding)]
addOverloads (Metadata _ _ _ _ meta) (Just (Symbol (SymbolOrAlias name@(Id "inj" _) [a,_]))) o (Fix (Pattern ix@(Symbol (SymbolOrAlias (Id "inj" _) [b,_])) _ [p])) children = 
  if a == b then 
    children  
  else
    let childMeta = (fromJust $ meta ix) !! 0
    in addOverloads childMeta (Just (Symbol (SymbolOrAlias name [a,b]))) o p children
addOverloads (Metadata _ _ overloads _ meta) (Just inj@(Symbol (SymbolOrAlias (Id "inj" _) _))) o (Fix (Pattern ix _ ps)) children = 
  let less = overloads ix
      metaPs = fromJust $ meta ix
      (Metadata _ _ _ _ childMeta) = (fromJust $ meta inj) !! 0
      childMaybe = listToMaybe $ catMaybes $ map childMeta less
  in case childMaybe of
    Nothing -> children
    Just metaTs -> 
      let items = concat $ zipWith4 getVar metaPs metaTs ps [0..length metaPs-1]
      in items ++ children
  where
    getVar :: Metadata -> Metadata -> Fix Pattern -> Int -> [(Constructor, VariableBinding)]
    getVar metaP metaT p i =
      let vars = addVarToRow Nothing ("-1" : (pack $ show i) : o) p []
          sortP = getSort metaP
          sortT = getSort metaT
          child = Symbol (SymbolOrAlias (Id "inj" AstLocationNone) [sortT, sortP])
          childOverloads = addOverloads metaP (Just child) ((pack $ show i) : o) p []
      in if sortP == sortT then [] else zip (replicate (length vars) child) vars ++ childOverloads
addOverloads _ _ _ _ children = children

filterMatrix :: Maybe Constructor -> ((Clause, Fix Pattern) -> Bool) -> (ClauseMatrix, Occurrence) -> ClauseMatrix
filterMatrix ix checkPattern ((ClauseMatrix (PatternMatrix cs@(c : _)) as), o) =
  let filteredRows = map checkPattern $ zip as $ getTerms c
      varsAs = addVars ix as c o
      newAs = filterByList filteredRows varsAs
      newCs = map (filterRows filteredRows) cs
  in ClauseMatrix (PatternMatrix newCs) newAs
  where
    filterRows :: [Bool] -> Column -> Column
    filterRows fr (Column md rs) =
      Column md (filterByList fr rs)
filterMatrix _ _ (cmx,_) = cmx

expandMatrix :: Constructor -> ClauseMatrix -> ClauseMatrix
expandMatrix ix (ClauseMatrix (PatternMatrix (c : cs)) as) =
  ClauseMatrix (PatternMatrix (expandColumn ix c as <> cs)) as
expandMatrix _ _ = error "Cannot expand empty matrix."

getScore :: [Clause] -> Column -> Double
getScore cs (Column m ps) = computeScore m (zip ps cs)

-- TODO: improve
computeScore :: Metadata -> [(Fix Pattern,Clause)] -> Double
computeScore _ [] = 0.0
computeScore m ((Fix (Pattern ix@(Symbol (SymbolOrAlias (Id "inj" _) _)) _ _),_):tl) = (1.0 / (fromIntegral $ length $ getInjections m ix)) + computeScore m tl
computeScore m ((Fix (Pattern ix _ _),_):tl) = (1.0 / (1.0 + (fromIntegral $ length $ getOverloads m ix))) + computeScore m tl
computeScore m ((Fix (ListPattern _ _ _ _ _),_):tl) = 1.0 + computeScore m tl
computeScore m ((Fix (As _ _ pat),c):tl) = computeScore m ((pat,c):tl)
computeScore m ((Fix Wildcard,_):tl) = min 0.0 $ computeScore m tl
computeScore m ((Fix (Variable _ _),_):tl) = min 0.0 $ computeScore m tl
computeScore m ((Fix (MapPattern [] [] Nothing _ _),_):tl) = 1.0 + computeScore m tl
computeScore m ((Fix (MapPattern [] [] (Just p) _ _),c):tl) = computeScore m ((p,c):tl)
computeScore m ((Fix (MapPattern ks vs _ e _),c):tl) = if computeScore m tl == -1.0 / 0.0 then -1.0 / 0.0 else snd $ computeMapScore m e c ks vs tl
computeScore m ((Fix (SetPattern [] Nothing _ _),_):tl) = 1.0 + computeScore m tl
computeScore m ((Fix (SetPattern [] (Just p) _ _),c):tl) = computeScore m ((p,c):tl)
computeScore m ((Fix (SetPattern es _ _ _),c):tl) = if computeScore m tl == -1.0 / 0.0 then -1.0 / 0.0 else snd $ computeSetScore c es tl

getBestKey :: Column -> [Clause] -> Maybe (Fix BoundPattern)
getBestKey (Column m (Fix (MapPattern (k:ks) vs _ e _):tl)) cs = fst $ computeMapScore m e (head cs) (k:ks) vs (zip tl $ tail cs)
getBestKey (Column _ (Fix (SetPattern (k:ks) _ _ _):tl)) cs = fst $ computeSetScore (head cs) (k:ks) (zip tl $ tail cs)
getBestKey (Column m (Fix Wildcard:tl)) cs = getBestKey (Column m tl) (tail cs)
getBestKey (Column m (Fix (Variable _ _):tl)) cs = getBestKey (Column m tl) (tail cs)
getBestKey _ _ = Nothing

computeMapScore :: Metadata -> SymbolOrAlias Object -> Clause -> [Fix Pattern] -> [Fix Pattern] -> [(Fix Pattern,Clause)] -> (Maybe (Fix BoundPattern), Double)
computeMapScore m e c ks vs tl =
  let zipped = zip ks vs
      scores = map (\(k,v) -> (if isBound getName c k then Just $ canonicalizePattern c k else Nothing,computeMapElementScore m e c tl (k,v))) zipped
  in maximumBy (comparing snd) scores

computeSetScore :: Clause -> [Fix Pattern] -> [(Fix Pattern,Clause)] -> (Maybe (Fix BoundPattern), Double)
computeSetScore c es tl =
  let scores = map (\e -> (if isBound getName c e then Just $ canonicalizePattern c e else Nothing,computeElementScore e c tl)) es
  in maximumBy (comparing snd) scores

minPositiveDouble :: Double
minPositiveDouble = encodeFloat 1 $ fst (floatRange (0.0 :: Double)) - floatDigits (0.0 :: Double)

computeMapElementScore :: Metadata -> SymbolOrAlias Object -> Clause -> [(Fix Pattern,Clause)] -> (Fix Pattern, Fix Pattern) -> Double
computeMapElementScore m e c tl (k,v) =
  let score = computeElementScore k c tl
  in if score == -1.0 / 0.0 then score else 
  let finalScore = score * computeScore (head $ fromJust $ getChildren m (HasKey False e (Ignoring m) Nothing)) [(v,c)]
  in if finalScore == 0.0 then minPositiveDouble else finalScore

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

mapPat :: Functor f => Clause -> f (Fix Pattern) -> f (Fix BoundPattern)
mapPat c' = fmap (canonicalizePattern c')

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
    mapContainsKey k' (Fix (MapPattern ks _ _ _ _)) = elem k' ks
    mapContainsKey _ _ = False
    canonicalizeClause :: Clause -> Clause
    canonicalizeClause (Clause a vars ranges children) =
      let hooks = map getHook vars
          os = map getOccurrence vars
          names = map show os
      in (Clause a (zipWith3 VariableBinding names hooks os) ranges children)

isBound :: Eq a => (VariableBinding -> a) -> Clause -> Fix (P a) -> Bool
isBound get c' (Fix (Pattern _ _ ps)) = all (isBound get c') ps
isBound get c' (Fix (ListPattern hd f tl' _ _)) = all (isBound get c') hd && all (isBound get c') tl' && maybe True (isBound get c') f
isBound get c' (Fix (MapPattern ks vs f _ _)) = all (isBound get c') ks && all (isBound get c') vs && maybe True (isBound get c') f
isBound get c' (Fix (SetPattern es f _ _)) = all (isBound get c') es && maybe True (isBound get c') f
isBound get c'@(Clause _ vars _ _) (Fix (As name _ p)) = 
  isBound get c' p && (elem name $ map get vars )
isBound _ _ (Fix Wildcard) = False
isBound get (Clause _ vars _ _) (Fix (Variable name _)) =
  elem name $ map get vars

expandColumn :: Constructor -> Column -> [Clause] -> [Column]
expandColumn ix (Column m ps) cs =
  let metas    = expandMetadata ix m
      expanded = map (expandPattern ix metas m) (zip ps cs)
      ps'' = map expandIfJust (zip expanded cs)
  in  zipWith Column metas (transpose ps'')

expandMetadata :: Constructor -> Metadata -> [Metadata]
expandMetadata ix (Metadata _ _ _ sort ms) =
  case ms ix of
    Just m -> m
    Nothing -> error $ show (ix,sort)

expandIfJust :: ([(Fix Pattern,Maybe (Constructor,Metadata))],Clause)
             -> [Fix Pattern]
expandIfJust ([],_) = []
expandIfJust ((p,Nothing):tl,c) = p : expandIfJust (tl,c)
expandIfJust ((p,Just (ix,m)):tl,c) =
  let metas = expandMetadata ix m
  in (expandIfJust (expandPattern ix metas m (p,c),c) !! 0) : expandIfJust (tl,c)

except :: Int -> [a] -> [a]
except i as =
  let (hd,tl) = splitAt i as
  in hd ++ (tail tl)

expandPattern :: Constructor
              -> [Metadata]
              -> Metadata
              -> (Fix Pattern,Clause)
              -> [(Fix Pattern,Maybe (Constructor,Metadata))]
expandPattern (List _ len) _ _ (Fix (ListPattern hd _ tl _ _), _) = zip (hd ++ (replicate (len - length hd - length tl) (Fix Wildcard)) ++ tl) (replicate len Nothing)
expandPattern _ _ _ (Fix (ListPattern _ _ _ _ _), _) = error "invalid list pattern"
expandPattern (Symbol (SymbolOrAlias name [a, _])) _ (Metadata _ _ _ _ meta) (Fix (Pattern ix@(Symbol (SymbolOrAlias (Id "inj" _) [b, _])) _ [fixedP]), _) = [(fixedP, if a == b then Nothing else Just (Symbol (SymbolOrAlias name [a,b]), (fromJust $ meta ix) !! 0))]
expandPattern (Symbol (SymbolOrAlias (Id "inj" _) _)) ms (Metadata _ _ overloads _ meta) (Fix (Pattern ix hookAtt fixedPs), cls) =
  let less = overloads ix
      childMeta = ms !! 0
      metaPs = fromJust $ meta ix
      validLess = case filter (isValidOverload fixedPs childMeta cls metaPs) less of
        [hd] -> hd
        _ -> error "invalid overload pattern"
  in [(Fix $ Pattern validLess hookAtt $ expandOverload fixedPs childMeta metaPs validLess, Nothing)]
  where
    expandOverload :: [Fix Pattern] -> Metadata -> [Metadata] -> Constructor -> [Fix Pattern]
    expandOverload ps (Metadata _ _ _ _ childMeta) metaPs less =
      let metaTs = fromJust $ childMeta less
      in zipWith3 expandOverload' ps metaPs metaTs
    expandOverload' :: Fix Pattern -> Metadata -> Metadata -> Fix Pattern
    expandOverload' p metaP metaT = 
      let sortP = getSort metaP
          sortT = getSort metaT
          child = Symbol (SymbolOrAlias (Id "inj" AstLocationNone) [sortT,sortP])
          maybeChild = if sortP == sortT then Nothing else Just (child,metaP)
      in (expandIfJust ([(p,maybeChild)],cls)) !! 0
expandPattern Empty _ _ (_, _) = []
expandPattern (NonEmpty _) _ _ (p,_) = [(p,Nothing)]
expandPattern (HasKey _ _ _ (Just p)) _ _ (m@(Fix (MapPattern ks vs f e o)),c) = 
  let canonKs = map (canonicalizePattern c) ks
      hasKey = elemIndex p canonKs
  in case hasKey of
       Just i -> [(vs !! i,Nothing), (Fix (MapPattern (except i ks) (except i vs) f e o), Nothing), (Fix Wildcard, Nothing)]
       Nothing -> [(Fix Wildcard,Nothing), (Fix Wildcard,Nothing), (m,Nothing)]
expandPattern (HasKey _ _ _ (Just p)) _ _ (m@(Fix (SetPattern es f e o)),c) = 
  let canonEs = map (canonicalizePattern c) es
      hasElem = elemIndex p canonEs
  in case hasElem of
       Just i -> [(Fix (SetPattern (except i es) f e o), Nothing), (Fix Wildcard,Nothing)]
       Nothing -> [(Fix Wildcard,Nothing), (m,Nothing)]
expandPattern (HasKey _ _ _ Nothing) _ _ _ = error "TODO: map/set choice"
expandPattern (HasNoKey _ _) _ _ (p,_) = [(p,Nothing)]
expandPattern _ _ _ ((Fix (MapPattern _ _ _ _ _)),_) = error "Invalid map pattern"
expandPattern _ _ _ ((Fix (SetPattern _ _ _ _)),_) = error "Invalid set pattern"
expandPattern _ _ _ (Fix (Pattern _ _ fixedPs), _)  = zip fixedPs $ replicate (length fixedPs) Nothing
expandPattern ix ms m (Fix (As _ _ pat), c)         = expandPattern ix ms m (pat, c)
expandPattern _ ms _ (Fix Wildcard, _)              = replicate (length ms) (Fix Wildcard,Nothing)
expandPattern _ ms _ (Fix (Variable _ _), _)        = replicate (length ms) (Fix Wildcard,Nothing)

data L a = L
           { getSpecializations :: ![(Text, a)]
           , getDefault         :: !(Maybe a)
           } deriving (Show, Eq, Functor, Ord)

type Hook = Text

data DecisionTree a = Leaf (Int, [Occurrence])
                    | Fail
                    | Switch Occurrence !(L a)
                    | SwitchLiteral Occurrence Int !(L a)
                    | CheckNull Occurrence !(L a)
                    | MakePattern Occurrence (Fix BoundPattern) !a
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
    toYaml (Anchor a (CheckNull o x)) = Y.maybeNamedMapping a
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
  toYaml (Variable (Just o) h) = Y.mapping
    ["hook" Y..= Y.toYaml (pack h)
    , "occurrence" Y..= Y.toYaml o
    ]
  toYaml (Variable Nothing _) = error "Unsupported map/set pattern"
  toYaml (As _ _ p) = Y.toYaml p
  toYaml (MapPattern _ _ _ _ o) = Y.toYaml o
  toYaml (SetPattern _ _ _ o) = Y.toYaml o
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
getLeaf :: Fringe -> [Fix Pattern] -> Clause -> Fix DecisionTree -> Fix DecisionTree
getLeaf os ps (Clause (Action a rhsVars maybeSideCondition) matchedVars ranges children) next =
  let row = zip os ps
      -- first, add all remaining variable bindings to the clause
      vars = nub $ foldr (\((o,_), p) -> (addVarToRow Nothing o p)) matchedVars row
      (_, overloadVars) = unzip children
      allVars = vars ++ overloadVars
      -- then group the bound variables by their name
      grouped = foldr (\(VariableBinding name hookAtt o) -> \m -> Map.insert (name,hookAtt) (o : Map.findWithDefault [] (name,hookAtt) m) m) Map.empty allVars
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
      (nonlinearLeaf) = Map.foldrWithKey (\(_, hookAtt) -> flip $ foldr $ makeEquality hookAtt) atomicLeaf nonlinearPairs
      sc = case maybeSideCondition of
             -- if there is no side condition, continue
             Nothing -> nonlinearLeaf
                          -- filter by the variables used in the side condition
             Just cond -> let condFiltered = filter (flip elem cond . fst) sorted
                              (_, condVars) = unzip condFiltered
                              newO = [pack $ "side_condition_" ++ (show a)]
                          -- evaluate the side condition and if it is true, continue, otherwise go to the next row
                          in function (pack $ "side_condition_" ++ (show a)) newO condVars "BOOL.Bool" (
                                 switchLiteral newO 1 [("1", nonlinearLeaf),
                                                       ("0", next)
                                                      ] Nothing)
      -- fill out the bindings for list range variables
      withRanges = foldr (\(o, hd, tl) -> Fix . Function "hook_LIST_range_long" o [tail o, ["lit", (pack $ show hd), "MINT.MInt 64"], ["lit", (pack $ show tl), "MINT.MInt 64"]] "LIST.List") sc ranges
  in foldr (\(inj, VariableBinding _ h o) -> Fix . MakePattern o (Fix $ Pattern inj Nothing [Fix $ Variable (Just $ tail o) h])) withRanges children
  where
    makeEquality :: String -> (Occurrence, Occurrence) -> Fix DecisionTree -> Fix DecisionTree
    -- compare the values of the variables with the same name and if they are equal, continue, otherwise go to the next row
    makeEquality hookAtt (o1,o2) dt = function (equalityFun hookAtt) ("eq" : (o1 ++ ["and"] ++ o2)) [o1, o2] "BOOL.Bool" (switchLiteral ("eq" : (o1 ++ ["and"] ++ o2)) 1 [("1", dt), ("0", next)] Nothing)

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

getRealScore :: ClauseMatrix -> Column -> Double
getRealScore (ClauseMatrix (PatternMatrix cs) as) c =
  let score = getScore as c in
  if score /= 0.0 then score else
  let unboundMapColumns = filter ((== (-1.0 / 0.0)) . getScore as) cs
      patterns = transpose $ map getTerms unboundMapColumns
      keys = map (concatMap getMapOrSetKeys) patterns
      vars = map (concatMap getVariables) keys
      boundVars = map getVariables $ getTerms c
      intersection = zipWith intersect vars boundVars
      needed = any (not . null) intersection
  in if needed then minPositiveDouble else 0.0
  where
    getMapOrSetKeys :: Fix Pattern -> [Fix Pattern]
    getMapOrSetKeys (Fix (SetPattern ks _ _ _)) = ks
    getMapOrSetKeys (Fix (MapPattern ks _ _ _ _)) = ks
    getMapOrSetKeys _ = []
    getVariables :: Fix Pattern -> [String]
    getVariables (Fix (Variable name _)) = [name]
    getVariables (Fix (As name _ p)) = name : getVariables p
    getVariables (Fix (Pattern _ _ ps)) = concatMap getVariables ps
    getVariables (Fix (ListPattern _ _ _ _ o)) = getVariables o
    getVariables (Fix (MapPattern _ _ _ _ o)) = getVariables o
    getVariables (Fix (SetPattern _ _ _ o)) = getVariables o
    getVariables (Fix Wildcard) = []

compilePattern :: (ClauseMatrix, Fringe) -> (Fix DecisionTree)
compilePattern firstCm =
  compilePattern' firstCm
  where
    compilePattern' :: (ClauseMatrix, Fringe) -> (Fix DecisionTree)
    compilePattern' (cm@(ClauseMatrix pm@(PatternMatrix cs) ac), os) = 
      case ac of
        [] -> Fix Fail
        hd:tl -> 
          if isWildcardRow pm then
            -- if there is only one row left, then try to match it and fail the matching if it fails
            -- otherwise, if it fails, try to match the remainder of hte matrix
            if length ac == 1 then getLeaf os (firstRow pm) hd failure else getLeaf os (firstRow pm) hd (compilePattern' ((ClauseMatrix (notFirstRow pm) tl), os))
          else 
          -- compute the column with the best score, choosing the first such column if they are equal
          let bestColIx = fst $ maximumBy (comparing (getRealScore cm . snd)) $ reverse $ indexed cs
              -- swap the columns and occurrences so that the best column is first
              cs' = swapAt 0 bestColIx cs
              os' = swapAt 0 bestColIx os
              pm' = PatternMatrix cs'
              cm' = (ClauseMatrix pm' ac,os')
              -- compute the hook attribute of the new first column
              hookAtt = hook pm'
              -- c9ompute the signature of the new first column
              s₁ = sigma₁ (snd $ head os') $ fst cm'
              -- specialize on each constructor in the signature
              ls = map (\c -> mSpecialize c cm') s₁
              -- compute the default matrix if it exists
              d  = mDefault s₁ cm'
              dt = case hookAtt of
                -- not matching a builtin, therefore construct a regular switch
                -- that matches the tag of the block.
                Nothing -> Fix $ Switch (fst $ head os') L
                    { getSpecializations = map (second compilePattern') ls
                    , getDefault = compilePattern' <$> d
                    }
                -- matching a bool, so match the integer value of the bool with a bitwidth of 1
                Just "BOOL.Bool" -> Fix $ SwitchLiteral (fst $ head os') 1 L
                    { getSpecializations = map (second compilePattern') ls
                    , getDefault = compilePattern' <$> d
                    }
                -- matching a list, so construct a node to decompose the list into its elements
                Just "LIST.List" -> listPattern (fst $ head os') ls d s₁ (head cs')
                -- matching a map, so construct a node to decompose the map by one of its elements
                Just "MAP.Map" -> mapPattern (fst $ head os') ls d s₁ (head cs') ac
                -- matching a set, so construct a node to decompose the set by one of its elements
                Just "SET.Set" -> setPattern (fst $ head os') ls d s₁ (head cs') ac
                -- matching an mint, so match the integer value of the mint with the specified bitwidth
                Just ('M':'I':'N':'T':'.':'M':'I':'n':'t':' ':bw) -> Fix $ SwitchLiteral (fst $ head os') (read bw) L
                    { getSpecializations = map (second compilePattern') ls
                    , getDefault = compilePattern' <$> d
                    }
                -- matching a string or int, so compare the value of the token against a list of constants
                Just hookName -> equalLiteral (fst $ head os') hookName ls d
          -- if necessary, generate a swap node
          in dt
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
    isWildcard (Fix (SetPattern [] (Just p) _ _)) = isWildcard p
    isWildcard (Fix (SetPattern _ _ _ _)) = False
    isWildcard (Fix (ListPattern _ _ _ _ _)) = False
    -- constructs a tree to test the current occurrence against each possible match in turn
    equalLiteral :: Occurrence -> String -> [(Text, (ClauseMatrix, Fringe))] -> Maybe (ClauseMatrix, Fringe) -> Fix DecisionTree
    -- if no specializations remain and a default exists, consume the occurrence and continue with the default
    equalLiteral litO _ [] (Just d) = Fix $ Switch litO L { getSpecializations = [], getDefault = Just $ compilePattern' d }
    -- if no specializations remain and no default exists, fail the match
    equalLiteral _ _ [] (Nothing) = Fix Fail
    -- consume each specialization one at a time and try to match it
    -- if it succeseds, consume the occurrence and continue with the specialized matrix
    -- otherweise, test the next literal
    equalLiteral litO hookName ((name,spec):tl) d =
      let newO = ["lit", name, pack hookName]
          eqO = "eq" : (litO ++ ["and"] ++ newO)
      in Fix $ MakePattern newO (Fix (Pattern (Literal (unpack name)) (Just hookName) [])) $ 
             Fix $ Function (equalityFun hookName) eqO [litO, newO] "BOOL.Bool" $
                 Fix $ SwitchLiteral eqO 1 $ L [("1", Fix $ Switch litO L 
                                                             { getSpecializations = [], getDefault = Just $ compilePattern' spec }),
                                                 ("0", equalLiteral litO hookName tl d)
                                                ] Nothing
    -- construct a tree to test the length of the list and bind the elements of the list to their occurrences
    listPattern :: Occurrence -> [(Text, (ClauseMatrix, Fringe))] -> Maybe (ClauseMatrix, Fringe) -> [Constructor] -> Column -> Fix DecisionTree
    listPattern listO ls d signature firstCol =
      let newO = "size" : listO
          (cons,matrices) = unzip ls
          specs = zip cons $ zip signature matrices
          maxList = maxListSize firstCol
      -- test the length of the list against the specializations of the matrix
      -- if it succeeds, bind the occurrences and continue with the specialized matrix
      -- otherwise, try the default case
      in Fix $ Function "hook_LIST_size_long" newO [listO] "MINT.MInt 64" $
           Fix $ SwitchLiteral newO 64 $ L 
             { getSpecializations = map (second (expandListPattern listO)) specs
             , getDefault = expandListPatternDefault listO maxList <$> d
             }
    -- get each element of the list specified in the list pattern and bind it to the occurrences,
    -- then compile the remaining matrix
    expandListPattern :: Occurrence -> (Constructor, (ClauseMatrix, Fringe)) -> Fix DecisionTree
    expandListPattern listO ((List _ i),cm') =
      foldl (listGet listO Nothing) (compilePattern' cm') [0..i-1]
    expandListPattern _ _ = error "invalid list pattern"
    -- get each element of the list and bind it to the occurrence, then compile the default matrix
    expandListPatternDefault :: Occurrence -> (Int,Int) -> (ClauseMatrix, Fringe) -> Fix DecisionTree
    expandListPatternDefault listO (hd,tl) cm' =
      foldl (listGet listO $ Just (hd,tl)) (foldl (listGet listO Nothing) (compilePattern' cm') [0..hd-1]) [hd..hd+tl-1]
    -- generate a single list lookup operation to bind one element of the list against its occurrence
    listGet :: Occurrence -> Maybe (Int, Int) -> Fix DecisionTree -> Int -> Fix DecisionTree
    listGet _ _ (Fix Fail) _ = Fix Fail
    listGet listO l dt o = 
      Fix $ Function "hook_LIST_get_long" ((pack $ show o) : listO) 
        [listO, case l of
                  Nothing -> ["lit", pack $ show o, "MINT.MInt 64"]
                  Just (hd,tl) -> ["lit", pack $ show (o-tl-hd), "MINT.MInt 64"]
        ] "STRING.String" dt
    mapPattern :: Occurrence -> [(Text, (ClauseMatrix, Fringe))] -> Maybe (ClauseMatrix, Fringe) -> [Constructor] -> Column -> [Clause] -> Fix DecisionTree
    mapPattern mapO ls d s c cs =
      let newO = "size" : mapO
      -- if Empty is in the signature, test whether the map is empty or not.
      in if elem Empty s then
        Fix $ Function "hook_MAP_size_long" newO [mapO] "MINT.MInt 64" $
          Fix $ SwitchLiteral newO 64 $ L
            { getSpecializations = map (second compilePattern') ls
            , getDefault = compilePattern' <$> d
            }
      else
        -- otherwise, get the best key and test whether the best key is in the map or not
        let key = getBestKey c cs
        in case key of
             Nothing -> Fix $ Switch mapO $ L
               { getSpecializations = map (second compilePattern') ls
               , getDefault = compilePattern' <$> d
               }
             Just k -> Fix $ MakePattern newO k $ 
                         Fix $ Function "hook_MAP_lookup_null" ((pack (show k ++ "_val")) : mapO) [mapO, newO] "STRING.String" $
                           Fix $ Function "hook_MAP_remove" ((pack (show k ++ "_rem")) : mapO) [mapO, newO] "MAP.Map" $
                             Fix $ CheckNull ((pack (show k ++ "_val")) : mapO) $ L
                               { getSpecializations = map (second compilePattern') ls
                               , getDefault = compilePattern' <$> d
                               }
    setPattern :: Occurrence -> [(Text, (ClauseMatrix, Fringe))] -> Maybe (ClauseMatrix, Fringe) -> [Constructor] -> Column -> [Clause] -> Fix DecisionTree
    setPattern setO ls d s c cs =
      let newO = "size" : setO
      -- if Empty is in the signature, test whether the set is empty or not.
      in if elem Empty s then
        Fix $ Function "hook_SET_size_long" newO [setO] "MINT.MInt 64" $
          Fix $ SwitchLiteral newO 64 $ L
            { getSpecializations = map (second compilePattern') ls
            , getDefault = compilePattern' <$> d
            }
      else
        -- otherwise, get the best element and test whether the best element is in the set or not
        let key = getBestKey c cs
        in case key of
             Nothing -> Fix $ Switch setO $ L
               { getSpecializations = map (second compilePattern') ls
               , getDefault = compilePattern' <$> d
               }
             Just k -> Fix $ MakePattern newO k $
                         Fix $ Function "hook_SET_in" ((pack (show k ++ "_val")) : setO) [newO, setO] "BOOL.Bool" $
                           Fix $ Function "hook_SET_remove" ((pack (show k ++ "_rem")) : setO) [setO, newO] "SET.Set" $
                             Fix $ SwitchLiteral ((pack (show k ++ "_val")) : setO) 1 $ L
                               { getSpecializations = map (second compilePattern') ls
                               , getDefault = compilePattern' <$> d
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
                        Fix (MakePattern o p a) -> let (m',child) = mapChild a in (addName m', Free (Anchor (Just $ name m') (MakePattern o p child)))
                        Fix (Function n o os s a) -> let (m',child) = mapChild a in (addName m', Free (Anchor (Just $ name m') (Function n o os s child)))
                        Fix (Switch o (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (Switch o (L s' d'))))
                        Fix (CheckNull o (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (CheckNull o (L s' d'))))
                        Fix (SwitchLiteral o bw (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (SwitchLiteral o bw (L s' d'))))
                      
                        
    mapSpec :: Map.Map (Fix DecisionTree) Alias -> [(Text,Fix DecisionTree)] -> (Map.Map (Fix DecisionTree) Alias,[(Text,Free Anchor Alias)])
    mapSpec m s =
      let (ts,as) = unzip s
          (m',as') = mapAccumL computeSharing m as
      in (m',zip ts as')
