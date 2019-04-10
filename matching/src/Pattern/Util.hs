module Pattern.Util
  ( except
  ) where

except :: Int -> [a] -> [a]
except i as =
  let (hd,tl) = splitAt i as
  in hd ++ tail tl
