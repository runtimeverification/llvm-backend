# Binary KORE Format 2.0

This document specifies a new binary format for KORE patterns. It is not intended to be backwards compatible with the Binary KORE 1.0 format, and will exist side by side with that format. This format has slightly different design goals, namely:

* Serialization of terms must be as fast as possible
* Terms do not need to be composable
* Optimized for serializing multiple terms in sequence
* Not optimized for very large terms.

## Preface

Byte values are written as space-separated pairs, similarly to the default
output of `xxd`. For example, `ABCD EFGH` represents the byte sequence `0xAB`,
`0xCD`, `0xEF`, `0xGH` (where `A`, `B`, ... are hexadecimal digits).

The serialization format assumes little-endianness throughout. For example, the
32-bit integer 1 would be serialized as `0100 0000`.

## Header

A stream of binary KORE terms depends on a header designed to make serialization as fast as possible by storing data about the terms that might be serialized. The header does not necessarily have to be serialized in the same stream as the terms being serialized, but it must be referenced when deserializing.

The header begins with a 4-byte magic number: the sequence `7f4b 5232`. It is followed by a 4-byte integer representing the version number (currently 1).

Following the version number are 3 4-byte integers: the size of the string table, the size of the sort table, and the size of the symbol table, represented as the number of entries, in that order.

### String table

Following the size of the symbol table is the string table itself. It contains a number of entries equal to the size serialized above. Each entry consists of the following:

1. A 4-byte integer length of the string in bytes.
2. The zero-terminated string itself.

### Sort table

Following the last entry in the string table is the sort table. It contains a number of entries equal to the size serialized above. Each entry consists of the following:

1. A 4-byte integer representing a 0-indexed array offset in the string table.
2. A 1-byte size representing the number of sort parameters of the sort.
3. For each sort parameter, a 4-byte integer representing the offset of the parameter in the sort table.

### Symbol table

Following the last entry in the sort table is the symbol table. It contains a number of entries equal to the size serialized above. Each entry consists of the following:

1. A 4-byte integer representing a 0-indexed array offset in the string table.
2. A 1-byte size representing the number of sort parameters of the symbol.
3. A 1-byte size representing the arity of the symbol.
4. For each sort parameter, a 4-byte integer representing the offset of the parameter in the sort table.

## Terms

Only concrete terms are currently supported by this serialization format. We do not support object or sort variables yet. This may change in a future version of this format.

Terms can be divided into two categories:

1. String patterns
2. Composite patterns.

### String patterns

A string pattern begins with the byte `00`. It is followed by:

1. A 8-byte integer representing the length of the string.
2. The zero-terminated string itself.

Strings in patterns do not utilize the string table because the string table is intended to be generated statically, and the strings that we may wish to serialize do not exist at that point in time.

### Composite patterns

A composite pattern begins with the byte `01`. It is followed by:

1. A 4-byte integer representing the offset of the symbol in the symbol table.
2. For each child of the symbol, the serialized data associated with the child.

## Serialization algorithm

A brief note is left here to explain exactly how to serialize terms using this format. We assume a definition exists with N tags and M sorts.

* The header has N + M + 1 strings in the string table. Strings 0 to N - 1 are the names of the symbols with the corresponding tags. Strings N to N + M - 1 are the names of the sorts of the definition. String N + M is '\dv'.$
* The header has M sorts, corresponding to each sort in the definition. M may not exactly be equal to the number of sort declarations in the kore definition if the definition has parametric sorts, since each instantiation of a parametric symbol exists separately in the sort table.
* The header has N + M symbols. Symbols 0 to N - 1 are the symbols with the corresponding tags. Symbols N to N + M - 1 are the symbol '\dv' instantiated with each sort in the sort table.
* Builtin constants are serialized with the assistance of a recursive descent function which is, for each constant, passed by its parent the '\dv' symbol index associated with its corresponding sort.
* Builtin collections are serialized with the assistance of a recursive descent function which is, for each collection, passed by its parent the unit, element, and concatenation symbol indices associated with its corresponding sort.
* String tokens are serialized the same way as builtin constants.
* Symbols with zero children are serialized by simply serializing their 32-bit tag.
* Symbols with nonzero children are serialized by serializing their 32-bit tag, followed by recursively serializing each child.
