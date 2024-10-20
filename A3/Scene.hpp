#pragma once

#include <iterator>
#include <stack>
#include <memory>
#include <map>
#include <utility>

#include "SceneNode.hpp"
#include "SceneNodeHelpers.hpp"

using NodeAndInheritedData = std::pair<SceneNode *, InheritedNodeData>;

// container for scene nodes
class Scene {
public:
    Scene();
    Scene(SceneNode* root);

    bool isEmpty();

    // iterator stuff
    struct PreOrderTraversalIterator {
        using iterator_category = std::forward_iterator_tag;
        using pointer_t = SceneNode *;
        using reference_t = SceneNode &;

        // ctors
        PreOrderTraversalIterator(); // for end()
        PreOrderTraversalIterator(pointer_t ptr);

        // operations
        reference_t operator*() const;
        pointer_t operator->() const;

        // prefix
        PreOrderTraversalIterator& operator++();
        // postfix
        PreOrderTraversalIterator operator++(int);

        friend bool operator==(const PreOrderTraversalIterator& a, const PreOrderTraversalIterator& b);
        friend bool operator!=(const PreOrderTraversalIterator& a, const PreOrderTraversalIterator& b);

        // other functions
        InheritedNodeData getInheritedData();

    private:
        std::stack<NodeAndInheritedData> m_nodeStack;

        void addNodesToStack(std::list<SceneNode*> & nodes, const InheritedNodeData & inheritedData);
    };

    PreOrderTraversalIterator begin() {return PreOrderTraversalIterator(m_sceneRoot.get());};
    PreOrderTraversalIterator end() {return PreOrderTraversalIterator();};

private:
    std::shared_ptr<SceneNode> m_sceneRoot; // owns the tree

    // std::map<NodeID, std::shared_ptr<SceneNode>> jointIDToNodeMap; // TODO
};
