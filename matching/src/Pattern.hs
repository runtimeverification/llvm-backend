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
               , Pattern(..)
               , Clause(..)
               , Action(..)
               , Occurrence
               , Index
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
import           Data.Deriving         (deriveOrd1, deriveShow1, deriveEq1)
import           Data.Function         (on)
import           Data.Functor.Foldable (Fix (..))
import           Data.List             (transpose,nub,sortBy,maximumBy)
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

data Constructor = Symbol (SymbolOrAlias Object)
                 | Literal String
                 | List (SymbolOrAlias Object) Int
                 deriving (Show, Eq, Ord)

type Index       = Int
data Pattern a   = Pattern Constructor (Maybe String) ![a]
                 | ListPattern
                   { getHead :: ![a]
                   , getFrame :: Maybe a
                   , getTail :: [a]
                   , element :: SymbolOrAlias Object
                   }
                 | As String a
                 | Wildcard
                 | Variable String
                 deriving (Show, Eq, Functor)

newtype PatternMatrix = PatternMatrix [Column]
                        deriving (Show)

type Occurrence   = [Int]
data Action       = Action
                    { getRuleNumber :: Int
                    , getRhsVars :: [String]
                    , getSideConditionVars :: Maybe [String]
                    }
                    deriving (Show)

data Clause       = Clause
                    { getAction :: Action
                    , getVariableBindings :: [(String, Occurrence)]
                    , getListRanges :: [(Occurrence, Int, Int)]
                    }
                    deriving (Show)

data ClauseMatrix = ClauseMatrix PatternMatrix ![Clause]
                    deriving (Show)

instance (Show Metadata) where
  show (Metadata _ _ sort _) = show sort

$(deriveEq1 ''Pattern)
$(deriveShow1 ''Pattern)

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
         -> Text
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

