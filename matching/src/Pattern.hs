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
               , Pattern(..)
               , Clause(..)
               , Action(..)
               , Occurrence
               , Index
               , Constructor
               , mkClauseMatrix
               , failure
               , leaf
               , switch
               , switchLit
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
import           Data.List             (transpose,nub,sortBy)
import           Data.Maybe            (mapMaybe,catMaybes,isJust,fromJust,listToMaybe)
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
              , getTerms    :: ![Fix Pattern]
              }

instance Show Column where
  showsPrec _ (Column _ ts) =
    showString "Column " . showList ts

data Metadata = Metadata
                { getLength :: !Integer
                , getInjections :: ![Constructor]
                , getSort :: Sort Object
                , getChildren :: Constructor -> Maybe [Metadata]
                }

type Constructor = Either (SymbolOrAlias Object) String

type Index       = Int
data Pattern a   = Pattern Constructor (Maybe String) ![a]
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
  Fix $ Function name vars sort child

simplify :: Occurrence -> Fix DecisionTree -> Fix DecisionTree
simplify o dt = switch o [] (Just dt)

swap :: Index
     -> Fix DecisionTree
     -> Fix DecisionTree
swap ix tm = Fix (Swap ix tm)

-- [ Matrix ]

sigma :: Column -> [Constructor]
sigma c =
  let used = mapMaybe ix $ getTerms c
      inj = nub $ filter isInj used
      metaC = getMetadata c
      allInjs = getInjections $ getMetadata c
      usedInjs = nub $ filter (\ix' -> or $ map (isSubsort metaC ix') inj) allInjs
      dups = used ++ usedInjs
  in nub dups
  where
    ix :: Fix Pattern -> Maybe Constructor
    ix (Fix (Pattern ix' _ _)) = Just ix'
    ix (Fix Wildcard)             = Nothing
    ix (Fix (Variable _))         = Nothing
    isInj :: Constructor -> Bool
    isInj (Left (SymbolOrAlias (Id "inj" _) _)) = True
    isInj _ = False
    isSubsort :: Metadata -> Constructor -> Constructor -> Bool
    isSubsort (Metadata _ _ _ meta) (Left (SymbolOrAlias name [a,_])) ix'@(Left (SymbolOrAlias _ [b,_])) =
      let (Metadata _ _ _ childMeta) = (fromJust $ meta ix') !! 0
          child = Left (SymbolOrAlias name [a,b])
      in isJust $ childMeta $ child
    isSubsort _ _ _ = error "invalid injection"
    

sigma₁ :: PatternMatrix -> [Constructor]
sigma₁ (PatternMatrix (c : _)) = sigma c
sigma₁ _                       = []

bitwidth :: PatternMatrix -> Maybe String
bitwidth (PatternMatrix (c : _)) = 
  let s = bw c
  in listToMaybe $ catMaybes $ s
  where
    bw :: Column -> [Maybe String]
    bw = map ix . getTerms
    ix :: Fix Pattern -> Maybe String
    ix (Fix (Pattern _ bw' _)) = bw'
    ix _ = Nothing
bitwidth _                       = Nothing

mSpecialize :: Constructor -> (ClauseMatrix, [Occurrence]) -> (Text, (ClauseMatrix, [Occurrence]))
mSpecialize ix (cm@(ClauseMatrix (PatternMatrix (c : _)) _), o : os) = 
   let newOs = expandOccurrence cm o ix <> os
       cm' = filterMatrix (Just ix) (checkPatternIndex ix (getMetadata c)) (cm,o)
       cm'' = expandMatrix ix cm'
   in (getConstructor ix, (cm'', newOs))
   where
     getConstructor :: Constructor -> Text
     getConstructor (Left sym) = pack $ unparseToString sym
     getConstructor (Right str) = pack str

mSpecialize _ _ = error "must have at least one column"

expandOccurrence :: ClauseMatrix -> Occurrence -> Constructor -> [Occurrence]
expandOccurrence (ClauseMatrix (PatternMatrix (c : _)) _) o ix =
  let (Metadata _ _ _ mtd) = getMetadata c
      a = length $ fromJust $ mtd ix
  in map (\i -> i : o) [0..a-1]
expandOccurrence _ _ _ = error "must have at least one column"

mDefault :: (ClauseMatrix, [Occurrence]) -> Maybe (ClauseMatrix, [Occurrence])
mDefault (cm@(ClauseMatrix (PatternMatrix (c : _)) _),o : os) =
  let (Metadata mtd _ _ _) = getMetadata c
      s₁ = sigma c
  in  if null s₁ || (toInteger $ length s₁) /= mtd
      then Just ((stripFirstColumn (filterMatrix Nothing isNotPattern (cm,o))),os)
      else Nothing
mDefault _ = Nothing

stripFirstColumn :: ClauseMatrix -> ClauseMatrix
stripFirstColumn (ClauseMatrix (PatternMatrix (_ : cs)) as) =
  ClauseMatrix (PatternMatrix cs) as
stripFirstColumn _ = error "must have at least one column"

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

isNotPattern :: Fix Pattern -> Bool
isNotPattern (Fix (Pattern _ _ _)) = False
isNotPattern _ = True

checkPatternIndex :: Constructor -> Metadata -> Fix Pattern -> Bool
checkPatternIndex _ _ (Fix Wildcard) = True
checkPatternIndex _ _ (Fix (Variable _)) = True
checkPatternIndex (Left (SymbolOrAlias (Id "inj" _) [a,c])) (Metadata _ _ _ meta) (Fix (Pattern ix@(Left (SymbolOrAlias name@(Id "inj" _) [b,c'])) _ [p])) =
  let m@(Metadata _ _ _ childMeta) = (fromJust $ meta ix) !! 0
      child = Left (SymbolOrAlias name [a,b])
  in c == c' && (a == b || ((isJust $ childMeta $ child) && checkPatternIndex child m p))
checkPatternIndex ix _ (Fix (Pattern ix' _ _)) = ix == ix'

addVars :: Maybe Constructor -> [Clause] -> [Fix Pattern] -> Occurrence -> [Clause]
addVars ix as c o =
  let rows = zip c as
  in map (\(p, (Clause a vars)) -> (Clause a (addVarToRow ix o vars p))) rows

addVarToRow :: Maybe Constructor -> Occurrence -> [(String, Occurrence)] -> Fix Pattern -> [(String, Occurrence)]
addVarToRow _ o vars (Fix (Variable name)) = (name, o) : vars
addVarToRow (Just (Left (SymbolOrAlias (Id "inj" _) [a,_]))) o vars (Fix (Pattern (Left (SymbolOrAlias (Id "inj" _) [b,_])) _ [Fix (Variable name)])) = if a == b then vars else (name, o) : vars
addVarToRow _ _ a _ = a

filterMatrix :: Maybe Constructor -> (Fix Pattern -> Bool) -> (ClauseMatrix, Occurrence) -> ClauseMatrix
filterMatrix ix checkPattern ((ClauseMatrix (PatternMatrix cs@(c : _)) as), o) =
  let filteredRows = map checkPattern (getTerms c)
      newCs = map (filterRows filteredRows) cs
      varsAs = addVars ix as (getTerms c) o
      newAs = filterByList filteredRows varsAs
  in ClauseMatrix (PatternMatrix newCs) newAs
  where
    filterRows :: [Bool] -> Column -> Column
    filterRows fr (Column md rs) =
      Column md (filterByList fr rs)
filterMatrix _ _ (cmx,_) = cmx

expandMatrix :: Constructor -> ClauseMatrix -> ClauseMatrix
expandMatrix ix (ClauseMatrix (PatternMatrix (c : cs)) as) =
  ClauseMatrix (PatternMatrix (expandColumn ix c <> cs)) as
expandMatrix _ _ = error "Cannot expand empty matrix."

expandColumn :: Constructor -> Column -> [Column]
expandColumn ix (Column m ps) =
  let metas    = expandMetadata ix m
      expanded = map (expandPattern ix metas) ps
      ps'' = map (expandIfJust ix metas) expanded
  in  zipWith Column metas (transpose ps'')

expandMetadata :: Constructor -> Metadata -> [Metadata]
expandMetadata ix (Metadata _ _ _ ms) =
  case ms ix of
    Just m -> m
    Nothing -> error $ show ix

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
expandPattern (Left (SymbolOrAlias name [a, _])) _ (Fix (Pattern (Left (SymbolOrAlias (Id "inj" _) [b, _])) _ [fixedP])) = ([fixedP], if a == b then Nothing else Just (Left (SymbolOrAlias name [a,b])))
expandPattern _ _ (Fix (Pattern _ _ fixedPs)) = (fixedPs,Nothing)
expandPattern _ ms (Fix Wildcard)              = (replicate (length ms) (Fix Wildcard), Nothing)
expandPattern _ ms (Fix (Variable _))          = (replicate (length ms) (Fix Wildcard), Nothing)

data L a = L
           { getSpecializations :: ![(Text, a)]
           , getDefault         :: !(Maybe a)
           } deriving (Show, Eq, Functor, Ord)

data DecisionTree a = Leaf (Int, [Occurrence])
                    | Fail
                    | Switch Occurrence !(L a)
                    | SwitchLit Occurrence Int !(L a)
                    | EqualLiteral Text Text !a
                    | Swap Index !a
                    | Function Text [Occurrence] Text !a  
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
    toYaml (Anchor a (SwitchLit o i x)) = Y.namedMapping a
      ["specializations" Y..= Y.array (map (\(i1, i2) -> Y.array [Y.toYaml i1, Y.toYaml i2]) (getSpecializations x))
      , "default" Y..= Y.toYaml (case (getDefault x) of
                                    Just d -> Y.toYaml d
                                    Nothing -> Y.null
                                )
      , "bitwidth" Y..= Y.toYaml i
      , "occurrence" Y..= Y.toYaml o
      ]
    toYaml (Anchor a (EqualLiteral h l x)) = Y.namedMapping a
      ["hook" Y..= Y.toYaml h
      , "literal" Y..= Y.toYaml l
      , "next" Y..= Y.toYaml x
      ]
    toYaml (Anchor a (Swap i x)) = Y.namedMapping a
      ["swap" Y..= Y.array [Y.toYaml i, Y.toYaml x]]
    toYaml (Anchor a (Function name bindings sort x)) = Y.namedMapping a
      ["function" Y..= Y.toYaml name
      , "sort" Y..= Y.toYaml sort
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
getLeaf ix os ps (Clause (Action a rhsVars maybeSideCondition) matchedVars) next =
  let row = zip os ps
      vars = foldr (\(o, p) -> \l -> (addVarToRow Nothing o l p)) matchedVars row
      sorted = sortBy (compare `on` fst) vars
      filtered = filter (flip elem rhsVars . fst) sorted
      (_, newVars) = unzip filtered
  in case maybeSideCondition of
    Nothing -> Fix $ Leaf (a, newVars)
    Just cond -> let condFiltered = filter (flip elem cond . fst) sorted
                     (_, condVars) = unzip condFiltered
                 in function (pack $ "side_condition_" ++ (show a)) condVars "BOOL.Bool" (switchLit [ix, 0] 1 [("1", (leaf a newVars)), ("0", next)] Nothing)

compilePattern :: Int -> (ClauseMatrix, [Occurrence]) -> (Fix DecisionTree)
compilePattern ix cm@((ClauseMatrix pm@(PatternMatrix _) ac), os) =
  case ac of
    [] -> Fix Fail
    hd:tl -> 
      if isWildcardRow pm then
      let (Clause (Action _ _ maybeSideCondition) _) = hd
          ix' = if isJust maybeSideCondition then ix+1 else ix
      in if length ac == 1 then getLeaf ix os (firstRow pm) hd failure else getLeaf ix os (firstRow pm) hd (compilePattern ix' ((ClauseMatrix (notFirstRow pm) tl), os))
      else 
      let bw = bitwidth pm
          s₁ = sigma₁ pm
          ls = map (`mSpecialize` cm) s₁
          d  = mDefault cm
      in case bw of
           Nothing -> Fix $ Switch (head os) L
               { getSpecializations = map (second (compilePattern ix)) ls
               , getDefault = compilePattern ix <$> d
               }
           Just "BOOL.Bool" -> Fix $ SwitchLit (head os) 1 L
               { getSpecializations = map (second (compilePattern ix)) ls
               , getDefault = compilePattern ix <$> d
               }
           Just "MINT.MInt" -> error "not supported yet: mint"
           Just hook -> equalLiteral ix hook ls d
  where
    isWildcardRow :: PatternMatrix -> Bool
    isWildcardRow = and . map isWildcard . firstRow
    isWildcard :: Fix Pattern -> Bool
    isWildcard (Fix Wildcard) = True
    isWildcard (Fix (Variable _)) = True
    isWildcard _ = False
    equalLiteral :: Int -> String -> [(Text, (ClauseMatrix, [Occurrence]))] -> Maybe (ClauseMatrix, [Occurrence]) -> Fix DecisionTree
    equalLiteral o _ [] (Just d) = compilePattern o d
    equalLiteral _ _ [] (Nothing) = Fix Fail
    equalLiteral o hook ((name,spec):tl) d = Fix $ EqualLiteral (pack hook) name $ Fix $ SwitchLit [ix, 0] 1 $ L [("1", compilePattern (o+1) spec),("0", equalLiteral (o+1) hook tl d)] Nothing


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
                        Fix (EqualLiteral h l a) -> let (m',child) = mapChild a in (addName m',Free (Anchor (Just $ name m') (EqualLiteral h l child)))
                        Fix (Function n os s a) -> let (m',child) = mapChild a in (addName m', Free (Anchor (Just $ name m') (Function n os s child)))
                        Fix (Switch o (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (Switch o (L s' d'))))
                        Fix (SwitchLit o bw (L s d)) -> let (m',s') = mapSpec m s in let (m'',d') = mapDefault m' d in (addName m'', Free (Anchor (Just $ name m'') (SwitchLit o bw (L s' d'))))
                        
    mapSpec :: Map.Map (Fix DecisionTree) Alias -> [(Text,Fix DecisionTree)] -> (Map.Map (Fix DecisionTree) Alias,[(Text,Free Anchor Alias)])
    mapSpec m s =
      let (ts,as) = unzip s
          (m',as') = mapAccumL computeSharing m as
      in (m',zip ts as')
