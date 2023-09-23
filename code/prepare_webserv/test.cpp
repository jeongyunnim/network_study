#include "Colors.hpp"
#include "MultiTree.hpp"

void leak(void)
{
	system("leaks $PPID");
}

int main()
{
	// atexit(leak);
	locationBlock *rootData = new locationBlock;
	rootData->uri = "/";
	MultiTreeNode *temp = new MultiTreeNode(rootData);
	MultiTree rootTree(*temp);

	locationBlock *home = new locationBlock;
	home->uri = "home/";
	locationBlock *src = new locationBlock;
	src->uri = "src/";
	locationBlock *http = new locationBlock;
	http->uri = "http/";
	locationBlock *httphtml = new locationBlock;
	httphtml->uri = "http/html/";
	locationBlock *verylong = new locationBlock;
	verylong->uri = "veryverylonglonglong/";

	locationBlock *test = new locationBlock;
	test->uri = "a/";

	locationBlock *ftp = new locationBlock;
	ftp->uri = "ftp/";
	locationBlock *html = new locationBlock;
	html->uri = "html/";

	rootTree.GetRoot()->AddChildNode(home);
	rootTree.searchNodeOrNull("/home/")->AddChildNode(src);
	rootTree.searchNodeOrNull("/home/src/")->AddChildNode(ftp);
	rootTree.searchNodeOrNull("/home/src/")->AddChildNode(http);
	rootTree.searchNodeOrNull("/home/src/")->AddChildNode(httphtml);
	rootTree.searchNodeOrNull("/home/src/http/html/")->AddChildNode(verylong);
	rootTree.searchNodeOrNull("/home/src/http/html/veryverylonglonglong/")->AddChildNode(test);
	rootTree.searchNodeOrNull("/home/src/http/")->AddChildNode(html);

	std::cout << Colors::BoldMagentaString("\n[let's get it on]") << std::endl;
	temp = rootTree.searchNodeOrNull("/home/sr/http/htm/eryverylonglonglong/index.html");
	if (temp == NULL)
		std::cout << "location block's URI: " << "N U L L" << std::endl;
	else
		std::cout << "location block's URI: " << temp->GetURI() << std::endl;
	return (0);
}