#include <iostream>
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
	int     strLen;
	struct sockaddr_in  serverAddr;

	if (argc != 3)
	{
		std::cerr << "Usage: [program] [ip] [port]" << std::endl;
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
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
	serverAddr.sin_port = htons(atoi(argv[2]));
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
        if (message[0] == 'Q' && message[1] == '\0')
            break ;
        write(sock, message, sizeof(message));
        memset(&message, 0, sizeof(message));
        strLen = read(sock, message, BUF_SIZE - 1);
        message[strLen] = '\0';
        std::cout << "massage from server: " << message << std::endl;
	}
	close(sock);
	return (0);
}