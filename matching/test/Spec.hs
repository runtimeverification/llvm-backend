{-# LANGUAGE FlexibleInstances #-}
{-# LANGUAGE InstanceSigs      #-}

module Main where

import           Data.Functor.Foldable (Fix (..))
import           Data.List             (transpose)
import           Data.Proxy            (Proxy (..))

import           Test.Tasty            (TestTree, defaultMain, testGroup)
import           Test.Tasty.HUnit      (testCase, (@?=))

import           Pattern               hiding (getMetadata)
import           Pattern.Class

data Lst  = Cns Lst -- index 1
          | Nil     -- index 0
          | Wld     -- wildcard
          deriving (Show, Eq)

cNil :: Index
cNil = 0

cCons :: Index
cCons = 1

instance IsPattern Lst where
  toPattern :: Lst -> Fix Pattern
  toPattern (Cns l) = Fix (Pattern cCons [Fix Wildcard, toPattern l])
  toPattern Nil     = Fix (Pattern cNil  [])
  toPattern Wld     = Fix Wildcard

instance HasMetadata Lst where
  getMetadata :: Proxy Lst -> Metadata
  getMetadata _ = Metadata
                    [ Metadata [] -- Nil
                    , Metadata [ Metadata []
                               , getMetadata (Proxy :: Proxy Lst)
                               ] -- Cns Lst (1)
                    ]

mkLstPattern :: [[Lst]] -> ClauseMatrix
mkLstPattern ls =
  let as = take (length ls) [1..]
      md = getMetadata (Proxy :: Proxy Lst)
      cs = fmap (Column md . (toPattern <$>)) (transpose ls)
  in case mkClauseMatrix cs as of
       Right matrix -> matrix
       Left  msg    -> error $ "Invalid definition: " ++ show msg

defaultPattern :: ClauseMatrix
defaultPattern =
  mkLstPattern [ [Nil, Wld]
               , [Wld, Nil]
               , [Wld, Wld] ]

appendPattern :: ClauseMatrix
appendPattern =
  mkLstPattern [ [Nil, Wld]
               , [Wld, Nil]
               , [Cns Wld, Cns Wld] ]

tests :: TestTree
tests = testGroup "Tests" [appendTests]

appendTests :: TestTree
appendTests = testGroup "Basic pattern compilation"
  [ testCase "Naive compilation of the append pattern" $
      compilePattern appendPattern @?=
        switch [ (cNil, leaf 1)
               , (cCons, simplify (simplify
                           (switch [ (cNil, leaf 2)
                                   , (cCons, leaf 3)
                                   ] Nothing )))
               ] Nothing
  ]

{-compileTests :: TestTree
compileTests = testGroup "Compiling Kore to Patterns"
  [ testCase "Compilation of imp.kore" $
     parseDefinition "imp.kore" @?= parseDefinition "imp.kore"
  ]-}

main :: IO ()
main = defaultMain tests
