#include "EnginePCH.h"
#include "NodeSet.h"
#include "Node.h"

NodeSet::NodeSet(vector<Node*>&& nodesByIndex, unordered_map<tstring, Node*>&& nodesByName) :
	m_nodesByIndex(nodesByIndex),
	m_nodesByName(nodesByName)
{
	if (m_nodesByIndex.size() > 0)
	{
		m_rootNode = m_nodesByIndex.front();
	}
}

NodeSet::~NodeSet()
{
	for (auto& bone : m_nodesByIndex)
	{
		SafeDelete(bone);
	}
	m_nodesByIndex.clear();
	m_nodesByName.clear();
}

const Node* NodeSet::GetNodeByName(const tstring& name) const
{
	auto find_it = m_nodesByName.find(name);
	if (find_it == m_nodesByName.end())
		return nullptr;

	return find_it->second;
}
