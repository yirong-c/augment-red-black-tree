#include "augment_red_black_tree_test.hpp"

#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif
#include <catch/catch.hpp>

typedef AugmentRedBlackTreeTest<int, char> Tree;

TEST_CASE("osrbt")
{
    Tree tree;
    std::pair<Tree::Iterator, bool> insert_result, insert_result2;
    insert_result = tree.Insert({10, 'a'});
    REQUIRE(insert_result.second);
    tree.Insert({20, 'a'});
    tree.Insert({30, 'a'});
    tree.Insert({40, 'a'});
    REQUIRE(tree.CheckTreeValid());
    tree.Delete(insert_result.first);
    REQUIRE(tree.CheckTreeValid());
    tree.Insert({50, 'c'});
    insert_result = tree.Insert({80, 'a'});
    REQUIRE(insert_result.second);
    REQUIRE(tree.CheckTreeValid());
    tree.Delete(insert_result.first);
    REQUIRE(tree.CheckTreeValid());
    insert_result = tree.Insert({60, 'a'});
    REQUIRE(insert_result.second);
    insert_result2 = tree.Insert({40, 'd'});
    REQUIRE_FALSE(insert_result2.second);
    REQUIRE(insert_result2.first->first == 40);
    REQUIRE(insert_result2.first->second == 'a');
    insert_result2 = tree.Insert({10, 'd'});
    REQUIRE(insert_result2.second);
    REQUIRE(insert_result2.first->first == 10);
    REQUIRE(insert_result2.first->second == 'd');
    tree.Insert({70, 'a'});
    REQUIRE(tree.CheckTreeValid());
    tree.Delete(insert_result.first);
    REQUIRE(tree.CheckTreeValid());
    insert_result = tree.Insert({90, 'a'});
    REQUIRE(insert_result.second);
    tree.Insert({100, 'a'});
    REQUIRE(tree.CheckTreeValid());
    tree.Delete(insert_result.first);
    REQUIRE(tree.CheckTreeValid());
    
    REQUIRE(tree.At(50) == 'c');
    REQUIRE(tree.Find(10) == insert_result2.first);
    REQUIRE(tree[50] == 'c');

    ++insert_result2.first;
    REQUIRE(tree.Find(20) == insert_result2.first);

    --insert_result2.first;
    REQUIRE(tree.Find(10) == insert_result2.first);

    REQUIRE(tree.Find(200) == tree.End());
    REQUIRE(tree[200] == 0);
    REQUIRE(tree.Find(200) != tree.End());
    REQUIRE(tree.Find(200)->first == 200);
    REQUIRE(tree.Find(200)->second == 0);
    tree[200] = 'e';
    REQUIRE(tree[200] == 'e');
    REQUIRE(tree.Find(200) != tree.End());
    REQUIRE(tree.Find(200)->first == 200);
    REQUIRE(tree.Find(200)->second == 'e');

    REQUIRE(tree.Begin()->first == 10);
    REQUIRE(tree.Begin()->second == 'd');

    REQUIRE(tree.Select(1) == tree.Find(10));
    REQUIRE(tree.Select(2) == tree.Find(20));
    REQUIRE(tree.Select(3) == tree.Find(30));
    REQUIRE(tree.Select(4) == tree.Find(40));
    REQUIRE(tree.Select(5) == tree.Find(50));
    REQUIRE(tree.Select(6) == tree.Find(70));
    REQUIRE(tree.Select(7) == tree.Find(100));

    REQUIRE(tree.Rank(tree.Find(10)) == 1);
    REQUIRE(tree.Rank(tree.Find(20)) == 2);
    REQUIRE(tree.Rank(tree.Find(30)) == 3);
    REQUIRE(tree.Rank(tree.Find(40)) == 4);
    REQUIRE(tree.Rank(tree.Find(50)) == 5);
    REQUIRE(tree.Rank(tree.Find(70)) == 6);
    REQUIRE(tree.Rank(tree.Find(100)) == 7);
}

TEST_CASE("check tree valid")
{
    Tree tree;
    std::pair<Tree::Iterator, bool> insert_result;
    insert_result = tree.Insert({200, 'a'});
    tree.Insert({100, 'a'});
    tree.Insert({300, 'a'});
    tree.Insert({270, 'a'});
    tree.Insert({350, 'a'});
    tree.Insert({230, 'a'});
    tree.Insert({280, 'a'});
    tree.Insert({250, 'a'});
    tree.Insert({240, 'a'});
    tree.Insert({260, 'a'});
    REQUIRE(tree.CheckTreeValid());
    tree.Delete(insert_result.first);
    REQUIRE(tree.CheckTreeValid());

    REQUIRE(tree.Select(1) == tree.Find(100));
    REQUIRE(tree.Select(2) == tree.Find(230));
    REQUIRE(tree.Select(3) == tree.Find(240));
    REQUIRE(tree.Select(4) == tree.Find(250));
    REQUIRE(tree.Select(5) == tree.Find(260));
    REQUIRE(tree.Select(6) == tree.Find(270));
    REQUIRE(tree.Select(7) == tree.Find(280));
    REQUIRE(tree.Select(8) == tree.Find(300));
    REQUIRE(tree.Select(9) == tree.Find(350));
}
