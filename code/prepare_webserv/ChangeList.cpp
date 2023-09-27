#include "ChangeList.hpp"

int UserData::parseHeader(std::string& field)
{
	std::istringstream ss(field);
	std::string header;
	std::string value;
	/**
	 * @brief 의사코드
	 * : 까지 읽음
	 * ; 까지 읽음
	 * 각 토큰별 제거
	 */
	return (0);
}

int	UserData::parseRequest(std::stringstream& request)
{
	std::string			temp;

	request.seekg(0);
	std::getline(request, temp, ' ');
	if (temp == "GET")
		_method = GET;
	else if (temp == "POST")
		_method = POST;
	else if (temp == "DELETE")
		_method = DELETE;
	else
	{
		_method = ERROR;
		std::cout << "Method Error" << std::endl;
		return (ERROR);
	}
	std::getline(request, _uri, ' ');
	std::getline(request, temp, '\n');
	if (*(temp.end() - 1) == '\r')
		temp.erase(temp.size() - 1);
	if (temp != "HTTP/1.1")
	{
		std::cout << temp << " version Error" << std::endl;
		return (ERROR);
	}

	while (1)
	{
		std::getline(request, temp, '\n');
		if (request.eof())
			return (0);
		else if (request.fail()) // 헤더가 너무 크다는 것.
			return (ERROR);
		else if (*(temp.end() - 1) == '\r')
			temp.erase(temp.size() - 1);
		if (temp == "")
			return (0);
		if (parseHeader(temp) == ERROR)
			return (ERROR);
	}
	return (ERROR);
}

int	UserData::generateGetResponse(void)
{
	std::ifstream requestedFile;
	std::string extTemp;

	if (_uri == "/")
		_uri = "/index.html";
	extTemp = _uri.substr(_uri.find('.') + 1);
	requestedFile.open("." + _uri, std::ios::binary);
	if (requestedFile.is_open() == false)
	{
		// 4XX error
		std::cerr << Colors::RedString("open failed: ." + _uri) << std::endl; 
		write(_fd, "HTTP/1.1 404 Not found\r\n\r\n", 26);
	}
	else
	{
		std::stringstream fileContent;
		requestedFile.seekg(0, std::ios::end);
		std::streampos fileSize = requestedFile.tellg();
		requestedFile.seekg(0, std::ios::beg);
		_response = "HTTP/1.1 200 OK\r\nContent-type: ";
		if (extTemp == "png")
			_response += "image/";
		else
			_response += "text/";
		_response += extTemp;
		_response += "\r\nContent-length: ";
		_response += std::to_string(fileSize);
		_response += "\r\n\r\n";
		fileContent << requestedFile.rdbuf();
		_response += fileContent.str();
		requestedFile.close();
		sendToClient(_fd);
	}
	return (0);
}

UserData::UserData(int fd)
	:	_fd(fd)
	,	_method(-1)
	,	_status(0)
	,	_headerFlag(0)
	,	_received(std::string())
	,	_response(std::string())
{}

UserData::~UserData(void) {}

const std::stringstream& UserData::getReceived(void) const
{
	return (_received);
}

const std::string& UserData::getResponse(void) const
{
	return (_response);
}

int	UserData::getMethod(void) const
{
	return (_method);
}

static int checkReceivedHeader(std::stringstream& ss)
{
	std::string line;

	ss.seekg(0, std::ios::end);
	if (ss.tellg() >= 1024)
		return (true);
	ss.seekg(0, std::ios::beg);
	while (1)
	{
		std::getline(ss, line, '\n');
		if (ss.eof() == true)
			return (false);
		else if (line == "" || line == "\r")
			return (true);
		else
			continue ;
	}
}

int	UserData::generateResponse(void)
{
	_headerFlag = checkReceivedHeader(_received);
	if (_headerFlag == ERROR)
		return (ERROR);
	else if (_headerFlag == false)
		return (0);
	else 
	{
		if (parseRequest(_received) == ERROR)
			return (ERROR);
		if (_method == GET)
			generateGetResponse();
	}
	return (0);
}


int UserData::recvFromClient(int fd)
{
	int len;

	len = read(fd, _buf, BUFFER_SIZE);
	std::cout << Colors::BoldBlue << "received message from client " << fd << "\n" << std::endl;
	for (int i = 0; i < len; i++)
	{
		std::cout << _buf[i];
		_received << _buf[i];
	}
	std::cout << Colors::Reset << std::endl;
	return (len);
}

void UserData::initUserData(void)
{
	_method = -1;
	_status = -1;
	_headerFlag = -1;
	_uri.clear();
	_received.str("");
	_response.clear();
}

int UserData::sendToClient(int fd)
{
	size_t len;

	std::cout << Colors::BoldMagenta << "send to client " << fd << "\n" << Colors::Reset<< std::endl;
	len = write(fd, _response.c_str(), _response.size()); // 큰 파일도 한 번에 보낼 수 있나?
	len = write(1, _response.c_str(), _response.size()); // 큰 파일도 한 번에 보낼 수 있나?
	if (len < 0)
	{
		std::cout << Colors::RedString("send() error") << std::endl;
		exit(1);
	}
	initUserData();
	return (len);
}

ChangeList::ChangeList(void)
	:	_keventVector(std::vector<struct kevent>())
{}

ChangeList::~ChangeList(void) 
{}

void ChangeList::changeEvent(uintptr_t ident, int filter, int flags)
{
	// fflags로 CGI 처리와 signal 등 처리 해야함.
	struct kevent	target;
	UserData *udata;
	/* init udata */

	target.ident = ident;
	target.filter = filter;
	target.flags = flags;
	target.fflags = 0;
	target.data = 0;
	if (flags == EV_DELETE)
	{
		kevent(ident, &target, 1, NULL, 0, 0);
	}
	else
	{
		udata = new UserData(ident);
		target.udata = udata;
		_keventVector.push_back(target);
	}
}

void ChangeList::clearEvent(void)
{
	_keventVector.clear();
}

std::vector<struct kevent>& ChangeList::getKeventVector(void)
{
	return (_keventVector);
}

size_t ChangeList::getSize(void)
{
	return (_keventVector.size());
}
