#include "SceneManager.hpp"

#include <cmath>

#include <glm/glm.hpp>
#include <optional>

#include "Scene.hpp"
#include "NodeID.hpp"
#include "SceneNode.hpp"
#include "helpers.hpp"

static inline std::map<NodeID, SceneNode *> generateJointTree (Scene & scene, SceneNode * root)
{
    std::map<NodeID, SceneNode *> jointIdToNodeMap;

    for(auto nodeIt = scene.begin(); nodeIt != scene.end(); ++nodeIt) {
        SceneNode * node = const_cast<SceneNode *> (&(*nodeIt));
        NodeID id = node->m_nodeId;

        if (node->m_nodeType == NodeType::JointNode) {
            jointIdToNodeMap.emplace(id, node);
        }
    }

    return std::move(jointIdToNodeMap);
}

static inline std::pair<NodeID, SceneNode *> findIdNodePairWithName (Scene & scene, const std::string & name)
{
    for(auto nodeIt = scene.begin(); nodeIt != scene.end(); ++nodeIt) {
        SceneNode * node = const_cast<SceneNode *> (&(*nodeIt));
        if(node->m_name == name) {
            return {node->m_nodeId, node};
        }
    }
    return {0, nullptr}; // not good behavuour but we are spaghetti-coding
}

// ~~~~~~~~~~~~~~~~~ Scene class ~~~~~~~~~~~~~~~~~

Scene::Scene()
{
}

// returns true if import successful, false otherwise
bool Scene::importSceneGraph(SceneNode *root)
{
    if (root) {
        m_sceneRoot.reset(root);
        return true;
    } else {
        return false;
    }
}

bool Scene::isEmpty() { return !m_sceneRoot; }

// ~~~~~~~~~~~~~~~~~ Iterator ~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~ ctors ~~~~~~~~~~~~~~~~~
Scene::PreOrderTraversalIterator::PreOrderTraversalIterator() {}

Scene::PreOrderTraversalIterator::PreOrderTraversalIterator(
    Scene::PreOrderTraversalIterator::pointer_t ptr)
{
    if (ptr) {
        InheritedNodeData nodeData = makeInheritableNodeData(*ptr);
        m_nodeStack.emplace(ptr, nodeData);
    }
}

// ~~~~~~~~~~~~~~~~~ operations ~~~~~~~~~~~~~~~~~

Scene::PreOrderTraversalIterator::const_reference_t
Scene::PreOrderTraversalIterator::operator*() const
{
    return const_cast<const_reference_t>(*m_nodeStack.top().first);
}

Scene::PreOrderTraversalIterator::const_pointer_t
Scene::PreOrderTraversalIterator::operator->() const
{
    return const_cast<const_pointer_t>(m_nodeStack.top().first);
}

Scene::PreOrderTraversalIterator &Scene::PreOrderTraversalIterator::operator++()
{
    NodeAndInheritedData currentNodeAndData = m_nodeStack.top();
    m_nodeStack.pop();
    addNodesToStack(currentNodeAndData.first->children,
                    makeInheritableNodeData(*currentNodeAndData.first,
                                         currentNodeAndData.second));
    return *this;
}

Scene::PreOrderTraversalIterator
Scene::PreOrderTraversalIterator::operator++(int)
{
    Scene::PreOrderTraversalIterator temp(*this);
    ++(*this);
    return temp;
}

bool operator==(const Scene::PreOrderTraversalIterator &a,
                const Scene::PreOrderTraversalIterator &b) {
    return a.m_nodeStack == b.m_nodeStack;
}

bool operator!=(const Scene::PreOrderTraversalIterator &a,
                const Scene::PreOrderTraversalIterator &b) {
    return a.m_nodeStack != b.m_nodeStack;
}

// ~~~~~~~~~~~~~~~~~ other functions ~~~~~~~~~~~~~~~~~
glm::mat4 Scene::PreOrderTraversalIterator::getInheritedTransformation() {
    return m_nodeStack.top().second.trans;
}

std::optional<NodeID> Scene::PreOrderTraversalIterator::getInheritedJointID() {
    return m_nodeStack.top().second.nodeId;
}

// ~~~~~~~~~~~~~~~~~ helpers ~~~~~~~~~~~~~~~~~

void Scene::PreOrderTraversalIterator::addNodesToStack(
    std::list<SceneNode *> &nodes,
    const InheritedNodeData &inheritedData)
{
    // adding children in reverse order to the stack
    for(auto nodeIt = nodes.crbegin(); nodeIt != nodes.crend(); ++nodeIt) {
        m_nodeStack.emplace(*nodeIt, inheritedData);
    }
}

// ~~~~~~~~~~~~~~~~~ InheritedNodeData ~~~~~~~~~~~~~~~~~

bool operator==(const InheritedNodeData & a, const InheritedNodeData & b)
{
    return a.trans == b.trans && a.nodeId == b.nodeId;
}

bool operator!=(const InheritedNodeData & a, const InheritedNodeData & b)
{
    return !(a == b);
}

InheritedNodeData makeInheritableNodeData(const SceneNode & thisNode)
{
    if (thisNode.m_nodeType == NodeType::JointNode)
    {
        JointNode * joint = static_cast<JointNode *>(const_cast<SceneNode*>(&thisNode));

        // must apply the joint specific rotation as this is stored separate from the main transformation matrix
        return {joint->getJointRotationMatrix() * thisNode.trans, thisNode.m_nodeId};
    } else {
        return {thisNode.trans, std::nullopt};
    }
}


InheritedNodeData makeInheritableNodeData(const SceneNode & thisNode,
                                       const InheritedNodeData &inheritedData)
{
    // transformation matrix should be multiplied down
    glm::mat4 newTransformation = thisNode.trans;

    // ID should be inherited from first joint parent
    std::optional<NodeID> newNodeId;

    if (thisNode.m_nodeType == NodeType::JointNode) {
        JointNode * joint = static_cast<JointNode *>(const_cast<SceneNode*>(&thisNode));

        // must apply the joint specific rotation as this is stored separate from the main transformation matrix
        newTransformation = joint->getJointRotationMatrix() * newTransformation;
        newNodeId = thisNode.m_nodeId;
    } else {
        newNodeId = inheritedData.nodeId;
    }

    newTransformation = inheritedData.trans * newTransformation;
    return {newTransformation, newNodeId};
}

