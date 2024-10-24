#pragma once

#include <set>
#include <vector>

using NodeID = unsigned int;

struct IdCollection {
	std::set<NodeID> nodeIdentities;

	bool isInCollection(const NodeID & id);
	void add(const NodeID & id);
	void remove(const NodeID & id);
	std::vector<NodeID> getAllIds() const;
};
