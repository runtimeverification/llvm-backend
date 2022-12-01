//***---  Adapted from https://github.com/BartoszMilewski/Okasaki/tree/master/RBTree  ---***//
// Implementing algorithms from https://matt.might.net/papers/germane2014deletion.pdf

#include "List.h"
#include <cassert>
#include <memory>

// 1. No red node has a red child.
// 2. Every path from root to empty node contains the same
// number of black nodes.

template<class T>
class RBTree
{
    enum Color { R, B, BB };

    static Color minusOneColor(Color c)
    {
        switch (c)
        {
            case  B: return  R;
            case BB: return  B;
            default: assert(false);
        }
    }

    struct Node
    {
        Node(Color c) : _c(c) {}
        Color _c;

        virtual bool isLeaf() const =0;
    };

    struct Leaf : public Node
    {
        Leaf(Color c) : Node(c) { assert(c == B || c == BB); }
        virtual bool isLeaf() const { return true; }
    };

    struct InternalNode : public Node
    {
        InternalNode(Color c, 
            std::shared_ptr<const Node> const & lft, 
            T val, 
            std::shared_ptr<const Node> const & rgt)
            : Node(c), _lft(lft), _val(val), _rgt(rgt)
        {}
        std::shared_ptr<const Node> _lft;
        T _val;
        std::shared_ptr<const Node> _rgt;

        virtual bool isLeaf() const { return false; }
    };

    explicit RBTree(std::shared_ptr<const Node> const & node) : _root(node) {} 

    Color rootColor() const
    {
        assert(!isEmpty());
        return _root->_c;
    }

    Color leafColor() const
    {
        assert(isEmpty());
        return _root->_c;
    }

public:
    RBTree() : _root(std::make_shared<const Leaf>(B)) {}

    RBTree(Color c) : _root(std::make_shared<const Leaf>(c)) {}

    RBTree(Color c, RBTree const & lft, T val, RBTree const & rgt)
        : _root(std::make_shared<const InternalNode>(c, lft._root, val, rgt._root))
    {
        assert(lft.isEmpty() || lft.root() < val);
        assert(rgt.isEmpty() || val < rgt.root());
    }

    RBTree(std::initializer_list<T> init)
    {
        RBTree t(B);
        for (T v : init)
        {
            t = t.inserted(v);
        }
        _root = t._root;
    }

    template<class I>
    RBTree(I b, I e)
    {
        RBTree t(B);
        for_each(b, e, [&t](T const & v){
            t = t.inserted(v);
        });
        _root = t._root;
    }

    bool isEmpty() const { return _root->isLeaf(); }

    T root() const
    {
        assert(!isEmpty());
        const InternalNode *r = static_cast<const InternalNode *>(_root.get());
        return r->_val;
    }

    RBTree left() const
    {
        assert(!isEmpty());
        const InternalNode *r = static_cast<const InternalNode *>(_root.get());
        return RBTree(r->_lft);
    }

    RBTree right() const
    {
        assert(!isEmpty());
        const InternalNode *r = static_cast<const InternalNode *>(_root.get());
        return RBTree(r->_rgt);
    }

    bool member(T x) const
    {
        if (isEmpty())
            return false;
        T y = root();
        if (x < y)
            return left().member(x);
        else if (y < x)
            return right().member(x);
        else
            return true;
    }

    RBTree inserted(T x) const
    {
        return ins(x).blacken();
    }

    RBTree deleted(T x) const
    {
        return redden().del(x);
    }

    // 1. No red node has a red child.
    void assert1() const
    {
        if (!isEmpty())
        {
            assert(rootColor() != BB);
            auto lft = left();
            auto rgt = right();
            if (rootColor() == R) 
            {
                assert(lft.isEmpty() || lft.rootColor() == B);
                assert(rgt.isEmpty() || rgt.rootColor() == B);
            }
            lft.assert1();
            rgt.assert1();
        }
        else
        {
            assert(leafColor() == B);
        }
    }

    // 2. Every path from root to empty node contains the same
    // number of black nodes.
    int countB() const
    {
        if (isEmpty())
            return 0;
        int lft = left().countB();
        int rgt = right().countB();
        assert(lft == rgt);
        return (rootColor() == B)? 1 + lft: lft;
    }

    void assertBST() const
    {
        if (isEmpty())
            return;
        left().assertBST();
        right().assertBST();
        assert(left().isEmpty() || left().root() < root());
        assert(right().isEmpty() || right().root() > root());
    }

private:
    RBTree ins(T x) const
    {
        assert(!isEmpty(BB));
        assert1();
 
        if (isEmpty(B))
            return RBTree(R, RBTree(), x, RBTree());
        T y = root();
        Color c = rootColor();
        if (c == B)
        {
            if (x < y)
                return balance(B, left().ins(x), y, right());
            else if (y < x)
                return balance(B, left(), y, right().ins(x));
            else
                return *this; // no duplicates
        }
        else
        {
            assert(c == R);
            if (x < y)
                return RBTree(R, left().ins(x), y, right());
            else if (y < x)
                return RBTree(R, left(), y, right().ins(x));
            else
                return *this; // no duplicates
        }
    }

