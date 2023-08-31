#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
// #include <netinet/in.h>
#include <sys/socket.h>

#define BUF_SIZE 50

int main(int argc, char *argv[])
{
	int     serverSocket, clientSocket;
	char    message[BUF_SIZE] = {0};
	int     strLen;

	struct sockaddr_in  serverAddr, clientAddr;
	socklen_t         clientAddrSize;

	if (argc != 2)
	{
		std::cerr << "Usage: [program] [port]" << std::endl;
		return (1);
	}
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
	{
		std::cerr << "Create server socket Error" << std::endl;
		return (1);
	}
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(atoi(argv[1]));
	if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1)
	{
		std::cerr << "bind server socket Error" << std::endl;
		return (1);
	}
	if (listen(serverSocket, 5) == -1)
	{
		std::cerr << "listen Error" << std::endl;
		return (1);
	}
	clientAddrSize = sizeof(clientSocket); // 이 부분 이해 제대로
	for (int i = 0; i < 5; i++)
	{
		std::cout << "wait client... " << std::endl;
		clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrSize);
		if (clientSocket == -1)
		{
			std::cerr << "accept() Error" << std::endl;
			return (1);
		}
		else
			std::cout << "client connected" << std::endl;
		while ((strLen = read(clientSocket, message, BUF_SIZE)) != 0)
		{
			std::cout << "massage from client: " << message << std::endl;
			for (int i = 0; message[i] != '\0'; i++)
			{
				if ('a' <= message[i] && message[i] <= 'z')
					message[i] -= 32;
			}
			write(clientSocket, message, strLen);
		}
		close(clientSocket);
	}
	close(serverSocket);
	return (0);
}