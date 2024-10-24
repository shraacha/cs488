#include "NodeID.hpp"

bool IdCollection::isInCollection(const NodeID & id)
{
    return nodeIdentities.count(id) > 0;
}

void IdCollection::add(const NodeID & id)
{
    nodeIdentities.insert(id);
}

void IdCollection::remove(const NodeID & id)
{
    nodeIdentities.erase(id);
}


std::vector<NodeID> IdCollection::getAllIds() const
{
    return {nodeIdentities.begin(), nodeIdentities.end()};
}