sigma :: Column -> [Constructor]
sigma c =
  let used = concatMap ix $ getTerms c
      inj = nub $ filter isInj used
      usedInjs = nub $ concatMap (getInjections $ getMetadata c) inj
      dups = used ++ usedInjs
  in nub dups
  where
    ix :: Fix Pattern -> [Constructor]
    ix (Fix (Pattern ix' _ _)) = [ix']
    ix (Fix (ListPattern hd Nothing tl s)) = [List s (length hd + length tl)]
    ix (Fix (ListPattern hd (Just _) tl s)) = map (List s) [0..(length hd + length tl)]
    ix (Fix (As _ pat))        = ix pat
    ix (Fix Wildcard)          = []
    ix (Fix (Variable _))      = []
    isInj :: Constructor -> Bool
    isInj (Symbol (SymbolOrAlias (Id "inj" _) _)) = True
    isInj _ = False
   

sigma₁ :: PatternMatrix -> [Constructor]
sigma₁ (PatternMatrix (c : _)) = sigma c
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
    ix (Fix (ListPattern _ _ _ _)) = Just "LIST.List"
    ix (Fix (As _ pat))        = ix pat
    ix (Fix Wildcard)          = Nothing
    ix (Fix (Variable _))      = Nothing
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

mSpecialize _ _ = error "must have at least one column"

expandOccurrence :: ClauseMatrix -> Occurrence -> Constructor -> [Occurrence]
expandOccurrence (ClauseMatrix (PatternMatrix (c : _)) _) o ix =
  let (Metadata _ _ _ mtd) = getMetadata c
      a = length $ fromJust $ mtd ix
  in map (\i -> i : o) [0..a-1]
expandOccurrence _ _ _ = error "must have at least one column"

mDefault :: (ClauseMatrix, [Occurrence]) -> Maybe (ClauseMatrix, [Occurrence])
mDefault (cm@(ClauseMatrix pm@(PatternMatrix (c : _)) _),o : os) =
  let (Metadata mtd _ _ _) = getMetadata c
      s₁ = sigma c
      infiniteLength = case hook pm of
        Nothing -> False
        Just "LIST.List" -> True
        Just "INT.Int" -> True
        Just "STRING.String" -> True
        Just _ -> False
  in  if infiniteLength || null s₁ || (toInteger $ length s₁) /= mtd
      then Just ((expandDefault (fromJust $ getMaxList c) (filterMatrix (getMaxList c) isDefault (cm,o))),expandDefaultOccurrence cm o <> os)
      else Nothing
mDefault _ = Nothing

maxListSize :: Column -> (Int, Int)
maxListSize c =
  let longestHead = maximum $ map getListHeadSize $ getTerms c
      longestTail = maximum $ map getListTailSize $ getTerms c
  in (longestHead, longestTail)
  where
    getListHeadSize :: Fix Pattern -> (Int)
    getListHeadSize (Fix (ListPattern hd _ _ _)) = length hd
    getListHeadSize (Fix (Variable _)) = -1
    getListHeadSize (Fix Wildcard) = -1
    getListHeadSize _ = error "unsupported list pattern"
    getListTailSize :: Fix Pattern -> (Int)
    getListTailSize (Fix (ListPattern _ _ tl _)) = length tl
    getListTailSize (Fix (Variable _)) = -1
    getListTailSize (Fix Wildcard) = -1
    getListTailSize _ = error "unsupported list pattern"

getMaxList :: Column -> Maybe Constructor
getMaxList c = 
  let list = head $ sigma c
      (hd,tl) = maxListSize c
  in case list of 
       List s _ -> Just $ List s $ (hd + tl)
       _ -> Nothing

expandDefault :: Constructor -> ClauseMatrix -> ClauseMatrix
expandDefault maxList (ClauseMatrix pm@(PatternMatrix (c : cs)) as) =
  case hook pm of
    Nothing -> ClauseMatrix (PatternMatrix cs) as
    Just "LIST.List" -> 
      ClauseMatrix (PatternMatrix $ expandColumn maxList c <> cs) as
    Just _ -> ClauseMatrix (PatternMatrix cs) as
expandDefault _ _ = error "must have at least one column"

expandDefaultOccurrence :: ClauseMatrix -> Occurrence -> [Occurrence]
expandDefaultOccurrence cm@(ClauseMatrix pm@(PatternMatrix (c : _)) _) o =
  case hook pm of
    Nothing -> []
    Just "LIST.List" ->
      let maxList = fromJust $ getMaxList c
      in expandOccurrence cm o maxList
    Just _ -> []
expandDefaultOccurrence _ _ = error "must have at least one column"

firstRow :: PatternMatrix -> [Fix Pattern]
firstRow (PatternMatrix cs) =
  map (\(Column _ _ (p : _)) -> p) cs
notFirstRow :: PatternMatrix -> PatternMatrix
notFirstRow (PatternMatrix cs) =
  PatternMatrix (map (\(Column m _ (_ : ps)) -> mkColumn m ps) cs)

filterByList :: [Bool] -> [a] -> [a]
filterByList (True  : bs) (x : xs) = x : filterByList bs xs
filterByList (False : bs) (_ : xs) = filterByList bs xs
filterByList _ _                   = []

isDefault :: Fix Pattern -> Bool
isDefault (Fix (Pattern _ _ _)) = False
isDefault (Fix (ListPattern _ Nothing _ _)) = False
isDefault (Fix (ListPattern _ (Just _) _ _)) = True
isDefault (Fix (As _ pat)) = isDefault pat
isDefault (Fix Wildcard) = True
isDefault (Fix (Variable _)) = True

checkPatternIndex :: Constructor -> Metadata -> Fix Pattern -> Bool
checkPatternIndex _ _ (Fix Wildcard) = True
checkPatternIndex ix m (Fix (As _ pat)) = checkPatternIndex ix m pat
checkPatternIndex _ _ (Fix (Variable _)) = True
checkPatternIndex (List _ len) _ (Fix (ListPattern hd Nothing tl _)) = len == (length hd + length tl)
checkPatternIndex (List _ len) _ (Fix (ListPattern hd (Just _) tl _)) = len >= (length hd + length tl)
checkPatternIndex _ _ (Fix (ListPattern _ _ _ _)) = False
checkPatternIndex (Symbol (SymbolOrAlias (Id "inj" _) [a,c])) (Metadata _ _ _ meta) (Fix (Pattern ix@(Symbol (SymbolOrAlias name@(Id "inj" _) [b,c'])) _ [p])) =
  let m@(Metadata _ _ _ childMeta) = (fromJust $ meta ix) !! 0
      child = Symbol (SymbolOrAlias name [a,b])
  in c == c' && (a == b || ((isJust $ childMeta $ child) && checkPatternIndex child m p))
checkPatternIndex ix _ (Fix (Pattern ix' _ _)) = ix == ix'

addVars :: Maybe Constructor -> [Clause] -> [Fix Pattern] -> Occurrence -> [Clause]
addVars ix as c o =
  let rows = zip c as
  in map (\(p, (Clause a vars ranges)) -> (Clause a (addVarToRow ix o vars p) $ addRange ix o ranges p)) rows

addVarToRow :: Maybe Constructor -> Occurrence -> [(String, Occurrence)] -> Fix Pattern -> [(String, Occurrence)]
addVarToRow _ o vars (Fix (Variable name)) = (name, o) : vars
addVarToRow _ o vars (Fix (As name _)) = (name, o) : vars
addVarToRow _ _ vars (Fix Wildcard) = vars
addVarToRow (Just (Symbol (SymbolOrAlias (Id "inj" _) [a,_]))) o vars (Fix (Pattern (Symbol (SymbolOrAlias (Id "inj" _) [b,_])) _ [Fix (Variable name)])) = if a == b then vars else (name, o) : vars
addVarToRow _ _ vars (Fix (Pattern _ _ _)) = vars
addVarToRow _ _ vars (Fix (ListPattern _ Nothing _ _)) = vars
addVarToRow (Just (List _ len)) o vars (Fix (ListPattern _ (Just (Fix (Variable name))) _ _)) = (name, len : o) : vars
addVarToRow _ _ vars (Fix (ListPattern _ (Just _) _ _)) = vars

addRange :: Maybe Constructor -> Occurrence -> [(Occurrence, Int, Int)] -> Fix Pattern -> [(Occurrence, Int, Int)]
addRange (Just (List _ len)) o ranges (Fix (ListPattern hd (Just _) tl _)) = (len : o, length hd, length tl) : ranges
addRange _ _ ranges _ = ranges

filterMatrix :: Maybe Constructor -> (Fix Pattern -> Bool) -> (ClauseMatrix, Occurrence) -> ClauseMatrix
filterMatrix ix checkPattern ((ClauseMatrix (PatternMatrix cs@(c : _)) as), o) =
  let filteredRows = map checkPattern (getTerms c)
      newCs = map (filterRows filteredRows) cs
      varsAs = addVars ix as (getTerms c) o
      newAs = filterByList filteredRows varsAs
  in ClauseMatrix (PatternMatrix newCs) newAs
  where
    filterRows :: [Bool] -> Column -> Column
    filterRows fr (Column md _ rs) =
      mkColumn md (filterByList fr rs)
filterMatrix _ _ (cmx,_) = cmx

expandMatrix :: Constructor -> ClauseMatrix -> ClauseMatrix
expandMatrix ix (ClauseMatrix (PatternMatrix (c : cs)) as) =
  ClauseMatrix (PatternMatrix (expandColumn ix c <> cs)) as
expandMatrix _ _ = error "Cannot expand empty matrix."

-- TODO: improve
computeScore :: Metadata -> [Fix Pattern] -> Double
computeScore _ [] = 0.0
computeScore m (Fix (Pattern ix@(Symbol (SymbolOrAlias (Id "inj" _) _)) _ _):tl) = (1.0 / (fromIntegral $ length $ getInjections m ix)) + computeScore m tl
computeScore m (Fix (Pattern _ _ _):tl) = 1.0 + computeScore m tl
computeScore m (Fix (ListPattern _ _ _ _):tl) = 1.0 + computeScore m tl
computeScore m (Fix (As _ pat):tl) = computeScore m (pat:tl)
computeScore _ (Fix Wildcard:_) = 0.0
computeScore _ (Fix (Variable _):_) = 0.0

mkColumn :: Metadata -> [Fix Pattern] -> Column
mkColumn m ps = Column m (computeScore m ps) ps

expandColumn :: Constructor -> Column -> [Column]
expandColumn ix (Column m _ ps) =
  let metas    = expandMetadata ix m
      expanded = map (expandPattern ix metas) ps
      ps'' = map (expandIfJust ix metas) expanded
  in  zipWith mkColumn metas (transpose ps'')

expandMetadata :: Constructor -> Metadata -> [Metadata]
expandMetadata ix (Metadata _ _ sort ms) =
  case ms ix of
    Just m -> m
    Nothing -> error $ show (ix,sort)

expandIfJust :: Constructor
             -> [Metadata]
             -> ([Fix Pattern],Maybe Constructor)
             -> [Fix Pattern]
expandIfJust _ _ (p,Nothing) = p
expandIfJust _ ms ([p],Just ix) =
  fst $ expandPattern ix ms p
expandIfJust _ _ (_,Just _) = error "invalid injection"

expandPattern :: Constructor
              -> [Metadata]
              -> Fix Pattern
              -> ([Fix Pattern],Maybe Constructor)
expandPattern (List _ len) _ (Fix (ListPattern hd _ tl _)) = (hd ++ (replicate (len - length hd - length tl) (Fix Wildcard)) ++ tl, Nothing)
expandPattern _ _ (Fix (ListPattern _ _ _ _)) = error "invalid list pattern"
expandPattern (Symbol (SymbolOrAlias name [a, _])) _ (Fix (Pattern (Symbol (SymbolOrAlias (Id "inj" _) [b, _])) _ [fixedP])) = ([fixedP], if a == b then Nothing else Just (Symbol (SymbolOrAlias name [a,b])))
expandPattern _ _ (Fix (Pattern _ _ fixedPs))  = (fixedPs,Nothing)
expandPattern ix ms (Fix (As _ pat))           = expandPattern ix ms pat
expandPattern _ ms (Fix Wildcard)              = (replicate (length ms) (Fix Wildcard), Nothing)
expandPattern _ ms (Fix (Variable _))          = (replicate (length ms) (Fix Wildcard), Nothing)

data L a = L
           { getSpecializations :: ![(Text, a)]
           , getDefault         :: !(Maybe a)
           } deriving (Show, Eq, Functor, Ord)

data DecisionTree a = Leaf (Int, [Occurrence])
                    | Fail
                    | Switch Occurrence !(L a)
                    | SwitchLiteral Occurrence Int !(L a)
                    | EqualLiteral Text Occurrence Text !a
                    | Swap Index !a
                    | Function Text Occurrence [Occurrence] Text !a  
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
    toYaml (Anchor a (Leaf (act, x))) = Y.namedMapping a [
        "action" Y..= Y.array [Y.toYaml act, Y.toYaml x]
      ]
    toYaml (Anchor a Fail) = Y.namedString a "fail"
    toYaml (Anchor a (Switch o x)) = Y.namedMapping a
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just i -> Y.toYaml i
                                    Nothing -> Y.null
                                )
      , "occurrence" Y..= Y.toYaml o
      ]
    toYaml (Anchor a (SwitchLiteral o i x)) = Y.namedMapping a
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just d -> Y.toYaml d
                                    Nothing -> Y.null
                                )
      , "bitwidth" Y..= Y.toYaml i
      , "occurrence" Y..= Y.toYaml o
      ]
    toYaml (Anchor a (EqualLiteral h o l x)) = Y.namedMapping a
      ["hook" Y..= Y.toYaml h
      , "occurrence" Y..= Y.toYaml o
      , "literal" Y..= Y.toYaml l
      , "next" Y..= Y.toYaml x
      ]
    toYaml (Anchor a (Swap i x)) = Y.namedMapping a
      ["swap" Y..= Y.array [Y.toYaml i, Y.toYaml x]]
    toYaml (Anchor a (Function name o bindings sort x)) = Y.namedMapping a
      ["function" Y..= Y.toYaml name
      , "sort" Y..= Y.toYaml sort
      , "occurrence" Y..= Y.toYaml o
      , "args" Y..= Y.toYaml bindings
      , "next" Y..= Y.toYaml x
      ]

instance Y.ToYaml Alias where
    toYaml (Alias name) = Y.alias name

serializeToYaml :: (Free Anchor Alias) -> B.ByteString
serializeToYaml = Y.toByteString . Y.toYaml

instance Y.ToYaml (Anchor (Free Anchor Alias)) => Y.ToYaml (Free Anchor Alias) where
    toYaml (Pure a) = Y.toYaml a
    toYaml (Free f) = Y.toYaml f

getLeaf :: Int -> [Occurrence] -> [Fix Pattern] -> Clause -> Fix DecisionTree -> Fix DecisionTree
getLeaf ix os ps (Clause (Action a rhsVars maybeSideCondition) matchedVars ranges) next =
  let row = zip os ps
      vars = foldr (\(o, p) -> \accum -> (addVarToRow Nothing o accum p)) matchedVars row
      sorted = sortBy (compare `on` fst) vars
      filtered = filter (flip elem rhsVars . fst) sorted
      (_, newVars) = unzip filtered
      l = case maybeSideCondition of
            Nothing -> Fix $ Leaf (a, newVars)
            Just cond -> let condFiltered = filter (flip elem cond . fst) sorted
                             (_, condVars) = unzip condFiltered
                         in function (pack $ "side_condition_" ++ (show a)) [ix, 0] condVars "BOOL.Bool" (switchLiteral [ix, 0] 1 [("1", (leaf a newVars)), ("0", next)] Nothing)
  in foldr (\(o, hd, tl) -> Fix . Function "hook_LIST_range_long" o [tail o, [hd, -1], [tl, -1]] "LIST.List") l ranges

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
compilePattern cm =
  compilePattern' 0 cm
  where
    compilePattern' :: Int -> (ClauseMatrix, [Occurrence]) -> (Fix DecisionTree)
    compilePattern' ix ((ClauseMatrix pm@(PatternMatrix cs) ac), os) = 
      case ac of
        [] -> Fix Fail
        hd:tl -> 
          if isWildcardRow pm then
          let (Clause (Action _ _ maybeSideCondition) _ _) = hd
              ix' = if isJust maybeSideCondition then ix+1 else ix
          in if length ac == 1 then getLeaf ix os (firstRow pm) hd failure else getLeaf ix os (firstRow pm) hd (compilePattern' ix' ((ClauseMatrix (notFirstRow pm) tl), os))
          else 
          let bestCol = maximumBy (comparing (getScore . snd)) (indexed cs)
              --if the first column is equal to bestCol then choose it instead so we don't generate a swap
              bestColIx = if (getScore $ snd bestCol) == (getScore $ head cs) then 0 else fst bestCol
              cs' = swapAt 0 bestColIx cs
              os' = swapAt 0 bestColIx os
              pm' = PatternMatrix cs'
              cm' = (ClauseMatrix pm' ac,os')
              hookAtt = hook pm'
              s₁ = sigma₁ pm'
              ls = map (`mSpecialize` cm') s₁
              d  = mDefault cm'
              dt = case hookAtt of
                Nothing -> Fix $ Switch (head os') L
                    { getSpecializations = map (second (compilePattern' ix)) ls
                    , getDefault = compilePattern' ix <$> d
                    }
                Just "BOOL.Bool" -> Fix $ SwitchLiteral (head os') 1 L
                    { getSpecializations = map (second (compilePattern' ix)) ls
                    , getDefault = compilePattern' ix <$> d
                    }
                Just "LIST.List" -> listPattern ix os' ls d s₁ (head cs)
                Just ('M':'I':'N':'T':'.':'M':'I':'n':'t':' ':bw) -> Fix $ SwitchLiteral (head os') (read bw) L
                    { getSpecializations = map (second (compilePattern' ix)) ls
                    , getDefault = compilePattern' ix <$> d
                    }
                Just hookName -> equalLiteral ix os' hookName ls d
          in if bestColIx == 0 then dt else Fix $ Swap bestColIx dt
    isWildcardRow :: PatternMatrix -> Bool
    isWildcardRow = and . map isWildcard . firstRow
    isWildcard :: Fix Pattern -> Bool
    isWildcard (Fix Wildcard) = True
    isWildcard (Fix (Variable _)) = True
    isWildcard (Fix (As _ pat)) = isWildcard pat
    isWildcard (Fix (Pattern _ _ _)) = False
    isWildcard (Fix (ListPattern _ _ _ _)) = False
    equalLiteral :: Int -> [Occurrence] -> String -> [(Text, (ClauseMatrix, [Occurrence]))] -> Maybe (ClauseMatrix, [Occurrence]) -> Fix DecisionTree
    equalLiteral o os _ [] (Just d) = Fix $ Switch (head os) L { getSpecializations = [], getDefault = Just $ compilePattern' o d }
    equalLiteral _ _ _ [] (Nothing) = Fix Fail
    equalLiteral o os hookName ((name,spec):tl) d = Fix $ EqualLiteral (pack hookName) [o, 0] name $ Fix $ SwitchLiteral [o, 0] 1 $ L [("1", Fix $ Switch (head os) L { getSpecializations = [], getDefault = Just $ compilePattern' (o+1) spec }),("0", equalLiteral (o+1) os hookName tl d)] Nothing
    listPattern :: Int -> [Occurrence] -> [(Text, (ClauseMatrix, [Occurrence]))] -> Maybe (ClauseMatrix, [Occurrence]) -> [Constructor] -> Column -> Fix DecisionTree
    listPattern o os ls d s c =
      let (cons,matrices) = unzip ls
          specs = zip cons $ zip s matrices
          maxList = maxListSize c
      in Fix $ Function "hook_LIST_size_long" [o, 0] [head os] "MINT.MInt 64" $
           Fix $ SwitchLiteral [o, 0] 64 $ L 
             { getSpecializations = map (second (expandListPattern (o+1) (head os))) specs
             , getDefault = expandListPatternDefault (o+1) (head os) maxList <$> d
             }
    expandListPattern :: Int -> Occurrence -> (Constructor, (ClauseMatrix, [Occurrence])) -> Fix DecisionTree
    expandListPattern nextO listO ((List _ i),cm') =
      foldl (listGet listO Nothing) (compilePattern' nextO cm') [0..i-1]
    expandListPattern _ _ _ = error "invalid list pattern"
    expandListPatternDefault :: Int -> Occurrence -> (Int,Int) -> (ClauseMatrix, [Occurrence]) -> Fix DecisionTree
    expandListPatternDefault nextO listO (hd,tl) cm' =
      foldl (listGet listO $ Just (hd,tl)) (foldl (listGet listO Nothing) (compilePattern' nextO cm') [0..hd-1]) [hd..hd+tl-1]
    listGet :: Occurrence -> Maybe (Int, Int) -> Fix DecisionTree -> Int -> Fix DecisionTree
    listGet listO l dt o = 
      Fix $ Function "hook_LIST_get_long" (o : listO) 
        [listO, case l of
                  Nothing -> [o, -1]
                  Just (hd,tl) -> [o-tl-hd, -1]
        ] "STRING.String" dt


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
                        Fix (EqualLiteral h o l a) -> let (m',child) = mapChild a in (addName m',Free (Anchor (Just $ name m') (EqualLiteral h o l child)))
                        Fix (Function n o os s a) -> let (m',child) = mapChild a in (addName m', Free (Anchor (Just $ name m') (Function n o os s child)))
                        Fix (Switch o (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (Switch o (L s' d'))))
                        Fix (SwitchLiteral o bw (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (SwitchLiteral o bw (L s' d'))))
                        
    mapSpec :: Map.Map (Fix DecisionTree) Alias -> [(Text,Fix DecisionTree)] -> (Map.Map (Fix DecisionTree) Alias,[(Text,Free Anchor Alias)])
    mapSpec m s =
      let (ts,as) = unzip s
          (m',as') = mapAccumL computeSharing m as
      in (m',zip ts as')
