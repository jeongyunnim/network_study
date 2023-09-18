#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/event.h>
#include "Colors.hpp"

/**
 * @brief nginx config 파일을 파싱한 뒤 필요한 스펙에 따라 서버를 열어야 함.
 */
class Server
{
public:

	Server(void);
	~Server(void);

	void				initServerAddress(unsigned short portNum);
	int					initServerListeningSocket(void);
	const uintptr_t&	getSocket();

private:

	Server& operator=(const Server& other);
	Server(const Server& rhs);

	struct sockaddr_in	_socketAddress;
	uintptr_t			_socket;

};