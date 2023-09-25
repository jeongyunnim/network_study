#pragma once
#include <vector>
#include "Server.hpp"
#define BUFFER_SIZE 1024
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
	const std::stringstream& getReceived(void) const;
	const std::string& getResponse(void) const;
	int	getMethod(void) const;
	bool getWriteFlag(void) const;
	int	generateResponse(void);
	int	generateGetRequest(void);
	int	parseRequest(std::stringstream& request);
	size_t recvFromClient(int fd);
	size_t sendToClient(int fd);
	
private:
	UserData(void);
	UserData(const UserData& rhs);
	UserData& operator=(const UserData& rhs);
	
	uintptr_t			_fd;
	char				_buf[BUFFER_SIZE];
	int					_method;
	bool				_writeFlag;
	std::string			_uri;
	std::stringstream	_received;
	std::string			_response;
	/* 
	 * 합칠 데이터를 여기에 만들어주는 것이 좋으려나? vector로 이어주게끔..
	 * 또 필요한 정보들이 무엇이 있을지 파악해야 함.
	 * 객체화 하는 것이 좋을 것 같다.
	 */
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