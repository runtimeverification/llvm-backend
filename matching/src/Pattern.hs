{-# LANGUAGE DeriveFunctor     #-}
{-# LANGUAGE FlexibleContexts  #-}
{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE TupleSections     #-}
{-# LANGUAGE TypeSynonymInstances #-}
{-# LANGUAGE FlexibleInstances #-}

module Pattern ( PatternMatrix(..)
               , ClauseMatrix(..)
               , Column(..)
               , Metadata(..)
               , Pattern(..)
               , Clause(..)
               , Action(..)
               , Occurrence
               , Index
               , mkClauseMatrix
               , failure
               , leaf
               , switch
               , switchLit
               , function
               , swap
               , simplify
               , DecisionTree(..)
               , compilePattern
               , serializeToYaml
               ) where

import           Data.Bifunctor        (second)
import           Data.Function         (on)
import           Data.Functor.Classes  (Eq1 (..), Show1 (..))
import           Data.Functor.Foldable (Fix (..), cata)
import           Data.List             (transpose,nub,sortBy)
import           Data.Maybe            (mapMaybe)
import           Data.Semigroup        ((<>))
import           Data.Text             (Text, pack)
import           TextShow              (showt)
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
                { getLength :: Integer
                , getChildren :: String -> [Metadata]
                }

type Index       = Int
data Pattern a   = Pattern String (Maybe Int) ![a]
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
                    }
                    deriving (Show)

data ClauseMatrix = ClauseMatrix PatternMatrix ![Clause]
                    deriving (Show)

instance Show1 Pattern where
  liftShowsPrec _     _      _ Wildcard = showString "_"
  liftShowsPrec _     showTs _ (Pattern ix _ t) =
    showString "P " . showString (show ix) .
    showString " "  . showTs t
  liftShowsPrec _     _      _ (Variable lbl) = showString ("$" ++ lbl)

instance Eq1 Pattern where
  liftEq _ Wildcard Wildcard = True
  liftEq eqT (Pattern ix bw ts) (Pattern ix' bw' ts') =
    ix == ix' && bw == bw' && and (zipWith eqT ts ts')
  liftEq _ _ _ = False

-- [ Builders ]

mkClauseMatrix :: [Column]
               -> [Action]
               -> Either Text (ClauseMatrix, [Occurrence])
mkClauseMatrix cs as = do
  validateColumnLength (length as) cs
  pure ((ClauseMatrix (PatternMatrix cs) (map (\a -> Clause a []) as)),map (\i -> [i]) [1..length cs]) 
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

switchLit :: Occurrence
          -> Int
          -> [(Text, Fix DecisionTree)]
          -> Maybe (Fix DecisionTree)
          -> Fix DecisionTree
switchLit o bw brs def =
  Fix $ SwitchLit o bw L { getSpecializations = brs
                    , getDefault = def }

function :: Text
         -> [Occurrence]
         -> Text
         -> Fix DecisionTree
         -> Fix DecisionTree
function name vars sort child =
  Fix $ Function (name, vars, sort, child)

simplify :: Occurrence -> Fix DecisionTree -> Fix DecisionTree
simplify o dt = switch o [] (Just dt)

swap :: Index
     -> Fix DecisionTree
     -> Fix DecisionTree
swap ix tm = Fix (Swap ix tm)

-- [ Matrix ]

sigma :: Column -> [String]
sigma = nub . mapMaybe ix . getTerms
  where
    ix :: Fix Pattern -> Maybe String
    ix (Fix (Pattern ix' _ _)) = Just ix'
    ix (Fix Wildcard)        = Nothing
    ix (Fix (Variable _))         = Nothing

sigma₁ :: PatternMatrix -> [String]
sigma₁ (PatternMatrix (c : _)) = sigma c
sigma₁ _                       = []

mSpecialize :: String -> (ClauseMatrix, [Occurrence]) -> (Text, (ClauseMatrix, [Occurrence]))
mSpecialize ix (cm, o : os) = 
   let newOs = expandOccurrence cm o ix <> os
       cm' = filterMatrix (checkPatternIndex ix) (cm,o)
       cm'' = expandMatrix ix cm'
  in (pack ix, (cm'', newOs))
mSpecialize _ _ = error "must have at least one column"

expandOccurrence :: ClauseMatrix -> Occurrence -> String -> [Occurrence]
expandOccurrence (ClauseMatrix (PatternMatrix (c : _)) _) o ix =
  let (Metadata _ mtd) = getMetadata c
      a = length (mtd ix)
  in map (\i -> i : o) [0..a-1]
expandOccurrence _ _ _ = error "must have at least one column"

mDefault :: (ClauseMatrix, [Occurrence]) -> Maybe (ClauseMatrix, [Occurrence])
mDefault (cm@(ClauseMatrix (PatternMatrix (c : _)) _),o : os) =
  let (Metadata mtd _) = getMetadata c
      s₁ = sigma c
  in  if null s₁ || (toInteger $ length s₁) /= mtd
      then Just ((stripFirstColumn (filterMatrix isNotPattern (cm,o))),os)
      else Nothing
mDefault _ = Nothing

mBitwidth :: PatternMatrix -> Maybe Int
mBitwidth (PatternMatrix (c : _)) =
  case (getTerms c) of
    Fix (Pattern _ bw _) : _ -> bw
    _ -> Nothing
mBitwidth _ = Nothing

stripFirstColumn :: ClauseMatrix -> ClauseMatrix
stripFirstColumn (ClauseMatrix (PatternMatrix (_ : cs)) as) =
  ClauseMatrix (PatternMatrix cs) as
stripFirstColumn _ = error "must have at least one column"

firstRow :: PatternMatrix -> [Fix Pattern]
firstRow (PatternMatrix cs) =
  map (\(Column _ (p : _)) -> p) cs

filterByList :: [Bool] -> [a] -> [a]
filterByList (True  : bs) (x : xs) = x : filterByList bs xs
filterByList (False : bs) (_ : xs) = filterByList bs xs
filterByList _ _                   = []

isNotPattern :: Fix Pattern -> Bool
isNotPattern (Fix (Pattern _ _ _)) = False
isNotPattern _ = True

checkPatternIndex :: String -> Fix Pattern -> Bool
checkPatternIndex _  (Fix Wildcard)        = True
checkPatternIndex _  (Fix (Variable _))         = True
checkPatternIndex ix (Fix (Pattern ix' _ _)) = ix == ix'

addVars :: [Clause] -> [Fix Pattern] -> Occurrence -> [Clause]
addVars as c o =
  let row = zip c as
  in map (\(p, (Clause a vars)) -> (Clause a (addVarToRow o vars p))) row

addVarToRow :: Occurrence -> [(String, Occurrence)] -> Fix Pattern -> [(String, Occurrence)]
addVarToRow o vars (Fix (Variable name)) = (name, o) : vars
addVarToRow _ a _ = a

filterMatrix :: (Fix Pattern -> Bool) -> (ClauseMatrix, Occurrence) -> ClauseMatrix
filterMatrix checkPattern ((ClauseMatrix (PatternMatrix cs@(c : _)) as), o) =
  let filteredRows = map checkPattern (getTerms c)
      newCs = map (filterRows filteredRows) cs
      varsAs = addVars as (getTerms c) o
      newAs = filterByList filteredRows varsAs
  in ClauseMatrix (PatternMatrix newCs) newAs
  where
    filterRows :: [Bool] -> Column -> Column
    filterRows fr (Column md rs) =
      Column md (filterByList fr rs)
filterMatrix _ (cmx,_) = cmx

expandMatrix :: String -> ClauseMatrix -> ClauseMatrix
expandMatrix ix (ClauseMatrix (PatternMatrix (c : cs)) as) =
  ClauseMatrix (PatternMatrix (expandColumn ix c <> cs)) as
expandMatrix _ _ = error "Cannot expand empty matrix."

expandColumn :: String -> Column -> [Column]
expandColumn ix (Column m ps) =
  let metas    = expandMetadata ix m
      patterns = map (expandPattern metas) ps
  in  zipWith Column metas (transpose patterns)

expandMetadata :: String -> Metadata -> [Metadata]
expandMetadata ix (Metadata _ ms) = ms ix

expandPattern :: [Metadata]
              -> Fix Pattern
              -> [Fix Pattern]
expandPattern _  (Fix (Pattern _ _ fixedPs)) = fixedPs
expandPattern ms (Fix Wildcard)              = replicate (length ms) (Fix Wildcard)
expandPattern ms (Fix (Variable _))          = replicate (length ms) (Fix Wildcard)

--[ Target language ]

data L a = L
           { getSpecializations :: [(Text, a)]
           , getDefault         :: Maybe a
           } deriving (Show, Eq, Functor)

data DecisionTree a = Leaf (Int, [Occurrence])
                    | Fail
                    | Switch Occurrence (L a)
                    | SwitchLit Occurrence Int (L a)
                    | Swap Index a
                    | Function (Text, [Occurrence], Text, a) 
                    deriving (Show, Eq, Functor)

instance Y.ToYaml a => Y.ToYaml (DecisionTree a) where
    toYaml (Leaf (act, x)) = Y.mapping [
        "action" Y..= Y.array [Y.toYaml act, Y.toYaml x]
      ]
    toYaml Fail = Y.string "fail"
    toYaml (Switch o x) = Y.mapping
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just i -> Y.toYaml i
                                    Nothing -> Y.null
                                )
      , "occurrence" Y..= Y.toYaml o
      ]
    toYaml (SwitchLit o i x) = Y.mapping
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just d -> Y.toYaml d
                                    Nothing -> Y.null
                                )
      , "bitwidth" Y..= Y.toYaml i
      , "occurrence" Y..= Y.toYaml o
      ]

    toYaml (Swap i x) = Y.mapping
      ["swap" Y..= Y.array [Y.toYaml i, Y.toYaml x]]
    toYaml (Function (name, bindings, sort, x)) = Y.mapping
      ["function" Y..= Y.toYaml name
      , "sort" Y..= Y.toYaml sort
      , "args" Y..= Y.toYaml bindings
      , "next" Y..= Y.toYaml x
      ]

serializeToYaml :: (Fix DecisionTree) -> B.ByteString
serializeToYaml = Y.toByteString . Y.toYaml

instance Y.ToYaml (Fix DecisionTree) where
    toYaml = cata Y.toYaml

instance Eq1 DecisionTree where
  liftEq _ Fail Fail = True
  liftEq _ (Leaf a) (Leaf a') = a == a'
  liftEq eqT (Switch o l) (Switch o' l') =
    o == o' && liftEq eqT l l'
  liftEq eqT (SwitchLit o bw l) (SwitchLit o' bw' l') =
    o == o' && bw == bw' && liftEq eqT l l'
  liftEq eqT (Function (n, os, s, t)) (Function (n', os', s', t')) =
    n == n' && os == os' && s == s' && t `eqT` t'
  liftEq eqT (Swap ix t) (Swap ix' t') =
    ix == ix' && t `eqT` t'
  liftEq _ _ _ = False

smEq :: (a -> b -> Bool)
     -> [(Text, a)]
     -> [(Text, b)]
     -> Bool
smEq eq s₁ s₂ =
  and (zipWith combine s₁ s₂)
  where
    combine (ix, a) (ix', a') = ix == ix' && a `eq` a'

instance Eq1 L where
  liftEq eqT (L s (Just d)) (L s' (Just d')) =
    smEq eqT s s' && d `eqT` d'
  liftEq eqT (L s Nothing) (L s' Nothing) =
    smEq eqT s s'
  liftEq _ _ _ = False

instance Show1 DecisionTree where
  liftShowsPrec _ _ _ (Leaf a) = showString $ "Leaf " ++ show a
  liftShowsPrec _ _ _ Fail     = showString "Fail"
  liftShowsPrec showT showL d (Switch o l) =
    showString "Switch " . showList o . showString " L(" .
    liftShowsPrec showT showL (d + 1) l . showString ")"
  liftShowsPrec showT showL d (SwitchLit o i l) =
    showString ("SwitchLit " ++ show i) . showString " " . showList o . showString " L(" .
    liftShowsPrec showT showL (d + 1) l . showString ")"
  liftShowsPrec showT _ d (Swap ix tm) =
    showString ("Swap " ++ show ix ++ " ") . showT (d + 1) tm
  liftShowsPrec showT _ d (Function (name,args,sort,tm)) =
    showString ("Function " ++ show name ++ "(" ++ show args ++ "):" ++ show sort) . showT (d + 1) tm

instance Show1 L where
  liftShowsPrec showT _ d (L sm dm) =
    let showSpec (index, tm) = showString (show index ++ ":")
                               . showT (d + 1) tm . showString ";"
        smString = foldl (.) id $ map showSpec sm
        dmString = maybe id (\s -> showString "*:" . (showT (d + 1)) s) dm
    in  smString . dmString

getLeaf :: [Occurrence] -> [Fix Pattern] -> Clause -> Fix DecisionTree
getLeaf os ps (Clause (Action a rhsVars maybeSideCondition) matchedVars) =
  let row = zip os ps
      vars = foldr (\(o, p) -> \l -> (addVarToRow o l p)) matchedVars row
      sorted = sortBy (compare `on` fst) vars
      filtered = filter (flip elem rhsVars . fst) sorted
      (_, newVars) = unzip filtered
  in case maybeSideCondition of
    Nothing -> Fix $ Leaf (a, newVars)
    Just cond -> let condFiltered = filter (flip elem cond . fst) sorted
                     (_, condVars) = unzip condFiltered
                 in function (pack $ "side_condition_" ++ (show a)) condVars "BOOL.Bool" (switchLit [0, 0] 1 [("1", (leaf a newVars)), ("0", failure)] Nothing)

compilePattern :: (ClauseMatrix, [Occurrence]) -> (Fix DecisionTree)
compilePattern cm@((ClauseMatrix pm@(PatternMatrix _) ac), os)
  | length ac == 0 = Fix Fail
  | isWildcardRow pm = getLeaf os (firstRow pm) (head ac)
  | otherwise =
    let s₁ = sigma₁ pm
        ls = map (`mSpecialize` cm) s₁
        d  = mDefault cm
        bw = mBitwidth pm
    in case bw of
         Nothing -> Fix $ Switch (head os) L
             { getSpecializations = map (second compilePattern) ls
             , getDefault = compilePattern <$> d
             }
         Just bw' -> Fix $ SwitchLit (head os) bw' L
             { getSpecializations = map (second compilePattern) ls
             , getDefault = compilePattern <$> d
             }
  where
    isWildcardRow :: PatternMatrix -> Bool
    isWildcardRow = and . map isWildcard . firstRow
    isWildcard :: Fix Pattern -> Bool
    isWildcard (Fix Wildcard) = True
    isWildcard (Fix (Variable _)) = True
    isWildcard _ = False
