#pragma once

#include "Node.h"

ENGINE_BEGIN

class ENGINE_API NodeSet
{
public:

	NodeSet(vector<Node*>&& nodesByIndex, unordered_map<tstring, Node*>&& nodesByName);

	~NodeSet();

public:

	inline const Node* GetRootNode() const { return m_rootNode; }

	inline size_t GetNodeCount() const { return m_nodesByIndex.size(); }

	inline const Node* GetNodeByIndex(uint index) const { return m_nodesByIndex[index]; }

	const Node* GetNodeByName(const tstring& name) const;

	_declspec(property(get = GetRootNode)) const Node* rootNode;
	_declspec(property(get = GetNodeCount)) size_t nodeCount;

private:

	Node*							m_rootNode = nullptr;

	vector<Node*>					m_nodesByIndex;

	unordered_map<tstring, Node*>	m_nodesByName;
};

ENGINE_END
