#ifndef _AUGMENT_RED_BLACK_TREE_HPP
#define _AUGMENT_RED_BLACK_TREE_HPP

#include <utility>
#include <iterator>
#include <stdexcept>

// ---------- declaration ----------

template <class Key, class T>
class AugmentRedBlackTree
{
public:
    typedef std::pair<const Key, T> ValueType;

#ifdef RBT_TESTING
protected:
#else
private:
#endif
    struct Node
    {
        Node* parent;
        Node* left;
        Node* right;
        ValueType value;
        enum { BLACK, RED } color;
        size_t size;
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
        friend class AugmentRedBlackTree<Key, T>;
        Iterator(Node* node, AugmentRedBlackTree<Key, T>* tree) 
            : node_(node), tree_(tree) {}
        Node* node_;
        AugmentRedBlackTree<Key, T>* tree_;
    };

    AugmentRedBlackTree();
    ~AugmentRedBlackTree();
    std::pair<Iterator, bool> Insert(const ValueType& value);
    void Delete(Iterator pos);
    void Clear();
    T& At(const Key& key);
    Iterator Find(const Key& key);
    Iterator Begin();
    Iterator End();
    T& operator[](Key&& key);
    Iterator Select(size_t rank);
    size_t Rank(Iterator pos);

#ifdef RBT_TESTING
protected:
#else
private:
#endif
    void LeftRotate(Node* subtree_root_node);
    void RightRotate(Node* subtree_root_node);
    void InsertFixup(Node* node);
    void Transplant(Node* old_node, Node* new_node);
    Node* TreeMinimum(Node* sub_tree_root);
    Node* TreeMaximum(Node* sub_tree_root);
    Node* TreeSuccessor(Node* node);
    Node* TreePredecessor(Node* node);
    void DeleteFixup(Node* node);
    Node* Select(Node* subtree_root_node, size_t rank);
    size_t Rank(Node* node);

    void InsertFixSize(Node* node);
    void DeleteFixSize(Node* node);

    Node* root_;
    Node* nil_;

};

// ---------- definition ----------

template <class Key, class T>
AugmentRedBlackTree<Key, T>::AugmentRedBlackTree()
{
    nil_ = new Node();
    nil_->color = Node::BLACK;
    nil_->size = 0;
    root_ = nil_;
}

template <class Key, class T>
AugmentRedBlackTree<Key, T>::~AugmentRedBlackTree()
{
    Clear();
    delete nil_;
}

template <class Key, class T>
void AugmentRedBlackTree<Key, T>::LeftRotate(Node* subtree_root_node) 
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
    new_root->size = subtree_root_node->size;
    subtree_root_node->size = subtree_root_node->left->size + subtree_root_node->right->size + 1;
}

template <class Key, class T>
void AugmentRedBlackTree<Key, T>::RightRotate(Node* subtree_root_node) 
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
    new_root->size = subtree_root_node->size;
    subtree_root_node->size = subtree_root_node->left->size + subtree_root_node->right->size + 1;
}

template <class Key, class T>
typename AugmentRedBlackTree<Key, T>::Iterator AugmentRedBlackTree<Key, T>::Find(const Key& key)
{
    Node* now;
    now = root_;
    while (now != nil_)
    {
        if (now->value.first == key)
            break;
        else if (now->value.first < key)
            now = now->right;
        else
            now = now->left;
    }
    return Iterator(now, this);
}
 
template <class Key, class T>
T& AugmentRedBlackTree<Key, T>::At(const Key& key)
{
    Node* now;
    now = root_;
    while (now != nil_)
    {
        if (now->value.first == key)
            return now->value.second;
        else if (now->value.first < key)
            now = now->right;
        else
            now = now->left;
    }
    throw std::out_of_range("the container does not have an element with the specified key");
}

template <class Key, class T>
T& AugmentRedBlackTree<Key, T>::operator[](Key&& key)
{
    Node *node, **now_ptr, *parent;
    parent = nil_;
    now_ptr = &root_;
    while (*now_ptr != nil_)
    {
        parent = *now_ptr;
        if (key == (*now_ptr)->value.first)
            return (*now_ptr)->value.second;
        else if (key < (*now_ptr)->value.first)
            now_ptr = &((*now_ptr)->left);
        else
            now_ptr = &((*now_ptr)->right);
    }
    node = new Node(std::make_pair(key, T()));
    *now_ptr = node;
    node->parent = parent;
    node->right = node->left = nil_;
    node->color = Node::RED;
    InsertFixSize(node);
    InsertFixup(node);
    return node->value.second;
}

