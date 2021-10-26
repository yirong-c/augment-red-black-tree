#ifndef _INTERVAL_TREE_HPP
#define _INTERVAL_TREE_HPP

#include <utility>
#include <iterator>
#include <stdexcept>
#include <limits.h>

// intervals are close interval

// ---------- declaration ----------

template <class Key, class T>
class IntervalTree
{
private:
    struct Interval
    {
        Key low;
        Key high;
    };

public:
    typedef std::pair<const Interval, T> ValueType;

private:
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

public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, ValueType>
    {
    public:
        Iterator& operator++() { node_ = tree_->TreeSuccessor(node_); return *this; }
        Iterator& operator--() { node_ = tree_->TreePredecessor(node_); return *this; }
        ValueType& operator*() const { return node_->value; }
        ValueType* operator->() const { return &(node_->value); }
        bool operator==(const Iterator& other) const { return node_ == other.node_; }
        bool operator!=(const Iterator& other) const { return node_ != other.node_; }
        Iterator() : node_(nullptr), tree_(nullptr) {}
    private:
        friend class IntervalTree<Key, T>;
        Iterator(Node* node, IntervalTree<Key, T>* tree) 
            : node_(node), tree_(tree) {}
        Node* node_;
        IntervalTree<Key, T>* tree_;
    };

    IntervalTree();
    ~IntervalTree();
    std::pair<Iterator, bool> Insert(const ValueType& value);
    void Delete(Iterator pos);
    void Clear();
    T& At(const Interval& interval);
    Iterator Find(const Interval& interval);
    Iterator Begin();
    Iterator End();
    T& operator[](Interval&& interval);

private:
    void LeftRotate(Node* subtree_root_node);
    void RightRotate(Node* subtree_root_node);
    void InsertFixup(Node* node);
    void Transplant(Node* old_node, Node* new_node);
    Node* TreeMinimum(Node* sub_tree_root);
    Node* TreeMaximum(Node* sub_tree_root);
    Node* TreeSuccessor(Node* node);
    Node* TreePredecessor(Node* node);
    void Delete(Node *node);
    void DeleteFixup(Node* node);
    Node* FindNodeByInterval(const Interval& interval);// find overlap
    Node** FindNodePtrByKey(const Key& key, Node **parent_ptr);// find key; parent_ptr is out-para
    bool IsOverlap(const Interval& first, const Interval& second);
    void FixMax(Node* node);// start to check max attribute from node->parent to root_

    Node* root_;
    Node* nil_;

};

// ---------- definition ----------

template <class Key, class T>
IntervalTree<Key, T>::IntervalTree()
{
    nil_ = new Node();
    nil_->color = Node::BLACK;
    root_ = nil_;
    nil_->max = INT_MIN;
}

template <class Key, class T>
IntervalTree<Key, T>::~IntervalTree()
{
    Clear();
    delete nil_;
}

template <class Key, class T>
void IntervalTree<Key, T>::LeftRotate(Node* subtree_root_node) 
{
    Node* new_root;
    new_root = subtree_root_node->right;
    subtree_root_node->right = new_root->left;
    if (subtree_root_node->right != nil_)
        subtree_root_node->right->parent = subtree_root_node;
    new_root->parent = subtree_root_node->parent;
    if (subtree_root_node->parent == nil_)
        root_ = new_root;
    else if (subtree_root_node->parent->left == subtree_root_node)
        subtree_root_node->parent->left = new_root;
    else
        subtree_root_node->parent->right = new_root;
    new_root->left = subtree_root_node;
    subtree_root_node->parent = new_root;
    new_root->max = subtree_root_node->max;
    subtree_root_node->max = std::max(subtree_root_node->left->max, subtree_root_node->right->max);
    subtree_root_node->max = std::max(subtree_root_node->max, subtree_root_node->value.first.high);
}

