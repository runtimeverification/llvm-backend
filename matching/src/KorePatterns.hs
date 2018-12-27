{-# LANGUAGE GADTs           #-}
{-# LANGUAGE PatternSynonyms #-}

module KorePatterns where

import Kore.AST.Common
       ( And (..), Application (..), Bottom (..), Equals (..), Implies (..),
       Pattern (..), Rewrites (..), Sort (..), SymbolOrAlias (..), Top (..) )
import Kore.AST.Kore
       ( pattern KoreObjectPattern, KorePattern )
import Kore.AST.MetaOrObject
       ( Object (..) )

pattern Implies_ :: Sort Object    -- ^ The sort level is fixed for our patterns
                 -> KorePattern v  -- ^ The hypothesis
                 -> KorePattern v  -- ^ The conclusion
                 -> KorePattern v  -- ^ The resulting pattern
pattern Implies_ sort hyp cnc = KoreObjectPattern (ImpliesPattern (Implies sort hyp cnc))

pattern App_ :: SymbolOrAlias Object -- ^ The symbol that points to the function that is being invoked
             -> [KorePattern v]      -- ^ The arguments passed to the function
             -> KorePattern v        -- ^ The resulting pattern
pattern App_ fnSymbol args =
  KoreObjectPattern (ApplicationPattern (Application fnSymbol args))

pattern And_ :: Sort Object    -- ^ The sort level is fixed for our patterns
             -> KorePattern v  -- ^ The first / left pattern
             -> KorePattern v  -- ^ The second / right pattern
             -> KorePattern v  -- ^ The resulting pattern
pattern And_ sort fst snd = KoreObjectPattern (AndPattern (And sort fst snd))

pattern Equals_ :: Sort Object    -- ^ The sort of the operand
                -> Sort Object    -- ^ The sort of the result
                -> KorePattern v  -- ^ The first operand
                -> KorePattern v  -- ^ The second operand
                -> KorePattern v  -- ^ The resulting pattern
pattern Equals_ opS reS fst snd = KoreObjectPattern (EqualsPattern (Equals opS reS fst snd))

pattern Rewrites_ :: Sort Object     -- ^ This is both the sort of the operands and the sort of the result and it is always at Object level.
                  -> KorePattern v   -- ^ The first / left pattern
                  -> KorePattern v   -- ^ The second / right pattern
                  -> KorePattern v   -- ^ The resulting pattern
pattern Rewrites_ s fst snd = KoreObjectPattern (RewritesPattern (Rewrites s fst snd))

pattern Top_ :: Sort Object    -- ^ The sort level is fixed for our patterns
             -> KorePattern v  -- ^ The resulting pattern
pattern Top_ s = KoreObjectPattern (TopPattern (Top s))

pattern Bottom_ :: Sort Object
                -> KorePattern v
pattern Bottom_ s = KoreObjectPattern (BottomPattern (Bottom s))
