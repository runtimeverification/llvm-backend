module KorePatterns where

import Control.Comonad.Trans.Cofree
       ( CofreeF (..) )
import Data.Functor.Foldable as Recursive


import Kore.AST.Common
       ( And (..), Application (..), Bottom (..), Equals (..), Implies (..),
       Pattern (..), Rewrites (..), SymbolOrAlias (..), Top (..) )
import Kore.AST.Kore
       ( KorePattern, UnifiedPattern(..) )
import Kore.AST.MetaOrObject
       ( Object (..), Meta (..) )
import Kore.Sort
       ( Sort (..) )

pattern KoreObjectPattern :: Functor domain
                          => Pattern Object domain variable
                              (KorePattern domain variable annotation)
                          -> KorePattern domain variable annotation
pattern KoreObjectPattern pat <- (Recursive.project -> _ :< UnifiedObjectPattern pat)

pattern KoreMetaPattern :: Functor domain
                        => Pattern Meta domain variable
                            (KorePattern domain variable annotation)
                        -> KorePattern domain variable annotation
pattern KoreMetaPattern pat <- (Recursive.project -> _ :< UnifiedMetaPattern pat)

pattern Implies_ :: Functor domain
                 => Sort Object    -- ^ The sort level is fixed for our patterns
                 -> KorePattern domain variable annotation  -- ^ The hypothesis
                 -> KorePattern domain variable annotation  -- ^ The conclusion
                 -> KorePattern domain variable annotation  -- ^ The resulting pattern
pattern Implies_ sort hyp cnc <- KoreObjectPattern (ImpliesPattern (Implies sort hyp cnc))

pattern App_ :: Functor domain
             => SymbolOrAlias Object -- ^ The symbol that points to the function that is being invoked
             -> [KorePattern domain variable annotation]      -- ^ The arguments passed to the function
             -> KorePattern domain variable annotation        -- ^ The resulting pattern
pattern App_ fnSymbol args <-
  KoreObjectPattern (ApplicationPattern (Application fnSymbol args))

pattern And_ :: Functor domain
             => Sort Object    -- ^ The sort level is fixed for our patterns
             -> KorePattern domain variable annotation  -- ^ The first / left pattern
             -> KorePattern domain variable annotation  -- ^ The second / right pattern
             -> KorePattern domain variable annotation  -- ^ The resulting pattern
pattern And_ sort fst snd <- KoreObjectPattern (AndPattern (And sort fst snd))

pattern Equals_ :: Functor domain
                => Sort Object    -- ^ The sort of the operand
                -> Sort Object    -- ^ The sort of the result
                -> KorePattern domain variable annotation  -- ^ The first operand
                -> KorePattern domain variable annotation  -- ^ The second operand
                -> KorePattern domain variable annotation  -- ^ The resulting pattern
pattern Equals_ opS reS fst snd <- KoreObjectPattern (EqualsPattern (Equals opS reS fst snd))

pattern Rewrites_ :: Functor domain
                  => Sort Object     -- ^ This is both the sort of the operands and the sort of the result and it is always at Object level.
                  -> KorePattern domain variable annotation   -- ^ The first / left pattern
                  -> KorePattern domain variable annotation   -- ^ The second / right pattern
                  -> KorePattern domain variable annotation   -- ^ The resulting pattern
pattern Rewrites_ s fst snd <- KoreObjectPattern (RewritesPattern (Rewrites s fst snd))

pattern Top_ :: Functor domain
             => Sort Object    -- ^ The sort level is fixed for our patterns
             -> KorePattern domain variable annotation  -- ^ The resulting pattern
pattern Top_ s <- KoreObjectPattern (TopPattern (Top s))

pattern Bottom_ :: Functor domain
                => Sort Object
                -> KorePattern domain variable annotation
pattern Bottom_ s <- KoreObjectPattern (BottomPattern (Bottom s))