template <class Key, class T>
void IntervalTree<Key, T>::RightRotate(Node* subtree_root_node) 
{
    Node* new_root;
    new_root = subtree_root_node->left;
    subtree_root_node->left = new_root->right;
    if (subtree_root_node->left != nil_)
        subtree_root_node->left->parent = subtree_root_node;
    new_root->parent = subtree_root_node->parent;
    if (subtree_root_node->parent == nil_)
        root_ = new_root;
    else if (subtree_root_node->parent->left == subtree_root_node)
        subtree_root_node->parent->left = new_root;
    else
        subtree_root_node->parent->right = new_root;
    new_root->right = subtree_root_node;
    subtree_root_node->parent = new_root;
    new_root->max = subtree_root_node->max;
    subtree_root_node->max = std::max(subtree_root_node->left->max, subtree_root_node->right->max);
    subtree_root_node->max = std::max(subtree_root_node->max, subtree_root_node->value.first.high);
}

template <class Key, class T>
typename IntervalTree<Key, T>::Node* IntervalTree<Key, T>::FindNodeByInterval(const Interval& interval)
{
    Node *now;
    now = root_;
    while (now != nil_ && IsOverlap(now->value.first, interval) == false)
    {
        if (now->left != nil_ && now->left->max >= interval.low)
            now = now->left;
        else
            now = now->right;
    }
    return now;
}

template <class Key, class T>
typename IntervalTree<Key, T>::Node** IntervalTree<Key, T>::FindNodePtrByKey
    (const Key& key, Node **parent_ptr)// parent_ptr is out-para
{
    Node **now_ptr;
    *parent_ptr = nil_;
    now_ptr = &root_;
    while (*now_ptr != nil_)
    {
        *parent_ptr = *now_ptr;
        if ((*now_ptr)->value.first.low == key)
            break;
        else if ((*now_ptr)->value.first.low > key)
            now_ptr = &((*now_ptr)->left);
        else
            now_ptr = &((*now_ptr)->right);
    }
    return now_ptr;
}

template <class Key, class T>
bool IntervalTree<Key, T>::IsOverlap(const Interval& first, const Interval& second)
{
    return !(first.high < second.low || second.high < first.low);
}

template <class Key, class T>
typename IntervalTree<Key, T>::Iterator IntervalTree<Key, T>::Find(const Interval& interval)
{
    return Iterator(FindNodeByInterval(interval), this);
}
 
template <class Key, class T>
T& IntervalTree<Key, T>::At(const Interval& interval)
{
    Node *node;
    node = FindNodeByInterval(interval);
    if (node == nil_) 
        throw std::out_of_range("the container does not have an element overlap with the specified interval");
    return node->value.second;
}

template <class Key, class T>
T& IntervalTree<Key, T>::operator[](Interval&& interval)
{
    Node **node_ptr, *parent;
    node_ptr = FindNodePtrByKey(interval.low, &parent);
    if (*node_ptr == nil_)
    {
        *node_ptr = new Node(std::make_pair(interval, T()));
        (*node_ptr)->parent = parent;
        (*node_ptr)->right = (*node_ptr)->left = nil_;
        (*node_ptr)->color = Node::RED;
        (*node_ptr)->max = interval.high;
        FixMax(*node_ptr);
        InsertFixup(*node_ptr);
    }
    return (*node_ptr)->value.second;
}

template <class Key, class T>
std::pair<typename IntervalTree<Key, T>::Iterator, bool> 
    IntervalTree<Key, T>::Insert(const ValueType& value)
{
    Node **node_ptr, *parent;
    node_ptr = FindNodePtrByKey(value.first.low, &parent);
    if (*node_ptr == nil_)
    {
        *node_ptr = new Node(value);
        (*node_ptr)->parent = parent;
        (*node_ptr)->right = (*node_ptr)->left = nil_;
        (*node_ptr)->color = Node::RED;
        (*node_ptr)->max = value.first.high;
        FixMax(*node_ptr);
        InsertFixup(*node_ptr);
        return std::make_pair(Iterator(*node_ptr, this), true);
    }
    else
    {
        return std::make_pair(Iterator(*node_ptr, this), false);
    }
}

