#pragma once

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

// helpers
template <typename T>
std::function<bool(T *, T *)>
dereferenceWrapper(const std::function<bool(const T &, const T &)> & func)
{
    return [func](T *t1, T *t2) -> bool {
        return func(*t1, *t2);
    };
}

// KD Tree
template <typename T>
struct KDNode {
    KDNode(T * item): m_item{item} {}

    T * m_item; // pointer into KDTree vec
    std::unique_ptr<KDNode<T>> m_left;
    std::unique_ptr<KDNode<T>> m_right;
};

template <typename Key> class KDTree {
  public:
    KDTree(const std::vector<Key> & keys, std::vector<std::function<bool(const Key &, const Key &)>> compare)
        : m_keys{keys}
    {
        std::vector<Key *> pointersToItems;
        for (Key & ref : m_keys) {
            pointersToItems.emplace_back(&ref);
        }

        for (const auto & ref : compare) {
            m_comparisonFunctors.emplace_back(dereferenceWrapper<Key>(ref));
        }

        m_tree = createBalancedKDTreeNode(pointersToItems, m_comparisonFunctors, getStartAxis());
    }

    Key findClosest(const Key &);

    Key findNClosest(const Key &, unsigned int n);

  private:
    std::vector<Key> m_keys;
    std::unique_ptr<KDNode<Key>> m_tree;
    std::vector<std::function<bool(Key *, Key *)>> m_comparisonFunctors;

    constexpr unsigned int getStartAxis() {
        return 0;
    };

    unsigned int getNextAxis(const unsigned int & i) {
        return (i + 1) % m_comparisonFunctors.size();
    };

    // desc:
    // - builds a KD tree based on the given items and comparisons
    //
    // params:
    // - items
    // a list of pointers to elements, this wil be modified.
    std::unique_ptr<KDNode<Key>> createBalancedKDTreeNode(
        const std::vector<Key *> & inputKeys,
        const std::vector<std::function<bool(Key *, Key *)>> &
            comparisonFunctors,
        unsigned int axis) {
        // sort
        std::vector<Key *> keys = inputKeys;
        std::sort(keys.begin(), keys.end(), comparisonFunctors[axis]);

        // get midpoint
        size_t midpoint = keys.size() / 2;

        // set midpoint as item for current node
        std::unique_ptr<KDNode<Key>> node = std::make_unique<KDNode<Key>>(keys[midpoint]);

        // extract right end of vector & construct right tree
        if (keys.size() - (midpoint + 1) != 0) {
            std::vector<Key *> rightSide{keys.begin() + midpoint + 1,
                                         keys.end()};
            node->m_right = createBalancedKDTreeNode(
                rightSide, comparisonFunctors, getNextAxis(axis));
        }

        // extract left end of vector is what is left & construct right tree
        // left end is what is remaining after the erasure
        if (midpoint != 0) {
            keys.erase(keys.begin() + midpoint, keys.end());
            node->m_left = createBalancedKDTreeNode(keys, comparisonFunctors,
                                                    getNextAxis(axis));
        }

        return node;
    }

    friend std::ostream & operator<<(std::ostream & os, const KDTree & tree) {
        std::deque<KDNode<Key> *> thisLevel;
        std::deque<KDNode<Key> *> nextLevel;

        thisLevel.push_front(tree.m_tree.get());

        while (!thisLevel.empty())
        {
            KDNode<Key> *thisNode = thisLevel.front();
            thisLevel.pop_front();

            if (thisNode != nullptr) {
                os << *thisNode->m_item << " ";

                if (thisNode->m_left) {
                    nextLevel.push_back(thisNode->m_left.get());
                } else {
                    nextLevel.push_back(nullptr);
                }

                if (thisNode->m_right) {
                    nextLevel.push_back(thisNode->m_right.get());
                } else {
                    nextLevel.push_back(nullptr);
                }
            } else {
                os << "** ";
            }

            if (thisLevel.empty() && !nextLevel.empty()) {
                std::swap(thisLevel, nextLevel);
                os << std::endl;
            }
        }

        return os;
    }
};
