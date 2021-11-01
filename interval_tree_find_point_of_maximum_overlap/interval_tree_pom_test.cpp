#include "interval_tree_pom.hpp"
#include "interval_tree_pom_test.hpp"

#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif
#include <catch/catch.hpp>

typedef IntervalTreePOM<int, char> Tree;
typedef IntervalTreePOMTest<int, char> TreeTest;

TEST_CASE("check data structure 1")
{
    bool thrown;
    Tree tree;
    TreeTest tree_test(tree);
    std::pair<Tree::Iterator, bool> insert_result, insert_result2;
    Tree::Iterator iterator;

    insert_result = tree.Insert({ { 40, 80 }, 'a' });
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({ { 10, 50 }, 'a' });
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({ { 30, 60 }, 'a' });
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({ { 70, 110 }, 'a' });
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({ { 90, 120 }, 'a' });
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
    
    insert_result = tree.Insert({ { 20, 100 }, 'a' });
    REQUIRE(insert_result.second);
    REQUIRE(tree_test.CheckTreeValid());
}

TEST_CASE("check data structure 2")
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

    // find pom
    REQUIRE(tree.FindPOM() == 17);

    // delete
    iterator = tree.FindByLowerKey(16);
    REQUIRE(*iterator == 'f');
    REQUIRE(iterator.GetInterval() == Tree::Interval(16, 21));

    tree.Delete(iterator);
    REQUIRE(tree_test.CheckTreeValid());

    iterator = tree.FindByLowerKey(15);
    REQUIRE(*iterator == 'b');
    REQUIRE(iterator.GetInterval() == Tree::Interval(15, 23));

    iterator = tree.FindByLowerKey(0);
    REQUIRE(*iterator == 'g');
    REQUIRE(iterator.GetInterval() == Tree::Interval(0, 3));

    tree.Delete(iterator);
    REQUIRE(tree_test.CheckTreeValid());

    iterator = tree.FindByLowerKey(0);
    REQUIRE(iterator == tree.End());
}
