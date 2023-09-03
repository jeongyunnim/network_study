#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
// #include <netinet/in.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int     sock;
	char    message[BUF_SIZE] = {0};
	struct sockaddr_in  serverAddr;
	int		argNum = 0;

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
	std::string temp;
	std::cin >> temp;
	for (std::string::iterator it = temp.begin(); it != temp.end(); it++)
	{
		if (std::isdigit(*it) == false)
		{
			close(sock);
			std::cout << "input Error" << std::endl;
			return (1);
		}
	}
	argNum = atoi(temp.c_str());
	for (int i = 0; i < argNum; i++)
	{
		std::cin >> message;
	    write(sock, message, strlen(message));
	}
	std::cin >> message;
	if (std::strcmp(message, "+") != 0 && std::strcmp(message, "-") != 0 && \
		std::strcmp(message, "/") != 0 && std::strcmp(message, "*") != 0)
	{
		std::cout << "operator error" << std::endl;
		close(sock);
		return (1);
	}
	else
	{
		std::cout << "operator " << message << std::endl;
	}
	write(sock, message, strlen(message));
	if (read(sock, message, BUF_SIZE - 1) == -1)
	{
		std::cerr << "read() error" << std::endl;
		return (1);
	}
	std::cout << "result from server: " << message << std::endl;
	close(sock);
	return (0);
}