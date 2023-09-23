#include "./MultiTree.hpp"
#include "./Colors.hpp"

MultiTreeNode::MultiTreeNode(locationBlock* data)
	:	mChildren(std::vector<MultiTreeNode *>())
	,	mParentNode(NULL)
	,	mData(data)
{}

MultiTreeNode::~MultiTreeNode(void)
{
	if (mData != NULL)
	{
		delete (mData);
	}
	for (std::vector<MultiTreeNode *>::iterator it = mChildren.begin(); it != mChildren.end(); it++)
	{
		delete (*it);
	}
}

size_t MultiTreeNode::GetChildrenSize(void) const
{
	return (mChildren.size());
}

const locationBlock* MultiTreeNode::GetLocationBlock(void) const
{
	return (mData);
}

const std::string& MultiTreeNode::GetURI(void) const
{
	return (mData->uri);
}

const std::vector<MultiTreeNode *>& MultiTreeNode::GetChildren(void)
{
	return (mChildren);
}

void MultiTreeNode::AddChildNode(locationBlock *target)
{
	MultiTreeNode *child = new MultiTreeNode(target);
	child->mParentNode = this;
	mChildren.push_back(child);
}







MultiTree::MultiTree(MultiTreeNode& root)
	: mRoot(&root)
{}

MultiTree::~MultiTree(void)
{
	delete (mRoot);
}

static MultiTreeNode* findNodeOrNullRecursive(MultiTreeNode* node, std::string target)
{
	MultiTreeNode*	temp = NULL;
	MultiTreeNode*	result = NULL;
	std::string		subString;

	if (node == NULL)
		return (NULL);
	else if (node->GetURI() != target.substr(0, node->GetURI().size()))
		return (NULL);
	target.erase(0, node->GetURI().size());
	if (target.size() == 0)
		return (node);
	for (std::vector<MultiTreeNode*>::const_iterator it = node->GetChildren().begin(); it != node->GetChildren().end(); it++)
	{
		subString = target.substr(0, (*it)->GetURI().size());
		if ((*it)->GetURI() == subString)
		{
			if (temp == NULL || temp->GetURI().size() < subString.size())
				temp = *it;
		}
	}
	result = findNodeOrNullRecursive(temp, target);
	if (result == NULL)
		return (node);
	else
		return (result);
}

MultiTreeNode* MultiTree::GetRoot(void)
{
	return (mRoot);
}

/**
 * @brief uri를 string으로 받아 해당하는 최대 깊이의 트리 노드를 반환하는 함수
 * 
 * @param target 설정을 적용할 uri
 * @return MultiTreeNode* 실패시: NULL 
 */
MultiTreeNode* MultiTree::searchNodeOrNull(std::string target)
{
	return (findNodeOrNullRecursive(mRoot, target));
}
