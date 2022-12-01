//***---  Adapted from https://github.com/BartoszMilewski/Okasaki/tree/master/RBTree  ---***//

#include "RBTree.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

template<class T>
void print(RBTree<T> const & t)
{
    forEach(t, [](T v)
    {
        std::cout << v << " ";
    });
    std::cout << std::endl;
}

void testInit()
{
    RBTree<int> t{ 50, 40, 30, 10, 20, 30, 100, 0, 45, 55, 25, 15 };
    print(t);
}

void hugeTest(std::vector<int> &v)
{
    std::vector<RBTree<int> > trees;
    trees.push_back(RBTree<int>());
    for (int i: v)
    {
        trees.push_back(trees.back().inserted(i));
    }
    for (int i: v)
    {
        trees.push_back(trees.back().deleted(i));
    }
    for (int i = 0; i < v.size(); i++ )
    {
        int idxI = 1 + i;
        assert(trees[idxI].member(v[i]));
        trees[idxI].assert1();
        trees[idxI].countB();
        trees[idxI].assertBST();
        int idxD = 1 + v.size() + i;
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
    std::string init =  "a red black tree walks into a bar "
                        "has johnny walker on the rocks "
                        "and quickly rebalances itself."
                        "A RED BLACK TREE WALKS INTO A BAR "
                        "HAS JOHNNY WALKER ON THE ROCKS "
                        "AND QUICKLY REBALANCES ITSELF.";
    auto t = inserted(RBTree<char>(), init.begin(), init.end());
    auto t1 = t.deleted('A');
    auto t2 = t1.deleted('Z');
    auto t3 = t2.deleted('.');
    auto t4 = t3.deleted('h');
    print(t);
    t.assert1();
    t.assertBST();
    std::cout << "Black depth: " << t.countB() << std::endl;
    std::cout << "Member z: " << t.member('z') << std::endl;
    std::for_each(init.begin(), init.end(), [t](char c) 
    {
        if (!t.member(c))
            std::cout << "Error: " << c << " not found\n";
    });
    std::cout << "--- Tree t1 ---" << std::endl;
    print(t1);
    t1.assert1();
    t1.assertBST();
    std::cout << "Black depth: " << t1.countB() << std::endl;
    std::cout << "Member A: " << t1.member('A') << std::endl;
    std::cout << "--- Tree t2 ---" << std::endl;
    print(t2);
    t2.assert1();
    t2.assertBST();
    std::cout << "Black depth: " << t2.countB() << std::endl;
    std::cout << "Member Z: " << t2.member('Z') << std::endl;
    std::cout << "--- Tree t3 ---" << std::endl;
    print(t3);
    t3.assert1();
    t3.assertBST();
    std::cout << "Black depth: " << t3.countB() << std::endl;
    std::cout << "Member .: " << t3.member('.') << std::endl;
    std::cout << "--- Tree t4 ---" << std::endl;
    print(t4);
    t4.assert1();
    t4.assertBST();
    std::cout << "Black depth: " << t4.countB() << std::endl;
    std::cout << "Member h: " << t4.member('h') << std::endl;
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
