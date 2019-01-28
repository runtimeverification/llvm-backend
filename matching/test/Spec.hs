{-# LANGUAGE FlexibleInstances #-}
{-# LANGUAGE InstanceSigs      #-}
{-# LANGUAGE OverloadedStrings #-}

module Main where

import           Data.Bits
                 ( shiftL )
import           Data.Functor.Foldable
                 ( Fix (..) )
import           Data.List
                 ( concat, transpose )
import           Data.Proxy
                 ( Proxy (..) )
import           Data.Semigroup
                 ( (<>) )
import           Kore.AST.Common
                 ( AstLocation (..), Id (..), Sort (..), SortActual (..) )

import qualified Data.Map.Strict as M

import Test.Tasty
       ( TestTree, defaultMain, testGroup )
import Test.Tasty.HUnit
       ( testCase, (@?=) )

import Everything
import Pattern.Class
import Pattern.Type hiding
       ( getMetadata )

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
  toPattern (Cns i l) = Fix (Pattern (Right $ Literal "cons") Nothing [toPattern i, toPattern l])
  toPattern Nil     = Fix (Pattern (Right $ Literal "nil") Nothing  [])
  toPattern Wld     = Fix Wildcard
  toPattern (Var v) = Fix (Variable v "STRING.String")

instance IsPattern IntPat where
  toPattern :: IntPat -> Fix Pattern
  toPattern (IntLit i) = Fix (Pattern (Right $ Literal $ show i) (Just "MINT.MInt 32") [])
  toPattern IntWld     = Fix Wildcard
  toPattern (IntVar v) = Fix (Variable v "INT.Int")

instance HasMetadata IntPat where
  getMetadata :: Proxy IntPat -> Metadata BoundPattern
  getMetadata _ = Metadata (shiftL 1 32) (const []) (const []) (SortActualSort (SortActual (Id "Int" AstLocationNone) [])) f
    where
      f :: Constructor BoundPattern -> Maybe [Metadata BoundPattern]
      f _ = Just []

instance HasMetadata Lst where
  getMetadata :: Proxy Lst -> Metadata BoundPattern
  getMetadata _ =
    let m = M.fromList
                    [ (LiteralConstructor $ Literal "nil", []) -- Nil
                    , (LiteralConstructor $ Literal "cons", [ getMetadata (Proxy :: Proxy IntPat)
                               , getMetadata (Proxy :: Proxy Lst)
                               ]) -- Cns Lst (1)
                    ]
    in Metadata (toInteger $ length m) (const []) (const []) (SortActualSort (SortActual (Id "Lst" AstLocationNone) [])) (flip M.lookup m)

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

mkLstPattern :: [([Lst],Maybe [String])] -> (ClauseMatrix Pattern BoundPattern, Fringe)
mkLstPattern pats =
  let as = take (length ls) [1..]
      (ls, conds) = unzip pats
      vs = map vars ls
      as' = zipWith3 Action as vs conds
      md = getMetadata (Proxy :: Proxy Lst)
      cs = fmap (Column md . (toPattern <$>)) (transpose ls)
  in case mkClauseMatrix cs as' of
       Right matrix -> matrix
       Left  msg    -> error $ "Invalid definition: " ++ show msg

defaultPattern :: (ClauseMatrix Pattern BoundPattern, Fringe)
defaultPattern =
  mkLstPattern [ ([Nil, Wld], Nothing)
               , ([Wld, Nil], Nothing)
               , ([Wld, Wld], Nothing) ]

appendPattern :: (ClauseMatrix Pattern BoundPattern, Fringe)
appendPattern =
  mkLstPattern [ ([Nil, Wld], Nothing)
               , ([Wld, Nil], Nothing)
               , ([Cns IntWld Wld, Cns IntWld Wld], Nothing) ]

appendBindPattern :: (ClauseMatrix Pattern BoundPattern, Fringe)
appendBindPattern =
  mkLstPattern [ ([Nil, Var "as"], Nothing)
               , ([Var "bs", Nil], Nothing)
               , ([Cns (IntVar "b") (Var "bs"), Cns (IntVar "a") (Var "as")], Nothing) ]

appendCondPattern :: (ClauseMatrix Pattern BoundPattern, Fringe)
appendCondPattern =
  mkLstPattern [ ([Nil, Var "as"], Nothing)
               , ([Var "bs", Nil], Nothing)
               , ([Cns (IntVar "b") (Var "bs"), Cns (IntVar "a") (Var "as")], Just ["as", "b"]) ]


matchHeadPattern :: (ClauseMatrix Pattern BoundPattern, Fringe)
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
        switch (Num 1 Base) [ ("nil", leaf 1 [])
               , ("cons", (switch (Num 2 Base) [ ("nil", leaf 2 [])
                                   , ("cons", leaf 3 [])
                                   ] Nothing ))
               ] Nothing
  , testCase "Naive compilation of the append pattern with variable bindings" $
      compilePattern appendBindPattern @?=
        switch (Num 1 Base) [ ("nil", leaf 1 [Num 2 Base])
               , ("cons", (switch (Num 2 Base) [ ("nil", leaf 2 [Num 1 Base])
                                   , ("cons", leaf 3 [(Num 0 $ Num 2 Base), (Num 1 $ Num 2 Base), (Num 0 $ Num 1 Base), (Num 1 $ Num 1 Base)])
                                   ] Nothing ))
               ] Nothing
  , testCase "Naive compilation of the append pattern with side condition" $
      compilePattern appendCondPattern @?=
        switch (Num 1 Base) [ ("nil", leaf 1 [Num 2 Base])
               , ("cons", (switch (Num 2 Base) [ ("nil", leaf 2 [Num 1 Base])
                                   , ("cons", (function "side_condition_3" (SC 3) [(Num 1 $ Num 2 Base), (Num 0 $ Num 1 Base)] "BOOL.Bool" (switchLiteral (SC 3) 1 [("1", leaf 3 [(Num 0 $ Num 2 Base), (Num 1 $ Num 2 Base), (Num 0 $ Num 1 Base), (Num 1 $ Num 1 Base)]), ("0", failure)] Nothing)))
                                   ] Nothing ))
               ] Nothing
  , testCase "Yaml serialization" $
      (serializeToYaml $ shareDt $ compilePattern $ appendBindPattern) @?=
        "&4\n" <>
        "specializations:\n" <>
        "- - nil\n" <>
        "  - &0\n" <>
        "    action:\n" <>
        "    - 1\n" <>
        "    - - - '2'\n" <>
        "- - cons\n" <>
        "  - &3\n" <>
        "    specializations:\n" <>
        "    - - nil\n" <>
        "      - &1\n" <>
        "        action:\n" <>
        "        - 2\n" <>
        "        - - - '1'\n" <>
        "    - - cons\n" <>
        "      - &2\n" <>
        "        action:\n" <>
        "        - 3\n" <>
        "        - - - '0'\n" <>
        "            - '2'\n" <>
        "          - - '1'\n" <>
        "            - '2'\n" <>
        "          - - '0'\n" <>
        "            - '1'\n" <>
        "          - - '1'\n" <>
        "            - '1'\n" <>
        "    default: null\n" <>
        "    occurrence:\n" <>
        "    - '2'\n" <>
        "default: null\n" <>
        "occurrence:\n" <>
        "- '1'\n"
  , testCase "Naive compilation of integer literal patterns" $
      compilePattern matchHeadPattern @?=
        switch (Num 1 Base) [ ("cons", (switchLiteral (Num 0 $ Num 1 Base) 32 [ ("0", leaf 1 [])
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
