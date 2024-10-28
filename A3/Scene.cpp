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
        m_jointIDToNodeMap = generateJointTree(*this, root);
        m_headIDtoNode = findIdNodePairWithName(*this, c_head_joint_name);

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


bool Scene::isValidId(const NodeID & id)
{
    return m_jointIDToNodeMap.count(id) > 0;
}

bool Scene::isHeadId(const NodeID & id)
{
    return id == m_headIDtoNode.first;
}

JointNode * Scene::getJoint(const NodeID & id)
{
    auto it = m_jointIDToNodeMap.find(id);
    if (it != m_jointIDToNodeMap.end())
    {
        return static_cast<JointNode *>(it->second);
    } else {
        return nullptr;
    }
}

void Scene::resetAllJoints()
{
    for(auto element : m_jointIDToNodeMap)
    {
        JointNode * joint = static_cast<JointNode *> (element.second);
        joint->reset_joint_x();
        joint->reset_joint_y();
    }
}

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
        return {glm::mat4(1.0f), std::nullopt};
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


// ~~~~~~~~~~~~~~~~~ Commands ~~~~~~~~~~~~~~~~~
MoveJointsCommand::MoveJointsCommand(Scene *scene,
                                     const std::vector<NodeID> &jointIds,
                                     const double &degreesX,
                                     const double &degreesY)
    : scene{scene}
{
    for (auto jointId : jointIds) {
        JointNode *joint = scene->getJoint(jointId);
        if (joint != nullptr) {
            jointAndRotation.emplace_back(
                joint, joint->get_clamped_addition_x(degreesX),
                joint->get_clamped_addition_y(degreesY));
        }
    }
}

void MoveJointsCommand::execute()
{
    for (auto item : jointAndRotation) {
        std::get<0>(item)->update_joint_x(std::get<1>(item));
        std::get<0>(item)->update_joint_y(std::get<2>(item));
    }
}

void MoveJointsCommand::undo()
{
    for (auto item : jointAndRotation) {
        std::get<0>(item)->update_joint_y(-std::get<2>(item));
        std::get<0>(item)->update_joint_x(-std::get<1>(item));
    }
}


// ~~~~~~~~~~~~~~~~~ Command List ~~~~~~~~~~~~~~~~~
void SceneCommandList::addCommand(std::unique_ptr<SceneCommand> command)
{
    commandList.erase(commandList.begin() + indexPlusOne, commandList.end());
    commandList.emplace_back(std::move(command));
    commandList[indexPlusOne++]->execute();
}

void SceneCommandList::undoCommand()
{
    if(indexPlusOne > 0)
    {
        commandList[--indexPlusOne]->undo();
    }
}

void SceneCommandList::redoCommand()
{
    if(indexPlusOne < commandList.size())
    {
        commandList[indexPlusOne++]->execute();
    }
}

void SceneCommandList::undoAll()
{
    for (auto it = commandList.rbegin(); it != commandList.rend(); ++it)
    {
        (*it)->undo();
    }
    indexPlusOne = 0;
}


inline void SceneCommandList::clearAll()
{
    commandList.erase(commandList.begin(), commandList.end());
    indexPlusOne = 0;
}

void SceneCommandList::undoAndClearAll()
{
    undoAll();
    clearAll();
}

size_t SceneCommandList::getLength()
{
    return commandList.size();
}

size_t SceneCommandList::getIndex()
{
    return indexPlusOne;
}
