#ifndef _INTERVAL_TREE_POM_HPP
#define _INTERVAL_TREE_POM_HPP

#include <utility>
#include <iterator>
#include <stdexcept>
#include <limits.h>

// intervals are close interval
// primary key is the lower key of interval
// primary key has to be unique

// ---------- declaration ----------

template <class Key, class T>
class IntervalTreePOM
{
public:
    struct Interval
    {
        Key low;// lower key of interval
        Key high;// higher key of interval
        Interval(Key low = Key(), Key high = Key()) : low(low), high(high) {}
        bool operator==(const Interval& other) const { return low == other.low && high == other.high; }
    };
    struct ValueType
    {
        Interval interval;
        T mapped_value;
        ValueType(Interval interval = Interval(), T mapped_value = T()) 
            : interval(interval), mapped_value(mapped_value) {}
    };
private:
    struct Node
    {
        Node* parent;
        Node* left;
        Node* right;
        enum { BLACK, RED } color;
        T mapped_value;// only lower endpoint
        const Key key;
        enum { LOWER = +1, HIGHER = -1 } endpoint_type;
        Node* related_endpoint;// other endpoint of the current interval
        /**
         * x.type_sum = x.left.type_sum + x.endpoint_type + x.right.type_sum
         */
        int type_sum;
        /**
         * x.left_max_overlap_num = x.left.max_overlap_num
         * x.overlap_num = x.left.type_sum + x.endpoint_type
         * x.right_max_overlap_num = x.overlap_num + x.right.max_overlap_num
         * x.max_overlap_num = max(x.left_max_overlap_num, x.overlap_num, x.right_max_overlap_num)
         * 
         * Notice that overlap_num only consider local overlap (in the subtree root at the node)
         * This says overlap_num is relatively
         */
        Node* max_overlap_node;// node with endpoint which has maximum number of overlap segments
        int max_overlap_num;// maximum number of overlap segments

        Node() : key(Key()) {}
        Node(const Key& key) : key(key) {}
    };

public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T>
    {
    public:
        // TODO
        Iterator& operator++() 
        {
            do node_ = tree_->TreeSuccessor(node_); 
            while (node_->endpoint_type == Node::HIGHER && node_ != tree_->nil_);
            return *this; 
        }
        Iterator& operator--() 
        { 
            do node_ = tree_->TreePredecessor(node_); 
            while (node_->endpoint_type == Node::HIGHER && node_ != tree_->nil_); 
            return *this; 
        }
        T& operator*() const { return node_->mapped_value; }
        T* operator->() const { return &(node_->mapped_value); }
        bool operator==(const Iterator& other) const { return node_ == other.node_; }
        bool operator!=(const Iterator& other) const { return node_ != other.node_; }
        Interval GetInterval() const
        {
            if (node_->endpoint_type == Node::LOWER) 
                return Interval(node_->key, node_->related_endpoint->key);
            else
                return Interval(node_->related_endpoint->key, node_->key); 
        }
        Iterator() : node_(nullptr), tree_(nullptr) {}
    private:
        friend class IntervalTreePOM<Key, T>;
        Iterator(Node* node, IntervalTreePOM<Key, T>* tree) 
            : node_(node), tree_(tree) {}
        Node* node_;
        IntervalTreePOM<Key, T>* tree_;
    };

    IntervalTreePOM();
    ~IntervalTreePOM();
    std::pair<Iterator, bool> Insert(const ValueType& value);
    void Delete(Iterator pos);
    Iterator FindByLowerKey(const Key& key);
    Key FindPOM();// find point of maximum overlap
    void Clear();
    Iterator Begin();
    Iterator End();

private:
    void LeftRotate(Node* subtree_root_node);
    void RightRotate(Node* subtree_root_node);
    void InsertFixup(Node* node);
    void Transplant(Node* old_node, Node* new_node);
    Node* TreeMinimum(Node* sub_tree_root);
    Node* TreeMaximum(Node* sub_tree_root);
    Node* TreeSuccessor(Node* node);
    Node* TreePredecessor(Node* node);
    void DeleteNode(Node *node);
    void DeleteFixup(Node* node);
    Node* FindNodeByLowerKey(const Key& key);
    Node** FindNodePtrByLowerKey(const Key& key, Node **parent_ptr);// parent_ptr is out-para
    Node** FindLeafNodePtrToInsertByKey(const Key& key, Node **parent_ptr);// parent_ptr is out-para
    bool MaintainAugmentedAttributesOfSingleNode(Node *node);
    void FixAugmentedAttributes(Node* node);// start to check augmented attributes from node->parent to root_

    Node* root_;
    Node* nil_;

};

// ---------- definition ----------

template <class Key, class T>
IntervalTreePOM<Key, T>::IntervalTreePOM()
{
    nil_ = new Node();
    nil_->color = Node::BLACK;
    nil_->type_sum = 0;
    root_ = nil_;
}

template <class Key, class T>
IntervalTreePOM<Key, T>::~IntervalTreePOM()
{
    Clear();
    delete nil_;
}

