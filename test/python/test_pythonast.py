# RUN: mkdir -p %t
# RUN: llvm-kompile pythonast --python %py-interpreter --python-output-dir %t
# RUN: cd %t
# RUN: %py-interpreter %s

import _kllvm
