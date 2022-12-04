//***---  Adapted from https://github.com/BartoszMilewski/Okasaki/tree/master/RBTree  ---***//

#include "RBTree.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

template<class T, class V>
void print(RBTree<T, V> const & t)
{
    forEach(t, [](T x, V v)
    {
        std::cout << "[ " << x << " -> " << v << " ] ";
    });
    std::cout << std::endl;
}

void testInit()
{
    std::vector<std::pair<int, int> > v = 
        { std::pair<int, int>( 50, 51 ),
          std::pair<int, int>( 40, 41 ),
          std::pair<int, int>( 30, 31 ),
          std::pair<int, int>( 10, 11 ),
          std::pair<int, int>( 20, 21 ),
          std::pair<int, int>( 30, 32 ),
          std::pair<int, int>( 100, 101),
          std::pair<int, int>( 0, 1 ),
          std::pair<int, int>( 45, 46),
          std::pair<int, int>( 55, 56),
          std::pair<int, int>( 25, 26),
          std::pair<int, int>( 15, 16)
        };
    RBTree<int, int> t(v.begin(), v.end());
    print(t);
}

void hugeTest(std::vector<int> &v)
{
    std::vector<RBTree<int, int> > trees;
    trees.push_back(RBTree<int, int>());
    for (int i: v)
    {
        trees.push_back(trees.back().inserted(i, i+1));
        trees.push_back(trees.back().inserted(i, i+2));
    }
    for (int i: v)
    {
        trees.push_back(trees.back().deleted(i));
    }
    for (int i = 0; i < v.size(); i++ )
    {
        int idxI1 = 1 + 2*i;
        assert(trees[idxI1].member(v[i]));
        assert(trees[idxI1].lookup(v[i]) == v[i]+1);
        trees[idxI1].assert1();
        trees[idxI1].countB();
        trees[idxI1].assertBST();
        int idxI2 = 1 + 2*i+1;
        assert(trees[idxI2].member(v[i]));
        assert(trees[idxI2].lookup(v[i]) == v[i]+2);
        trees[idxI2].assert1();
        trees[idxI2].countB();
        trees[idxI2].assertBST();
        int idxD = 1 + 2*v.size() + i;
        assert(!trees[idxD].member(v[i]));
        trees[idxD].assert1();
        trees[idxD].countB();
        trees[idxD].assertBST();
    }
    assert(trees[0].isEmpty());
    trees[0].assert1();
    trees[0].countB();
    trees[0].assertBST();
}

void makeTestVectors(int n, std::vector<int> &asc, std::vector<int> &desc, std::vector<int> &rnd)
{
    asc.clear(); desc.clear(); rnd.clear();
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        asc.push_back(i);
        desc.push_back(n-i-1);
        rnd.push_back(rand());
    }
}

void testConcat()
{
    std::vector<RBTree<int, int> > op1;
    op1.push_back(RBTree<int, int>());
    std::vector<std::pair<int, int> > v1 =
        {
            std::pair<int, int>( 1, 1 ),
            std::pair<int, int>( 3, 3 ),
            std::pair<int, int>( 5, 5 ),
            std::pair<int, int>( 7, 7 ),
        };
    op1.push_back(RBTree<int, int>(v1.begin(), v1.end()));
for (RBTree r : op1)
{
print(r);
}
    std::vector<RBTree<int, int> > op2;
    op2.push_back(RBTree<int, int>());
    std::vector<std::pair<int, int> > v2 =
        {
            std::pair<int, int>( 2, 3 ),
            std::pair<int, int>( 4, 5 ),
            std::pair<int, int>( 6, 7 ),
            std::pair<int, int>( 8, 9 ),
        };
    op2.push_back(RBTree<int, int>(v2.begin(), v2.end()));
for (RBTree r : op2)
{
print(r);
}

    std::vector<RBTree<int, int> > res;
    for (RBTree t1: op1)
    {
        for (RBTree t2: op2)
        {
            res.push_back(mapConcat(t1, t2));
            res.push_back(mapConcat(t2, t1));
        }
    }
    for (int i = 0; i < res.size(); i++)
    {
        RBTree<int, int> r = res[i];
        r.assert1();
        r.countB();
        r.assertBST();
std::cout << "tree " << i << " :" << std::endl;
print(r);
    }

}

void testInserted()
{
    std::vector<RBTree<int, int> > trees;
    std::vector< std::vector<std::pair<int, int > > > ins;

    trees.push_back(RBTree<int, int>());
    std::vector<std::pair<int, int> > v1 =
        {
            std::pair<int, int>( 1, 1 ),
            std::pair<int, int>( 3, 3 ),
            std::pair<int, int>( 5, 5 ),
            std::pair<int, int>( 7, 7 ),
        };
    trees.push_back(RBTree<int, int>(v1.begin(), v1.end()));
    ins.push_back(v1);
    v1.clear();
    v1 =
        {
            std::pair<int, int>( 2, 2 ),
            std::pair<int, int>( 4, 4 ),
            std::pair<int, int>( 6, 6 ),
            std::pair<int, int>( 8, 8 ),
        };
    ins.push_back(v1);
    v1.clear();
    v1 =
        {
            std::pair<int, int>( 1, 2 ),
            std::pair<int, int>( 3, 4 ),
            std::pair<int, int>( 5, 6 ),
            std::pair<int, int>( 7, 8 ),
        };
    ins.push_back(v1);
    v1.clear();
    v1 =
        {
            std::pair<int, int>( 9, 1 ),
            std::pair<int, int>( 9, 2 ),
            std::pair<int, int>( 9, 3 ),
        };
    ins.push_back(v1);
    v1.clear();

for (RBTree r : trees)
{
print(r);
}
    std::vector<RBTree<int, int> > res;
    for (RBTree t: trees)
    {
        for (std::vector<std::pair<int, int> > v: ins)
        {
            res.push_back(inserted(t, v.begin(), v.end()));
        }
    }
    for (int i = 0; i < res.size(); i++)
    {
        RBTree<int, int> r = res[i];
        r.assert1();
        r.countB();
        r.assertBST();
std::cout << "tree " << i << " :" << std::endl;
print(r);
    }

}

int main()
{
    testInit();
    std::vector<int> a, d, r;
    makeTestVectors(20, a, d, r);
    std::cout << "--- Begin Asc test ---" << std::endl;
    hugeTest(a);
    std::cout << "--- End Asc test ---" << std::endl;
    std::cout << "--- Begin Desc test ---" << std::endl;
    hugeTest(d);
    std::cout << "--- End Desc test ---" << std::endl;
    std::cout << "--- Begin Rand test ---" << std::endl;
    hugeTest(r);
    std::cout << "--- End Rand test ---" << std::endl;
    std::cout << "--- Begin Concat test ---" << std::endl;
    testConcat();
    std::cout << "--- End Concat test ---" << std::endl;
    std::cout << "--- Begin Inserted test ---" << std::endl;
    testInserted();
    std::cout << "--- End Inserted test ---" << std::endl;

    return 0;

}
