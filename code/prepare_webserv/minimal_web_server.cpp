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
#include <sys/event.h> // Include kqueue library
#include "Colors.hpp"

#define BUF_SIZE 65536 // OS socket 버퍼 사이즈를 알아내서 컨트롤 해주는 것이 좋을까?
#define MAX_EVENTS 50 // 이벤트 수. 무엇을 나타내는가?
#define PORT_NUM 8080
#define CRLF "\r\n"

// enum method
// {
// 	GET,
// 	POST,
// 	PUT,
// } method;

typedef struct headerInfo
{
	std::string method;
	std::string url;
	std::string version;
} headerInfo;

void	initServerSocket(uintptr_t& serverSocket, struct sockaddr_in& serv_adr)
{
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(PORT_NUM);

	if (bind(serverSocket, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
	{
		std::cout << "bind() error" << std::endl;
		exit(1);
	}
	if (listen(serverSocket, 3) == -1)
	{
		std::cout << "listen() error" << std::endl;
		exit(1);
	}
}

void	initKqueue(int& kq)
{
	kq = kqueue();
	if (kq == -1)
	{
		perror("kqueue() error");
		exit(1);
	}
}

int	parseBuf(const char *buf, headerInfo& info)
{
	std::stringstream bufStream(buf);

	std::getline(bufStream, info.method, ' ');
	/* 유효성 검증 */
	std::getline(bufStream, info.url, ' ');
	/* 유효성 검증 */
	std::getline(bufStream, info.version, '\n');
	/* 유효성 검증 */
	if (*(info.version.end() - 1) == '\r')
	{
		std::cout << "yes this is ascii 13" << std::endl;
		info.version.erase(info.version.size() - 1);
	}
	std::cout << info.method << ' ' << info.url << ' ' << info.version << std::endl;
	return (0);
}

int main(void)
{
	uintptr_t serverSocket, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_size;
	int str_len, i;
	char buf[BUF_SIZE];
	std::string bufStr;

	initServerSocket(serverSocket, serv_adr);
	std::cout << "server socket is now listen." << std::endl;
	int kq;
	struct kevent changelist[2], *eventlist;
	int nev;

	initKqueue(kq);

	EV_SET(&changelist[0], serverSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, changelist, 1, NULL, 0, NULL) == -1)
	{
		perror("kevent() error");
		exit(1);
	}

	// eventlist 배열 동적 할당
	eventlist = (struct kevent *)malloc(sizeof(struct kevent) * MAX_EVENTS);
	headerInfo clientRequest;
	while (1)
	{
		nev = kevent(kq, NULL, 0, eventlist, MAX_EVENTS, NULL);
		if (nev == -1)
		{
			perror("kevent() error");
			break;
		}
		for (i = 0; i < nev; i++)
		{
			if (eventlist[i].ident == serverSocket) // 연결 요청을 받음.
			{
				adr_size = sizeof(clnt_adr);
				clnt_sock = accept(serverSocket, (struct sockaddr *)&clnt_adr, &adr_size);

				EV_SET(&changelist[0], clnt_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, changelist, 1, NULL, 0, NULL) == -1)
				{
					perror("kevent() error");
					exit(1);
				}
				std::cout << "connected client: " << clnt_sock << std::endl;
			}
			else
			{
				str_len = read(eventlist[i].ident, buf, BUF_SIZE);
				/**
				 * string에 담아서 파싱해도 괜찮을까? 속도 등 효율 측면에서 생각해볼 때.
				 * CRLF까지 읽어야 하므로.. 일단 개행까지 읽으며 처리해주고, 보낼 때만 잘 해결을 해주면 괜찮으려나?
				 * 파싱 규칙을 파악하기 위해서 RFC 문서를 읽도록 하자. 
				 * <Click with CMD> https://www.rfc-editor.org/rfc/rfc2616
				 */
				std::cout << Colors::Magenta << "received message: " << buf << Colors::Reset << std::endl;
				if (str_len == 0) // close request
				{
					EV_SET(&changelist[0], eventlist[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					if (kevent(kq, changelist, 1, NULL, 0, NULL) == -1)
					{
						perror("kevent() error");
						exit(1);
					}
					close(eventlist[i].ident);
					printf("closed client: %lu\n", eventlist[i].ident);
				}
				else
				{
					parseBuf(buf, clientRequest);
					/* client 요청 분석 */
					if (clientRequest.method == "GET") // 대문자 소문자 처리 해야 하나?
					{
						std::fstream requestedFile;
						if (clientRequest.url == "/")
						{
							requestedFile.open("./index.html");
							std::cout << Colors::BlueString("open ./index.html") << std::endl;
						}
						else
						{
							requestedFile.open("." + clientRequest.url);
							std::cout << Colors::BlueString("open ." + clientRequest.url) << std::endl;
						}
						if (requestedFile.is_open() == false)
						{
							// 4XX error
							std::cerr << Colors::RedString("client request error: " + clientRequest.url) << std::endl; 
							shutdown(eventlist[i].ident, SHUT_RDWR);
						}
						else
						{
							std::string bufTemp;
							std::getline(requestedFile, bufTemp, static_cast<char>EOF); // 만약 파일이 버퍼의 크기보다 크다면..??
							bufTemp += CRLF;
							write(eventlist[i].ident, "HTTP/1.1 200 OK\r\n\r\n", 19);
							write(eventlist[i].ident, bufTemp.c_str(), bufTemp.size());
							std::cout << Colors::Cyan << clientRequest.url << " send complete" << Colors::Reset << std::endl;
						}
					}
					/**
					 * @brief 각 요청에 대한 HTTP 헤더 / 바디를 생성해주는 모듈 필요
					 * 
					 */
					// write(1, "HTTP/1.1 200 OK\r\n\r\n", 19);
					// write(eventlist[i].ident, bufStr.c_str(), bufStr.size());
					// shutdown(eventlist[i].ident, SHUT_RDWR);
				}
			}
		}
	}
	free(eventlist); // 동적으로 할당한 메모리 해제
	close(serverSocket);
	close(kq);
	return (0);
}

