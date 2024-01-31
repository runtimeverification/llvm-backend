module BYTES-COW-2
  imports BYTES
  imports INT

  syntax Foo ::= "a" | "b" | "c" | "d"
  syntax Holder ::= f(Bytes) | "empty"

  configuration
    <k> $PGM </k>
    <a> empty </a>
    <b> empty </b>


  rule
    <k> a => b </k>
    <a> _ => f(b"first") </a>

  rule
    <k> b => c </k>
    <a> f(_) #as A </a>
    <b> _ => A </b>

  rule
    <k> c => d </k>
    <b> f(B) => f(replaceAtBytes(B, 0, b"bad__")) </b>
endmodule
