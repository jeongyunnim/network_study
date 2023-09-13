#include "Server.hpp"

Server::Server(void)
	:	_socket(0)
{
	memset(&_socketAddress, 0, sizeof(_socketAddress));
}

Server::~Server(void) {}

void	Server::initServerAddress(unsigned short portNum)
{
	_socket = socket(PF_INET, SOCK_STREAM, 0);
	_socketAddress.sin_family = AF_INET;
	_socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	_socketAddress.sin_port = htons(portNum);
}

int		Server::initServerListeningSocket(void)
{
	if (bind(_socket, (struct sockaddr *)&_socketAddress, sizeof(_socketAddress)) == -1)
	{
		std::cout << "bind() error" << std::endl;
		return (-1);
	}
	if (listen(_socket, 5) == -1) // backlog 계획 해야 함. seige 부하테스트
	{
		std::cout << "listen() error" << std::endl;
		return (-1);
	}
	return (0);
}	

const uintptr_t&	Server::getSocket()
{
	return (_socket);
}
