#ifndef LOCATION_H
#define LOCATION_H

#include <string>

namespace kllvm {
namespace parser {

class position {
public:
  std::string filename;
  unsigned line;
  unsigned column;

  void lines(int count = 1) {
    if (count) {
      column = 1;
      line = add_(line, count, 1);
    }
  }

  void columns(int count = 1) {
    column = add_(column, count, 1);
  }

private:
  static unsigned add_(unsigned lhs, int rhs, int min) {
    return static_cast<unsigned>(std::max(min, static_cast<int>(lhs) + rhs));
  }
};

inline position&
operator+= (position& res, int width)
{
  res.columns(width);
  return res;
}
 
template <typename YYChar>
std::basic_ostream<YYChar>&
operator<< (std::basic_ostream<YYChar>& ostr, const position& pos) {
  ostr << pos.filename << ':';
  return ostr << pos.line << '.' << pos.column;
}

class location {
public:
  location(std::string filename):
    begin({filename, 1, 1}), end({filename, 1, 1}) {}

  position begin;
  position end;

  void step() {
    begin = end;
  }

  void columns(int count = 1) {
    end += count;
  }

  void lines(int count = 1) {
    end.lines(count);
  }

};

template <typename YYChar>
std::basic_ostream<YYChar>&
operator<< (std::basic_ostream<YYChar>& ostr, const location& loc) {
  unsigned end_col = 0 < loc.end.column ? loc.end.column - 1 : 0;
  ostr << loc.begin;
  if (loc.begin.filename != loc.end.filename)
    ostr << '-' << loc.end.filename << ':' << loc.end.line << '.' << end_col;
  else if (loc.begin.line < loc.end.line)
    ostr << '-' << loc.end.line << '.' << end_col;
  else if (loc.begin.column < end_col)
    ostr << '-' << end_col;
  return ostr;
}

}
}

#endif
