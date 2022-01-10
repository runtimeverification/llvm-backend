{ mkShell
, llvm-backend
, llvm-backend-matching
, mavenix
}:

mkShell {
  inputsFrom = [
    llvm-backend
    llvm-backend-matching
  ];

  nativeBuildInputs = [
    mavenix.cli
  ];
}
