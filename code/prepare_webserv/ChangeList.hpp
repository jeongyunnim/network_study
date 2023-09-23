#pragma once
#include "Server.hpp"
#include <vector>
#define BUFFER_SIZE 1024

class UserData
{
public:

	UserData(void);
	~UserData(void);
	
private:
	UserData(const UserData& rhs);
	UserData& operator=(const UserData& rhs);
	
	uintptr_t			_fd;
	char				_buf[BUFFER_SIZE];
	std::stringstream	_received;
	std::stringstream	_response;
	/* 
	 * 합칠 데이터를 여기에 만들어주는 것이 좋으려나? vector로 이어주게끔..
	 * 또 필요한 정보들이 무엇이 있을지 파악해야 함.
	 * 객체화 하는 것이 좋을 것 같다.
	 */
} udata;
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