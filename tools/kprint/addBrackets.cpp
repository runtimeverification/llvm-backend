#include "kllvm/ast/AST.h"

#include <memory>

using namespace kllvm;

enum Fixity {
  EMPTY = 0,
  BARE_LEFT = 1,
  BARE_RIGHT = 2,
  BARE_BOTH = 3
};

int getPosition(std::string terminals, int position) {
  int terminalPos = 0;
  int ntIdx = 0;
  for (; terminalPos < terminals.size(); terminalPos++) {
    if (terminals[terminalPos] == '0') {
      if (ntIdx == position) {
        break;
      }
      ntIdx++;
    }
  }
  return terminalPos;
}

bool hasTerminalAtIdx(std::string terminals, int position) {
  if (position < 0 || position >= terminals.length()) {
    return false;
  }
  return terminals[position] == '1';
}

Fixity getFixity(int position, KORECompositePattern *outer, PrettyPrintData const& data) {
  int result = EMPTY;
  std::string terminals = data.terminals.at(outer->getConstructor()->getName());
  int terminalPos = getPosition(terminals, position);
  if (!hasTerminalAtIdx(terminals, terminalPos+1)) {
    result |= BARE_RIGHT;
  }
  if (!hasTerminalAtIdx(terminals, terminalPos-1)) {
    result |= BARE_LEFT;
  }
  return (Fixity) result;
}

Fixity getFixity(KORECompositePattern *outer, PrettyPrintData const& data) {
  std::string name = outer->getConstructor()->getName();
  int result = EMPTY;
  if (data.terminals.at(name)[0] == '0') {
    result = result | BARE_LEFT;
  }
  if (data.terminals.at(name)[data.terminals.at(name).size()-1] == '0') {
    result = result | BARE_RIGHT;
  }
  return (Fixity)result;
}

KORECompositePattern *getLeftCapture(KORECompositePattern *previousLeftCapture, KORECompositePattern *outer, int position, PrettyPrintData const& data) {
  Fixity fixity = getFixity(outer, data);
  if (position == 0 && (fixity & BARE_LEFT)) {
    return previousLeftCapture;
  } else {
    return outer;
  }
}

KORECompositePattern *getRightCapture(KORECompositePattern *previousRightCapture, KORECompositePattern *outer, int position, PrettyPrintData const& data) {
  Fixity fixity = getFixity(outer, data);
  if (position == outer->getArguments().size() - 1 && (fixity & BARE_RIGHT)) {
    return previousRightCapture;
  } else {
    return outer;
  }
}

bool lessThanEq(PrettyPrintData const& data, KORESort *s1, KORESort *s2) {
  return *s1 == *s2 || (data.subsorts.count(s1) && data.subsorts.at(s1).count(s2));
}

bool isPriorityWrong(KORECompositePattern *outer, KORECompositePattern *inner, int position, PrettyPrintData const& data) {
  std::string outerName = outer->getConstructor()->getName();
  std::string innerName = inner->getConstructor()->getName();
  KORESort *innerSort = inner->getSort().get();
  KORESort *outerSort = outer->getConstructor()->getArguments()[position].get();
  if (!lessThanEq(data, innerSort, outerSort)) {
    return true;
  }
  if (data.priorities.count(outerName) && data.priorities.at(outerName).count(innerName)) {
    return true;
  }
  std::string terminals = data.terminals.at(outerName);
  int terminalPos = getPosition(terminals, position);
  if (data.leftAssoc.count(outerName) && data.leftAssoc.at(outerName).count(innerName) && terminalPos == terminals.size() - 1) {
    return true;
  }
  if (data.rightAssoc.count(outerName) && data.rightAssoc.at(outerName).count(innerName) && terminalPos == 0) {
    return true;
  }
  return false;
}

