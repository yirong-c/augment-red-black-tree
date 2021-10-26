#ifndef _INTERVAL_TREE_TEST_HPP
#define _INTERVAL_TREE_TEST_HPP

#include "interval_tree.hpp"

#include <iostream>

template <class Key, class T>
class IntervalTreeTest
{
public:
    struct Interval
    {
        Key low;
        Key high;
    };

    typedef std::pair<const Interval, T> ValueType;

    struct Node
    {
        Node* parent;
        Node* left;
        Node* right;
        ValueType value;
        enum { BLACK, RED } color;
        Key max;
        Node() {}
        Node(const ValueType& value) : value(value) {}
    };

    class TreePublic
    {
    public:
        Node* root_;
        Node* nil_;
    };

    Node* TreeMinimum(Node* sub_tree_root);
    Node* TreeSuccessor(Node* node);

    IntervalTreeTest(IntervalTree<Key, T>& tree);
    bool CheckNodeValid(const Node* node);
    int CheckRBSubtreeValid(const Node* subtree_root);
    bool CheckIntervalTreeValid(const Node* subtree_root);
    bool CheckTreeValid();

    TreePublic& tree_;
};

template <class Key, class T>
typename IntervalTreeTest<Key, T>::Node* IntervalTreeTest<Key, T>::TreeMinimum(Node* sub_tree_root)
{
    while (sub_tree_root->left != tree_.nil_)
        sub_tree_root = sub_tree_root->left;
    return sub_tree_root;
}

template <class Key, class T>
typename IntervalTreeTest<Key, T>::Node* IntervalTreeTest<Key, T>::TreeSuccessor(Node* node)
{
    Node* parent;
    if (node->right != tree_.nil_)
        return TreeMinimum(node->right);
    parent = node->parent;
    while (parent != tree_.nil_ && parent->right == node)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

template <class Key, class T>
IntervalTreeTest<Key, T>::IntervalTreeTest(IntervalTree<Key, T>& tree) : tree_(*(TreePublic*)&tree) {}

template <class Key, class T>
bool IntervalTreeTest<Key, T>::CheckNodeValid(const Node* node)
{
    if (node->parent != tree_.nil_ && node->parent->left != node && node->parent->right != node)
    {
        return false;
    }
    if (node->right != tree_.nil_ && node->right->parent != node)
    {
        return false;
    }
    if (node->left != tree_.nil_ && node->left->parent != node)
    {
        return false;
    }
    return true;
}

// return number of black nodes on the simple path from the subtree_root to descendant leaves
// return -1 means the RBT is invalid
template <class Key, class T>
int IntervalTreeTest<Key, T>::CheckRBSubtreeValid(const Node* subtree_root)
{
    int black_node_num, left_black_node_num, right_black_node_num;
    if (subtree_root == tree_.nil_)
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
bool IntervalTreeTest<Key, T>::CheckIntervalTreeValid(const Node* subtree_root)
{
    Key max;
    if (subtree_root == tree_.nil_) return true;
    max = std::max(subtree_root->left->max, subtree_root->right->max);
    max = std::max(max, subtree_root->value.first.high);
    if (max != subtree_root->max) return false;
    if (CheckIntervalTreeValid(subtree_root->left) == false) return false;
    if (CheckIntervalTreeValid(subtree_root->right) == false) return false;
    return true;
}

template <class Key, class T>
bool IntervalTreeTest<Key, T>::CheckTreeValid()
{
    Node* now;
    const Key* last_key_ptr;
    last_key_ptr = nullptr;
    now = TreeMinimum(tree_.root_);

    if (tree_.root_->color != Node::BLACK || tree_.nil_->color != Node::BLACK)
        return false;
    if (tree_.nil_->max != INT_MIN)
        return false;
    
    while (now != tree_.nil_)
    {
        //test node relationship
        if (CheckNodeValid(now) == false)
            return false;
        //test BST
        if (last_key_ptr && *last_key_ptr >= now->value.first.low)
            return false;
        last_key_ptr = &now->value.first.low;
        //next
        now = TreeSuccessor(now);
    }

    if (CheckRBSubtreeValid(tree_.root_) == -1)
        return false;

    if (CheckIntervalTreeValid(tree_.root_) == false)
        return false;
    
    if (tree_.root_->color != Node::BLACK || tree_.nil_->color != Node::BLACK)
        return false;
    if (tree_.nil_->max != INT_MIN)
        return false;
    
    return true;
}

#endif
