#include "ChangeList.hpp"

int	UserData::parseRequest(std::stringstream& request)
{
	std::string			temp;

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
		/* 유효성 검증 */
		if (request.eof())
			break ;
		else if (*(temp.end() - 1) == '\r')
			temp.erase(temp.size() - 1);
		//필요한 정보 저장하기..(map?) 그런데 굳이 저장 해야 하나? 파싱하면서 플래그만 켜주면 되는 거 아닌가?
		if (temp == "")
			break ;
	}
	//post요청 어떻게?
	return (0);
}

int	UserData::generateGetRequest(void)
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
		shutdown(_fd, SHUT_RDWR); // Linger 옵션을 걸고 close를 해야하는가?
	}
	else
	{
		std::stringstream fileContent;
		requestedFile.seekg(0, std::ios::end);
		std::streampos fileSize = requestedFile.tellg();
		requestedFile.seekg(0, std::ios::beg);
		// HTTP version 낮은 걸로 바꿔서 보낼 수 있어야 함.
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
		_writeFlag = true;
	}
	return (0);
}

UserData::UserData(int fd)
	:	_fd(fd)
	,	_method(-1)
	,	_writeFlag(0)
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

bool	UserData::getWriteFlag(void) const
{
	return (_writeFlag);
}

int	UserData::generateResponse(void)
{
	if (parseRequest(_received) == ERROR)
	{
		return (ERROR);
	}
	if (_method == GET)
		generateGetRequest();
	return (0);
}


size_t UserData::recvFromClient(int fd)
{
	int len;

	len = read(fd, _buf, BUFFER_SIZE);
	std::cout << Colors::BoldBlue << "received message from client " << fd << std::endl;
	for (int i = 0; i < len; i++)
	{
		std::cout << _buf[i];
		_received << _buf[i];
	}
	return (len);
}

size_t UserData::sendToClient(int fd)
{
	size_t len;

	std::cout << Colors::BoldMagenta << "send to client " << fd << "\n" << Colors::Reset<< std::endl;
	len = write(fd, _response.c_str(), _response.size()); // 큰 파일도 한 번에 보낼 수 있나?
	if (len < 0)
	{
		std::cout << Colors::RedString("send() error") << std::endl;
	}
	return (len);
}

ChangeList::ChangeList(void)
	:	_keventVector(std::vector<struct kevent>())
{}

ChangeList::~ChangeList(void) {}

void ChangeList::changeEvent(uintptr_t ident, int filter, int flags)
{
	// fflags로 CGI 처리와 signal 등 처리 해야함.
	struct kevent	target;
	UserData *udata;
	/* init udata */
	if (flags == EV_DELETE)
	{
		for (std::vector<struct kevent>::iterator it = _keventVector.begin(); it != _keventVector.end(); it++)
		{
			if (it->ident == ident)
			{
				delete static_cast<UserData*>(it->udata);
				return ;
			}
		}
	}
	udata = new UserData(ident);
	target.ident = ident;
	target.filter = filter;
	target.flags = flags;
	target.fflags = 0;
	target.data = 0;
	target.udata = udata;
	_keventVector.push_back(target);
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
