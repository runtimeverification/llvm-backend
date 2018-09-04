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
               , Index
               , mkClauseMatrix
               , failure
               , leaf
               , switch
               , swap
               , simplify
               , DecisionTree(..)
               , compilePattern
               , serializeToYaml
               ) where

import           Data.Bifunctor        (second)
import           Data.Functor.Classes  (Eq1 (..), Show1 (..))
import           Data.Functor.Foldable (Fix (..), cata)
import           Data.Maybe            (mapMaybe)
import           Data.Semigroup        ((<>))
import           Data.Text             (Text)
import           TextShow              (showt)
import qualified Data.Yaml.Builder as Y
import qualified Data.ByteString as B

data Column = Column
              { getMetadata :: !Metadata
              , getTerms    :: ![Fix Pattern]
              } deriving (Eq)

instance Show Column where
  showsPrec _ (Column _ ts) =
    showString "Column " . showList ts

newtype Metadata = Metadata [Metadata]
                 deriving (Show, Eq)

type Index       = Int
data Pattern a   = Pattern Index ![a]
                 | Wildcard
                 | Var String
                 deriving (Show, Eq, Functor)

newtype PatternMatrix = PatternMatrix [Column]
                        deriving (Show, Eq)

type Action       = Int

data ClauseMatrix = ClauseMatrix PatternMatrix ![Action]
                    deriving (Show, Eq)

instance Show1 Pattern where
  liftShowsPrec _     _      _ Wildcard = showString "_"
  liftShowsPrec _     showTs _ (Pattern ix t) =
    showString "P " . showString (show ix) .
    showString " "  . showTs t
  liftShowsPrec _     _      _ (Var lbl) = showString ("$" ++ lbl)

instance Eq1 Pattern where
  liftEq _ Wildcard Wildcard = True
  liftEq eqT (Pattern ix ts) (Pattern ix' ts') =
    ix == ix' && and (zipWith eqT ts ts')
  liftEq _ _ _ = False

-- [ Builders ]

mkClauseMatrix :: [Column]
               -> [Action]
               -> Either Text ClauseMatrix
mkClauseMatrix cs as = do
  validateColumnLength (length as) cs
  pure (ClauseMatrix (PatternMatrix cs) as)
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

leaf :: Action -> Fix DecisionTree
leaf a = Fix (Leaf (a, []))

switch :: [(Index, Fix DecisionTree)]
       -> Maybe (Fix DecisionTree)
       -> Fix DecisionTree
switch brs def =
  Fix $ Switch L { getSpecializations = brs
                 , getDefault = def }

simplify :: Fix DecisionTree -> Fix DecisionTree
simplify dt = switch [] (Just dt)

swap :: Index
     -> Fix DecisionTree
     -> Fix DecisionTree
swap ix tm = Fix (Swap ix tm)

-- [ Matrix ]

