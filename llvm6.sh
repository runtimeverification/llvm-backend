#!/bin/bash
export PATH="/usr/local/opt/flex/bin:/usr/local/opt/llvm@6/bin:$PATH"
export LDFLAGS="-L/usr/local/opt/bison/lib -L/usr/local/opt/llvm@6/lib -L/usr/local/opt/flex/lib"
export CPPFLAGS="-I/usr/local/opt/llvm@6/include -I/usr/local/opt/flex/include"
export PATH="/usr/local/opt/bison/bin:$PATH"
export FLEX_EXECUTABLE="/usr/local/opt/flex/bin/flex"
export BISON_EXECUTABLE="/usr/local/opt/bison/bin/bison"
