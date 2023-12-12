{ mkShell, llvm-backend, llvm-backend-matching, mavenix-cli }:

mkShell {
  inputsFrom = [ llvm-backend llvm-backend-matching ];

  nativeBuildInputs = [ mavenix-cli ];
}
