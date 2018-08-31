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
               , switchLit
               , swap
               , simplify
               , DecisionTree(..)
               , compilePattern
               , serializeToYaml
               ) where

import           Data.Bifunctor        (second)
import           Data.Functor.Classes  (Eq1 (..), Show1 (..))
import           Data.Functor.Foldable (Fix (..), cata)
import           Data.List             (transpose,nub)
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

newtype Metadata = Metadata (Int, String -> [Metadata])

type Index       = Int
data Pattern a   = Pattern String (Maybe Int) ![a]
                 | Wildcard
                 | Variable String
                 deriving (Show, Eq, Functor)

newtype PatternMatrix = PatternMatrix [Column]
                        deriving (Show)

type Action       = Int

data ClauseMatrix = ClauseMatrix PatternMatrix ![Action]
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

switch :: [(Text, Fix DecisionTree)]
       -> Maybe (Fix DecisionTree)
       -> Fix DecisionTree
switch brs def =
  Fix $ Switch L { getSpecializations = brs
                 , getDefault = def }

switchLit :: [(Text, Fix DecisionTree)]
          -> Int
          -> Maybe (Fix DecisionTree)
          -> Fix DecisionTree
switchLit brs bw def =
  Fix $ SwitchLit bw L { getSpecializations = brs
                    , getDefault = def }

simplify :: Fix DecisionTree -> Fix DecisionTree
simplify dt = switch [] (Just dt)

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

mSpecialize :: String -> ClauseMatrix -> (Text, ClauseMatrix)
mSpecialize ix = (pack ix, ) . expandMatrix ix . (filterMatrix (checkPatternIndex ix))

mDefault :: ClauseMatrix -> Maybe ClauseMatrix
mDefault cm@(ClauseMatrix (PatternMatrix (c : _)) _) =
  let (Metadata (mtd,_)) = getMetadata c
      s₁ = sigma c
  in  if null s₁ || length s₁ /= mtd
      then Just (stripFirstColumn (filterMatrix isNotPattern cm))
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

filterMatrix :: (Fix Pattern -> Bool) -> ClauseMatrix -> ClauseMatrix
filterMatrix checkPattern (ClauseMatrix (PatternMatrix cs@(c : _)) as) =
  let filteredRows = map checkPattern (getTerms c)
      newCs = map (filterRows filteredRows) cs
      newAs = filterByList filteredRows as
  in ClauseMatrix (PatternMatrix newCs) newAs
  where
    filterRows :: [Bool] -> Column -> Column
    filterRows fr (Column md rs) =
      Column md (filterByList fr rs)
filterMatrix _ cmx = cmx

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
expandMetadata ix (Metadata (_,ms)) = ms ix

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
  liftEq eqT (SwitchLit bw l) (SwitchLit bw' l') =
    bw == bw' && liftEq eqT l l'
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
        bw = mBitwidth pm
    in case bw of
         Nothing -> Fix $ Switch L
             { getSpecializations = map (second compilePattern) ls
             , getDefault = compilePattern <$> d
             }
         Just bw' -> Fix $ SwitchLit bw' L
             { getSpecializations = map (second compilePattern) ls
             , getDefault = compilePattern <$> d
             }
  where
    isWildcardRow :: PatternMatrix -> Bool
    isWildcardRow = and . map (Fix Wildcard ==) . firstRow