bool requiresBracketWithSimpleAlgorithm(KORECompositePattern *outer, KORECompositePattern *leftCapture, KORECompositePattern *rightCapture, KOREPattern *inner, int position, PrettyPrintData const& data) {
  if (auto innerComposite = dynamic_cast<KORECompositePattern *>(inner)) {
    std::string innerName = innerComposite->getConstructor()->getName();
    if (innerName == outer->getConstructor()->getName()) {
      if (data.assoc.count(innerName)) {
        return false;
      }
    }
    if (innerName == "\\dv") {
      return false;
    }
    Fixity fixity = getFixity(position, outer, data);
    if (fixity == EMPTY) {
      return false;
    }
    bool priority = isPriorityWrong(outer, innerComposite, position, data);
    if (priority) {
      return true;
    }
    if (data.terminals.at(innerName) == "0") {
      return false;
    }
    Fixity innerFixity = getFixity(innerComposite, data);
    if ((innerFixity & BARE_RIGHT) && rightCapture != nullptr) {
      bool inversePriority = isPriorityWrong(innerComposite, rightCapture, innerComposite->getArguments().size() - 1, data);
      Fixity rightCaptureFixity = getFixity(rightCapture, data);
      if (!inversePriority && (rightCaptureFixity & BARE_LEFT)) {
        return true;
      }
    }
    if ((innerFixity & BARE_LEFT) && leftCapture != nullptr) {
      bool inversePriority = isPriorityWrong(innerComposite, leftCapture, 0, data);
      Fixity leftCaptureFixity = getFixity(leftCapture, data);
      if (!inversePriority && (leftCaptureFixity & BARE_RIGHT)) {
        return true;
      }
    }
    return false;
  } else {
    return false;
  }
}

sptr<KOREPattern> addBrackets(sptr<KOREPattern> inner, KORECompositePattern *outer, KORECompositePattern *leftCapture, KORECompositePattern *rightCapture, int position, PrettyPrintData const& data) {
  if (requiresBracketWithSimpleAlgorithm(outer, leftCapture, rightCapture, inner.get(), position, data)) {
    sptr<KORESort> outerSort = outer->getConstructor()->getArguments()[position];
    sptr<KORESort> innerSort = inner->getSort();
    for (auto &entry : data.brackets) {
      bool isCorrectOuterSort = lessThanEq(data, entry.first, outerSort.get());
      if (isCorrectOuterSort) {
        for (KORESymbol *s : entry.second) {
          bool isCorrectInnerSort = lessThanEq(data, innerSort.get(), s->getArguments()[0].get());
          if (isCorrectInnerSort) {
            sptr<KORECompositePattern> result = KORECompositePattern::Create(s);
            result->addArgument(inner);
            return result;
          }
        }
      }
    }
    sptr<KORECompositePattern> result = KORECompositePattern::Create("bracket");
    result->addArgument(inner);
    result->getConstructor()->addSort(innerSort);
    return result;
  }
  return inner;
}

sptr<KOREPattern> addBrackets(sptr<KOREPattern> t, KORECompositePattern *previousLeftCapture, KORECompositePattern *previousRightCapture, PrettyPrintData const& data) {
  if (auto outer = dynamic_cast<KORECompositePattern *>(t.get())) {
    if (outer->getConstructor()->getName() == "\\dv") {
      return t;
    }
    std::vector<sptr<KOREPattern>> newItems;

    sptr<KORECompositePattern> result = KORECompositePattern::Create(outer->getConstructor());
    int position = 0;
    for (auto &inner : outer->getArguments()) {
      KORECompositePattern *leftCapture = getLeftCapture(previousLeftCapture, outer, position, data);
      KORECompositePattern *rightCapture = getRightCapture(previousRightCapture, outer, position, data);
      sptr<KOREPattern> newInner = addBrackets(inner, outer, leftCapture, rightCapture, position, data);
      newInner = addBrackets(newInner, leftCapture, rightCapture, data);
      result->addArgument(newInner);
      position++;
    }
    return result;
  } else {
    return t;
  }
}

sptr<KOREPattern> addBrackets(sptr<KOREPattern> t, PrettyPrintData const& data) {
  return addBrackets(t, nullptr, nullptr, data);
}
