#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <deque>

#define BUF_SIZE 50

enum e_oprator
{
	PLUS=0,
	MINUS,
	DIVED,
	MULTI
};

int main(int argc, char *argv[])
{
	int     serverSocket, clientSocket;
	char    message[BUF_SIZE] = {0};
	long long result;
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
	clientAddrSize = sizeof(clientSocket);
	std::deque<int> args;
	std::string checker;
	int operatorFlag;
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
		operatorFlag = 0;
		while ((strLen = read(clientSocket, message, BUF_SIZE)) != 0)
		{
			checker.clear();
			checker = message;
			std::cout << "message: " << checker << std::endl;
			for (std::string::iterator it = checker.begin(); it != checker.end(); it++)
			{
				if (std::isdigit(*it) == false)
				{
					operatorFlag = 1;
					break ;
				}
			}
			if (operatorFlag == 1)
				break ;
			args.push_back(atoi(message));
		}
		if (checker == "+")
		{
			for (std::deque<int>::iterator it = args.begin(); it != args.end(); it++)
			{
				if (it == args.begin())
					result = *it;
				else
					result += *it;
			}
			write(clientSocket, std::to_string(result).c_str() , BUF_SIZE - 1);
		}
		else if (checker == "-")
		{
			for (std::deque<int>::iterator it = args.begin(); it != args.end(); it++)
			{
				if (it == args.begin())
					result = *it;
				else
					result -= *it;
			}
			write(clientSocket, std::to_string(result).c_str() , BUF_SIZE - 1);
		}
		else if (checker == "*")
		{
			for (std::deque<int>::iterator it = args.begin(); it != args.end(); it++)
			{
				if (it == args.begin())
					result = *it;
				else
					result *= *it;
			}
			write(clientSocket, std::to_string(result).c_str() , BUF_SIZE - 1);
		}
		else if (checker == "/")
		{
			for (std::deque<int>::iterator it = args.begin(); it != args.end(); it++)
			{
				if (it == args.begin())
					result = *it;
				else if (*it != 0)
					result /= *it;
				else
				{
					write(clientSocket, "Dived by zero error", 20);
					break ;
				}
			}
			write(clientSocket, std::to_string(result).c_str() , BUF_SIZE - 1);
		}
		else
			write(clientSocket, "invalid argument error", 23);
		close(clientSocket);
		args.clear();
	}
	close(serverSocket);
	return (0);
}