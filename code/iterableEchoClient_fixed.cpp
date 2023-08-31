#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
// #include <netinet/in.h>
#include <sys/socket.h>

#define BUF_SIZE 100

int main(int argc, char *argv[])
{
	int     sock;
	char    message[BUF_SIZE] = {0};
	int     strLen, receiveLen, receiveCount;
	struct sockaddr_in  serverAddr;

	if (argc != 2)
	{
		std::cerr << "Usage: [program] [port]" << std::endl;
		return (1);
	}
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		std::cerr << "Create server socket Error" << std::endl;
		return (1);
	}
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(atoi(argv[1]));
    if (connect(sock, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1)
    {
        std::cerr << "connect() error" << std::endl;
        return (1);
    }
    else
        std::cout << "connected" << std::endl;
	while (1)
	{
		std::cin >> message;
        if (strcmp(message, "q") == 0)
            break ;
        strLen = write(sock, message, sizeof(message)); // 이 부분부터 변경
		receiveLen = 0;
		while (receiveLen < strLen)
		{
			receiveCount = read(sock, &message[receiveLen], BUF_SIZE - 1);
			if (receiveCount == -1)
			{
				std::cerr << "read() error" << std::endl;
				return (1);
			}
			receiveLen += receiveCount;
		}
		message[receiveLen] = '\0';
		std::cout << "massage from server: " << message << std::endl;
	}
	close(sock);
	return (0);
}