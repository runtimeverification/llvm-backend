{-# LANGUAGE FlexibleInstances #-}
{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE InstanceSigs      #-}

module Main where

import           Data.Bits             (shiftL)
import           Data.Functor.Foldable (Fix (..))
import           Data.List             (transpose,concat)
import qualified Data.Map.Strict       as M
import           Data.Proxy            (Proxy (..))
import           Data.Semigroup        ((<>))
import           Kore.AST.Common       (Id (..), Sort(..), SortActual(..), AstLocation(..))

import           Test.Tasty            (TestTree, defaultMain, testGroup)
import           Test.Tasty.HUnit      (testCase, (@?=))

import           Pattern               hiding (getMetadata)
import           Pattern.Class

data IntPat = IntLit Int
            | IntWld
            | IntVar String
            deriving (Show, Eq)

data Lst  = Cns IntPat Lst -- index 1
          | Nil     -- index 0
          | Wld     -- wildcard
          | Var String
          deriving (Show, Eq)

instance IsPattern Lst where
  toPattern :: Lst -> Fix Pattern
  toPattern (Cns i l) = Fix (Pattern (Right "cons") Nothing [toPattern i, toPattern l])
  toPattern Nil     = Fix (Pattern (Right "nil") Nothing  [])
  toPattern Wld     = Fix Wildcard
  toPattern (Var v) = Fix (Variable v)

instance IsPattern IntPat where
  toPattern :: IntPat -> Fix Pattern
  toPattern (IntLit i) = Fix (Pattern (Right $ show i) (Just "MINT.MInt 32") [])
  toPattern IntWld     = Fix Wildcard
  toPattern (IntVar v) = Fix (Variable v)

instance HasMetadata IntPat where
  getMetadata :: Proxy IntPat -> Metadata
  getMetadata _ = Metadata (shiftL 1 32) (const []) (SortActualSort (SortActual (Id "Int" AstLocationNone) [])) f
    where
      f :: Constructor -> Maybe [Metadata]
      f _ = Just []

instance HasMetadata Lst where
  getMetadata :: Proxy Lst -> Metadata
  getMetadata _ =
    let m = M.fromList
                    [ (Right "nil", []) -- Nil
                    , (Right "cons", [ getMetadata (Proxy :: Proxy IntPat)
                               , getMetadata (Proxy :: Proxy Lst)
                               ]) -- Cns Lst (1)
                    ]
    in Metadata (toInteger $ length m) (const []) (SortActualSort (SortActual (Id "Lst" AstLocationNone) [])) (flip M.lookup m)

vars :: [Lst] -> [String]
vars l = concat (map varLst l)
  where
    varLst :: Lst -> [String]
    varLst Nil = []
    varLst Wld = []
    varLst (Var s) = [s]
    varLst (Cns i l') = varInt i <> varLst l'
    varInt :: IntPat -> [String]
    varInt (IntLit _) = []
    varInt IntWld = []
    varInt (IntVar s) = [s]

mkLstPattern :: [([Lst],Maybe [String])] -> (ClauseMatrix, [Occurrence])
mkLstPattern pats =
  let as = take (length ls) [1..]
      (ls, conds) = unzip pats
      vs = map vars ls
      as' = zipWith3 Action as vs conds 
      md = getMetadata (Proxy :: Proxy Lst)
      cs = fmap (mkColumn md . (toPattern <$>)) (transpose ls)
  in case mkClauseMatrix cs as' of
       Right matrix -> matrix
       Left  msg    -> error $ "Invalid definition: " ++ show msg

defaultPattern :: (ClauseMatrix, [Occurrence])
defaultPattern =
  mkLstPattern [ ([Nil, Wld], Nothing)
               , ([Wld, Nil], Nothing)
               , ([Wld, Wld], Nothing) ]

appendPattern :: (ClauseMatrix, [Occurrence])
appendPattern =
  mkLstPattern [ ([Nil, Wld], Nothing)
               , ([Wld, Nil], Nothing)
               , ([Cns IntWld Wld, Cns IntWld Wld], Nothing) ]

appendBindPattern :: (ClauseMatrix, [Occurrence])
appendBindPattern =
  mkLstPattern [ ([Nil, Var "as"], Nothing)
               , ([Var "bs", Nil], Nothing)
               , ([Cns (IntVar "b") (Var "bs"), Cns (IntVar "a") (Var "as")], Nothing) ]

appendCondPattern :: (ClauseMatrix, [Occurrence])
appendCondPattern =
  mkLstPattern [ ([Nil, Var "as"], Nothing)
               , ([Var "bs", Nil], Nothing)
               , ([Cns (IntVar "b") (Var "bs"), Cns (IntVar "a") (Var "as")], Just ["as", "b"]) ]


matchHeadPattern :: (ClauseMatrix, [Occurrence])
matchHeadPattern =
  mkLstPattern [ ([Cns (IntLit 0) Wld], Nothing)
               , ([Cns (IntLit 1) Wld], Nothing)
               , ([Cns (IntLit (-1)) Wld], Nothing)
               , ([Cns (IntLit 1000000) Wld], Nothing) ]

tests :: TestTree
tests = testGroup "Tests" [appendTests]

appendTests :: TestTree
appendTests = testGroup "Basic pattern compilation"
  [ testCase "Naive compilation of the append pattern" $
      compilePattern appendPattern @?=
        switch [1] [ ("nil", leaf 1 [])
               , ("cons", swap 2
                           (switch [2] [ ("nil", leaf 2 [])
                                   , ("cons", leaf 3 [])
                                   ] Nothing ))
               ] Nothing
  , testCase "Naive compilation of the append pattern with variable bindings" $
      compilePattern appendBindPattern @?=
        switch [1] [ ("nil", leaf 1 [[2]])
               , ("cons", swap 2
                           (switch [2] [ ("nil", leaf 2 [[1]])
                                   , ("cons", leaf 3 [[0, 2], [1, 2], [0, 1], [1, 1]])
                                   ] Nothing ))
               ] Nothing
  , testCase "Naive compilation of the append pattern with side condition" $
      compilePattern appendCondPattern @?=
        switch [1] [ ("nil", leaf 1 [[2]])
               , ("cons", swap 2
                           (switch [2] [ ("nil", leaf 2 [[1]])
                                   , ("cons", (function "side_condition_3" [[1, 2], [0, 1]] "BOOL.Bool" (switchLiteral [0, 0] 1 [("1", leaf 3 [[0, 2], [1, 2], [0, 1], [1, 1]]), ("0", failure)] Nothing)))
                                   ] Nothing ))
               ] Nothing
  , testCase "Yaml serialization" $
      (serializeToYaml $ shareDt $ compilePattern $ appendBindPattern) @?= 
        "&5\n" <>
        "specializations:\n" <>
        "- - nil\n" <>
        "  - &0\n" <>
        "    action:\n" <>
        "    - 1\n" <>
        "    - - - 2\n" <>
        "- - cons\n" <>
        "  - &4\n" <>
        "    swap:\n" <>
        "    - 2\n" <>
        "    - &3\n" <>
        "      specializations:\n" <>
        "      - - nil\n" <>
        "        - &1\n" <>
        "          action:\n" <>
        "          - 2\n" <>
        "          - - - 1\n" <>
        "      - - cons\n" <>
        "        - &2\n" <>
        "          action:\n" <>
        "          - 3\n" <>
        "          - - - 0\n" <>
        "              - 2\n" <>
        "            - - 1\n" <>
        "              - 2\n" <>
        "            - - 0\n" <>
        "              - 1\n" <>
        "            - - 1\n" <>
        "              - 1\n" <>
        "      default: null\n" <>
        "      occurrence:\n" <>
        "      - 2\n" <>
        "default: null\n" <>
        "occurrence:\n" <>
        "- 1\n"
  , testCase "Naive compilation of integer literal patterns" $
      compilePattern matchHeadPattern @?=
        switch [1] [ ("cons", (switchLiteral [0, 1] 32 [ ("0", leaf 1 [])
                                     , ("1", leaf 2 [])
                                     , ("-1", leaf 3 [])
                                     , ("1000000", leaf 4 [])
                                     ] (Just failure) ))
               ] (Just failure)
  ]
{-compileTests :: TestTree
compileTests = testGroup "Compiling Kore to Patterns"
  [ testCase "Compilation of imp.kore" $
     parseDefinition "imp.kore" @?= parseDefinition "imp.kore"
  ]-}

main :: IO ()
main = defaultMain tests
