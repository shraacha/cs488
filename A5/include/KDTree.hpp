#pragma once

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "KPriorityQueue.hpp"

// helpers
template <typename T, typename R>
static std::function<R(T *, T *)>
dereferenceWrapper(const std::function<R(const T &, const T &)> & func)
{
    return [func](T *t1, T *t2) -> R { return func(*t1, *t2); };
}

// KD Tree
template <typename T> struct KDNode
{
    KDNode(T *item) : m_item{item}, m_left{nullptr}, m_right{nullptr}
    {
        if (item == nullptr) {
            throw std::runtime_error("Attempting to construct KDNode with nullptr.");
        }
    }

    T *m_item; // pointer into KDTree vec
    std::shared_ptr<KDNode<T>> m_left;
    std::shared_ptr<KDNode<T>> m_right;

    T& operator*() const {
        return *m_item;
    }
};

template <typename T, typename R>
static std::function<R(std::shared_ptr<KDNode<T>>, std::shared_ptr<KDNode<T>>)>
sharedPointerDereferenceWrapperKDNode(
    const std::function<R(const T &, const T &)> & func)
{
    return [func](std::shared_ptr<KDNode<T>> t1,
                  std::shared_ptr<KDNode<T>> t2) -> R
    { return func(**t1, **t2); };
}

/* desc:
 * - A KD tree stucture to store K-dimensional data.
 * - each comparion should be orthogonal
 *   This comparison should be strictly less than
 * - must provide a comparison function and a distance function for each axis
 */
template <typename T_Key, typename T_Distance> class KDTree
{
    using depth_t = unsigned int;
    using axis_t = unsigned int;
    using node_t = KDNode<const T_Key>;

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

        return **result;
    };

    std::vector<T_Key> getKNearest(const T_Key & key, unsigned int k)
    {
        KPriorityQueue<std::shared_ptr<node_t>> queue(
            k,
            [&](const std::shared_ptr<node_t> & k1,
                const std::shared_ptr<node_t> & k2) -> bool
            { return isNodeCloser(key, k1, k2); });

        queue = getKNearestNodes(key, m_tree, getStartAxis(), queue);

        std::vector<T_Key> returnVec;
        for(auto & ref : queue.getQueue()) {
            returnVec.emplace_back(**ref);
        }

        return returnVec;
    }

    const std::vector<T_Key> & getKeys() const { return m_keys; }

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
        return i % m_comparisonFunctors.size();
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

    // desc:
    // - builds a KD tree based on the given items and comparisons
    //
    // params:
    // - items
    // a list of pointers to elements, this wil be modified.
    std::shared_ptr<node_t>
    createBalancedKDTreeNode(std::vector<T_Key *> keys,
                             depth_t depth)
    {
        if (keys.size() == 0)
        {
            return nullptr;
        }

        // sort
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

    bool isNodeCloser(const T_Key & target,
                      const std::shared_ptr<node_t> & node1,
                      const std::shared_ptr<node_t> & node2)
    {

        if (distSquared(target, **node1) < distSquared(target, **node2))
        {
            return true;
        }
        else
        {
            return false;
        }
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
            if (isNodeCloser(target, node1, node2))
            {
                return node1;
            }
            else
            {
                return node2;
            }
        }
    }

    std::shared_ptr<node_t> getNearestNode(const T_Key & target,
                                           const std::shared_ptr<node_t> root,
                                           const depth_t & depth)
    {
        if (!root)
        {
            return nullptr;
        }

        std::shared_ptr<node_t> nextBranch, otherBranch;

        if (m_comparisonFunctors[getAxis(depth)](target, **root))
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
        if (!best) {
            throw std::runtime_error("Unexpected null node in getNearestNode search");
        }

        T_Distance radiusSquared = distSquared(target, **best);
        T_Distance distToSplitPlane =
            m_distanceFunctors[getAxis(depth)](target, **root);

        if (radiusSquared > distToSplitPlane * distToSplitPlane)
        {
            temp = getNearestNode(target, otherBranch, depth + 1);
            best = getNearestNode(target, temp, best);
        }

        return best;
    }

    KPriorityQueue<std::shared_ptr<node_t>>
    getKNearestNodes(const T_Key & target, const std::shared_ptr<node_t> root,
                     const depth_t & depth, KPriorityQueue<std::shared_ptr<node_t>> queue)
    {
        if (!root)
        {
            return queue;
        }

        std::shared_ptr<node_t> nextBranch, otherBranch;

        if (m_comparisonFunctors[getAxis(depth)](target, **root))
        {
            nextBranch = root->m_left;
            otherBranch = root->m_right;
        }
        else
        {
            nextBranch = root->m_right;
            otherBranch = root->m_left;
        }

        queue = getKNearestNodes(target, nextBranch, depth + 1, std::move(queue));
        queue.push(root);

        T_Distance radiusSquared = distSquared(target, **queue.getTop());
        T_Distance distToSplitPlane =
            m_distanceFunctors[getAxis(depth)](target, **root);

        if (!queue.isFull() || radiusSquared > distToSplitPlane * distToSplitPlane)
        {
            queue = getKNearestNodes(target, otherBranch, depth + 1, std::move(queue));
        }

        return queue;
    }

    // output
    friend std::ostream & operator<<(std::ostream & os, const KDTree & tree)
    {
        std::deque<std::shared_ptr<node_t>> thisLevel;
        std::deque<std::shared_ptr<node_t>> nextLevel;

        thisLevel.push_front(tree.m_tree);

        while (!thisLevel.empty())
        {
            std::shared_ptr<node_t> thisNode = thisLevel.front();
            thisLevel.pop_front();

            if (thisNode)
            {
                os << **thisNode << " ";

                if (thisNode->m_left)
                {
                    nextLevel.push_back(thisNode->m_left);
                }
                else
                {
                    nextLevel.push_back(nullptr);
                }

                if (thisNode->m_right)
                {
                    nextLevel.push_back(thisNode->m_right);
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
