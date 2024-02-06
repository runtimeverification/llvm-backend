#ifndef RBTREE_HEADER_H
#define RBTREE_HEADER_H

/* Adapted from https://github.com/BartoszMilewski/Okasaki/tree/master/RBTree */
/* Implementing algorithms from                                               *
 * https://matt.might.net/papers/germane2014deletion.pdf                      */

#include "runtime/fmt_error_handling.h"
#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace rb_tree {

//---               Ordered map on top of a red-black tree.                ---//

// 1. No red node has a red child.
// 2. Every path from root to empty node contains the same
// number of black nodes.

template <class T, class V>
class RBTree {
  // Colors used by the red black tree: Red (R), Black (B), and Double Black
  // (BB). BB is a transitory color that allows to temporarily preserve the
  // black invariant when certain operations occur, and is gradually removed
  // while they are being completed. A red-black tree is never expected to have
  // any nodes or leaves of color BB after an operation is performed on it.
  enum class Color { R, B, BB };

  // Return the "previous" color, i.e., the one containing one less black.
  static Color minus_one_color(Color c) {
    switch (c) {
    case Color::B: return Color::R;
    case Color::BB: return Color::B;
    default: KLLVM_HOOK_INVALID_ARGUMENT("Unexpected color");
    }
  }

  // Virtual class, for a node of the red-black tree.
  struct Node {
    Node(Color c)
        : c_(c)
        , s_(0) { }
    Color c_; // Color of this tree Node
    size_t s_; // Size of the tree with root Node

    // Return true if this object is a Leaf.
    virtual bool is_leaf() const = 0;
    virtual ~Node() = default;
  };

  // A Leaf of the red-black tree.
  struct Leaf : public Node {
    // Create a new Leaf object with the given color. Only B and BB colors are
    // valid arguments.
    Leaf(Color c)
        : Node(c) {
      assert(c == Color::B || c == Color::BB);
    }
    virtual bool is_leaf() const override { return true; }
    virtual ~Leaf() = default;
  };

  // An internal node of the red-black tree.
  struct InternalNode : public Node {
    // Create a new InternalNode object with the given lft and rgt children,
    // key and value, and color.
    InternalNode(
        Color c, std::shared_ptr<Node> lft, T key, V val,
        std::shared_ptr<Node> rgt)
        : Node(c)
        , lft_(lft)
        , data_(key, val)
        , rgt_(rgt) {
      this->s_ = 1 + lft_->s_ + rgt_->s_;
    }
    std::shared_ptr<Node> lft_; // Left child
    std::pair<T, V> data_; // data_.first: Node key. data_.second: Node value.
    std::shared_ptr<Node> rgt_; // Right child

    virtual bool is_leaf() const override { return false; }
    virtual ~InternalNode() = default;
  };

  // Create an empty red-black tree, with the specified color. Only B and BB
  // are valid colors for this constructor.
  RBTree(Color c)
      : root_(std::make_shared<Leaf>(c)) { }

  // Create a red-black tree, with a root of the spefified color, key and value,
  // and children lft and rgt.
  RBTree(Color c, RBTree const &lft, T key, V val, RBTree const &rgt)
      : root_(
          std::make_shared<InternalNode>(c, lft.root_, key, val, rgt.root_)) {
    assert(lft.empty() || lft.root_key() < key);
    assert(rgt.empty() || key < rgt.root_key());
  }

  explicit RBTree(std::shared_ptr<Node> node)
      : root_(node) { }

  // Return this Node's color when it is not empty.
  Color root_color() const {
    assert(!empty());
    return root_->c_;
  }

  // Return this Node's color when it is empty.
  Color leaf_color() const {
    assert(empty());
    return root_->c_;
  }

public:
  // Create an empty red-black tree.
  RBTree()
      : root_(std::make_shared<Leaf>(Color::B)) { }

  // Create a red-black tree with elements from the container designated by the
  // beginning and end iterator arguments. The container should contain elements
  // of type std::pair<T,V>.
  template <class I>
  RBTree(I b, I e) {
    RBTree t(Color::B);
    for_each(b, e, [&t](std::pair<T, V> const &p) {
      t = t.inserted(p.first, p.second);
    });
    root_ = t.root_;
  }

  // Return true if this tree is empty.
  bool empty() const { return root_->is_leaf(); }