template <class Key, class T>
bool IntervalTreePOM<Key, T>::MaintainAugmentedAttributesOfSingleNode(Node *node)
{
    int type_sum, max_overlap_num, overlap_num, right_max_overlap_num;
    Node *max_overlap_node;
    bool modified;
    /* maintain type_sum */
    type_sum = node->left->type_sum + node->endpoint_type + node->right->type_sum;
    /* maintain max_overlap */
    overlap_num = node->left->type_sum + node->endpoint_type;
    if (node->left != nil_ && node->left->max_overlap_num > overlap_num)
    {
        max_overlap_num = node->left->max_overlap_num;
        max_overlap_node = node->left->max_overlap_node;
    }
    else
    {
        max_overlap_num = overlap_num;
        max_overlap_node = node;
    }
    if (node->right != nil_)
    {
        right_max_overlap_num = overlap_num + node->right->max_overlap_num;
        if (right_max_overlap_num > max_overlap_num)
        {
            max_overlap_num = right_max_overlap_num;
            max_overlap_node = node->right->max_overlap_node;
        }
    }
    /* do modify */
    modified = false;
    if (node->type_sum != type_sum)
    {
        node->type_sum = type_sum;
        modified = true;
    }
    if (node->max_overlap_num != max_overlap_num)
    {
        node->max_overlap_num = max_overlap_num;
        modified = true;
    }
    if (node->max_overlap_node != max_overlap_node)
    {
        node->max_overlap_node = max_overlap_node;
        modified = true;
    }
    return modified;
}

template <class Key, class T>
void IntervalTreePOM<Key, T>::LeftRotate(Node* subtree_root_node) 
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
    /* maintain attributes for augment */
    new_root->type_sum = subtree_root_node->type_sum;
    new_root->max_overlap_node = subtree_root_node->max_overlap_node;
    new_root->max_overlap_num = subtree_root_node->max_overlap_num;
    MaintainAugmentedAttributesOfSingleNode(subtree_root_node);
}

template <class Key, class T>
void IntervalTreePOM<Key, T>::RightRotate(Node* subtree_root_node) 
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
    /* maintain attributes for augment */
    new_root->type_sum = subtree_root_node->type_sum;
    new_root->max_overlap_node = subtree_root_node->max_overlap_node;
    new_root->max_overlap_num = subtree_root_node->max_overlap_num;
    MaintainAugmentedAttributesOfSingleNode(subtree_root_node);
}

template <class Key, class T>
typename IntervalTreePOM<Key, T>::Node* IntervalTreePOM<Key, T>::FindNodeByLowerKey(const Key& key)
{
    Node* now;
    now = root_;
    while (now != nil_)
    {
        if (now->key == key && now->endpoint_type == Node::LOWER)
            break;
        if (now->key > key)
            now = now->left;
        else
            now = now->right;
    }
    return now;
}

template <class Key, class T>
typename IntervalTreePOM<Key, T>::Node** IntervalTreePOM<Key, T>::FindNodePtrByLowerKey
    (const Key& key, Node **parent_ptr)// parent_ptr is out-para
{
    Node **now_ptr;
    *parent_ptr = nil_;
    now_ptr = &root_;
    while (*now_ptr != nil_)
    {
        *parent_ptr = *now_ptr;
        if ((*now_ptr)->key == key && (*now_ptr)->endpoint_type == Node::LOWER)
            break;
        if ((*now_ptr)->key > key)
            now_ptr = &((*now_ptr)->left);
        else
            now_ptr = &((*now_ptr)->right);
    }
    return now_ptr;
}

template <class Key, class T>
typename IntervalTreePOM<Key, T>::Node** IntervalTreePOM<Key, T>::FindLeafNodePtrToInsertByKey
    (const Key& key, Node **parent_ptr)// parent_ptr is out-para
{
    Node **now_ptr;
    *parent_ptr = nil_;
    now_ptr = &root_;
    while (*now_ptr != nil_)
    {
        *parent_ptr = *now_ptr;
        if ((*now_ptr)->key > key)
            now_ptr = &((*now_ptr)->left);
        else
            now_ptr = &((*now_ptr)->right);
    }
    return now_ptr;
}

template <class Key, class T>
typename IntervalTreePOM<Key, T>::Iterator IntervalTreePOM<Key, T>::FindByLowerKey(const Key& key)
{
    return Iterator(FindNodeByLowerKey(key), this);
}
 
