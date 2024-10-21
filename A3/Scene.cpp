#include <glm/glm.hpp>

#include "Scene.hpp"
#include "SceneNode.hpp"
#include "SceneNodeHelpers.hpp"

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
    m_globalRotationNode->rotateRadians(axis, theta);
}

void Scene::translate(const glm::vec3 & translation)
{
    m_globalTranslationNode->translate(translation);
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

Scene::PreOrderTraversalIterator::reference_t
Scene::PreOrderTraversalIterator::operator*() const
{
    return *m_nodeStack.top().first;
}

Scene::PreOrderTraversalIterator::pointer_t
Scene::PreOrderTraversalIterator::operator->() const
{
    return m_nodeStack.top().first;
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
InheritedNodeData Scene::PreOrderTraversalIterator::getInheritedData() {
    return m_nodeStack.top().second;
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
