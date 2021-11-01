#ifndef _INTERVAL_TREE_POM_TEST_HPP
#define _INTERVAL_TREE_POM_TEST_HPP

#include "interval_tree_pom.hpp"

#include <iostream>
#include <map>

template <class Key, class T>
class IntervalTreePOMTest
{
public:
    struct Interval
    {
        Key low;// lower key of interval
        Key high;// higher key of interval
    };
    struct ValueType
    {
        Interval interval;
        T mapped_value;
    };

    struct Node
    {
        Node* parent;
        Node* left;
        Node* right;
        enum { BLACK, RED } color;
        T mapped_value;
        const Key key;
        enum { LOWER = +1, HIGHER = -1 } endpoint_type;
        Node* related_endpoint;
        int type_sum;
        Node* max_overlap_node;
        int max_overlap_num;
        Node() {}
        Node(const Key& key) : key(key) {}
    };

    class TreePublic
    {
    public:
        Node* root_;
        Node* nil_;
    };

    Node* TreeMinimum(Node* sub_tree_root);
    Node* TreeMaximum(Node* sub_tree_root);
    Node* TreeSuccessor(Node* node);

    IntervalTreePOMTest(IntervalTreePOM<Key, T>& tree);
    bool CheckNodeValid(const Node* node);
    bool CheckBSTAndIteratorValid(const Node* root);
    int CheckRBSubtreeValid(const Node* subtree_root);
    bool CheckIntervalTreeValid(Node* subtree_root);
    int CountOverlapNum(Node* subtree_root, std::map<Node*, int>& overlap_num_map);// overlap_num_map is out-para; return max_overlap_num
    bool CheckIterator();
    bool CheckTreeValid();

    IntervalTreePOM<Key, T>& tree_;
    TreePublic& tree_pub_;
};

template <class Key, class T>
typename IntervalTreePOMTest<Key, T>::Node* IntervalTreePOMTest<Key, T>::TreeMinimum(Node* sub_tree_root)
{
    while (sub_tree_root->left != tree_pub_.nil_)
        sub_tree_root = sub_tree_root->left;
    return sub_tree_root;
}

template <class Key, class T>
typename IntervalTreePOMTest<Key, T>::Node* IntervalTreePOMTest<Key, T>::TreeMaximum(Node* sub_tree_root)
{
    while (sub_tree_root->right != tree_pub_.nil_)
        sub_tree_root = sub_tree_root->right;
    return sub_tree_root;
}

