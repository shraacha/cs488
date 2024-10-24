#include <cmath>

#include <glm/glm.hpp>

#include "Scene.hpp"
#include "SceneNode.hpp"

// ~~~~~~~~~~~~~~~~~ Scene class ~~~~~~~~~~~~~~~~~

Scene::Scene()
{
    m_globalTranslationNode = new SceneNode("GLOBAL_TRANSLATION_NODE");
    m_globalRotationNode = new SceneNode("GLOBAL_ROTATION_NODE");

    m_globalTranslationNode->add_child(m_globalRotationNode);
    // the points should be rotated first before being translated

    // lifetime of the above two tied to this
    m_sceneRoot.reset(m_globalTranslationNode);
}

// returns true if import successful, false otherwise
bool Scene::importSceneGraph(SceneNode *root)
{
    if (root) {
        m_globalRotationNode->add_child(root);
        return true;
    } else {
        return false;
    }
}

bool Scene::isEmpty() { return !m_sceneRoot; }

void Scene::rotate(const glm::vec3 & axis, const double & theta)
{
    // make these checks, otherwise the rotation might be invalid
    if (glm::length(axis) != 0 && !std::isnan(theta)) {
        m_globalRotationNode->rotateRadians(axis, theta);
    }
}

void Scene::translate(const glm::vec3 & translation)
{
    m_globalTranslationNode->translate(translation);
}

void Scene::resetRotation()
{
    m_globalRotationNode->trans = glm::mat4();
}
void Scene::resetTranslation()
{
    m_globalTranslationNode->trans = glm::mat4();
}

// ~~~~~~~~~~~~~~~~~ Iterator ~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~ ctors ~~~~~~~~~~~~~~~~~
Scene::PreOrderTraversalIterator::PreOrderTraversalIterator() {}

Scene::PreOrderTraversalIterator::PreOrderTraversalIterator(
    Scene::PreOrderTraversalIterator::pointer_t ptr)
{
    if (ptr) {
        InheritedNodeData nodeData = getInheritedNodeData(*ptr);
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
                    getInheritedNodeData(*currentNodeAndData.first,
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

NodeID Scene::PreOrderTraversalIterator::getInheritedJointID() {
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

InheritedNodeData getInheritedNodeData(const SceneNode & thisNode)
{
    return {glm::mat4(1.0f), thisNode.m_nodeId};
}


InheritedNodeData getInheritedNodeData(const SceneNode & thisNode,
                                       const InheritedNodeData &inheritedData)
{
    // transformation matrix should be multiplied down
    glm::mat4 newTransformation = inheritedData.trans * thisNode.trans;

    // ID should be inherited from first joint parent
    NodeID newNodeId;

    if (thisNode.m_nodeType == NodeType::JointNode) {
        newNodeId = thisNode.m_nodeId;
    } else {
        newNodeId = inheritedData.nodeId;
    }

    return {newTransformation, newNodeId};
}
