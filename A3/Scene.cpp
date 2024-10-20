#include <glm/glm.hpp>

#include "Scene.hpp"
#include "SceneNode.hpp"
#include "SceneNodeHelpers.hpp"

// ~~~~~~~~~~~~~~~~~ Scene class ~~~~~~~~~~~~~~~~~

Scene::Scene() {}

Scene::Scene(SceneNode *root) : m_sceneRoot{root} {}

bool Scene::isEmpty() { return !m_sceneRoot; }

// ~~~~~~~~~~~~~~~~~ Iterator ~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~ ctors ~~~~~~~~~~~~~~~~~
Scene::PreOrderTraversalIterator::PreOrderTraversalIterator() {}

Scene::PreOrderTraversalIterator::PreOrderTraversalIterator(
    Scene::PreOrderTraversalIterator::pointer_t ptr)
{
    if (ptr) {
        InheritedNodeData nodeData = getInheritedNodeData(*ptr); // TODO uncomment
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