template <class Key, class T>
typename IntervalTreePOMTest<Key, T>::Node* IntervalTreePOMTest<Key, T>::TreeSuccessor(Node* node)
{
    Node* parent;
    if (node->right != tree_pub_.nil_)
        return TreeMinimum(node->right);
    parent = node->parent;
    while (parent != tree_pub_.nil_ && parent->right == node)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

template <class Key, class T>
IntervalTreePOMTest<Key, T>::IntervalTreePOMTest(IntervalTreePOM<Key, T>& tree) 
    : tree_(tree), tree_pub_(*(TreePublic*)&tree) {}

template <class Key, class T>
bool IntervalTreePOMTest<Key, T>::CheckNodeValid(const Node* node)
{
    if (node->parent != tree_pub_.nil_ && node->parent->left != node && node->parent->right != node)
    {
        return false;
    }
    if (node->right != tree_pub_.nil_ && node->right->parent != node)
    {
        return false;
    }
    if (node->left != tree_pub_.nil_ && node->left->parent != node)
    {
        return false;
    }
    return true;
}

// return number of black nodes on the simple path from the subtree_root to descendant leaves
// return -1 means the RBT is invalid
template <class Key, class T>
int IntervalTreePOMTest<Key, T>::CheckRBSubtreeValid(const Node* subtree_root)
{
    int black_node_num, left_black_node_num, right_black_node_num;
    if (subtree_root == tree_pub_.nil_)
        return 1;
    if (subtree_root->color == Node::RED && 
        (subtree_root->left->color != Node::BLACK || subtree_root->right->color != Node::BLACK))
        return -1;
    left_black_node_num = CheckRBSubtreeValid(subtree_root->left);
    right_black_node_num = CheckRBSubtreeValid(subtree_root->right);
    if (left_black_node_num == -1 || right_black_node_num == -1 || 
        left_black_node_num != right_black_node_num)
        return -1;
    return left_black_node_num + ((subtree_root->color == Node::BLACK) ? 1 : 0);
}

template <class Key, class T>
bool IntervalTreePOMTest<Key, T>::CheckIntervalTreeValid(Node* subtree_root)
{
    int max_overlap_num;
    std::map<Node*, int> overlap_num_map;
    if (subtree_root == tree_pub_.nil_) return true;
    // check endpoint_type
    if (subtree_root->endpoint_type != Node::LOWER && subtree_root->endpoint_type != Node::HIGHER)
        return false;
    // check related_endpoint
    if (subtree_root->related_endpoint->related_endpoint != subtree_root) return false;
    // check type_sum
    if (subtree_root->type_sum != subtree_root->left->type_sum + subtree_root->endpoint_type + 
        subtree_root->right->type_sum)
        return false;
    // check max_overlap
    max_overlap_num = CountOverlapNum(subtree_root, overlap_num_map);
    if (subtree_root->max_overlap_num != max_overlap_num) return false;
    if (overlap_num_map.at(subtree_root->max_overlap_node) != max_overlap_num) return false;
    if (subtree_root->max_overlap_node != subtree_root && 
        (subtree_root->left == tree_pub_.nil_ || subtree_root->max_overlap_node != subtree_root->left->max_overlap_node) &&
        (subtree_root->right == tree_pub_.nil_ || subtree_root->max_overlap_node != subtree_root->right->max_overlap_node))
        return false;
    // check leaf
    if (subtree_root->left == tree_pub_.nil_ && subtree_root->right == tree_pub_.nil_)
    {
        if (subtree_root->type_sum != subtree_root->endpoint_type) return false;
        if (subtree_root->max_overlap_num != subtree_root->endpoint_type) return false;
    }
    // check children
    if (CheckIntervalTreeValid(subtree_root->left) == false) return false;
    if (CheckIntervalTreeValid(subtree_root->right) == false) return false;
    return true;
}

template <class Key, class T>
int IntervalTreePOMTest<Key, T>::CountOverlapNum(Node* subtree_root, std::map<Node*, int>& overlap_num_map)
{
    Node *now_overlap_node, *end_node;
    int now_overlap_num, max_overlap_num;
    end_node = TreeSuccessor(TreeMaximum(subtree_root));
    now_overlap_num = 0;
    max_overlap_num = INT_MIN;
    for (now_overlap_node = TreeMinimum(subtree_root); now_overlap_node != end_node; 
        now_overlap_node = TreeSuccessor(now_overlap_node))
    {
        now_overlap_num += now_overlap_node->endpoint_type;
        if (now_overlap_num > max_overlap_num) max_overlap_num = now_overlap_num;
        overlap_num_map.insert(std::make_pair(now_overlap_node, now_overlap_num));
    }
    return max_overlap_num;
}

template <class Key, class T>
bool IntervalTreePOMTest<Key, T>::CheckBSTAndIteratorValid(const Node* root)
{
    typename IntervalTreePOM<Key, T>::Iterator it;
    Node* now;
    const Key* last_key_ptr;
    last_key_ptr = nullptr;
    now = TreeMinimum(tree_pub_.root_);
    it = tree_.Begin();
    while (now != tree_pub_.nil_)
    {
        // test node relationship
        if (CheckNodeValid(now) == false)
            return false;
        // test BST
        if (last_key_ptr && *last_key_ptr > now->key)
            return false;
        last_key_ptr = &(now->key);
        // test iterator
        if (now->endpoint_type == Node::LOWER)
        {
            if (*it != now->mapped_value) return false;
            if (it.GetInterval() != typename IntervalTreePOM<Key, T>::Interval(now->key, now->related_endpoint->key)) 
                return false;
            ++it;
        }
        // next
        now = TreeSuccessor(now);
    }
    if (it != tree_.End()) return false;
    return true;
}

template <class Key, class T>
bool IntervalTreePOMTest<Key, T>::CheckTreeValid()
{
    if (tree_pub_.root_->color != Node::BLACK || tree_pub_.nil_->color != Node::BLACK)
        return false;
    if (tree_pub_.nil_->type_sum != 0)
        return false;
    if (CheckBSTAndIteratorValid(tree_pub_.root_) == false)
        return false;
    if (CheckRBSubtreeValid(tree_pub_.root_) == -1)
        return false;
    if (CheckIntervalTreeValid(tree_pub_.root_) == false)
        return false;
    if (tree_pub_.root_->color != Node::BLACK || tree_pub_.nil_->color != Node::BLACK)
        return false;
    if (tree_pub_.nil_->type_sum != 0)
        return false;
    return true;
}

#endif