  // Return the key stored in the root Node of this tree.
  T const &root_key() const {
    assert(!empty());
    InternalNode const *r = static_cast<InternalNode const *>(root_.get());
    return r->data_.first;
  }

  // Return the value stored in the root Node of this tree.
  V const &root_val() const {
    assert(!empty());
    InternalNode const *r = static_cast<InternalNode const *>(root_.get());
    return r->data_.second;
  }

  // Return a const reference to the data (key-value pair) stored in the root
  // Node of this tree.
  std::pair<T, V> const &root_data() const {
    assert(!empty());
    InternalNode const *r = static_cast<InternalNode const *>(root_.get());
    return r->data_;
  }

  /* WARNING: The following method returns a non-const reference in order to  *
   * enable altering this data structure (and potentially ones implemented on *
   * top of it, namely RangeMap) in place. The red-black tree is intended to  *
   * be immutable: the exposed functionality should not generally alter the   *
   * tree in place. This API is not intended to change the structure or data  *
   * of the tree, e.g., key-value pairs, colors, child-parent relations. It   *
   * is intended for internal use, available so that data orthogonal to the   *
   * data structure can be altered, e.g., GC bits, memory locations of blocks *
   * that contain the nodes, etc (see include/runtime/header.h). Only use     *
   * this API to request a non-const reference if you in fact need to edit    *
   * the data structure in place for a specific reason, e.g. garbage          *
   * collection.                                                              */
  // Return a reference to the data (key-value pair) stored in the root Node of
  // this tree.
  std::pair<T, V> &root_data_mutable() {
    assert(!empty());
    InternalNode *r = static_cast<InternalNode *>(root_.get());
    return r->data_;
  }

  // Return the left subtree of this tree.
  RBTree left() const {
    assert(!empty());
    InternalNode const *r = static_cast<InternalNode const *>(root_.get());
    return RBTree(r->lft_);
  }

  // Return the right subtree of this tree.
  RBTree right() const {
    assert(!empty());
    InternalNode const *r = static_cast<InternalNode const *>(root_.get());
    return RBTree(r->rgt_);
  }

  // Return the size of this tree, i.e., the number of non-leaf nodes.
  size_t size() const { return root_->s_; }

  // Return true if key x is found in this tree. Otherwise, return false.
  bool contains(T const &x) const {
    if (empty()) {
      return false;
    }
    T const &y = root_key();
    if (x < y) {
      return left().contains(x);
    } else if (y < x) {
      return right().contains(x);
    } else {
      return true;
    }
  }

  // Return the corresponding value if key x is found in this tree. Otherwise,
  // throw an exception.
  V const &at(T const &x) const {
    if (empty()) {
      KLLVM_HOOK_INVALID_ARGUMENT("Key not found for treemap lookup");
    }
    T const &y = root_key();
    if (x < y) {
      return left().at(x);
    } else if (y < x) {
      return right().at(x);
    } else {
      return root_val();
    }
  }

  // Return a new red-black tree that contains all key-value pairs in this
  // tree plus the key value pair [x -> v]. If key x was already in this tree,
  // key x will be associated with value v in the resulting tree instead.
  RBTree inserted(T const &x, V const &v) const { return ins(x, v).blacken(); }

  // Return a new red-black tree that does not contain the key-value pair for
  // key x, if any.
  RBTree deleted(T const &x) const { return redden().del(x); }

  // Return a red-black tree that is the concatenation of this tree and tree t.
  // Throw an exception if the sets of the keys in this tree and tree t are not
  // disjoint.
  RBTree concat(RBTree const &t) const {
    RBTree res = *this;
    for_each(t, [&res, this](T const &x, V const &v) {
      if (!contains(x)) {
        res = res.inserted(x, v);
      } else {
        KLLVM_HOOK_INVALID_ARGUMENT("Duplicate keys in map concatenation");
      }
    });
    return res;
  }

