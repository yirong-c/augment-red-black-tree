#include "interval_tree.hpp"
#include "interval_tree_test.hpp"

#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif
#include <catch/catch.hpp>

typedef IntervalTree<int, char> Tree;
typedef IntervalTreeTest<int, char> TreeTest;

TEST_CASE("example")
{
    bool thrown;
    Tree tree;
    TreeTest tree_test(tree);
    std::pair<Tree::Iterator, bool> insert_result, insert_result2;
    Tree::Iterator iterator;

    // insert
    insert_result = tree.Insert({{5, 8}, 'a'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({{8, 9}, 'a'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({{16, 21}, 'f'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({{25, 30}, 'a'});
    REQUIRE(insert_result.second);
    
    insert_result = tree.Insert({{15, 23}, 'b'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({{17, 19}, 'a'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({{26, 26}, 'a'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({{0, 3}, 'g'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({{6, 10}, 'a'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({{19, 20}, 'a'});
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());

    // insert repeat key
    insert_result = tree.Insert({{15, 30}, 'c'});
    REQUIRE_FALSE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());

    // find
    iterator = tree.Find({22, 25});
    REQUIRE(iterator->first.low == 15);
    REQUIRE(iterator->first.high == 23);
    
    iterator = tree.Find({11, 14});
    REQUIRE(iterator == tree.End());

    // at
    REQUIRE('b' == tree.At({22, 25}));
    
    thrown = false;
    try
    {
        tree.At({11, 14});
    }
    catch(const std::exception& e)
    {
        thrown = true;
    }
    REQUIRE(thrown);

    // operator[]
    tree[{25, 50}] = 'd';
    iterator = tree.Find({24, 26});
    REQUIRE(iterator->first.low == 25);
    REQUIRE(iterator->first.high == 30);
    REQUIRE(iterator->second == 'd');
    REQUIRE(tree_test.CheckTreeValid());

    tree[{24, 35}] = 'e';
    iterator = tree.Find({24, 24});
    REQUIRE(iterator->first.low == 24);
    REQUIRE(iterator->first.high == 35);
    REQUIRE(iterator->second == 'e');
    REQUIRE(tree_test.CheckTreeValid());
    
    iterator = tree.Find({24, 25});
    REQUIRE(iterator->first.low == 25);
    REQUIRE(iterator->first.high == 30);
    REQUIRE(iterator->second == 'd');

    // delete
    iterator = tree.Find({21, 21});
    REQUIRE(iterator->first.low == 16);
    REQUIRE(iterator->first.high == 21);
    REQUIRE(iterator->second == 'f');

    tree.Delete(iterator);
    REQUIRE(tree_test.CheckTreeValid());

    iterator = tree.Find({21, 21});
    REQUIRE(iterator->first.low == 15);
    REQUIRE(iterator->first.high == 23);
    REQUIRE(iterator->second == 'b');

    iterator = tree.Find({1, 1});
    REQUIRE(iterator->first.low == 0);
    REQUIRE(iterator->first.high == 3);
    REQUIRE(iterator->second == 'g');

    tree.Delete(iterator);
    REQUIRE(tree_test.CheckTreeValid());

    iterator = tree.Find({1, 1});
    REQUIRE(iterator == tree.End());
}
