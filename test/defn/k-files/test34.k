requires "json.md"
module TEST
  imports JSON

  rule 0 => String2JSON("[{\"foo\": 0, \"bar\": 0.0, \"baz\": true, \"qux\": \"foo\", \"null\": null}]")
  rule 1 => JSON2String([{"foo": 0, "bar": 0.0, "baz": true, "qux": "foo", "null": null}])
endmodule