  // This method throws an exception if the red invariant does not hold for
  // this red-black tree.
  // Red invariant: No red node has a red child.
  void assert_red_invariant() const {
    if (!empty()) {
      if (root_color() == Color::BB) {
        KLLVM_HOOK_INVALID_ARGUMENT("Red invariant failed");
      }
      auto lft = left();
      auto rgt = right();
      if (root_color() == Color::R) {
        if (!(lft.empty() || lft.root_color() == Color::B)) {
          KLLVM_HOOK_INVALID_ARGUMENT("Red invariant failed");
        }
        if (!(rgt.empty() || rgt.root_color() == Color::B)) {
          KLLVM_HOOK_INVALID_ARGUMENT("Red invariant failed");
        }
      }
      lft.assert_red_invariant();
      rgt.assert_red_invariant();
    } else {
      if (leaf_color() != Color::B) {
        KLLVM_HOOK_INVALID_ARGUMENT("Red invariant failed");
      }
    }
  }

  // This method returns the black length of this red-black tree. It throws an
  // exception if the black invariant does not hold for this tree.
  // Black invariant: Every path from root to empty node contains the same
  // number of black nodes.
  int assert_black_invariant() const {
    if (empty()) {
      return 0;
    }
    int lft = left().assert_black_invariant();
    int rgt = right().assert_black_invariant();
    if (lft != rgt) {
      KLLVM_HOOK_INVALID_ARGUMENT("Black invariant failed");
    }
    return (root_color() == Color::B) ? 1 + lft : lft;
  }

  // This method throws an exception if the binary search tree invariant does
  // not hold for this tree.
  // Binary search tree invariant: The key of each node must be greater than the
  // key of each left child, and less than the key of each right child.
  void assert_BST_invariant() const {
    if (empty()) {
      return;
    }
    left().assert_BST_invariant();
    right().assert_BST_invariant();
    if (!(left().empty() || left().root_key() < root_key())) {
      KLLVM_HOOK_INVALID_ARGUMENT("BST invariant failed");
    }
    if (!(right().empty() || right().root_key() > root_key())) {
      KLLVM_HOOK_INVALID_ARGUMENT("BST invariant failed");
    }
  }

private:
  RBTree ins(T const &x, V const &v) const {
    assert(!empty(Color::BB));

    if (empty(Color::B)) {
      return RBTree(Color::R, RBTree(), x, v, RBTree());
    }
    T const &ykey = root_key();
    V const &yval = root_val();
    Color c = root_color();
    if (c == Color::B) {
      if (x < ykey) {
        return balance(Color::B, left().ins(x, v), ykey, yval, right());
      } else if (ykey < x) {
        return balance(Color::B, left(), ykey, yval, right().ins(x, v));
      } else {
        return RBTree(Color::B, left(), x, v, right());
      }
    } else {
      assert(c == Color::R);
      if (x < ykey) {
        return RBTree(Color::R, left().ins(x, v), ykey, yval, right());
      } else if (ykey < x) {
        return RBTree(Color::R, left(), ykey, yval, right().ins(x, v));
      } else {
        return RBTree(Color::R, left(), x, v, right());
      }
    }
  }

  RBTree del(T const &x) const {
    assert(!empty(Color::BB));

    // Black leaf
    if (empty(Color::B)) {
      return RBTree();
    }
    T const &ykey = root_key();
    V const &yval = root_val();
    // Singleton red node
    if (singleton(Color::R)) {
      if (ykey == x) {
        return RBTree();
      } else {
        return *this;
      }
    }
    // Singleton black node
    if (singleton(Color::B)) {
      if (ykey == x) {
        return RBTree(Color::BB);
      } else {
        return *this;
      }
    }
    // Black root with single left red child
    if (only_left_child(Color::B, Color::R)) {
      assert(left().left().empty(Color::B));
      assert(left().right().empty(Color::B));
      if (ykey == x) {
        return left().paint(Color::B);
      } else if (ykey < x) {
        return *this;
      } else { // ykey > x
        return RBTree(Color::B, left().del(x), ykey, yval, RBTree());
      }
    }
    // Otherwise
    if (ykey < x) {
      return rotate(root_color(), left(), ykey, yval, right().del(x));
    } else if (ykey > x) {
      return rotate(root_color(), left().del(x), ykey, yval, right());
    } else { // ykey == x
      T minkey;
      V minval;
      RBTree new_right = right().min_delete(minkey, minval);
      return rotate(root_color(), left(), minkey, minval, new_right);
    }
  }

  RBTree redden() const {
    if (non_empty(Color::B) && left().non_empty(Color::B)
        && right().non_empty(Color::B)) {
      return paint(Color::R);
    }
    if (singleton(Color::B)) {
      return paint(Color::R);
    }
    return *this;
  }

