# augment red black tree

This module is a part of [yirong-c/CLRS](https://github.com/yirong-c/CLRS)

## Description

### Order Statistics Trees

Red-black trees
that support computation of the rank of an elements in O(lg n).

### Interval Trees

Red-black trees
that support operations on intervals.

## File Structure

```bash
.
├── interval_tree
│   ├── interval_tree.hpp               # main part of interval tree
│   ├── interval_tree_test.cpp          # test cases (catch2)
│   └── interval_tree_test.hpp          # auxiliary test functions
├── order_statistics_tree
│   ├── order_statistics_tree.hpp       # main part of order statistics tree
│   ├── order_statistics_tree_test.cpp  # test cases (catch2)
│   └── order_statistics_tree_test.hpp  # auxiliary test functions
└── readme.md
```

## Bibliography

Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). Introduction to algorithms  (Third edition.). MIT Press.
