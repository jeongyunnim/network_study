#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void)
{
	FILE *fp = fopen("../test.txt", "rb");
	if (fp == NULL)
	{
		std::cout << "open error" << std::endl;
		return (1);
	}

	int hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == -1)
	{
		std::cout << "Error: socket." << std::endl;
		return (1);
	}
	sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(80);
	svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (::bind(hSocket, (sockaddr*)&svraddr, sizeof(svraddr)) == -1)
	{
		std::cout << "Error: 소켓에 IP주소와 port를 바인드할 수 없음." << std::endl;
		return (1);
	}
	if (::listen(hSocket, SOMAXCONN) == -1)
	{
		std::cout << "Error: listen." << std::endl;
		return (1);
	}

	sockaddr_in clientaddr = { 0 };
	socklen_t nAddrLen = sizeof(clientaddr);
	int hClient = 0;

	hClient = ::accept(hSocket, (sockaddr*)&clientaddr, &nAddrLen);
	if (hClient == -1)
	{
		std::cerr << "accept() error" << std::endl;
		exit (1);
	}
	else
		std::cout << "새 클라이언트 생성" << std::endl;

	char	byteBuffer[65536];
	int nRead, nSent, i = 0;
	while ((nRead = fread(byteBuffer, sizeof(char), 65536, fp)) > 0)
	{
		nSent = ::send(hClient, byteBuffer, nRead, 0);
		std::cout << "[" << ++i << "] sent: " << nSent << std::endl; // 읽은 만큼 보냈는가? -> 수신측에서 여유가 없어서 수신을 못했었을 수도 있다.
		fflush(stdout);
	}
	if (nRead == 0)
		close(hClient);
	else
		exit (1);
	close(hSocket);
	std::cout << "close server" << std::endl;
	return (0);
}