#include "SceneNodeHelpers.hpp"

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