    RBTree del(T x) const
    {
        assert(!isEmpty(BB));
 
        // Black leaf
        if (isEmpty(B))
        {
            return RBTree();
        }
        T y = root();
        // Singleton red node
        if (singleton(R))
        {
            if (y == x)
                return RBTree();
            else
                return *this;
        }
        // Singleton black node
        if (singleton(B))
        {
            if (y == x)
                return RBTree(BB);
            else
                return *this;
        }
        // Black root with single left red child
        if (onlyLeftChild(B, R))
        {
            assert(left().left().isEmpty(B));
            assert(left().right().isEmpty(B));
            if (y == x)
                return left().paint(B);
            else if (y < x)
                return *this;
            else // y > x
                return RBTree(B, left().del(x), y, RBTree());
        }
        // Otherwise
        if (y < x)
            return rotate(rootColor(), left(), y, right().del(x));
        else if (y > x)
            return rotate(rootColor(), left().del(x), y, right());
        else // y == x
        {
            std::pair<T, RBTree> p = right().minDelete();
            T min = p.first;
            RBTree newRight = p.second;
            return rotate(rootColor(), left(), min, newRight);
        }
    }

    RBTree redden() const
    {
        if (nonEmpty(B)
            && left().nonEmpty(B)
            && right().nonEmpty(B))
            return paint(R);
        if (singleton(B))
            return paint(R);
        return *this;
    }

    RBTree blacken() const
    {
        if (doubledLeft() || doubledRight())
            return paint(B);
        return *this;
    }

    std::pair<T, RBTree> minDelete() const
    {
        // Empty tree
        assert(!isEmpty());
        
        // Singleton red node
        if (singleton(R))
        {
            return std::make_pair(root(), RBTree());
        }
        // Singleton black node
        if (singleton(B))
        {
            return std::make_pair(root(), RBTree(BB));
        }
        // Black node with single right child
        if (onlyRightChild(B, R))
        {
            assert(right().left().isEmpty(B));
            assert(right().right().isEmpty(B));
            return std::make_pair(root(), right().paint(B));
        }
        // Otherwise
        std::pair<T, RBTree> p = left().minDelete();
        T min = p.first;
        RBTree newLeft = p.second;
        return std::make_pair(min, rotate(rootColor(), newLeft, root(), right()));
    }

    static RBTree rotate(Color c, RBTree const & lft, T x, RBTree const & rgt)
    {
        // Red parent
        if (c == R)
        {
            if (lft.nonEmpty(BB) && rgt.nonEmpty(B))
                return balance(B
                            , RBTree(R, lft.paint(B), x, rgt.left())
                            , rgt.root()
                            , rgt.right());
            else if (lft.isEmpty(BB) && rgt.nonEmpty(B))
                return balance(B
                            , RBTree(R, RBTree(), x, rgt.left())
                            , rgt.root()
                            , rgt.right());
            else if (lft.nonEmpty(B) && rgt.nonEmpty(BB))
                return balance(B
                            , lft.left()
                            , lft.root()
                            , RBTree(R, lft.right(), x, rgt.paint(B)));
            else if (lft.nonEmpty(B) && rgt.isEmpty(BB))
                return balance(B
                            , lft.left()
                            , lft.root()
                            , RBTree(R, lft.right(), x, RBTree()));
            else return RBTree(c, lft, x, rgt);
        }
        // Black parent 
        if (c == B)
        {
            if (lft.nonEmpty(BB) && rgt.nonEmpty(B))
                return balance(BB
                            , RBTree(R, lft.paint(B), x, rgt.left())
                            , rgt.root()
                            , rgt.right());
            else if (lft.isEmpty(BB) && rgt.nonEmpty(B))
                return balance(BB
                            , RBTree(R, RBTree(), x, rgt.left())
                            , rgt.root()
                            , rgt.right());
            else if (lft.nonEmpty(B) && rgt.nonEmpty(BB))
                return balance(BB
                            , lft.left()
                            , lft.root()
                            , RBTree(R, lft.right(), x, rgt.paint(B)));
            else if (lft.nonEmpty(B) && rgt.isEmpty(BB))
                return balance(BB
                            , lft.left()
                            , lft.root()
                            , RBTree(R, lft.right(), x, RBTree()));
            else if (lft.nonEmpty(BB) && rgt.nonEmpty(R) && rgt.left().nonEmpty(B))
                return RBTree(B
                            , balance(B, RBTree(R, lft.paint(B), x, rgt.left().left()), rgt.left().root(), rgt.left().right())
                            , rgt.root()
                            , rgt.right());
            else if (lft.isEmpty(BB) && rgt.nonEmpty(R) && rgt.left().nonEmpty(B))
                return RBTree(B
                            , balance(B, RBTree(R, RBTree(), x, rgt.left().left()), rgt.left().root(), rgt.left().right())
                            , rgt.root()
                            , rgt.right());
            else if (lft.nonEmpty(R) && lft.right().nonEmpty(B) && rgt.nonEmpty(BB))
                return RBTree(B
                            , lft.left()
                            , lft.root()
                            , balance(B, lft.right().left(), lft.right().root(), RBTree(R, lft.right().right(), x, rgt.paint(B))));
            else if (lft.nonEmpty(R) && lft.right().nonEmpty(B) && rgt.isEmpty(BB))
                return RBTree(B
                            , lft.left()
                            , lft.root()
                            , balance(B, lft.right().left(), lft.right().root(), RBTree(R, lft.right().right(), x, RBTree())));
            else return RBTree(c, lft, x, rgt);
        }
        // Otherwise
        return RBTree(c, lft, x, rgt);
    }