template <class Key, class T>
void IntervalTree<Key, T>::InsertFixup(Node* node)
{
    Node *uncle, *grandparent;
    void(IntervalTree<Key, T>::*grandparent_rotate)(Node*);
    while (node->parent->color == Node::RED)
    {
        grandparent = node->parent->parent;
        if (node->parent == grandparent->left)
        {
            uncle = grandparent->right;
            grandparent_rotate = &IntervalTree<Key, T>::RightRotate;
        }
        else
        {
            uncle = grandparent->left;
            grandparent_rotate = &IntervalTree<Key, T>::LeftRotate;
        }
        if (uncle->color == Node::RED)
        {
            uncle->color = node->parent->color = Node::BLACK;
            grandparent->color = Node::RED;
            node = grandparent;
        }
        else
        {
            if (node == node->parent->right)
            {
                node = node->parent;
                LeftRotate(node);
                // notice that varible grandparent do not need to update here,
                // since it is still the grandparent of varible node
            }
            node->parent->color = Node::BLACK;
            grandparent->color = Node::RED;
            (this->*grandparent_rotate)(grandparent);
        }
    }
    root_->color = Node::BLACK;
}

template <class Key, class T>
void IntervalTree<Key, T>::Transplant(Node* old_node, Node* new_node)
{
    if (old_node->parent == nil_)
        root_ = new_node;
    else if (old_node == old_node->parent->left)
        old_node->parent->left = new_node;
    else
        old_node->parent->right = new_node;
    new_node->parent = old_node->parent;
}

template <class Key, class T>
typename IntervalTree<Key, T>::Node* IntervalTree<Key, T>::TreeMinimum(Node* sub_tree_root)
{
    while (sub_tree_root->left != nil_)
        sub_tree_root = sub_tree_root->left;
    return sub_tree_root;
}

template <class Key, class T>
typename IntervalTree<Key, T>::Node* IntervalTree<Key, T>::TreeMaximum(Node* sub_tree_root)
{
    while (sub_tree_root->right != nil_)
        sub_tree_root = sub_tree_root->right;
    return sub_tree_root;
}

