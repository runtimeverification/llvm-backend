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

int main()
{
    testInit();
    std::vector<int> a, d, r;
    makeTestVectors(10000, a, d, r);
    std::cout << "--- Begin Asc test ---" << std::endl;
    hugeTest(a);
    std::cout << "--- End Asc test ---" << std::endl;
    std::cout << "--- Begin Desc test ---" << std::endl;
    hugeTest(d);
    std::cout << "--- End Desc test ---" << std::endl;
    std::cout << "--- Begin Rand test ---" << std::endl;
    hugeTest(r);
    std::cout << "--- End Rand test ---" << std::endl;

    return 0;

}
