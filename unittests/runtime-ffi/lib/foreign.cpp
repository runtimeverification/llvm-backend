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

int times_two(int x) {
  return x * 2;
}

unsigned int utimes_two(unsigned int x) {
  return x * 2;
}

int times(int x, int y) {
  return x * y;
}

int get_x(void) {
  return x;
}

void increase_x(void) {
  x++;
}

int times_point(struct point p) {
  return p.x * p.y;
}

int times_point2(struct point2 p) {
  return p.p.x * p.p.y;
}

struct point construct_point(int x, int y) {
  return {.x = x, .y = y};
}

int add_ints(int n, ...) {
  int sum = 0;
  va_list vl;
  va_start(vl, n);

  for (int i = 0; i < n; i++) {
    sum += va_arg(vl, int);
  }

  va_end(vl);

  return sum;
}

int pointer_test(int *x) {
  return x == 0 ? 0 : *x;
}
}
