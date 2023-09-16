#include <algorithm>
#include <vector>
#include "Server.hpp"

#define BUF_SIZE 65536 // OS socket 버퍼 사이즈를 알아내서 컨트롤 해주는 것이 좋을까?
#define PORT_NUM 8082
#define CRLF "\r\n"

typedef struct headerInfo
{
	std::string method;
	std::string url;
	std::string version;
	bool		keepAliveFlag;
} headerInfo;

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
	std::stringstream	bufStream(buf);
	std::string			token;

	std::getline(bufStream, info.method, ' ');
	/* 유효성 검증 */
	std::getline(bufStream, info.url, ' ');
	/* 유효성 검증 */
	std::getline(bufStream, info.version, '\n');
	/* 유효성 검증 */
	if (*(info.version.end() - 1) == '\r')
	{
		std::cout << "yes this is ascii " << static_cast<int>(info.version[info.version.size() - 1])<< std::endl;
		info.version.erase(info.version.size() - 1);
	}
	std::cout << info.method << ' ' << info.url << ' ' << info.version << std::endl;
	// while (1) // header 정보 받아오기
	// {
	// 	std::getline(bufStream, token, '\n');
	// 	/* 유효성 검증 */
	// 	if (bufStream.eof())
	// 		break ;
	// 	else if (*(info.version.end() - 1) == '\r')
	// 		info.version.erase(info.version.size() - 1);
	//	else
	//		//필요한 정보 map에 저장하기.. 그런데 굳이 저장 해야 하나? 파싱하면서 플래그만 켜주면 되는 거 아닌가?
	// }

	return (0);
}

int main(void)
{
	Server server;

	/* 서버 소켓 활성화(listen) */
	server.initServerAddress(PORT_NUM);
	if (server.initServerListeningSocket() == -1)
		return (1);
	std::cout << "server socket is now listen." << std::endl;

	uintptr_t clnt_sock;
	struct sockaddr_in clnt_adr;
	socklen_t adr_size;
	int str_len, i;


	int kq;
	initKqueue(kq); // kernel queue file descriptor 생성

	struct kevent changelist[2]; // change list와 event list를 따로 두는 이유
	std::vector<struct kevent> eventlist; // 동적 배열에 할당 일반적으로 MacOS는 Kqueue 최대 개수에 제한을 두지 않는다.
	int eventNum; // 발생한 이벤트 개수


	// udata에 각 소켓의 버퍼, 설정, 에러 등을 담아서 이벤트 발생 시 해당 부분을 핸들링 할 수 있도록 한다.
	EV_SET(&changelist[0], server.getSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL); 
	/* 서버 리슨 소켓은 읽기만 하면 되므로 */
	if (kevent(kq, changelist, 1, NULL, 0, NULL) == -1)
	{
		perror("kevent() error");
		exit(1);
	}

	// eventlist 배열 동적 할당 
	headerInfo clientRequest;
	while (1)
	{
		eventNum = kevent(kq, NULL, 0, &eventlist[0], eventlist.size(), NULL);
		if (eventNum == -1)
		{
			perror("kevent() error");
			break;
		}
		for (i = 0; i < eventNum; i++)
		{
			if (eventlist[i].ident == server.getSocket()) // 연결 요청을 받음.
			{
				adr_size = sizeof(clnt_adr);
				clnt_sock = accept(server.getSocket(), (struct sockaddr *)&clnt_adr, &adr_size);
				EV_SET(&changelist[0], clnt_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, changelist, 1, NULL, 0, NULL) == -1)
				{
					perror("kevent() error");
					exit(1);
				}
				std::cerr << "connected client: " << clnt_sock << std::endl;
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
				std::cout << Colors::Magenta << "received message\n" << buf << Colors::Reset << std::endl;
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
					// if (소켓의 요청이 keep alive 일 때)
					int opt = true;
					setsockopt(clnt_sock, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
					std::string extTemp;
					
					if (clientRequest.method == "GET") // 대문자 소문자 처리 해야 하나?
					{
						std::ifstream requestedFile;
						if (clientRequest.url == "/")
						{
							clientRequest.url = "/index.html";
						}
						extTemp = clientRequest.url.substr(clientRequest.url.find('.') + 1);
						requestedFile.open("." + clientRequest.url, std::ios::in | std::ios::binary);
						std::cout << Colors::BlueString("try open ." + clientRequest.url) << std::endl;
						if (requestedFile.is_open() == false)
						{
							// 4XX error
							std::cerr << Colors::RedString("open failed: ." + clientRequest.url) << std::endl; 
							write(eventlist[i].ident, "HTTP/1.1 404 Not found\r\n\r\n", 26);
							shutdown(eventlist[i].ident, SHUT_RDWR);
						}
						else
						{
							std::stringstream responseHeaders;
							requestedFile.seekg(0, std::ios::end);
							std::streampos fileSize = requestedFile.tellg();
							requestedFile.seekg(0, std::ios::beg);
							// HTTP version 낮은 걸로 바꿔서 보낼 수 있어야 함.
							responseHeaders << "HTTP/1.1 200 OK\r\n";
							responseHeaders << "Content-type: ";
							if (extTemp == "png")
							{
								responseHeaders << "image/png\r\n";
							}
							else
							{
								responseHeaders << "text/" << extTemp << CRLF;
							}
							responseHeaders << "Content-length: " << fileSize << "\r\n\r\n";
							write(1, responseHeaders.str().c_str(), responseHeaders.str().size());
							write(eventlist[i].ident, responseHeaders.str().c_str(), responseHeaders.str().size());
							while (requestedFile.eof() == false)
							{
								requestedFile.read(buf, sizeof(buf));
								write(eventlist[i].ident, buf, requestedFile.gcount());
								// write(1, buf, requestedFile.gcount());
							}
							requestedFile.close();
							std::cout << Colors::Cyan << clientRequest.url << " send complete" << Colors::Reset << std::endl;
							// shutdown(eventlist[i].ident, SHUT_RDWR);
						}
					}
					/**
					 * 각 요청에 대한 HTTP 헤더 / 바디를 생성해주는 모듈 필요
					 */
				}
			}
		}
	}
	close(server.getSocket());
	close(kq);
	return (0);
}

