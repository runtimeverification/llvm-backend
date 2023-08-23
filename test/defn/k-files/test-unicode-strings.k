
// unicode.k

module UNICODE-SYNTAX
    imports STRING-SYNTAX
    syntax Strings ::= List{String,","}
endmodule

module UNICODE
    imports UNICODE-SYNTAX
    imports STRING

    configuration <k> $PGM:Strings </k>
    		  <out> .Strings </out>
    rule <k> SS:Strings => . ...</k>
    	 <out> _ => goAll(SS) </out>
    
    syntax Strings ::= goAll(Strings) [function]
    syntax String ::= go(String) [function]
    
    rule goAll(.Strings) => .Strings
    rule goAll(S:String, SS:Strings) => go(S) , goAll(SS)

    rule go(S:String) => S [owise]

    rule go("A\x00B") => "goodA\x00B"
    
    rule go("\x24") => "good$"
    
    rule go("£") => "good\xA3"
    
    rule go("\u0418") => "goodИ"
    
    rule go("ह") => "good\u0939"
    
    rule go("\u20AC") => "good€"
    
    rule go("한") => "good\uD55C"
    
    rule go("𐍈") => "good\U00010348"

endmodule
