extern "C" {
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
}
