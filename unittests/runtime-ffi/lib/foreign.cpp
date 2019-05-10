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
}