  RBTree blacken() const {
    if (doubled_left() || doubled_right()) {
      return paint(Color::B);
    }
    return *this;
  }

  RBTree min_delete(T &minkey, V &minval) const {
    // Empty tree
    assert(!empty());

    // Singleton red node
    if (singleton(Color::R)) {
      minkey = root_key();
      minval = root_val();
      return RBTree();
    }
    // Singleton black node
    if (singleton(Color::B)) {
      minkey = root_key();
      minval = root_val();
      return RBTree(Color::BB);
    }
    // Black node with single right child
    if (only_right_child(Color::B, Color::R)) {
      assert(right().left().empty(Color::B));
      assert(right().right().empty(Color::B));
      minkey = root_key();
      minval = root_val();
      return right().paint(Color::B);
    }
    // Otherwise
    RBTree new_left = left().min_delete(minkey, minval);
    return rotate(root_color(), new_left, root_key(), root_val(), right());
  }

  static RBTree rotate(
      Color c, RBTree const &lft, T const &x, V const &v, RBTree const &rgt) {
    // Red parent
    if (c == Color::R) {
      if (lft.non_empty(Color::BB) && rgt.non_empty(Color::B)) {
        return balance(
            Color::B, RBTree(Color::R, lft.paint(Color::B), x, v, rgt.left()),
            rgt.root_key(), rgt.root_val(), rgt.right());
      } else if (lft.empty(Color::BB) && rgt.non_empty(Color::B)) {
        return balance(
            Color::B, RBTree(Color::R, RBTree(), x, v, rgt.left()),
            rgt.root_key(), rgt.root_val(), rgt.right());
      } else if (lft.non_empty(Color::B) && rgt.non_empty(Color::BB)) {
        return balance(
            Color::B, lft.left(), lft.root_key(), lft.root_val(),
            RBTree(Color::R, lft.right(), x, v, rgt.paint(Color::B)));
      } else if (lft.non_empty(Color::B) && rgt.empty(Color::BB)) {
        return balance(
            Color::B, lft.left(), lft.root_key(), lft.root_val(),
            RBTree(Color::R, lft.right(), x, v, RBTree()));
      } else {
        return RBTree(c, lft, x, v, rgt);
      }
    }
    // Black parent
    if (c == Color::B) {
      if (lft.non_empty(Color::BB) && rgt.non_empty(Color::B)) {
        return balance(
            Color::BB, RBTree(Color::R, lft.paint(Color::B), x, v, rgt.left()),
            rgt.root_key(), rgt.root_val(), rgt.right());
      } else if (lft.empty(Color::BB) && rgt.non_empty(Color::B)) {
        return balance(
            Color::BB, RBTree(Color::R, RBTree(), x, v, rgt.left()),
            rgt.root_key(), rgt.root_val(), rgt.right());
      } else if (lft.non_empty(Color::B) && rgt.non_empty(Color::BB)) {
        return balance(
            Color::BB, lft.left(), lft.root_key(), lft.root_val(),
            RBTree(Color::R, lft.right(), x, v, rgt.paint(Color::B)));
      } else if (lft.non_empty(Color::B) && rgt.empty(Color::BB)) {
        return balance(
            Color::BB, lft.left(), lft.root_key(), lft.root_val(),
            RBTree(Color::R, lft.right(), x, v, RBTree()));
      } else if (
          lft.non_empty(Color::BB) && rgt.non_empty(Color::R)
          && rgt.left().non_empty(Color::B)) {
        return RBTree(
            Color::B,
            balance(
                Color::B,
                RBTree(Color::R, lft.paint(Color::B), x, v, rgt.left().left()),
                rgt.left().root_key(), rgt.left().root_val(),
                rgt.left().right()),
            rgt.root_key(), rgt.root_val(), rgt.right());
      } else if (
          lft.empty(Color::BB) && rgt.non_empty(Color::R)
          && rgt.left().non_empty(Color::B)) {
        return RBTree(
            Color::B,
            balance(
                Color::B, RBTree(Color::R, RBTree(), x, v, rgt.left().left()),
                rgt.left().root_key(), rgt.left().root_val(),
                rgt.left().right()),
            rgt.root_key(), rgt.root_val(), rgt.right());
      } else if (
          lft.non_empty(Color::R) && lft.right().non_empty(Color::B)
          && rgt.non_empty(Color::BB)) {
        return RBTree(
            Color::B, lft.left(), lft.root_key(), lft.root_val(),
            balance(
                Color::B, lft.right().left(), lft.right().root_key(),
                lft.right().root_val(),
                RBTree(
                    Color::R, lft.right().right(), x, v, rgt.paint(Color::B))));
      } else if (
          lft.non_empty(Color::R) && lft.right().non_empty(Color::B)
          && rgt.empty(Color::BB)) {
        return RBTree(
            Color::B, lft.left(), lft.root_key(), lft.root_val(),
            balance(
                Color::B, lft.right().left(), lft.right().root_key(),
                lft.right().root_val(),
                RBTree(Color::R, lft.right().right(), x, v, RBTree())));
      } else {
        return RBTree(c, lft, x, v, rgt);
      }
    }
    // Otherwise
    return RBTree(c, lft, x, v, rgt);
  }

