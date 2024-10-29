#pragma once

#include <iterator>
#include <memory>
#include <optional>
#include <stack>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "JointNode.hpp"

// This struct stores data that should be inherited from the parent node in some form
struct InheritedNodeData {
    glm::mat4 trans;
    std::optional<NodeID> nodeId;
};

bool operator==(const InheritedNodeData & a, const InheritedNodeData & b);
bool operator!=(const InheritedNodeData & a, const InheritedNodeData & b);

InheritedNodeData makeInheritableNodeData(const SceneNode & thisNode);

// controls how data is inherited
InheritedNodeData makeInheritableNodeData(const SceneNode & thisNode,
                                       const InheritedNodeData &inheritedData);

using NodeAndInheritedData = std::pair<SceneNode *, InheritedNodeData>;

// container for scene nodes
class Scene {
public:
    Scene();

    bool importSceneGraph(SceneNode* root);

    bool isEmpty();

    // in radians
    void rotate(const glm::vec3 & axis, const double & theta);
    void translate(const glm::vec3 & translation);

    void resetRotation();
    void resetTranslation();

    // ids
    bool isValidId(const NodeID & id);
    bool isHeadId(const NodeID & id);
    JointNode* getJoint(const NodeID & id);
    void resetAllJoints();

    // iterator stuff
    struct PreOrderTraversalIterator {
        using iterator_category = std::forward_iterator_tag;
        using pointer_t = SceneNode *;
        using const_pointer_t = const SceneNode *;
        using reference_t = SceneNode &;
        using const_reference_t = const SceneNode &;

        // ctors
        PreOrderTraversalIterator(); // for end()
        PreOrderTraversalIterator(pointer_t ptr);

        // operations
        const_reference_t operator*() const;
        const_pointer_t operator->() const;

        // prefix
        PreOrderTraversalIterator& operator++();
        // postfix
        PreOrderTraversalIterator operator++(int);

        friend bool operator==(const PreOrderTraversalIterator& a, const PreOrderTraversalIterator& b);
        friend bool operator!=(const PreOrderTraversalIterator& a, const PreOrderTraversalIterator& b);

        // other functions
        glm::mat4 getInheritedTransformation();
        std::optional<NodeID> getInheritedJointID();

    private:
        std::stack<NodeAndInheritedData> m_nodeStack;

        void addNodesToStack(std::list<SceneNode*> & nodes, const InheritedNodeData & inheritedData);
    };

    PreOrderTraversalIterator begin() {return PreOrderTraversalIterator(m_sceneRoot.get());};
    PreOrderTraversalIterator end() {return PreOrderTraversalIterator();};

private:
    std::unique_ptr<SceneNode> m_sceneRoot; // owns the tree
};
