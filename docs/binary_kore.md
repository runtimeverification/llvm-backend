# Binary KORE Format

This document specifies version `1.0.0` of the binary format for KORE patterns.
Note that in places, the format is slightly redundant to allow for a future
extension to entire KORE definitions.

## Preface

Byte values are written as space-separated pairs, similarly to the default
output of `xxd`. For example, `ABCD EFGH` represents the byte sequence `0xAB`,
`0xCD`, `0xEF`, `0xGH` (where `A`, `B`, ... are hexadecimal digits).

The serialization format assumes little-endianness throughout. For example, the
32-bit integer 1 would be serialized as `0100 0000`.

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

Directly-encoded strings are the byte `01`, followed by a 4-byte length, then
the bytes of the string.

Interned strings are the byte `02`, followed by a 4-byte integer value. This
value represents the number of bytes to count _backwards_ in the buffer (from
the byte immediately following the backreference count) to find the _first
byte of the length_ of a directly-encoded string.

For example, consider the following sequence of bytes:
```
0104 0000 0056 7856 7802 0d00 0000 ff
```

First, the string `wVwV` is encoded directly with its length (splitting the
prefix and length from the characters, `01 0400 0000 5678 5678`). Then, a
backreference of 13 bytes is encoded (`02 0d00 0000`). This backreference is
counted from the byte `ff`, and therefore points to the `04` byte at the start
of the directly-encoded string's length.

## KORE

This section describes how KORE symbols, patterns and sorts are encoded.

### Sorts

Sort variables are encoded as the byte `07` followed by the variable name as a
string.

Composite sorts first encode each of their arguments (recursively, following
this schema), then the byte `06`. This is followed by a 2-byte arity, then the
sort name as a string.

### Symbols

The serialized format for KORE symbols is redundant. Currently, only symbols as
part of a pattern are representable. Parts of this representation are reserved
for future extensions.

First, the formal sort arguments to the symbol are serialized in sequence
following the sort schema above.

Then, the byte sequence `08 0000` is emitted, followed by the number of formal
arguments (as a 16-bit integer), then by `0000`.

Finally, the name of the symbol is emitted as a string following the
representation above.

### Patterns

KORE string patterns are encoded as the byte `05`, then the string data.

Composite patterns encode each of their arguments (recursively, following this
schema), then the constructor symbol (as described above). These are followed by
the byte `04`, and a 2-byte arity for the pattern.

## Manipulating Binary Terms

This binary format is designed for easy composition through concatenation. To do
so, the leading 10 bytes (header and version number must be stripped from each
term).

To apply a constructor to a list of arguments (after stripping their prefixes):
1. Begin with a new header and version prefix.
1. Concatenate the serialized argument terms to the header.
2. Concatenate the serialized constructor with no arguments.
3. Drop the last 2 bytes of the file, and append the appropriate arity bytes.

This representation may be less compact than if it had been constructed from the
equivalent textual KORE (due to missed opportunities for string interning across
multiple terms).
