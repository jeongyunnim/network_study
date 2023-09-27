#pragma once
#include <vector>
#include "Server.hpp"
#define BUFFER_SIZE 1024

// enum eHeaders
// {

// } eHeader;

enum eMethod
{
	GET=0,
	POST,
	DELETE,
	ERROR=-1
};

typedef struct headerInfo
{
	std::string url;
	std::string version;
	bool		keepAliveFlag;
} headerInfo;

class UserData
{
public:

	UserData(int fd);
	~UserData(void);
	void initUserData(void);
	const std::stringstream& getReceived(void) const;
	const std::string& getResponse(void) const;
	int	getMethod(void) const;
	int	getfd(void) const;
	int	generateResponse(void);
	int	generateGetResponse(void);
	int	parseRequest(std::stringstream& request);
	int	parseHeader(std::string& field);
	int recvFromClient(int fd);
	int sendToClient(int fd);
	
private:
	UserData(void);
	UserData(const UserData& rhs);
	UserData& operator=(const UserData& rhs);
	
	uintptr_t			_fd;
	char				_buf[BUFFER_SIZE];
	int					_method;
	int					_status;
	int					_headerFlag;
	std::string			_uri;
	std::stringstream	_received;
	std::string			_response;
};
class ChangeList // 얘는 kqueue manage class가 될 수 있음.
{
public:
	ChangeList(void);
	~ChangeList(void);
	void changeEvent(uintptr_t nIdent, int nFilter, int nFlags);
	void clearEvent(void);
	std::vector<struct kevent>& getKeventVector(void);
	size_t getSize(void);
	size_t getUdata(void);

private:

	ChangeList& operator=(const ChangeList& rhs);
	ChangeList(const ChangeList& other);

	std::vector<struct kevent>	_keventVector;
};