  // Called only when parent is B or BB.
  static RBTree balance(
      Color c, RBTree const &lft, T const &x, V const &v, RBTree const &rgt) {
    if (lft.doubled_left()) {
      return RBTree(
          minus_one_color(c), lft.left().paint(Color::B), lft.root_key(),
          lft.root_val(), RBTree(Color::B, lft.right(), x, v, rgt));
    } else if (lft.doubled_right()) {
      return RBTree(
          minus_one_color(c),
          RBTree(
              Color::B, lft.left(), lft.root_key(), lft.root_val(),
              lft.right().left()),
          lft.right().root_key(), lft.right().root_val(),
          RBTree(Color::B, lft.right().right(), x, v, rgt));
    } else if (rgt.doubled_left()) {
      return RBTree(
          minus_one_color(c), RBTree(Color::B, lft, x, v, rgt.left().left()),
          rgt.left().root_key(), rgt.left().root_val(),
          RBTree(
              Color::B, rgt.left().right(), rgt.root_key(), rgt.root_val(),
              rgt.right()));
    } else if (rgt.doubled_right()) {
      return RBTree(
          minus_one_color(c), RBTree(Color::B, lft, x, v, rgt.left()),
          rgt.root_key(), rgt.root_val(), rgt.right().paint(Color::B));
    } else {
      return RBTree(c, lft, x, v, rgt);
    }
  }

  bool empty(Color c) const { return empty() && leaf_color() == c; }

  bool non_empty(Color c) const { return !empty() && root_color() == c; }

  bool singleton(Color c) const {
    return !empty() && root_color() == c && left().empty(Color::B)
           && right().empty(Color::B);
  }

  bool only_left_child(Color parent_color, Color child_color) const {
    return !empty() && root_color() == parent_color && !left().empty()
           && left().root_color() == child_color && right().empty(Color::B);
  }

  bool only_right_child(Color parent_color, Color child_color) const {
    return !empty() && root_color() == parent_color && left().empty(Color::B)
           && !right().empty() && right().root_color() == child_color;
  }

  bool doubled_left() const {
    return !empty() && root_color() == Color::R && !left().empty()
           && left().root_color() == Color::R;
  }

  bool doubled_right() const {
    return !empty() && root_color() == Color::R && !right().empty()
           && right().root_color() == Color::R;
  }

  RBTree paint(Color c) const {
    assert(!empty());
    return RBTree(c, left(), root_key(), root_val(), right());
  }

private:
  std::shared_ptr<Node> root_;
};

// Recursively (using inorder traversal) apply function f to all elements of
// tree t. Function f must accept two arguments of types T and V respectively.
template <class T, class V, class F>
void for_each(RBTree<T, V> const &t, F &&f) {
  if (!t.empty()) {
    for_each(t.left(), std::forward<F>(f));
    std::invoke(f, t.root_key(), t.root_val());
    for_each(t.right(), std::forward<F>(f));
  }
}

// Return a red-black tree with all elements in t, and then also from the
// container designated by the beginning and end iterator arguments. The
// container should contain elements of type std::pair<T,V>.
template <class T, class V, class I>
RBTree<T, V> inserted(RBTree<T, V> const &t, I it, I end) {
  if (it == end) {
    return t;
  }
  T key = it->first;
  V val = it->second;
  auto t1 = t.inserted(key, val);
  return inserted(t1, ++it, end);
}

} // namespace rb_tree

#endif // RBTREE_HEADER_H