template <class Key, class T>
std::pair<typename AugmentRedBlackTree<Key, T>::Iterator, bool> 
    AugmentRedBlackTree<Key, T>::Insert(const ValueType& value)
{
    Node *node, **now_ptr, *parent;
    parent = nil_;
    now_ptr = &root_;
    while (*now_ptr != nil_)
    {
        parent = *now_ptr;
        if (value.first == (*now_ptr)->value.first)
            return std::make_pair(Iterator(*now_ptr, this), false);
        else if (value.first < (*now_ptr)->value.first)
            now_ptr = &((*now_ptr)->left);
        else
            now_ptr = &((*now_ptr)->right);
    }
    node = new Node(value);
    *now_ptr = node;
    node->parent = parent;
    node->right = node->left = nil_;
    node->color = Node::RED;
    InsertFixSize(node);
    InsertFixup(node);
    return std::make_pair(Iterator(node, this), true);
}

template <class Key, class T>
void AugmentRedBlackTree<Key, T>::InsertFixSize(Node* node)
{
    Node *now;
    now = root_;
    while (now != node)
    {
        ++now->size;
        if (node->value.first < now->value.first) now = now->left;
        else now = now->right;
    }
    now->size = 1;
}

template <class Key, class T>
void AugmentRedBlackTree<Key, T>::InsertFixup(Node* node)
{
    Node *uncle, *grandparent;
    void(AugmentRedBlackTree<Key, T>::*grandparent_rotate)(Node*);
    while (node->parent->color == Node::RED)
    {
        grandparent = node->parent->parent;
        if (node->parent == grandparent->left)
        {
            uncle = grandparent->right;
            grandparent_rotate = &AugmentRedBlackTree<Key, T>::RightRotate;
        }
        else
        {
            uncle = grandparent->left;
            grandparent_rotate = &AugmentRedBlackTree<Key, T>::LeftRotate;
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
void AugmentRedBlackTree<Key, T>::Transplant(Node* old_node, Node* new_node)
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
typename AugmentRedBlackTree<Key, T>::Node* AugmentRedBlackTree<Key, T>::TreeMinimum(Node* sub_tree_root)
{
    while (sub_tree_root->left != nil_)
        sub_tree_root = sub_tree_root->left;
    return sub_tree_root;
}

template <class Key, class T>
typename AugmentRedBlackTree<Key, T>::Node* AugmentRedBlackTree<Key, T>::TreeMaximum(Node* sub_tree_root)
{
    while (sub_tree_root->right != nil_)
        sub_tree_root = sub_tree_root->right;
    return sub_tree_root;
}

template <class Key, class T>
typename AugmentRedBlackTree<Key, T>::Node* AugmentRedBlackTree<Key, T>::TreeSuccessor(Node* node)
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
typename AugmentRedBlackTree<Key, T>::Node* AugmentRedBlackTree<Key, T>::TreePredecessor(Node* node)
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
void AugmentRedBlackTree<Key, T>::Delete(Iterator pos)
{
    Node *old, *replaced, *replaced_replaced;
    bool is_black_deleted;
    old = pos.node_;
    replaced = old;
    is_black_deleted = replaced->color == Node::BLACK;
    if (replaced->left == nil_)
    {
        // do NOT need two "replaced" to store moved node
        replaced_replaced = replaced->right;
        Transplant(replaced, replaced_replaced);
    }
    else if (replaced->right == nil_)
    {
        // do NOT need two "replaced" to store moved node
        replaced_replaced = replaced->left;
        Transplant(replaced, replaced_replaced);
    }
    else
    {
        // DO need two "replaced" to store moved node
        // re-assign varible "replaced" and "is_black_deleted"
        replaced = TreeMinimum(old->right);
        is_black_deleted = replaced->color == Node::BLACK;
        replaced_replaced = replaced->right;
        if (replaced->parent == old)
        {
            // neccessary when "replaced_replaced" is nil_
            replaced_replaced->parent = replaced;
        }
        else
        {
            Transplant(replaced, replaced_replaced);
            replaced_replaced->size = replaced->size;
            replaced->right = old->right;
            replaced->right->parent = replaced;
        }
        Transplant(old, replaced);
        replaced->size = old->size;
        replaced->left = old->left;
        replaced->left->parent = replaced;
        replaced->color = old->color;// this is why "is_black_deleted" need to re-assign in this case
    }
    DeleteFixSize(replaced_replaced);
    if (is_black_deleted)
        // In order to maintain property 5,
        // "replaced_replaced" node has extra black (either "doubly black" or "red-and-black", contributes either 2 or 1)
        DeleteFixup(replaced_replaced);
    delete old;
}

template <class Key, class T>
void AugmentRedBlackTree<Key, T>::DeleteFixSize(Node* node)
{
    while (node != root_)
    {
        node = node->parent;
        --node->size;
    }
}

template <class Key, class T>
void AugmentRedBlackTree<Key, T>::DeleteFixup(Node* node)
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
void AugmentRedBlackTree<Key, T>::Clear()
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
typename AugmentRedBlackTree<Key, T>::Iterator AugmentRedBlackTree<Key, T>::Begin()
{
    return Iterator(TreeMinimum(root_), this);
}

template <class Key, class T>
typename AugmentRedBlackTree<Key, T>::Iterator AugmentRedBlackTree<Key, T>::End()
{
    return Iterator(nil_, this);
}

// rank start by 1

// recursive
// template <class Key, class T>
// typename AugmentRedBlackTree<Key, T>::Iterator AugmentRedBlackTree<Key, T>::Select(size_t rank)
// {
//     if (rank <= 0 || rank >= root_->size) throw std::out_of_range("invalid rank");
//     return Iterator(Select(root_, rank), this);
// }

// template <class Key, class T>
// typename AugmentRedBlackTree<Key, T>::Node* AugmentRedBlackTree<Key, T>::Select
//     (Node* subtree_root_node, size_t rank)
// {
//     size_t root_rank;
//     root_rank = subtree_root_node->left->size + 1;
//     if (rank == root_rank) return subtree_root_node;
//     else if (rank < root_rank) return Select(subtree_root_node->left, rank);
//     else return Select(subtree_root_node->right, rank - root_rank);
// }

// nonrecursive
template <class Key, class T>
typename AugmentRedBlackTree<Key, T>::Iterator AugmentRedBlackTree<Key, T>::Select(size_t rank)
{
    return Iterator(Select(root_, rank), this);
}

template <class Key, class T>
typename AugmentRedBlackTree<Key, T>::Node* AugmentRedBlackTree<Key, T>::Select
    (Node* subtree_root_node, size_t rank)
{
    size_t root_rank;
    root_rank = subtree_root_node->left->size + 1;
    while (rank != root_rank)
    {
        if (rank < root_rank) 
        {
            subtree_root_node = subtree_root_node->left;
        }
        else 
        {
            subtree_root_node = subtree_root_node->right;
            rank -= root_rank;
        }
        root_rank = subtree_root_node->left->size + 1;
    }
    return subtree_root_node;
}

// recursive
// template <class Key, class T>
// size_t AugmentRedBlackTree<Key, T>::Rank(Iterator pos)
// {
//     return Rank(pos.node_);
// }

// template <class Key, class T>
// size_t AugmentRedBlackTree<Key, T>::Rank(Node* node)
// {
//     if (node == root_)
//         return node->left->size + 1;
//     else if (node == node->parent->left)
//         return Rank(node->parent) - node->right->size - 1;
//     else
//         return Rank(node->parent) + node->left->size + 1;
// }

// nonrecursive
template <class Key, class T>
size_t AugmentRedBlackTree<Key, T>::Rank(Iterator pos)
{
    return Rank(pos.node_);
}

template <class Key, class T>
size_t AugmentRedBlackTree<Key, T>::Rank(Node* node)
{
    size_t rank;
    rank = node->left->size + 1;
    while (node != root_)
    {
        if (node == node->parent->right) 
            rank += (node->parent->left->size + 1);
        node = node->parent;
    }
    return rank;
}

#endif
