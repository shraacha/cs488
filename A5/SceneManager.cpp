#include "SceneManager.hpp"

#include <cmath>

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "debug.hpp"

static inline std::pair<NodeID, SceneNode *> findIdNodePairWithName (SceneManager & scene, const std::string & name)
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

SceneManager::SceneManager()
{
}

// returns true if import successful, false otherwise
bool SceneManager::importSceneGraph(SceneNode *root)
{
    if (root) {
        m_sceneRoot = root;
        return true;
    } else {
        return false;
    }
}

bool SceneManager::isEmpty() { return !m_sceneRoot; }

// ~~~~~~~~~~~~~~~~~ Iterator ~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~ ctors ~~~~~~~~~~~~~~~~~
SceneManager::PreOrderTraversalIterator::PreOrderTraversalIterator() {}

SceneManager::PreOrderTraversalIterator::PreOrderTraversalIterator(
    SceneManager::PreOrderTraversalIterator::pointer_t ptr)
{
    if (ptr) {
        m_nodeStack.emplace(ptr, InheritedNodeData());
    }
}

// ~~~~~~~~~~~~~~~~~ operations ~~~~~~~~~~~~~~~~~

SceneManager::PreOrderTraversalIterator::const_reference_t
SceneManager::PreOrderTraversalIterator::operator*() const
{
    return const_cast<const_reference_t>(*m_nodeStack.top().first);
}

SceneManager::PreOrderTraversalIterator::const_pointer_t
SceneManager::PreOrderTraversalIterator::operator->() const
{
    return const_cast<const_pointer_t>(m_nodeStack.top().first);
}

SceneManager::PreOrderTraversalIterator &SceneManager::PreOrderTraversalIterator::operator++()
{
    NodeAndInheritedData currentNodeAndData = m_nodeStack.top();
    m_nodeStack.pop();

    // DLOG("top node: %s", (currentNodeAndData).first->m_name.c_str());
    addNodesToStack(currentNodeAndData.first->children,
                    makeInheritableNodeData(*currentNodeAndData.first,
                                         currentNodeAndData.second));
    return *this;
}

SceneManager::PreOrderTraversalIterator
SceneManager::PreOrderTraversalIterator::operator++(int)
{
    SceneManager::PreOrderTraversalIterator temp(*this);
    ++(*this);
    return temp;
}

bool operator==(const SceneManager::PreOrderTraversalIterator &a,
                const SceneManager::PreOrderTraversalIterator &b) {
    return a.m_nodeStack == b.m_nodeStack;
}

bool operator!=(const SceneManager::PreOrderTraversalIterator &a,
                const SceneManager::PreOrderTraversalIterator &b) {
    return a.m_nodeStack != b.m_nodeStack;
}

// ~~~~~~~~~~~~~~~~~ other functions ~~~~~~~~~~~~~~~~~
glm::mat4 SceneManager::PreOrderTraversalIterator::getInheritedTransformation() {
    return m_nodeStack.top().second.trans;
}

std::optional<NodeID> SceneManager::PreOrderTraversalIterator::getInheritedJointID() {
    return m_nodeStack.top().second.nodeId;
}


const GeometryNode * SceneManager::PreOrderTraversalIterator::asGeometryNode()
{
    return static_cast<const GeometryNode *>(m_nodeStack.top().first);
}
// ~~~~~~~~~~~~~~~~~ helpers ~~~~~~~~~~~~~~~~~

void SceneManager::PreOrderTraversalIterator::addNodesToStack(
    std::list<SceneNode *> &nodes,
    const InheritedNodeData &inheritedData)
{
    // adding children in reverse order to the stack
    for(auto nodeIt = nodes.crbegin(); nodeIt != nodes.crend(); ++nodeIt) {
        m_nodeStack.emplace(*nodeIt, inheritedData);
        // DLOG("adding node to stack: %s", (*nodeIt)->m_name.c_str());
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

