# Binary KORE Format

This document specifies version `1.1.0` of the binary format for KORE patterns.
Note that in places, the format is slightly redundant to allow for a future
extension to entire KORE definitions.

## Preface

Byte values are written as space-separated pairs, similarly to the default
output of `xxd`. For example, `ABCD EFGH` represents the byte sequence `0xAB`,
`0xCD`, `0xEF`, `0xGH` (where `A`, `B`, ... are hexadecimal digits).

The serialization format assumes little-endianness throughout. For example, the
32-bit integer 1 would be serialized as `0100 0000`.

## Length Fields

Lengths and arities are encoded as variable-length sequences of bytes. Each byte
in the sequence uses its highest bit as a continuation marker if it is set, and
the low 7 bits as a component of the length data.

For example, consider the following sequences of bits:
```
0000 0001
^
no more to follow


1000 0011 0000 0001
^         ^
another   byte follows
```

The first represents a length of 1, while the second represents `3 + (1 << 7)`,
or 131. Because the continuation bit in the first byte is set, the data value of
the next byte (low 7 bits of value 1) should be shifted left by 7 places, and
its continuation bit considered additionally.

The maximum value that can be encoded using this scheme is `2^63`, which
corresponds to 9 bytes with all possible data bits set. Sequences of more than 9
bytes are malformed.

## Header

Binary KORE terms should begin with the byte sequence `7f4b 4f52 45`, followed by
three 16-bit values representing the major, minor and patch versions of the
format used.

For example, a file of version `1.0.0` should begin:
```
7f4b 4f52 4501 0000 0000 00
```

## Strings

Two representations for strings are available: direct and interned.

Directly-encoded strings are the byte `01`, followed by a length field, then
the bytes of the string.

Interned strings are the byte `02`, followed by a length field. This
field represents the number of bytes to count _backwards_ in the buffer (from
the byte immediately following the backreference count) to find the _first
byte of the length_ of a directly-encoded string.

For example, consider the following sequence of bytes:
```
0104 5678 5678 0207 ff
```

First, the string `VxVx` is encoded directly with its length (the single
uncontinued byte `04`). Splitting the prefix and length from the characters,
this is `0104 5678 5678`). Then, a variable-length backreference of 7 bytes is
encoded (`0207`). This backreference is counted from the byte `ff`, and
therefore points to the `04` byte at the start of the directly-encoded string's
length.

## KORE

This section describes how KORE symbols, patterns and sorts are encoded.

### Sorts

Sort variables are encoded as the byte `07` followed by the variable name as a
string.

Composite sorts first encode each of their arguments (recursively, following
this schema), then the byte `06`. This is followed by a length field
representing the sort arity, then the sort name as a string.

### Symbols

First, the formal sort arguments to the symbol are serialized in sequence
following the sort schema above.

Then, the byte `08` is emitted, followed by the number of formal arguments as a
length field.

Finally, the name of the symbol is emitted as a string following the
representation above.

### Patterns

KORE string patterns are encoded as the byte `05`, then the string data.

Composite patterns encode each of their arguments (recursively, following this
schema), then the constructor symbol (as described above). These are followed by
the byte `04`, and a variable-length arity for the pattern.

Pattern variables encode their sort argument, followed by the byte `09`, then a
variable as described below.

### Variables

KORE variables are encoded as the byte `0D`, followed by a string representing
the variable name.

## Manipulating Binary Terms

This binary format is designed for easy composition through concatenation. To do
so, the leading 10 bytes (header and version number must be stripped from each
term).

To apply a constructor to a list of arguments (after stripping their prefixes):
1. Begin with a new header and version prefix.
1. Concatenate the serialized argument terms to the header.
2. Concatenate the serialized constructor with no arguments.
3. Drop the arity at the end of the file, and append the appropriate arity bytes.

This representation may be less compact than if it had been constructed from the
equivalent textual KORE (due to missed opportunities for string interning across
multiple terms).

# Caveats

The textual KORE format supports the `\{left,right}-assoc` symbols, which are
implemented as syntactic sugar to allow associative lists to be written in a
flat format. These symbols do not exist in the parsed AST corresponding to a
textual KORE pattern, only in the textual representation.

Because the binary KORE format uses a postfix representation for constructor
symbols, supporting this sugared format would require a complex pre-processing
step. Consequently, binary KORE files should _not_ contain uses of `\left-assoc`
or `\right-assoc`. This does not affect the existing textual KORE format.

# Previous Versions

## `1.0.0`

Version `1.0.0` of this format used _fixed length_ fields for string lengths,
backreferences and constructor arities. The lengths of these fields are as
follows; each is encoded directly as a little-endian integer value:

| Data                  | Bytes |
| :--                   |   --: |
| String length         |     4 |
| String backreference  |     4 |
| Sort arity            |     2 |
| Term arity            |     2 |
| Symbol argument count |     2 |

The deserializer supports terms encoded using version `1.0.0`.