sigma :: Column -> [Index]
sigma = mapMaybe ix . getTerms
  where
    ix :: Fix Pattern -> Maybe Index
    ix (Fix (Pattern ix' _)) = Just ix'
    ix (Fix Wildcard)        = Nothing
    ix (Fix (Var _))         = Nothing

sigma₁ :: PatternMatrix -> [Index]
sigma₁ (PatternMatrix (c : _)) = sigma c
sigma₁ _                       = []

mSpecialize :: Index -> ClauseMatrix -> (Index, ClauseMatrix)
mSpecialize ix = (ix, ) . expandMatrix ix . filterByIndex ix

mDefault :: ClauseMatrix -> Maybe ClauseMatrix
mDefault (ClauseMatrix (PatternMatrix (c : cs)) as) =
  let (Metadata mtd) = getMetadata c
      s₁ = sigma c
  in  if null s₁ || length s₁ /= length mtd
      then Just (ClauseMatrix (PatternMatrix cs) as)
      else Nothing
mDefault _ = Nothing

firstRow :: PatternMatrix -> [Fix Pattern]
firstRow (PatternMatrix cs) =
  map (\(Column _ (p : _)) -> p) cs

filterByList :: [Bool] -> [a] -> [a]
filterByList (True  : bs) (x : xs) = x : filterByList bs xs
filterByList (False : bs) (_ : xs) = filterByList bs xs
filterByList _ _                   = []

filterByIndex :: Index -> ClauseMatrix -> ClauseMatrix
filterByIndex ix (ClauseMatrix (PatternMatrix cs@(c : _)) as) =
  let filteredRows = map checkPatternIndex (getTerms c)
      newCs = map (filterRows filteredRows) cs
      newAs = filterByList filteredRows as
  in ClauseMatrix (PatternMatrix newCs) newAs
  where
    checkPatternIndex :: Fix Pattern -> Bool
    checkPatternIndex (Fix Wildcard)        = True
    checkPatternIndex (Fix (Var _))         = True
    checkPatternIndex (Fix (Pattern ix' _)) = ix == ix'
    filterRows :: [Bool] -> Column -> Column
    filterRows fr (Column md rs) =
      Column md (filterByList fr rs)
filterByIndex _ cmx = cmx

expandMatrix :: Index -> ClauseMatrix -> ClauseMatrix
expandMatrix ix (ClauseMatrix (PatternMatrix (c : cs)) as) =
  ClauseMatrix (PatternMatrix (expandColumn ix c <> cs)) as
expandMatrix _ _ = error "Cannot expand empty matrix."

expandColumn :: Index -> Column -> [Column]
expandColumn ix (Column m ps) =
  let metas    = expandMetadata ix m
      patterns = map (expandPattern metas) ps
  in  zipWith Column metas patterns

expandMetadata :: Index -> Metadata -> [Metadata]
expandMetadata ix (Metadata ms) =
  let (Metadata ms') = ms !! ix
  in  ms'

expandPattern :: [Metadata]
              -> Fix Pattern
              -> [Fix Pattern]
expandPattern _  (Fix (Pattern _ fixedPs)) = fixedPs
expandPattern ms (Fix Wildcard)             = replicate (length ms) (Fix Wildcard)
expandPattern ms (Fix (Var _ ))             = replicate (length ms) (Fix Wildcard)

--[ Target language ]

data L a = L
           { getSpecializations :: [(Int, a)]
           , getDefault         :: Maybe a
           } deriving (Show, Eq, Functor)

type Binding = (Int, Int)

data DecisionTree a = Leaf (Action, [Binding])
                    | Fail
                    | Switch (L a)
                    | SwitchLit Int (L a)
                    | Swap Index a
                    | Function (Text, [Binding], Text, a) 
                    deriving (Show, Eq, Functor)

instance Y.ToYaml a => Y.ToYaml (DecisionTree a) where
    toYaml (Leaf (act, x)) = Y.mapping [
        "action" Y..= Y.array [Y.toYaml act, Y.array (map (\(i1,i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) x)]
      ]
    toYaml Fail = Y.string "fail"
    toYaml (Switch x) = Y.mapping
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just i -> Y.toYaml i
                                    Nothing -> Y.null
                                )
      ]
    toYaml (SwitchLit i x) = Y.mapping
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just d -> Y.toYaml d
                                    Nothing -> Y.null
                                )
      , "bitwidth" Y..= Y.toYaml i
      ]

    toYaml (Swap i x) = Y.mapping
      ["swap" Y..= Y.array [Y.toYaml i, Y.toYaml x]]
    toYaml (Function (name, bindings, sort, x)) = Y.mapping
      ["function" Y..= Y.toYaml name
      , "sort" Y..= Y.toYaml sort
      , "args" Y..= Y.array (map (\(i1,i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) bindings)
      , "next" Y..= Y.toYaml x
      ]

serializeToYaml :: (Fix DecisionTree) -> B.ByteString
serializeToYaml = Y.toByteString . Y.toYaml

instance Y.ToYaml (Fix DecisionTree) where
    toYaml = cata Y.toYaml

instance Eq1 DecisionTree where
  liftEq _ Fail Fail = True
  liftEq _ (Leaf a) (Leaf a') = a == a'
  liftEq eqT (Switch l) (Switch l') =
    liftEq eqT l l'
  liftEq eqT (Swap ix t) (Swap ix' t') =
    ix == ix' && t `eqT` t'
  liftEq _ _ _ = False

smEq :: (a -> b -> Bool)
     -> [(Index, a)]
     -> [(Index, b)]
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
  liftShowsPrec showT showL d (Switch l) =
    showString "Switch L(" .
    liftShowsPrec showT showL (d + 1) l . showString ")"
  liftShowsPrec showT showL d (SwitchLit i l) =
    showString ("SwitchLit " ++ show i ++ "L(") .
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

compilePattern :: ClauseMatrix -> (Fix DecisionTree)
compilePattern cm@(ClauseMatrix pm@(PatternMatrix _) ac)
  | length ac == 0 = Fix Fail
  | isWildcardRow pm = Fix $ Leaf $ (head ac, [])
  | otherwise =
    let s₁ = sigma₁ pm
        ls = map (`mSpecialize` cm) s₁
        d  = mDefault cm
    in  Fix $ Switch L
        { getSpecializations = map (second compilePattern) ls
        , getDefault = compilePattern <$> d
        }
  where
    isWildcardRow :: PatternMatrix -> Bool
    isWildcardRow = and . map (Fix Wildcard ==) . firstRow
