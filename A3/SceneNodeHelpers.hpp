#pragma once

#include "SceneNode.hpp"

// This struct stores data that should be inherited from the parent node in some form
struct InheritedNodeData {
    glm::mat4 trans;
    NodeID nodeId;
};

bool operator==(const InheritedNodeData & a, const InheritedNodeData & b);
bool operator!=(const InheritedNodeData & a, const InheritedNodeData & b);

InheritedNodeData getInheritedNodeData(const SceneNode & thisNode);

// controls how data is inherited
InheritedNodeData getInheritedNodeData(const SceneNode & thisNode,
                                       const InheritedNodeData &inheritedData);
