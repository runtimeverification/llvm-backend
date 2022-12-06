//***---  Adapted from https://github.com/BartoszMilewski/Okasaki/tree/master/RBTree  ---***//
// Implementing algorithms from https://matt.might.net/papers/germane2014deletion.pdf

#include <cassert>
#include <memory>
#include <string>
#include <stdexcept>

// 1. No red node has a red child.
// 2. Every path from root to empty node contains the same
// number of black nodes.

#define CONSTRUCT_MSG_AND_THROW(msg)                                          \
    do {                                                                      \
        std::string emsg = std::string("[")                                   \
                           + std::string(__func__)                            \
                           + std::string("] ")                               \
                           + std::string(msg);                                \
        throw std::invalid_argument(emsg);                                    \
        } while (false)


template<class T, class V>
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
        Node(Color c) : _c(c), _s(0) {}
        Color _c;
        size_t _s;

        virtual bool isLeaf() const =0;
        virtual ~Node() {};
    };

    struct Leaf : public Node
    {
        Leaf(Color c) : Node(c) { assert(c == B || c == BB); }
        virtual bool isLeaf() const override { return true; }
        virtual ~Leaf() {};
    };

    struct InternalNode : public Node
    {
        InternalNode(Color c, 
            std::shared_ptr<const Node> const & lft, 
            T key,
            V val,
            std::shared_ptr<const Node> const & rgt)
            : Node(c), _lft(lft), _key(key), _val(val), _rgt(rgt)
        {
            this->_s = 1 + _lft->_s + _rgt->_s;
        }
        std::shared_ptr<const Node> _lft;
        T _key;
        V _val;
        std::shared_ptr<const Node> _rgt;

        virtual bool isLeaf() const override { return false; }
        virtual ~InternalNode() {};
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

    RBTree(Color c, RBTree const & lft, T key, V val, RBTree const & rgt)
        : _root(std::make_shared<const InternalNode>(c, lft._root, key, val, rgt._root))
    {
        assert(lft.isEmpty() || lft.root() < key);
        assert(rgt.isEmpty() || key < rgt.root());
    }

    template<class I>
    RBTree(I b, I e)
    {
        RBTree t(B);
        for_each(b, e, [&t](std::pair<T, V> const & p){
            t = t.inserted(p.first, p.second);
        });
        _root = t._root;
    }

    bool isEmpty() const { return _root->isLeaf(); }

    T root() const
    {
        assert(!isEmpty());
        const InternalNode *r = static_cast<const InternalNode *>(_root.get());
        return r->_key;
    }

    V rootVal() const
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

    size_t size() const
    {
        return _root->_s;
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

    V lookup(T x) const
    {
        if (isEmpty())
            CONSTRUCT_MSG_AND_THROW("Key not found for map lookup");
        T y = root();
        if (x < y)
            return left().lookup(x);
        else if (y < x)
            return right().lookup(x);
        else
            return rootVal();
    }

    RBTree inserted(T x, V v) const
    {
        return ins(x, v).blacken();
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
    RBTree ins(T x, V v) const
    {
        assert(!isEmpty(BB));
        assert1();
 
        if (isEmpty(B))
            return RBTree(R, RBTree(), x, v, RBTree());
        T y = root();
        V yVal = rootVal();
        Color c = rootColor();
        if (c == B)
        {
            if (x < y)
                return balance(B, left().ins(x, v), y, yVal, right());
            else if (y < x)
                return balance(B, left(), y, yVal, right().ins(x, v));
            else
                return RBTree(B, left(), x, v, right());
        }
        else
        {
            assert(c == R);
            if (x < y)
                return RBTree(R, left().ins(x, v), y, yVal, right());
            else if (y < x)
                return RBTree(R, left(), y, yVal, right().ins(x, v));
            else
                return RBTree(R, left(), x, v, right());
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
        V yVal = rootVal();
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
                return RBTree(B, left().del(x), y, yVal, RBTree());
        }
        // Otherwise
        if (y < x)
            return rotate(rootColor(), left(), y, yVal, right().del(x));
        else if (y > x)
            return rotate(rootColor(), left().del(x), y, yVal, right());
        else // y == x
        {
            std::pair<std::pair<T, V>, RBTree> p = right().minDelete();
            T minK = p.first.first;
            V minV = p.first.second;
            RBTree newRight = p.second;
            return rotate(rootColor(), left(), minK, minV, newRight);
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

    std::pair<std::pair<T, V>, RBTree> minDelete() const
    {
        // Empty tree
        assert(!isEmpty());
        
        // Singleton red node
        if (singleton(R))
        {
            return std::make_pair(std::make_pair(root(), rootVal()), RBTree());
        }
        // Singleton black node
        if (singleton(B))
        {
            return std::make_pair(std::make_pair(root(), rootVal()), RBTree(BB));
        }
        // Black node with single right child
        if (onlyRightChild(B, R))
        {
            assert(right().left().isEmpty(B));
            assert(right().right().isEmpty(B));
            return std::make_pair(std::make_pair(root(), rootVal()), right().paint(B));
        }
        // Otherwise
        std::pair<std::pair<T, V>, RBTree> p = left().minDelete();
        T minK = p.first.first;
        V minV = p.first.second;
        RBTree newLeft = p.second;
        return std::make_pair(std::make_pair(minK, minV), rotate(rootColor(), newLeft, root(), rootVal(), right()));
    }

    static RBTree rotate(Color c, RBTree const & lft, T x, V v, RBTree const & rgt)
    {
        // Red parent
        if (c == R)
        {
            if (lft.nonEmpty(BB) && rgt.nonEmpty(B))
                return balance(B
                            , RBTree(R, lft.paint(B), x, v, rgt.left())
                            , rgt.root()
                            , rgt.rootVal()
                            , rgt.right());
            else if (lft.isEmpty(BB) && rgt.nonEmpty(B))
                return balance(B
                            , RBTree(R, RBTree(), x, v, rgt.left())
                            , rgt.root()
                            , rgt.rootVal()
                            , rgt.right());
            else if (lft.nonEmpty(B) && rgt.nonEmpty(BB))
                return balance(B
                            , lft.left()
                            , lft.root()
                            , lft.rootVal()
                            , RBTree(R, lft.right(), x, v, rgt.paint(B)));
            else if (lft.nonEmpty(B) && rgt.isEmpty(BB))
                return balance(B
                            , lft.left()
                            , lft.root()
                            , lft.rootVal()
                            , RBTree(R, lft.right(), x, v, RBTree()));
            else return RBTree(c, lft, x, v, rgt);
        }
        // Black parent 
        if (c == B)
        {
            if (lft.nonEmpty(BB) && rgt.nonEmpty(B))
                return balance(BB
                            , RBTree(R, lft.paint(B), x, v, rgt.left())
                            , rgt.root()
                            , rgt.rootVal()
                            , rgt.right());
            else if (lft.isEmpty(BB) && rgt.nonEmpty(B))
                return balance(BB
                            , RBTree(R, RBTree(), x, v, rgt.left())
                            , rgt.root()
                            , rgt.rootVal()
                            , rgt.right());
            else if (lft.nonEmpty(B) && rgt.nonEmpty(BB))
                return balance(BB
                            , lft.left()
                            , lft.root()
                            , lft.rootVal()
                            , RBTree(R, lft.right(), x, v, rgt.paint(B)));
            else if (lft.nonEmpty(B) && rgt.isEmpty(BB))
                return balance(BB
                            , lft.left()
                            , lft.root()
                            , lft.rootVal()
                            , RBTree(R, lft.right(), x, v, RBTree()));
            else if (lft.nonEmpty(BB) && rgt.nonEmpty(R) && rgt.left().nonEmpty(B))
                return RBTree(B
                            , balance(B, RBTree(R, lft.paint(B), x, v, rgt.left().left()), rgt.left().root(), rgt.left().rootVal(), rgt.left().right())
                            , rgt.root()
                            , rgt.rootVal()
                            , rgt.right());
            else if (lft.isEmpty(BB) && rgt.nonEmpty(R) && rgt.left().nonEmpty(B))
                return RBTree(B
                            , balance(B, RBTree(R, RBTree(), x, v, rgt.left().left()), rgt.left().root(), rgt.left().rootVal(), rgt.left().right())
                            , rgt.root()
                            , rgt.rootVal()
                            , rgt.right());
            else if (lft.nonEmpty(R) && lft.right().nonEmpty(B) && rgt.nonEmpty(BB))
                return RBTree(B
                            , lft.left()
                            , lft.root()
                            , lft.rootVal()
                            , balance(B, lft.right().left(), lft.right().root(), lft.right().rootVal(), RBTree(R, lft.right().right(), x, v, rgt.paint(B))));
            else if (lft.nonEmpty(R) && lft.right().nonEmpty(B) && rgt.isEmpty(BB))
                return RBTree(B
                            , lft.left()
                            , lft.root()
                            , lft.rootVal()
                            , balance(B, lft.right().left(), lft.right().root(), lft.right().rootVal(), RBTree(R, lft.right().right(), x, v, RBTree())));
            else return RBTree(c, lft, x, v, rgt);
        }
        // Otherwise
        return RBTree(c, lft, x, v, rgt);
    }

    // Called only when parent is black or double black
    static RBTree balance(Color c, RBTree const & lft, T x, V v, RBTree const & rgt)
    {
        if (lft.doubledLeft())
            return RBTree(minusOneColor(c)
                        , lft.left().paint(B)
                        , lft.root()
                        , lft.rootVal()
                        , RBTree(B, lft.right(), x, v, rgt));
        else if (lft.doubledRight())
            return RBTree(minusOneColor(c)
                        , RBTree(B, lft.left(), lft.root(), lft.rootVal(), lft.right().left())
                        , lft.right().root()
                        , lft.right().rootVal()
                        , RBTree(B, lft.right().right(), x, v, rgt));
        else if (rgt.doubledLeft())
            return RBTree(minusOneColor(c)
                        , RBTree(B, lft, x, v, rgt.left().left())
                        , rgt.left().root()
                        , rgt.left().rootVal()
                        , RBTree(B, rgt.left().right(), rgt.root(), rgt.rootVal(), rgt.right()));
        else if (rgt.doubledRight())
            return RBTree(minusOneColor(c)
                        , RBTree(B, lft, x, v, rgt.left())
                        , rgt.root()
                        , rgt.rootVal()
                        , rgt.right().paint(B));
        else
            return RBTree(c, lft, x, v, rgt);
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
        return RBTree(c, left(), root(), rootVal(), right());
    }

private:
    std::shared_ptr<const Node> _root;
};

template<class T, class V, class F>
void forEach(RBTree<T, V> const & t, F f) {
    if (!t.isEmpty()) {
        forEach(t.left(), f);
        f(t.root(), t.rootVal());
        forEach(t.right(), f);
    }
}

template<class T, class V, class Beg, class End>
RBTree<T, V> inserted(RBTree<T, V> t, Beg it, End end)
{
    if (it == end)
        return t;
    T key = it->first;
    V val = it->second;
    auto t1 = t.inserted(key, val);
    return inserted(t1, ++it, end);
}

template<class T, class V>
RBTree<T, V> mapConcat(RBTree<T, V> const & a, RBTree<T, V> const & b)
{
    RBTree<T, V> res = a;
    forEach(b, [&res, &a](T const & x, V const & v){
        if (!a.member(x))
            res = res.inserted(x, v);
        else
           CONSTRUCT_MSG_AND_THROW("Duplicate keys in map concatenation");
    });
    return res;
}



