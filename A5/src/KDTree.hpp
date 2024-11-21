#pragma once

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

// helpers
template <typename T, typename R>
std::function<R(T *, T *)>
dereferenceWrapper(const std::function<R(const T &, const T &)> & func)
{
    return [func](T *t1, T *t2) -> R { return func(*t1, *t2); };
}

// KD Tree
template <typename T> struct KDNode
{
    KDNode(T *item) : m_item{item}, m_left{nullptr}, m_right{nullptr} {}

    T *m_item; // pointer into KDTree vec
    std::shared_ptr<KDNode<T>> m_left;
    std::shared_ptr<KDNode<T>> m_right;
};

/* desc:
 * - A KD tree stucture to store K-dimensional data.
 * - each comparion should be orthogonal
 * - must provide a comparison function and a distance function for each axis
 */
template <typename T_Key, typename T_Distance> class KDTree
{
    using depth_t = unsigned int;
    using axis_t = unsigned int;
    using node_t = KDNode<T_Key>;

  public:
    KDTree(
        const std::vector<T_Key> & keys,
        std::vector<std::function<bool(const T_Key &, const T_Key &)>> compare,
        std::vector<std::function<T_Distance(const T_Key &, const T_Key &)>>
            distance)
        : m_keys{keys}, m_comparisonFunctors{compare},
          m_distanceFunctors{distance}
    {
        if (compare.size() != distance.size())
        {
            throw std::logic_error(
                "Non-equal number of comparison functors and distance functors "
                "passed to KDTree constructor.");
        }

        std::vector<T_Key *> pointersToItems;
        for (T_Key & ref : m_keys)
        {
            pointersToItems.emplace_back(&ref);
        }

        m_tree = createBalancedKDTreeNode(pointersToItems, getStartAxis());
    }

    T_Key getNearest(const T_Key & key)
    {
        std::shared_ptr<node_t> result =
            getNearestNode(key, m_tree, getStartAxis());

        if (!result)
        {
            throw std::runtime_error("getNearest error.");
        }

        return *result->m_item;
    };

    T_Key getNNearest(const T_Key & key, unsigned int n);

  private:
    std::vector<T_Key> m_keys;
    std::shared_ptr<node_t> m_tree;
    std::vector<std::function<bool(const T_Key &, const T_Key &)>>
        m_comparisonFunctors;
    std::vector<std::function<T_Distance(const T_Key &, const T_Key &)>>
        m_distanceFunctors;

    constexpr depth_t getStartAxis() { return 0; };

    axis_t getAxis(const depth_t & i)
    {
        return (i + 1) % m_comparisonFunctors.size();
    };

    T_Distance distSquared(const T_Key & key1, const T_Key & key2)
    {
        T_Distance distSquared = T_Distance{};
        for (const auto & distanceFunction : m_distanceFunctors)
        {
            T_Distance dist = distanceFunction(key1, key2);
            distSquared += dist * dist;
        }

        return distSquared;
    }

    std::shared_ptr<node_t>
    getNearestNode(const T_Key & target, const std::shared_ptr<node_t> & node1,
                   const std::shared_ptr<node_t> & node2)
    {
        if (node1 && !node2)
        {
            return node1;
        }
        else if (node2 && !node1)
        {
            return node2;
        }
        else if (!node1 && !node2)
        {
            return nullptr;
        }
        else
        {
            if (distSquared(target, *node1->m_item) <=
                distSquared(target, *node2->m_item))
            {
                return node1;
            }
            else
            {
                return node2;
            }
        }
    }

    // desc:
    // - builds a KD tree based on the given items and comparisons
    //
    // params:
    // - items
    // a list of pointers to elements, this wil be modified.
    std::shared_ptr<node_t>
    createBalancedKDTreeNode(const std::vector<T_Key *> & inputKeys,
                             depth_t depth)
    {
        if (inputKeys.size() == 0)
        {
            return nullptr;
        }

        // sort
        std::vector<T_Key *> keys = inputKeys;
        size_t midpoint = keys.size() / 2;
        std::nth_element(
            keys.begin(), keys.begin() + midpoint, keys.end(),
            dereferenceWrapper(m_comparisonFunctors[getAxis(depth)]));

        // set midpoint as item for current node
        std::shared_ptr<node_t> node = std::make_shared<node_t>(keys[midpoint]);

        // extract right end of vector & construct right tree
        std::vector<T_Key *> rightSide{keys.begin() + midpoint + 1, keys.end()};
        node->m_right = createBalancedKDTreeNode(rightSide, depth + 1);

        // extract left end of vector is what is left & construct right tree
        keys.erase(keys.begin() + midpoint, keys.end());
        node->m_left = createBalancedKDTreeNode(keys, depth + 1);

        return node;
    }

    std::shared_ptr<node_t> getNearestNode(const T_Key & target,
                                           std::shared_ptr<node_t> root,
                                           const depth_t & depth)
    {
        if (!root)
        {
            return nullptr;
        }

        std::shared_ptr<node_t> nextBranch, otherBranch;

        if (m_comparisonFunctors[getAxis(depth)](target, *root->m_item))
        {
            nextBranch = root->m_left;
            otherBranch = root->m_right;
        }
        else
        {
            nextBranch = root->m_right;
            otherBranch = root->m_left;
        }

        std::shared_ptr<node_t> temp =
            getNearestNode(target, nextBranch, depth + 1);
        std::shared_ptr<node_t> best = getNearestNode(target, temp, root);

        T_Distance radiusSquared = distSquared(target, *best->m_item);
        T_Distance distToSplitPlane =
            m_distanceFunctors[getAxis(depth)](target, *root->m_item);

        if (radiusSquared > distToSplitPlane * distToSplitPlane)
        {
            std::shared_ptr<node_t> temp =
                getNearestNode(target, otherBranch, depth + 1);
            getNearestNode(target, temp, best);
            std::shared_ptr<node_t> best = getNearestNode(target, temp, best);
        }

        return best;
    }

    friend std::ostream & operator<<(std::ostream & os, const KDTree & tree)
    {
        std::deque<node_t *> thisLevel;
        std::deque<node_t *> nextLevel;

        thisLevel.push_front(tree.m_tree.get());

        while (!thisLevel.empty())
        {
            node_t *thisNode = thisLevel.front();
            thisLevel.pop_front();

            if (thisNode != nullptr)
            {
                os << *thisNode->m_item << " ";

                if (thisNode->m_left)
                {
                    nextLevel.push_back(thisNode->m_left.get());
                }
                else
                {
                    nextLevel.push_back(nullptr);
                }

                if (thisNode->m_right)
                {
                    nextLevel.push_back(thisNode->m_right.get());
                }
                else
                {
                    nextLevel.push_back(nullptr);
                }
            }
            else
            {
                os << "** ";
            }

            if (thisLevel.empty() && !nextLevel.empty())
            {
                std::swap(thisLevel, nextLevel);
                os << std::endl;
            }
        }

        return os;
    }
};