template <class Key, class T>
typename IntervalTree<Key, T>::Node* IntervalTree<Key, T>::TreeSuccessor(Node* node)
{
    Node* parent;
    if (node->right != this->nil_)
        return this->TreeMinimum(node->right);
    parent = node->parent;
    while (parent != this->nil_ && parent->right == node)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

template <class Key, class T>
typename IntervalTree<Key, T>::Node* IntervalTree<Key, T>::TreePredecessor(Node* node)
{
    Node* parent;
    if (node->left)
        return TreeMaximum(node->left);
    parent = node->parent;
    while (parent && parent->left == node)
    {
        node = parent;
        parent = node->parent;
    }
    return parent;
}

template <class Key, class T>
void IntervalTree<Key, T>::Delete(Iterator pos)
{
    Delete(pos.node_);
}

template <class Key, class T>
void IntervalTree<Key, T>::Delete(Node *node)
{
    Node *replaced, *replaced_replaced;
    bool is_black_deleted;
    replaced = node;
    is_black_deleted = replaced->color == Node::BLACK;
    if (replaced->left == nil_)
    {
        // do NOT need two "replaced" to store moved node
        replaced_replaced = replaced->right;
        Transplant(replaced, replaced_replaced);
        FixMax(replaced_replaced);// replaced_replaced->max will NOT be checked
    }
    else if (replaced->right == nil_)
    {
        // do NOT need two "replaced" to store moved node
        replaced_replaced = replaced->left;
        Transplant(replaced, replaced_replaced);
        FixMax(replaced_replaced);// replaced_replaced->max will NOT be checked
    }
    else
    {
        // DO need two "replaced" to store moved node
        // re-assign varible "replaced" and "is_black_deleted"
        replaced = TreeMinimum(node->right);
        is_black_deleted = replaced->color == Node::BLACK;
        replaced_replaced = replaced->right;
        if (replaced->parent == node)
        {
            // neccessary when "replaced_replaced" is nil_
            replaced_replaced->parent = replaced;
        }
        else
        {
            Transplant(replaced, replaced_replaced);
            replaced->right = node->right;
            replaced->right->parent = replaced;
        }
        Transplant(node, replaced);
        replaced->left = node->left;
        replaced->left->parent = replaced;
        replaced->color = node->color;// this is why "is_black_deleted" need to re-assign in this case
        FixMax(replaced_replaced);// replaced_replaced->max will NOT be checked
        FixMax(replaced->right);// so replaced->max will be checked
    }
    if (is_black_deleted)
        // In order to maintain property 5,
        // "replaced_replaced" node has extra black (either "doubly black" or "red-and-black", contributes either 2 or 1)
        DeleteFixup(replaced_replaced);
    delete node;
}

// start to check max attribute from node->parent to root_
template <class Key, class T>
void IntervalTree<Key, T>::FixMax(Node* node)
{
    Key max;
    while (node != root_)
    {
        node = node->parent;
        max = std::max(node->left->max, node->right->max);
        max = std::max(max, node->value.first.high);
        if (node->max != max) 
            node->max = max;
        else
            break;
    }
}

template <class Key, class T>
void IntervalTree<Key, T>::DeleteFixup(Node* node)
{
    Node* sibling;
    while (node != root_ && node->color == Node::BLACK)
    {
        if (node == node->parent->left)
        {
            sibling = node->parent->right;
            if (sibling->color == Node::RED)
            {
                sibling->color = Node::BLACK;
                node->parent->color = Node::RED;
                LeftRotate(node->parent);
                sibling = node->parent->right;
            }
            if (sibling->left->color == Node::BLACK && sibling->right->color == Node::BLACK)
            {
                sibling->color = Node::RED;
                node = node->parent;
            }
            else
            {
                if (sibling->right->color == Node::BLACK)
                {
                    sibling->left->color = Node::BLACK;
                    sibling->color = Node::RED;
                    RightRotate(sibling);
                    sibling = node->parent->right;
                }
                sibling->color = node->parent->color;
                node->parent->color = Node::BLACK;
                sibling->right->color = Node::BLACK;
                LeftRotate(node->parent);
                node = root_;
            }
        }
        else
        {
            sibling = node->parent->left;
            if (sibling->color == Node::RED)
            {
                sibling->color = Node::BLACK;
                node->parent->color = Node::RED;
                RightRotate(node->parent);
                sibling = node->parent->left;
            }
            if (sibling->left->color == Node::BLACK && sibling->right->color == Node::BLACK)
            {
                sibling->color = Node::RED;
                node = node->parent;
            }
            else
            {
                if (sibling->left->color == Node::BLACK)
                {
                    sibling->right->color = Node::BLACK;
                    sibling->color = Node::RED;
                    LeftRotate(sibling);
                    sibling = node->parent->left;
                }
                sibling->color = node->parent->color;
                node->parent->color = Node::BLACK;
                sibling->left->color = Node::BLACK;
                RightRotate(node->parent);
                node = root_;
            }
        }
    }
    node->color = Node::BLACK;
}

template <class Key, class T>
void IntervalTree<Key, T>::Clear()
{
    Node *now, *parent;
    if (root_ == nil_) return;
    now = TreeMinimum(root_);
    while (now != nil_)
    {
        while (now->right != nil_)
        {
            now = TreeMinimum(now->right);
        }
        parent = now->parent;
        while (parent != nil_ && parent->right == now)
        {
            delete now;
            now = parent;
            parent = now->parent;
        }
        delete now;
        now = parent;
    }
    root_ = nil_;
}

template <class Key, class T>
typename IntervalTree<Key, T>::Iterator IntervalTree<Key, T>::Begin()
{
    return Iterator(TreeMinimum(root_), this);
}

template <class Key, class T>
typename IntervalTree<Key, T>::Iterator IntervalTree<Key, T>::End()
{
    return Iterator(nil_, this);
}

#endif
