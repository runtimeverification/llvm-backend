#include <cstdarg>

extern "C" {
  struct point {
    int x;
    int y;
  };

  struct point2 {
    struct point p;
  };

  int x = 1;

  int timesTwo(int x) {
    return x * 2;
  }

  unsigned int utimesTwo(unsigned int x) {
    return x * 2;
  }

  int times(int x, int y) {
    return x * y;
  }

  int getX(void) {
    return x;
  }

  void increaseX(void) {
    x++;
  }

  int timesPoint(struct point p) {
    return p.x * p.y;
  }

  int timesPoint2(struct point2 p) {
    return p.p.x * p.p.y;
  }

  struct point constructPoint(int x, int y) {
    return {.x = x, .y = y};
  }

  int addInts(int n, ...) {
    int sum = 0;
    va_list vl;
    va_start(vl, n);

    for (int i = 0; i < n; i++) {
      sum += va_arg(vl, int);
    }

    va_end(vl);

    return sum;
  }

  int pointerTest(int * x) {
    return x == 0 ? 0 : *x;
  }
}
