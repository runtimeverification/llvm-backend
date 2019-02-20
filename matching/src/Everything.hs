module Everything ( mkClauseMatrix
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

import Control.Monad.Free
       ( Free (..) )
import Data.Bifunctor
       ( first, second )
import Data.Deriving
       ( deriveEq1, deriveOrd1, deriveShow1 )
import Data.Function
       ( on )
import Data.Functor.Foldable
       ( Fix (..) )
import Data.List
       ( elemIndex, intersect, maximumBy, nub, sortBy, transpose, zipWith4 )
import Data.List.Index
       ( indexed )
import Data.Maybe
       ( catMaybes, fromJust, fromMaybe, isJust, listToMaybe, mapMaybe )
import Data.Ord
       ( comparing )
import Data.Semigroup
       ( (<>) )
import Data.Text
       ( Text, pack )
import Data.Traversable
       ( mapAccumL )
import Kore.AST.Common
       ( SymbolOrAlias (..) )
import Kore.AST.Identifier
       ( AstLocation (..), Id (..) )
import Kore.Unparser
       ( unparseToString )
import TextShow
       ( showt )

import qualified Data.ByteString as B
import qualified Data.Map.Strict as Map
import qualified Data.Yaml.Builder as Y

import Pattern
import Pattern.Map
import Pattern.Optimiser.Score
import Pattern.Set
import Pattern.Type
import Pattern.Var

-- [ Builders ]

mkClauseMatrix :: [Column Pattern BoundPattern]
               -> [Action]
               -> Either Text (ClauseMatrix Pattern BoundPattern, Fringe)
mkClauseMatrix cs as = do
  validateColumnLength (length as) cs
  pure (ClauseMatrix (PatternMatrix cs) (map (\a -> Clause a [] [] []) as), map (\i -> (Num i Base, False)) [1..length cs])
  where
    validateColumnLength :: Int
                         -> [Column Pattern BoundPattern]
                         -> Either Text ()
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

sigma :: Bool
      -> Column Pattern BoundPattern
      -> [Clause BoundPattern]
      -> [Constructor BoundPattern]
sigma exact c cs =
  let used = concat $ zipWith ix cs $ getTerms c
      bestKey = getBestKey c cs
      bestUsed = case bestKey of
                   Nothing -> used
                   Just k -> filter (isBest k) used
      usedInjs = nub $ concatMap (getInjections $ getMetadata c) bestUsed
      dups = if exact then bestUsed else bestUsed ++ usedInjs
      nodups = nub dups
  in if Empty `elem` nodups then [Empty] else filter (/= Empty) nodups
  where
    ix :: Clause BoundPattern
       -> Fix Pattern
       -> [Constructor BoundPattern]
    ix _ (Fix (Pattern ix' _ _)) = [getPatternConstructor ix']
    ix _ (Fix (ListPattern hd Nothing tl s _)) = [List s (length hd + length tl)]
    -- if the list has a frame, then matching lists longer than the current head and tail
    -- is performed via the default case of the switch, which means that we need
    -- to create a switch case per list of lesser length
    ix _ (Fix (ListPattern hd (Just _) tl s _)) = map (List s) [0..(length hd + length tl)]
    ix cls p@(Fix MapPattern{}) =
      let (cs', kont) = getMapCs c cls p in
        cs' ++ maybe [] (ix cls) kont
    ix cls p@(Fix SetPattern{}) =
      let (cs', kont) = getSetCs c cls p in
        cs' ++ maybe [] (ix cls) kont
    ix cls (Fix (As _ _ pat))      = ix cls pat
    ix cls (Fix (Or ps)) = concatMap (ix cls) ps
    ix _ (Fix Wildcard)          = []
    ix _ (Fix (Variable _ _))    = []
    isBest :: Fix BoundPattern
           -> Constructor BoundPattern
           -> Bool
    isBest k (HasKey _ _ _ (Just k')) = k == k'
    isBest k (HasNoKey _ (Just k')) = k == k'
    isBest _ (HasKey _ _ _ Nothing) = False
    isBest _ (HasNoKey _ Nothing) = False
    isBest _ _ = True


sigma₁ :: Bool
       -> ClauseMatrix Pattern BoundPattern
       -> [Constructor BoundPattern]
sigma₁ exact (ClauseMatrix (PatternMatrix (c : _)) as) = sigma exact c as
sigma₁ _ _                       = []

hook :: PatternMatrix Pattern BoundPattern -> Maybe String
hook (PatternMatrix (c : _)) =
  let s = bw c
  in listToMaybe $ catMaybes s
  where
    bw :: Column Pattern BoundPattern -> [Maybe String]
    bw = map ix . getTerms
    ix :: Fix Pattern -> Maybe String
    ix (Fix (Pattern _ bw' _)) = bw'
    ix (Fix ListPattern{}) = Just "LIST.List"
    ix (Fix MapPattern {}) = getMapHookName
    ix (Fix SetPattern{}) = Just "SET.Set"
    ix (Fix (As _ _ pat))    = ix pat
    ix (Fix (Or ps))      = listToMaybe $ catMaybes $ map ix ps
    ix (Fix Wildcard)        = Nothing
    ix (Fix (Variable _ _))  = Nothing
hook _                       = Nothing

mSpecialize :: Constructor BoundPattern
            -> (ClauseMatrix Pattern BoundPattern, Fringe)
            -> (Text, (ClauseMatrix Pattern BoundPattern, Fringe))
mSpecialize ix (cm@(ClauseMatrix (PatternMatrix (c : _)) _), o : os) =
   let newOs = expandOccurrence cm o ix <> os
       cm' = filterMatrix (Just ix) (checkPatternIndex ix (getMetadata c)) (expandOrPatterns cm,fst o)
       cm'' = expandMatrix ix cm'
   in (getConstructor ix, (cm'', newOs))
   where
     getConstructor :: Constructor BoundPattern -> Text
     getConstructor (SymbolConstructor (Symbol sym)) = pack $ unparseToString sym
     getConstructor (LiteralConstructor (Literal str)) = str
     getConstructor (List _ i) = pack $ show i
     getConstructor Empty = "0"
     getConstructor (NonEmpty _) = error "Invalid map pattern"
     getConstructor HasKey{} = "1"
     getConstructor (HasNoKey _ _) = "0"
mSpecialize _ _ = error "must have at least one column"

tshow :: Show a => a -> Text
tshow = pack . show

expandOccurrence :: ClauseMatrix Pattern BoundPattern
                 -> (Occurrence, Bool)
                 -> Constructor BoundPattern
                 -> Fringe
expandOccurrence (ClauseMatrix (PatternMatrix (c : _)) _) o ix =
  case ix of
    Empty -> []
    NonEmpty _ -> [o]
    HasKey isSet _ _ (Just k) ->
      if isSet
      then [(Rem (tshow k) (fst o), False), o]
      else [(Value (tshow k) (fst o), False), (Rem (tshow k) (fst o), False), o]
    HasKey _ _ _ Nothing -> error "Invalid map/set pattern"
    HasNoKey _ _ -> [o]
    _ -> let (Metadata _ _ _ _ mtd) = getMetadata c
             a = length $ fromJust $ mtd ix
             os = map (\i -> Num i $ fst o) [0..a-1]
             isExact = isInj ix
         in zip os $ replicate (length os) isExact
  where
    isInj :: Constructor BoundPattern -> Bool
    isInj (SymbolConstructor (Symbol (SymbolOrAlias (Id "inj" _) _))) = True
    isInj _ = False
expandOccurrence _ _ _ = error "must have at least one column"

mDefault :: [Constructor BoundPattern]
         -> (ClauseMatrix Pattern BoundPattern, Fringe)
         -> Maybe (ClauseMatrix Pattern BoundPattern, Fringe)
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
  in  if infiniteLength || null s₁ || elem Empty s₁ || (not isMapOrSet && toInteger (length s₁) /= mtd)
      then Just (expandDefault (getDefaultConstructor c s₁) (hook pm) (filterMatrix (getDefaultConstructor c s₁) isDefault (expandOrPatterns cm,fst o)), expandDefaultOccurrence cm o s₁ <> os)
      else Nothing
mDefault _ _ = Nothing

maxListSize :: Column Pattern BoundPattern -> (Int, Int)
maxListSize c =
  let longestHead = maximum $ map getListHeadSize $ getTerms c
      longestTail = maximum $ map getListTailSize $ getTerms c
  in (longestHead, longestTail)
  where
    getListHeadSize :: Fix Pattern -> Int
    getListHeadSize (Fix (ListPattern hd _ _ _ _)) = length hd
    getListHeadSize (Fix (Variable _ _)) = -1
    getListHeadSize (Fix Wildcard) = -1
    getListHeadSize _ = error "unsupported list pattern"
    getListTailSize :: Fix Pattern -> Int
    getListTailSize (Fix (ListPattern _ _ tl _ _)) = length tl
    getListTailSize (Fix (Variable _ _)) = -1
    getListTailSize (Fix Wildcard) = -1
    getListTailSize _ = error "unsupported list pattern"

getDefaultConstructor :: Column Pattern BoundPattern
                      -> [Constructor BoundPattern]
                      -> Maybe (Constructor BoundPattern)
getDefaultConstructor c s =
  if Empty `elem` s then Just $ NonEmpty $ Ignoring $ getMetadata c
  else case s of
         [] -> Nothing
         list:_ -> let (hd,tl) = maxListSize c
                   in case list of
                        List sym _ -> Just $ List sym (hd + tl)
                        _ -> Nothing

expandDefault :: Maybe (Constructor BoundPattern)
              -> Maybe String
              -> ClauseMatrix Pattern BoundPattern
              -> ClauseMatrix Pattern BoundPattern
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

expandDefaultOccurrence :: ClauseMatrix Pattern BoundPattern
                        -> (Occurrence,Bool)
                        -> [Constructor BoundPattern]
                        -> Fringe
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

firstRow :: PatternMatrix Pattern BoundPattern
         -> [Fix Pattern]
firstRow (PatternMatrix cs) =
  map (\(Column _ (p : _)) -> p) cs
notFirstRow :: PatternMatrix Pattern BoundPattern
            -> PatternMatrix Pattern BoundPattern
notFirstRow (PatternMatrix cs) =
  PatternMatrix (map (\(Column m (_ : ps)) -> Column m ps) cs)

filterByList :: [Bool] -> [a] -> [a]
filterByList (True  : bs) (x : xs) = x : filterByList bs xs
filterByList (False : bs) (_ : xs) = filterByList bs xs
filterByList _ _                   = []

isDefault :: (Clause BoundPattern, Fix Pattern) -> Bool
isDefault (_, Fix Pattern{}) = False
isDefault (_, Fix (ListPattern _ Nothing _ _ _)) = False
isDefault (_, Fix (ListPattern _ (Just _) _ _ _)) = True
isDefault (c, Fix (As _ _ pat)) = isDefault (c,pat)
isDefault (c, Fix (Or ps)) = any isDefault (zip (replicate (length ps) c) ps)
isDefault (_, Fix Wildcard) = True
isDefault (_, Fix (Variable _ _)) = True
isDefault (c, p@(Fix MapPattern{})) =
  isDefaultMap c p
isDefault (_, Fix (SetPattern _ (Just _) _ _)) = True
isDefault (_, Fix (SetPattern es Nothing _ _)) = not (null es)

mightUnify :: Fix BoundPattern -> Fix BoundPattern -> Bool
mightUnify (Fix Wildcard) _ = True
mightUnify _ (Fix Wildcard) = True
mightUnify _ (Fix (Variable _ _)) = True
mightUnify (Fix (Variable _ _)) _ = True
mightUnify (Fix (Or ps)) p = any (mightUnify p) ps
mightUnify p (Fix (Or ps)) = any (mightUnify p) ps
mightUnify (Fix (As _ _ p)) p' = mightUnify p p'
mightUnify p (Fix (As _ _ p')) = mightUnify p p'
mightUnify (Fix (Pattern c _ ps)) (Fix (Pattern c' _ ps')) = c == c' && and (zipWith mightUnify ps ps')
mightUnify (Fix ListPattern{}) (Fix ListPattern{}) = True
mightUnify x@(Fix MapPattern{}) y =
  mightUnifyMap x y
mightUnify (Fix SetPattern{}) (Fix SetPattern{}) = True
mightUnify (Fix Pattern{}) _ = False
mightUnify (Fix ListPattern{}) _ = False
mightUnify (Fix SetPattern{}) _ = False

-- returns true if the specified constructor is an overload of the current pattern and can match it
isValidOverload :: [Fix Pattern]
                -> Metadata BoundPattern
                -> Clause BoundPattern
                -> [Metadata BoundPattern]
                -> Constructor BoundPattern -> Bool
isValidOverload ps (Metadata _ _ _ _ childMeta) cls metaPs less =
  case childMeta less of
    Nothing -> False
    Just metaTs ->
      let items = zipWith3 isValidChild ps metaPs metaTs
      in and items
  where
    isValidChild :: Fix Pattern
                 -> Metadata BoundPattern
                 -> Metadata BoundPattern
                 -> Bool
    isValidChild p metaP@(Metadata _ _ _ _ m) metaT =
      let sortP = getSort metaP
          sortT = getSort metaT
          child = Symbol (SymbolOrAlias (Id "inj" AstLocationNone) [sortT, sortP])
      in sortP == sortT || isJust (m (SymbolConstructor child)) && checkPatternIndex (SymbolConstructor child) metaP (cls,p)

checkPatternIndex :: Constructor BoundPattern
                  -> Metadata BoundPattern
                  -> (Clause BoundPattern, Fix Pattern) -> Bool
checkPatternIndex _ _ (_, Fix Wildcard) = True
checkPatternIndex ix m (c, Fix (As _ _ pat)) = checkPatternIndex ix m (c,pat)
checkPatternIndex _ _ (_, Fix (Variable _ _)) = True
checkPatternIndex ix m (c, Fix (Or ps)) = any (checkPatternIndex ix m) (zip (replicate (length ps) c) ps)
checkPatternIndex (List _ len) _ (_, Fix (ListPattern hd Nothing tl _ _)) = len == (length hd + length tl)
checkPatternIndex (List _ len) _ (_, Fix (ListPattern hd (Just _) tl _ _)) = len >= (length hd + length tl)
checkPatternIndex _ _ (_, Fix ListPattern{}) = False
checkPatternIndex (SymbolConstructor (Symbol (SymbolOrAlias (Id "inj" _) [a,c]))) (Metadata _ _ _ _ meta) (cls, Fix (Pattern ix@(Left (Symbol (SymbolOrAlias name@(Id "inj" _) [b,c']))) _ [p])) =
  let m@(Metadata _ _ _ _ childMeta) = head (fromJust $ meta $ getPatternConstructor ix)
      child = Symbol (SymbolOrAlias name [a,b])
  in c == c' && (a == b || (isJust (childMeta (SymbolConstructor child)) && checkPatternIndex (SymbolConstructor child) m (cls,p)))
checkPatternIndex inj@(SymbolConstructor (Symbol (SymbolOrAlias (Id "inj" _) _))) (Metadata _ _ overloads _ meta) (cls, Fix (Pattern ix _ ps)) =
  let less = overloads $ getPatternConstructor ix
      childMeta = head (fromJust $ meta inj)
  in any (isValidOverload ps childMeta cls $ fromJust $ meta $ getPatternConstructor ix) less

checkPatternIndex ix _ (_, Fix (Pattern ix' _ _)) = ix == getPatternConstructor ix'
checkPatternIndex c m p@(_, Fix MapPattern{}) =
  checkMapPatternIndex mightUnify c m p
checkPatternIndex Empty _ (_, Fix (SetPattern es _ _ _)) = null es
checkPatternIndex HasKey{} _ (_, Fix (SetPattern _ (Just _) _ _)) = True
checkPatternIndex (HasKey _ _ _ (Just p)) _ (c, Fix (SetPattern es Nothing _ _)) = any (mightUnify p) $ map (canonicalizePattern c) es
checkPatternIndex (HasKey _ _ _ Nothing) _ _ = error "TODO: map/set choice"
checkPatternIndex (HasNoKey _ (Just p)) _ (c, Fix (SetPattern es _ _ _)) =
  let canonEs = map (canonicalizePattern c) es
  in p `notElem` canonEs
checkPatternIndex (HasNoKey _ Nothing) _ _ = error "TODO: map/set choice"
checkPatternIndex _ _ (_, Fix SetPattern{}) = error "Invalid map pattern"

addVars :: Maybe (Constructor BoundPattern)
        -> [Clause BoundPattern]
        -> Column Pattern BoundPattern
        -> Occurrence
        -> [Clause BoundPattern]
addVars ix as (Column m ps) o =
  let rows = zip ps as
  in map (\(p, Clause a vars ranges children) -> Clause a (addVarToRow ix o p vars) (addRange ix o p ranges) $ addOverloads m ix (Num 0 o) p children) rows

addVarToRow :: Maybe (Constructor BoundPattern)
            -> Occurrence
            -> Fix Pattern
            -> [VariableBinding]
            -> [VariableBinding]
addVarToRow _ o (Fix (Variable name hookAtt)) vars = VariableBinding name hookAtt o : vars
addVarToRow ix o (Fix (As name hookAtt p)) vars = VariableBinding name hookAtt o : addVarToRow ix o p vars
addVarToRow _ _ (Fix Wildcard) vars = vars
addVarToRow _ _ (Fix (Or{})) _ = error "Unexpanded or pattern"
addVarToRow (Just (SymbolConstructor (Symbol (SymbolOrAlias (Id "inj" _) [a,_])))) o (Fix (Pattern (Left (Symbol (SymbolOrAlias (Id "inj" _) [b,_]))) _ [p])) vars = if a == b then vars else addVarToRow Nothing o p vars
addVarToRow _ _ (Fix Pattern{}) vars = vars
addVarToRow _ _ (Fix (ListPattern _ Nothing _ _ _)) vars = vars
addVarToRow (Just (List _ len)) o (Fix (ListPattern _ (Just p) _ _ _)) vars = addVarToRow Nothing (Num len o) p vars
addVarToRow _ _ (Fix (ListPattern _ (Just _) _ _ _)) vars = vars
addVarToRow c o p@(Fix MapPattern{}) vars =
  addMapVarToRow addVarToRow c o p vars
addVarToRow _ o (Fix (SetPattern [] (Just p) _ _)) vars = addVarToRow Nothing o p vars
addVarToRow _ _ (Fix SetPattern{}) vars = vars

addRange :: Maybe (Constructor BoundPattern)
         -> Occurrence
         -> Fix Pattern
         -> [(Occurrence, Int, Int)]
         -> [(Occurrence, Int, Int)]
addRange (Just (List _ len)) o (Fix (ListPattern hd (Just (Fix (Variable _ _))) tl _ _)) ranges = (Num len o, length hd, length tl) : ranges
addRange _ _ _ ranges = ranges

-- computes the list of injections that are bound to variables in the current pattern as a result of matching on an overload of the current
-- pattern. these injections are bound to variables despite not existing in the current term, so they need to be tracked so they can be
-- created later
addOverloads :: Metadata BoundPattern
             -> Maybe (Constructor BoundPattern)
             -> Occurrence -> Fix Pattern
             -> [(Constructor BoundPattern, VariableBinding)]
             -> [(Constructor BoundPattern, VariableBinding)]
addOverloads (Metadata _ _ _ _ meta) (Just (SymbolConstructor (Symbol (SymbolOrAlias name@(Id "inj" _) [a,_])))) o (Fix (Pattern ix@(Left (Symbol (SymbolOrAlias (Id "inj" _) [b,_]))) _ [p])) children =
  if a == b then -- exact match, don't recurse
    children
  else -- flexible injection, so recurse into child
    let metaB = head (fromJust $ meta $ getPatternConstructor ix)
    in addOverloads metaB (Just (SymbolConstructor $ Symbol (SymbolOrAlias name [a,b]))) o p children
addOverloads (Metadata _ _ overloads _ meta) (Just inj@(SymbolConstructor (Symbol (SymbolOrAlias (Id "inj" _) _)))) o (Fix (Pattern ix _ ps)) children =
  let less = overloads $ getPatternConstructor ix
      metaPs = fromJust (meta $ getPatternConstructor ix)
      (Metadata _ _ _ _ childMeta) = head (fromJust $ meta inj) -- metadata for child of injection
      childMaybe = listToMaybe $ mapMaybe childMeta less
  in case childMaybe of
    Nothing -> children -- no overloads exist
    Just metaTs ->
      let items = concat $ zipWith4 getVar metaPs metaTs ps [0..length metaPs-1] -- compute variable bindings
      in items ++ children
  where
    getVar :: Metadata BoundPattern
           -> Metadata BoundPattern
           -> Fix Pattern
           -> Int
           -> [(Constructor BoundPattern, VariableBinding)]
    getVar metaP metaT p i =
      let vars = addVarToRow Nothing (Inj $ Num i o) p [] -- compute variable bindings for this pattern
          sortP = getSort metaP
          sortT = getSort metaT
          child = Symbol (SymbolOrAlias (Id "inj" AstLocationNone) [sortT, sortP])
          childOverloads = addOverloads metaP (Just (SymbolConstructor child)) (Num i o) p [] -- recurse into child term
      in if sortP == sortT then
        [] -- exact match, so no bindings
      else zip (replicate (length vars) (SymbolConstructor child)) vars ++ childOverloads
addOverloads _ _ _ _ children = children

filterMatrix :: Maybe (Constructor BoundPattern)
             -> ((Clause BoundPattern, Fix Pattern) -> Bool)
             -> (ClauseMatrix Pattern BoundPattern, Occurrence)
             -> ClauseMatrix Pattern BoundPattern
filterMatrix ix checkPattern (ClauseMatrix (PatternMatrix cs@(c : _)) as, o) =
  let filteredRows = zipWith (curry checkPattern) as (getTerms c)
      varsAs = addVars ix as c o
      newAs = filterByList filteredRows varsAs
      newCs = map (filterRows filteredRows) cs
  in ClauseMatrix (PatternMatrix newCs) newAs
  where
    filterRows :: [Bool]
               -> Column Pattern BoundPattern
               -> Column Pattern BoundPattern
    filterRows fr (Column md rs) =
      Column md (filterByList fr rs)
filterMatrix _ _ (cmx,_) = cmx

expandOrPatterns :: ClauseMatrix Pattern BoundPattern -> ClauseMatrix Pattern BoundPattern
expandOrPatterns (ClauseMatrix (PatternMatrix (c : cs)) as) =
  let p0s = getTerms c
  in (ClauseMatrix (PatternMatrix ((expandOrColumn c) : (map (expandOrColumns p0s) cs))) (expandIfOr p0s as))
  where
    expandOrColumn :: Column Pattern BoundPattern -> Column Pattern BoundPattern
    expandOrColumn (Column m ps) = Column m (concatMap expandOrs ps)
    expandOrs :: Fix Pattern -> [Fix Pattern]
    expandOrs (Fix (Or ps)) = ps
    expandOrs p = [p]
    expandOrColumns :: [Fix Pattern] -> Column Pattern BoundPattern -> Column Pattern BoundPattern
    expandOrColumns p0s (Column m ps) = Column m (expandIfOr p0s ps)
    expandIfOr :: [Fix Pattern] -> [a] -> [a]
    expandIfOr p0s items =
      let zipped = zip p0s items
      in concatMap expandRowIfOr zipped
    expandRowIfOr :: (Fix Pattern,a) -> [a]
    expandRowIfOr ((Fix (Or ps)),a) = replicate (length ps) a
    expandRowIfOr (_,a) = [a]
expandOrPatterns cmx = cmx

expandMatrix :: Constructor BoundPattern
             -> ClauseMatrix Pattern BoundPattern
             -> ClauseMatrix Pattern BoundPattern
expandMatrix ix (ClauseMatrix (PatternMatrix (c : cs)) as) =
  ClauseMatrix (PatternMatrix (expandColumn ix c as <> cs)) as
expandMatrix _ _ = error "Cannot expand empty matrix."

getScore :: [Clause BoundPattern]
         -> Column Pattern BoundPattern
         -> Double
getScore cs c@(Column m ps) =
  let score = computeScore m (zip ps cs)
  in if score /= score then error ("found a NaN score: " ++ (show (cs,c))) else score

-- TODO: improve
computeScore :: Metadata BoundPattern
             -> [(Fix Pattern, Clause BoundPattern)] -> Double
computeScore _ [] = 0.0
computeScore m ((Fix (Pattern ix@(Left (Symbol (SymbolOrAlias (Id "inj" _) _))) _ _),_):tl) = (1.0 / (1.0 + fromIntegral (length $ getInjections m (getPatternConstructor ix)))) + computeScore m tl
computeScore m ((Fix (Pattern ix _ _),_):tl) = (1.0 / (1.0 + fromIntegral (length $ getOverloads m $ getPatternConstructor ix))) + computeScore m tl
computeScore m ((Fix ListPattern{}, _) : tl) = 1.0 + computeScore m tl
computeScore m ((Fix (As _ _ pat),c):tl) = computeScore m ((pat,c):tl)
computeScore m ((Fix Wildcard,_):tl) = min 0.0 $ computeScore m tl
computeScore m ((Fix (Variable _ _),_):tl) = min 0.0 $ computeScore m tl
computeScore m ((Fix (Or ps),c):tl) = computeScore m ((zip ps (replicate (length ps) c)) ++ tl)
computeScore m pc@((Fix MapPattern{}, _) : _) =
  computeMapScore computeScore m pc
computeScore m ((Fix (SetPattern [] Nothing _ _),_):tl) = 1.0 + computeScore m tl
computeScore m ((Fix (SetPattern [] (Just p) _ _),c):tl) = computeScore m ((p,c):tl)
computeScore m ((Fix (SetPattern es _ _ _),c):tl) = if computeScore m tl == -1.0 / 0.0 then -1.0 / 0.0 else snd $ computeSetScore c es tl

-- | Gets the best key or element to use from a set or map when computing
-- the score
getBestKey :: Column Pattern BoundPattern
           -> [Clause BoundPattern]
           -> Maybe (Fix BoundPattern)
getBestKey columns@(Column _ (Fix MapPattern{} : _)) clauses =
  getBestMapKey computeScore columns clauses
getBestKey (Column _ (Fix (SetPattern (k:ks) _ _ _):tl)) cs = fst $ computeSetScore (head cs) (k:ks) (zip tl $ tail cs)
getBestKey (Column m (Fix Wildcard:tl)) cs = getBestKey (Column m tl) (tail cs)
getBestKey (Column m (Fix (Variable _ _):tl)) cs = getBestKey (Column m tl) (tail cs)
getBestKey _ _ = Nothing

computeSetScore :: Clause BoundPattern
                -> [Fix Pattern]
                -> [(Fix Pattern, Clause BoundPattern)]
                -> (Maybe (Fix BoundPattern), Double)
computeSetScore c es tl =
  let scores = map (\e -> (if isBound getName c e then Just $ canonicalizePattern c e else Nothing,computeElementScore e c tl)) es
  in maximumBy (comparing snd) scores

minPositiveDouble :: Double
minPositiveDouble = encodeFloat 1 $ fst (floatRange (0.0 :: Double)) - floatDigits (0.0 :: Double)

expandColumn :: Constructor BoundPattern
             -> Column Pattern BoundPattern
             -> [Clause BoundPattern]
             -> [Column Pattern BoundPattern]
expandColumn ix (Column m ps) cs =
  let metas    = expandMetadata ix m
      expanded = map (expandPattern ix metas m) (zip ps cs)
      ps'' = zipWith (curry expandIfJust) expanded cs
  in  zipWith Column metas (transpose ps'')

expandMetadata :: Constructor BoundPattern
               -> Metadata BoundPattern
               -> [Metadata BoundPattern]
expandMetadata ix (Metadata _ _ _ sort ms) =
  fromMaybe (error $ show (ix, sort)) (ms ix)

expandIfJust :: ([(Fix Pattern,
                   Maybe (Constructor BoundPattern,
                          Metadata BoundPattern ))], Clause BoundPattern)
             -> [Fix Pattern]
expandIfJust ([],_) = []
expandIfJust ((p,Nothing):tl,c) = p : expandIfJust (tl,c)
expandIfJust ((p,Just (ix,m)):tl,c) =
  let metas = expandMetadata ix m
  in head (expandIfJust (expandPattern ix metas m (p,c),c)) : expandIfJust (tl,c)

except :: Int -> [a] -> [a]
except i as =
  let (hd,tl) = splitAt i as
  in hd ++ tail tl

expandPattern :: Constructor BoundPattern
              -> [Metadata BoundPattern]
              -> Metadata BoundPattern
              -> (Fix Pattern,Clause BoundPattern)
              -> [(Fix Pattern,
                   Maybe (Constructor BoundPattern,
                          Metadata BoundPattern))]
expandPattern (List _ len) _ _ (Fix (ListPattern hd _ tl _ _), _) = zip (hd ++ replicate (len - length hd - length tl) (Fix Wildcard) ++ tl) (replicate len Nothing)
expandPattern _ _ _ (Fix ListPattern{}, _) = error "invalid list pattern"
expandPattern
  (SymbolConstructor (Symbol (SymbolOrAlias name [a, _]))) _
  (Metadata _ _ _ _ meta)
  (Fix (Pattern (Left ix@(Symbol (SymbolOrAlias (Id "inj" _) [b, _]))) _ [fixedP]), _) =
  [(fixedP,
     if a == b
     then Nothing
     else Just ( SymbolConstructor (Symbol (SymbolOrAlias name [a,b]))
               , head (fromJust $ meta (SymbolConstructor ix))))]
expandPattern
  (SymbolConstructor (Symbol (SymbolOrAlias (Id "inj" _) _))) ms
  (Metadata _ _ overloads _ meta)
  (Fix (Pattern ix hookAtt fixedPs), cls) =
  let less = overloads $ getPatternConstructor ix
      childMeta = head ms
      metaPs = fromJust $ meta $ getPatternConstructor ix
      validLess = case filter (isValidOverload fixedPs childMeta cls metaPs) less of
        [hd] -> hd
        _ -> error "invalid overload pattern"
  in [(Fix $ Pattern (getConstructorPattern validLess) hookAtt $ expandOverload fixedPs childMeta metaPs validLess, Nothing)]
  where
    expandOverload :: [Fix Pattern]
                   -> Metadata BoundPattern
                   -> [Metadata BoundPattern]
                   -> Constructor BoundPattern
                   -> [Fix Pattern]
    expandOverload ps (Metadata _ _ _ _ childMeta) metaPs less =
      let metaTs = fromJust $ childMeta less
      in zipWith3 expandOverload' ps metaPs metaTs
    expandOverload' :: Fix Pattern
                    -> Metadata BoundPattern
                    -> Metadata BoundPattern
                    -> Fix Pattern
    expandOverload' p metaP metaT =
      let sortP = getSort metaP
          sortT = getSort metaT
          child = Symbol (SymbolOrAlias (Id "inj" AstLocationNone) [sortT,sortP])
          maybeChild = if sortP == sortT then Nothing else Just (SymbolConstructor child, metaP)
      in head (expandIfJust ([(p,maybeChild)],cls))
expandPattern Empty _ _ (_, _) = []
expandPattern (NonEmpty _) _ _ (p,_) = [(p,Nothing)]
expandPattern s ms m pc@(Fix MapPattern{},_) =
  expandMapPattern s ms m pc
expandPattern (HasKey _ _ _ (Just p)) _ _ (m@(Fix (SetPattern es f e o)),c) =
  let canonEs = map (canonicalizePattern c) es
      hasElem = elemIndex p canonEs
  in case hasElem of
       Just i -> [(Fix (SetPattern (except i es) f e o), Nothing), (Fix Wildcard,Nothing)]
       Nothing -> [(Fix Wildcard,Nothing), (m,Nothing)]
expandPattern (HasKey _ _ _ Nothing) _ _ _ = error "TODO: map/set choice"
expandPattern (HasNoKey _ _) _ _ (p,_) = [(p,Nothing)]
expandPattern _ _ _ (Fix SetPattern{},_) = error "Invalid set pattern"
expandPattern _ _ _ (Fix (Pattern _ _ fixedPs), _)  = zip fixedPs $ replicate (length fixedPs) Nothing
expandPattern ix ms m (Fix (As _ _ pat), c)         = expandPattern ix ms m (pat, c)
expandPattern _ _ _ (Fix (Or{}), _)                 = error "Unexpanded or pattern"
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
      , "default" Y..= Y.toYaml (case getDefault x of
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
      , "default" Y..= Y.toYaml (case getDefault x of
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

instance Y.ToYaml Alias where
    toYaml (Alias name) = Y.alias name

serializeToYaml :: Free Anchor Alias -> B.ByteString
serializeToYaml = Y.toByteString . Y.toYaml

instance Y.ToYaml (Anchor (Free Anchor Alias)) => Y.ToYaml (Free Anchor Alias) where
    toYaml (Pure a) = Y.toYaml a
    toYaml (Free f) = Y.toYaml f

-- gets the decision tree for handling the leaf of a particular row
getLeaf :: Fringe -> [Fix Pattern] -> Clause BoundPattern -> Fix DecisionTree -> Fix DecisionTree
getLeaf os ps (Clause (Action a rhsVars maybeSideCondition) matchedVars ranges children) next =
  let row = zip os ps
      -- first, add all remaining variable bindings to the clause
      vars = nub $ foldr (\((o,_), p) -> addVarToRow Nothing o p) matchedVars row
      (_, overloadVars) = unzip children
      allVars = vars ++ overloadVars
      -- then group the bound variables by their name
      grouped = foldr (\(VariableBinding name hookAtt o) m -> Map.insert (name,hookAtt) (o : Map.findWithDefault [] (name,hookAtt) m) m) Map.empty allVars
      -- compute the variables bound more than once
      nonlinear = Map.filter ((> 1) . length) grouped
      nonlinearPairs = Map.map (\l -> zip l (tail l)) nonlinear
      -- get some random occurrence bound for each variable in the map
      deduped = Map.foldrWithKey (\(name,_) l -> (:) (name,head l)) [] grouped
      -- sort alphabetically by variable name
      sorted = sortBy (compare `on` fst) deduped
      -- filter by the variables used in the rhs
      filtered = filter (flip elem rhsVars . fst) sorted
      (_, newVars) = unzip filtered
      atomicLeaf = leaf a newVars
      -- check that all occurrences of the same variable are equal
      nonlinearLeaf = Map.foldrWithKey (\(_, hookAtt) -> flip $ foldr $ makeEquality hookAtt) atomicLeaf nonlinearPairs
      sc = case maybeSideCondition of
             -- if there is no side condition, continue
             Nothing -> nonlinearLeaf
                          -- filter by the variables used in the side condition
             Just cond -> let condFiltered = filter (flip elem cond . fst) sorted
                              (_, condVars) = unzip condFiltered
                              newO = SC a
                          -- evaluate the side condition and if it is true, continue, otherwise go to the next row
                          in function (pack $ "side_condition_" ++ show a) newO condVars "BOOL.Bool" (
                                 switchLiteral newO 1 [("1", nonlinearLeaf),
                                                       ("0", next)
                                                      ] Nothing)
      -- fill out the bindings for list range variables
      withRanges = foldr (\(o@(Num _ o'), hd, tl) -> Fix . Function "hook_LIST_range_long" o [o', Lit (pack $ show hd) "MINT.MInt 64", Lit (pack $ show tl) "MINT.MInt 64"] "LIST.List") sc ranges
  in foldr (\(inj, VariableBinding _ h (Inj o)) -> Fix . MakePattern (Inj o) (Fix $ Pattern inj Nothing [Fix $ Variable (Just o) h])) withRanges (fmap (first getConstructorPattern) children)
  where
    makeEquality :: String -> (Occurrence, Occurrence) -> Fix DecisionTree -> Fix DecisionTree
    -- compare the values of the variables with the same name and if they are equal, continue, otherwise go to the next row
    makeEquality hookAtt (o1,o2) dt = function (equalityFun hookAtt) (Equal o1 o2) [o1, o2] "BOOL.Bool" (switchLiteral (Equal o1 o2) 1 [("1", dt), ("0", next)] Nothing)

equalityFun :: String -> Text
equalityFun "BOOL.Bool" = "hook_BOOL_eq"
equalityFun "FLOAT.Float" = "hook_FLOAT_trueeq"
equalityFun "INT.Int" = "hook_INT_eq"
equalityFun "STRING.String" = "hook_KEQUAL_eq"
equalityFun s = error ("unexpected hook: " ++ s)

swapAt :: Int -> Int -> [a] -> [a]
swapAt i j xs
  | i == j = xs
  | i >  j = swapAt j i xs
  | otherwise =
    let elemI = xs !! i
        elemJ = xs !! j
        left = take i xs
        middle = take (j - i - 1) (drop (i + 1) xs)
        right = drop (j + 1) xs
    in left ++ (elemJ:middle) ++ (elemI:right)

getRealScore :: ClauseMatrix Pattern BoundPattern
             -> Column Pattern BoundPattern
             -> Double
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
    getMapOrSetKeys p@(Fix MapPattern{}) =
      getMapKeys p
    getMapOrSetKeys _ = []
    getVariables :: Fix Pattern -> [String]
    getVariables (Fix (Variable name _)) = [name]
    getVariables (Fix (As name _ p)) = name : getVariables p
    getVariables (Fix (Or ps)) = concatMap getVariables ps
    getVariables (Fix (Pattern _ _ ps)) = concatMap getVariables ps
    getVariables (Fix (ListPattern _ _ _ _ o)) = getVariables o
    getVariables p@(Fix MapPattern{}) =
      getMapVariables getVariables p
    getVariables (Fix (SetPattern _ _ _ o)) = getVariables o
    getVariables (Fix Wildcard) = []

compilePattern :: (ClauseMatrix Pattern BoundPattern, Fringe)
               -> Fix DecisionTree
compilePattern = compilePattern'
  where
    compilePattern' :: (ClauseMatrix Pattern BoundPattern, Fringe)
                    -> Fix DecisionTree
    compilePattern' (cm@(ClauseMatrix pm@(PatternMatrix cs) ac), os) =
      case ac of
        [] -> Fix Fail
        hd:tl ->
          if isWildcardRow pm then
            -- if there is only one row left, then try to match it and fail the matching if it fails
            -- otherwise, if it fails, try to match the remainder of hte matrix
            if length ac == 1 then getLeaf os (firstRow pm) hd failure else getLeaf os (firstRow pm) hd (compilePattern' (ClauseMatrix (notFirstRow pm) tl, os))
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
              ls = map (`mSpecialize` cm') s₁
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
    isWildcardRow :: PatternMatrix Pattern BoundPattern -> Bool
    isWildcardRow = all isWildcard . firstRow
    isWildcard :: Fix Pattern -> Bool
    isWildcard (Fix Wildcard) = True
    isWildcard (Fix (Variable _ _)) = True
    isWildcard (Fix (As _ _ pat)) = isWildcard pat
    isWildcard (Fix (Or ps)) = or $ map isWildcard ps
    isWildcard (Fix Pattern{}) = False
    isWildcard (Fix (MapPattern [] [] (Just p) _ _)) = isWildcard p
    isWildcard (Fix MapPattern{}) = False
    isWildcard (Fix (SetPattern [] (Just p) _ _)) = isWildcard p
    isWildcard (Fix SetPattern{}) = False
    isWildcard (Fix ListPattern{}) = False
    -- constructs a tree to test the current occurrence against each possible match in turn
    equalLiteral :: Occurrence
                 -> String
                 -> [(Text, (ClauseMatrix Pattern BoundPattern, Fringe))]
                 -> Maybe (ClauseMatrix Pattern BoundPattern, Fringe)
                 -> Fix DecisionTree
    -- if no specializations remain and a default exists, consume the occurrence and continue with the default
    equalLiteral litO _ [] (Just d) = Fix $ Switch litO L { getSpecializations = [], getDefault = Just $ compilePattern' d }
    -- if no specializations remain and no default exists, fail the match
    equalLiteral _ _ [] Nothing = Fix Fail
    -- consume each specialization one at a time and try to match it
    -- if it succeseds, consume the occurrence and continue with the specialized matrix
    -- otherweise, test the next literal
    equalLiteral litO hookName ((name,spec):tl) d =
      let newO = Lit name $ pack hookName
          eqO = Equal litO newO
      in Fix $ MakePattern newO (Fix (Pattern (Right (Literal name)) (Just hookName) [])) $
             Fix $ Function (equalityFun hookName) eqO [litO, newO] "BOOL.Bool" $
                 Fix $ SwitchLiteral eqO 1 $ L [("1", Fix $ Switch litO L
                                                             { getSpecializations = [], getDefault = Just $ compilePattern' spec }),
                                                 ("0", equalLiteral litO hookName tl d)
                                                ] Nothing
    -- construct a tree to test the length of the list and bind the elements of the list to their occurrences
    listPattern :: Occurrence
                -> [(Text, (ClauseMatrix Pattern BoundPattern, Fringe))]
                -> Maybe (ClauseMatrix Pattern BoundPattern, Fringe)
                -> [Constructor BoundPattern]
                -> Column Pattern BoundPattern
                -> Fix DecisionTree
    listPattern listO ls d signature firstCol =
      let newO = Size listO
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
    expandListPattern :: Occurrence
                      -> (Constructor BoundPattern,
                           (ClauseMatrix Pattern BoundPattern, Fringe))
                      -> Fix DecisionTree
    expandListPattern listO (List _ i, cm') =
      foldl (listGet listO Nothing) (compilePattern' cm') [0..i-1]
    expandListPattern _ _ = error "invalid list pattern"
    -- get each element of the list and bind it to the occurrence, then compile the default matrix
    expandListPatternDefault :: Occurrence -> (Int,Int) -> (ClauseMatrix Pattern BoundPattern, Fringe) -> Fix DecisionTree
    expandListPatternDefault listO (hd,tl) cm' =
      foldl (listGet listO $ Just (hd,tl)) (foldl (listGet listO Nothing) (compilePattern' cm') [0..hd-1]) [hd..hd+tl-1]
    -- generate a single list lookup operation to bind one element of the list against its occurrence
    listGet :: Occurrence -> Maybe (Int, Int) -> Fix DecisionTree -> Int -> Fix DecisionTree
    listGet _ _ (Fix Fail) _ = Fix Fail
    listGet listO l dt o =
      Fix $ Function "hook_LIST_get_long" (Num o listO)
        [listO, case l of
                  Nothing -> Lit (pack $ show o) "MINT.MInt 64"
                  Just (hd,tl) -> Lit (pack $ show (o-tl-hd)) "MINT.MInt 64"
        ] "STRING.String" dt
    mapPattern :: Occurrence
               -> [(Text, (ClauseMatrix Pattern BoundPattern, Fringe))]
               -> Maybe (ClauseMatrix Pattern BoundPattern, Fringe)
               -> [Constructor BoundPattern]
               -> Column Pattern BoundPattern
               -> [Clause BoundPattern]
               -> Fix DecisionTree
    mapPattern mapO ls d s c cs =
      let newO = Size mapO
      -- if Empty is in the signature, test whether the map is empty or not.
      in if Empty `elem` s then
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
                         Fix $ Function "hook_MAP_lookup_null" (Value (tshow k) mapO) [mapO, newO] "STRING.String" $
                           Fix $ Function "hook_MAP_remove" (Rem (tshow k) mapO) [mapO, newO] "MAP.Map" $
                             Fix $ CheckNull (Value (tshow k) mapO) $ L
                               { getSpecializations = map (second compilePattern') ls
                               , getDefault = compilePattern' <$> d
                               }
    setPattern :: Occurrence
               -> [(Text, (ClauseMatrix Pattern BoundPattern, Fringe))]
               -> Maybe (ClauseMatrix Pattern BoundPattern, Fringe)
               -> [Constructor BoundPattern]
               -> Column Pattern BoundPattern
               -> [Clause BoundPattern] -> Fix DecisionTree
    setPattern setO ls d s c cs =
      let newO = Size setO
      -- if Empty is in the signature, test whether the set is empty or not.
      in if Empty `elem` s then
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
                         Fix $ Function "hook_SET_in" (Value (tshow k) setO) [newO, setO] "BOOL.Bool" $
                           Fix $ Function "hook_SET_remove" (Rem (tshow k) setO) [setO, newO] "SET.Set" $
                             Fix $ SwitchLiteral (Value (tshow k) setO) 1 $ L
                               { getSpecializations = map (second compilePattern') ls
                               , getDefault = compilePattern' <$> d
                               }


shareDt :: Fix DecisionTree -> Free Anchor Alias
shareDt =
  snd . computeSharing Map.empty
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