    // Called only when parent is black or double black
    static RBTree balance(Color c, RBTree const & lft, T x, RBTree const & rgt)
    {
        if (lft.doubledLeft())
            return RBTree(minusOneColor(c)
                        , lft.left().paint(B)
                        , lft.root()
                        , RBTree(B, lft.right(), x, rgt));
        else if (lft.doubledRight())
            return RBTree(minusOneColor(c)
                        , RBTree(B, lft.left(), lft.root(), lft.right().left())
                        , lft.right().root()
                        , RBTree(B, lft.right().right(), x, rgt));
        else if (rgt.doubledLeft())
            return RBTree(minusOneColor(c)
                        , RBTree(B, lft, x, rgt.left().left())
                        , rgt.left().root()
                        , RBTree(B, rgt.left().right(), rgt.root(), rgt.right()));
        else if (rgt.doubledRight())
            return RBTree(minusOneColor(c)
                        , RBTree(B, lft, x, rgt.left())
                        , rgt.root()
                        , rgt.right().paint(B));
        else
            return RBTree(c, lft, x, rgt);
    }

    bool isEmpty(Color c) const
    {
        return isEmpty()
            && leafColor() == c;
    }

    bool nonEmpty(Color c) const
    {
        return !isEmpty()
            && rootColor() == c;
    }

    bool singleton(Color c) const
    {
        return !isEmpty()
            && rootColor() == c
            && left().isEmpty(B)
            && right().isEmpty(B);
    }

    bool onlyLeftChild(Color parentColor, Color childColor) const
    {
        return !isEmpty()
            && rootColor() == parentColor
            && !left().isEmpty()
            && left().rootColor() == childColor
            && right().isEmpty(B);
    }

    bool onlyRightChild(Color parentColor, Color childColor) const
    {
        return !isEmpty()
            && rootColor() == parentColor
            && left().isEmpty(B)
            && !right().isEmpty()
            && right().rootColor() == childColor;
    }

    bool doubledLeft() const 
    {
        return !isEmpty()
            && rootColor() == R
            && !left().isEmpty()
            && left().rootColor() == R;
    }

    bool doubledRight() const 
    {
        return !isEmpty()
            && rootColor() == R
            && !right().isEmpty()
            && right().rootColor() == R;
    }

    RBTree paint(Color c) const
    {
        assert(!isEmpty());
        return RBTree(c, left(), root(), right());
    }

private:
    std::shared_ptr<const Node> _root;
};

template<class T, class F>
void forEach(RBTree<T> const & t, F f) {
    if (!t.isEmpty()) {
        forEach(t.left(), f);
        f(t.root());
        forEach(t.right(), f);
    }
}

template<class T, class Beg, class End>
RBTree<T> inserted(RBTree<T> t, Beg it, End end)
{
    if (it == end)
        return t;
    T item = *it;
    auto t1 = inserted(t, ++it, end);
    return t1.inserted(item);
}

template<class T>
RBTree<T> treeUnion(RBTree<T> const & a, RBTree<T> const & b)
{
    // a u b = a + (b \ a)
    RBTree<T> res = a;
    forEach(b, [&res, &a](T const & v){
        if (!a.member(v))
            res.inserted(v);
    });
    return res;
}

// Remove elements in set from a list
template<class T>
List<T> rem_from_list(List<T> const & lst, RBTree<T> const & set)
{
    List<T> res;
    lst.forEach([&res, &set](T const & v) {
        if (!set.member(v))
            res = res.pushed_front(v);
    });
    return res;
}




