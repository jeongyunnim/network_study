#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

typedef struct locationBlock
{
	std::string 				uri;
	bool 						get; /*= false;*/
	bool 						post;/*= false;*/
	bool 						del; /*= false;*/
	bool 						autoIndex; /*= false;*/
	std::string 				index; /*= "index.html";*/
	std::string 				alias; // alias 경로
	std::pair<int, std::string>	return_pair; // <statusCode, uri>. redirection도 포함됨
} locationBlock;

class MultiTreeNode
{
public:

	MultiTreeNode(locationBlock* data);
	~MultiTreeNode(void);
	size_t GetChildrenSize(void) const;
	const locationBlock* GetLocationBlock(void) const;
	const std::string& GetURI(void) const;
	const std::vector<MultiTreeNode*>& GetChildren(void);
	void AddChildNode(locationBlock *target);

private:

	MultiTreeNode(void);
	MultiTreeNode& operator=(const MultiTreeNode& rhs);
	MultiTreeNode(const MultiTreeNode& other);

	std::vector<MultiTreeNode*> mChildren;
	MultiTreeNode*				mParentNode;
	locationBlock*				mData;

};

class MultiTree
{
public:

	MultiTree(MultiTreeNode& root);
	~MultiTree(void);
	MultiTreeNode* GetRoot(void);
	MultiTreeNode* searchNodeOrNull(std::string target);

private:

	MultiTree(void);
	MultiTree& operator=(const MultiTree& rhs);
	MultiTree(const MultiTree& other);

	MultiTreeNode* mRoot;
};