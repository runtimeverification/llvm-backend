#include <cstdio>

int segfault() {
  int * x = NULL;
  int y = *x;
  return y;
}

void errtest() {
  fprintf( stderr, "Error" );
}

int main(int argc, char ** argv) {
  if (argc == 2) {
    segfault();
  } else {
    errtest();
  }
  return 0;
}
