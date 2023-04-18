// emoji.k

module EMOJI-SYNTAX
    syntax Emoji ::= r"🙁|🙂" [token]
endmodule

module EMOJI
    imports EMOJI-SYNTAX
    rule <k> 🙁 => 🙂 ... </k>
endmodule
