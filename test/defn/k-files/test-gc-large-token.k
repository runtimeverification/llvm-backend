// Regression test for the large-token young-generation GC bug.
//
// A String token whose payload exceeds BLOCK_SIZE (1 MiB) is flagged NOT_YOUNG by
// init_with_len, but kore_alloc_token used to place it in the young semispace anyway.
// The collector never migrates a NOT_YOUNG object out of the young from-space (which is
// reclaimed on every collection), so such a token dangled: a later allocation overwrote
// its bytes and a subsequent collection walked a corrupted header.
//
// This definition (a) builds a single > 1 MiB String token, (b) keeps it live in the
// <keep> cell across (c) heavy subsequent allocation that forces collections and reuses
// the abandoned region, then (d) reads the token's length back. On the buggy runtime the
// length is corrupted (or the run crashes); with the fix the token lives in the old
// generation and its length survives intact.
module TEST
  imports STRING-BUFFER
  imports INT
  imports STRING
  imports MAP

  configuration <T>
    <k> $PGM:Pgm </k>
    <buffer> .StringBuffer </buffer>
    <keep> "":String </keep>
    <m> .Map </m>
  </T>

  syntax Cmd ::= appendN(Int, String)
               | freeze()
               | churn(Int)
  syntax Pgm ::= List{Cmd,";"}

  // Build a > 1 MiB buffer by appending S, I times.
  rule <k> appendN(I, S) ; Cmds => appendN(I -Int 1, S) ; Cmds </k>
       <buffer> SB => SB +String S </buffer>
    requires I >Int 0
  rule <k> appendN(I, _) ; Cmds => Cmds </k>
    requires I ==Int 0

  // Materialize the buffer as one > 1 MiB String token and stash it as a live GC root.
  rule <k> freeze() ; Cmds => Cmds </k>
       <buffer> SB </buffer>
       <keep> _ => StringBuffer2String(SB) </keep>

  // Allocate heavily to force collections after freeze() and reuse the abandoned region.
  rule <k> churn(I) ; Cmds => churn(I -Int 1) ; Cmds </k>
       <m> M => M [ Int2String(I) <- I ] </m>
    requires I >Int 0
  rule <k> churn(I) ; Cmds => Cmds </k>
    requires I ==Int 0

  // Read the kept token back: its length must have survived the collections intact.
  // Clear the bulky cells so the checked output stays small.
  rule <k> .Pgm => lengthString(S) </k>
       <keep> S => "" </keep>
       <buffer> _ => .StringBuffer </buffer>
       <m> _ => .Map </m>
endmodule