template <class Key, class T>
std::pair<typename IntervalTreePOM<Key, T>::Iterator, bool> 
    IntervalTreePOM<Key, T>::Insert(const ValueType& value)
{
    Node **node_ptr, *lower_node, *higher_node,  *parent;
    node_ptr = FindNodePtrByLowerKey(value.interval.low, &parent);
    if (*node_ptr == nil_)
    {
        /* insert lower endpoint */
        lower_node = new Node(value.interval.low);
        *node_ptr = lower_node;
        lower_node->parent = parent;
        lower_node->left = nil_;
        lower_node->right = nil_;
        lower_node->color = Node::RED;
        lower_node->mapped_value = value.mapped_value;
        lower_node->endpoint_type = Node::LOWER;
        MaintainAugmentedAttributesOfSingleNode(lower_node);
        FixAugmentedAttributes(lower_node);
        InsertFixup(lower_node);
        /* insert higher endpoint */
        node_ptr = FindLeafNodePtrToInsertByKey(value.interval.high, &parent);
        higher_node = new Node(value.interval.high);
        *node_ptr = higher_node;
        lower_node->related_endpoint = higher_node;
        higher_node->related_endpoint = lower_node;
        higher_node->parent = parent;
        higher_node->left = nil_;
        higher_node->right = nil_;
        higher_node->color = Node::RED;
        higher_node->endpoint_type = Node::HIGHER;
        MaintainAugmentedAttributesOfSingleNode(higher_node);
        FixAugmentedAttributes(higher_node);
        InsertFixup(higher_node);
        return std::make_pair(Iterator(lower_node, this), true);
    }
    else
    {
        return std::make_pair(Iterator(nil_, this), false);
    }
}

template <class Key, class T>
void IntervalTreePOM<Key, T>::InsertFixup(Node* node)
{
    Node *uncle, *grandparent;
    while (node->parent->color == Node::RED)
    {
        grandparent = node->parent->parent;
        if (node->parent == grandparent->left)
        {
            uncle = grandparent->right;
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
                RightRotate(grandparent);
            }
        }
        else
        {
            uncle = grandparent->left;
            if (uncle->color == Node::RED)
            {
                uncle->color = node->parent->color = Node::BLACK;
                grandparent->color = Node::RED;
                node = grandparent;
            }
            else
            {
                if (node == node->parent->left)
                {
                    node = node->parent;
                    RightRotate(node);
                    // notice that varible grandparent do not need to update here,
                    // since it is still the grandparent of varible node
                }
                node->parent->color = Node::BLACK;
                grandparent->color = Node::RED;
                LeftRotate(grandparent);
            }
        }
    }
    root_->color = Node::BLACK;
}

template <class Key, class T>
void IntervalTreePOM<Key, T>::Transplant(Node* old_node, Node* new_node)
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
typename IntervalTreePOM<Key, T>::Node* IntervalTreePOM<Key, T>::TreeMinimum(Node* sub_tree_root)
{
    while (sub_tree_root->left != nil_)
        sub_tree_root = sub_tree_root->left;
    return sub_tree_root;
}

template <class Key, class T>
typename IntervalTreePOM<Key, T>::Node* IntervalTreePOM<Key, T>::TreeMaximum(Node* sub_tree_root)
{
    while (sub_tree_root->right != nil_)
        sub_tree_root = sub_tree_root->right;
    return sub_tree_root;
}

template <class Key, class T>
typename IntervalTreePOM<Key, T>::Node* IntervalTreePOM<Key, T>::TreeSuccessor(Node* node)
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
typename IntervalTreePOM<Key, T>::Node* IntervalTreePOM<Key, T>::TreePredecessor(Node* node)
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
void IntervalTreePOM<Key, T>::Delete(Iterator pos)
{
    DeleteNode(pos.node_->related_endpoint);
    DeleteNode(pos.node_);
}

template <class Key, class T>
void IntervalTreePOM<Key, T>::DeleteNode(Node *node)
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
        FixAugmentedAttributes(replaced_replaced);// replaced_replaced will NOT be checked
    }
    else if (replaced->right == nil_)
    {
        // do NOT need two "replaced" to store moved node
        replaced_replaced = replaced->left;
        Transplant(replaced, replaced_replaced);
        FixAugmentedAttributes(replaced_replaced);// replaced_replaced will NOT be checked
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
        FixAugmentedAttributes(replaced_replaced);// replaced_replaced will NOT be checked
        FixAugmentedAttributes(replaced->right);// so replaced will be checked
    }
    if (is_black_deleted)
        // In order to maintain property 5,
        // "replaced_replaced" node has extra black (either "doubly black" or "red-and-black", contributes either 2 or 1)
        DeleteFixup(replaced_replaced);
    delete node;
}

// start to check augmented attributes from node->parent to root_
template <class Key, class T>
void IntervalTreePOM<Key, T>::FixAugmentedAttributes(Node* node)
{
    Key max;
    while (node != root_)
    {
        node = node->parent;
        if (MaintainAugmentedAttributesOfSingleNode(node) == false) break;
    }
}

template <class Key, class T>
void IntervalTreePOM<Key, T>::DeleteFixup(Node* node)
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
void IntervalTreePOM<Key, T>::Clear()
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
typename IntervalTreePOM<Key, T>::Iterator IntervalTreePOM<Key, T>::Begin()
{
    return Iterator(TreeMinimum(root_), this);
}

template <class Key, class T>
typename IntervalTreePOM<Key, T>::Iterator IntervalTreePOM<Key, T>::End()
{
    return Iterator(nil_, this);
}

template <class Key, class T>
Key IntervalTreePOM<Key, T>::FindPOM()
{
    return root_->max_overlap_node->key;
}

#endif
