#include <algorithm>
#include <vector>
#include "Server.hpp"
#include "ChangeList.hpp"

#define BUFFER_SIZE 1024 // OS socket 버퍼 사이즈를 알아내서 컨트롤 해주는 것이 좋을까?
#define PORT_NUM 8080
#define MAX_NEVENTS 10
#define ERROR -1
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

int	handleRequestGET(headerInfo& clientRequest, struct kevent client)
{
	std::ifstream requestedFile;
	std::string extTemp;

	if (clientRequest.url == "/")
		clientRequest.url = "/index.html";
	extTemp = clientRequest.url.substr(clientRequest.url.find('.') + 1);
	requestedFile.open("." + clientRequest.url, std::ios::binary);
	if (requestedFile.is_open() == false)
	{
		// 4XX error
		std::cerr << Colors::RedString("open failed: ." + clientRequest.url) << std::endl; 
		write(client.ident, "HTTP/1.1 404 Not found\r\n\r\n", 26);
		shutdown(client.ident, SHUT_RDWR);
	}
	else
	{
		requestedFile.seekg(0, std::ios::end);
		std::streampos fileSize = requestedFile.tellg();
		requestedFile.seekg(0, std::ios::beg);
		// HTTP version 낮은 걸로 바꿔서 보낼 수 있어야 함.
		static_cast<struct udata *>(client.udata)->response << "HTTP/1.1 200 OK\r\nContent-type: ";
		if (extTemp == "png")
			static_cast<struct udata *>(client.udata)->response << "image/";
		else
			static_cast<struct udata *>(client.udata)->response << "text/";
		static_cast<struct udata *>(client.udata)->response << extTemp << CRLF << "Content-length: " << fileSize << "\r\n\r\n";
		// write(1, response.str().c_str(), response.str().size());
		write(client.ident, static_cast<struct udata *>(client.udata)->response.str().c_str(), static_cast<struct udata *>(client.udata)->response.str().size());
		while (requestedFile.eof() == false)
		{
			requestedFile.read(static_cast<struct udata *>(client.udata)->buf, sizeof(static_cast<struct udata *>(client.udata)->buf));
			write(client.ident, static_cast<struct udata *>(client.udata)->buf, requestedFile.gcount());
		}
		requestedFile.close();
	}
	return (0);
}

int main(void)
{
	Server server;

	/* 서버 소켓 활성화(listen) */
	server.initServerAddress(PORT_NUM);
	if (server.initServerListeningSocket() == -1)
		return (1);
	std::cout << Colors::GreenString("server socket is now listen.") << std::endl;

	uintptr_t clnt_sock;
	struct sockaddr_in clnt_adr;
	socklen_t adr_size;
	int str_len, i;

	int kq;
	initKqueue(kq); // kernel queue file descriptor 생성

	ChangeList changeList;
	// 하지만 error를 따로 이벤트 감지할 필요가 있는지의 의문이 든다. -> eventlist[i].flag & EV_ERROR == true 와 어떤 차이가 있는가?
	struct kevent eventList[MAX_NEVENTS]; 
	// 일반적으로 MacOS는 Kqueue 최대 개수에 제한을 두지 않는다. 설정할 수 있게 해야 함.
	int occurEventNum; // kevent에서 발생한 이벤트 개수
	
	// udata에 각 소켓의 버퍼, 설정, 에러 등을 담아서 이벤트 발생 시 해당 부분을 핸들링 할 수 있도록 한다.
	changeList.changeEvent(server.getSocket(), EVFILT_READ, EV_ADD);
	headerInfo clientRequest;

	while (1)
	{
		occurEventNum = kevent(kq, changeList.getKeventVector().data(), changeList.getKeventVector().size(), eventList, MAX_NEVENTS, NULL);
		if (occurEventNum == -1)
		{
			perror("kevent() error");
			break;
		}
		changeList.clearEvent();
		for (i = 0; i < occurEventNum; i++)
		{
			if (eventList[i].ident == server.getSocket()) // 연결 요청을 받음.
			{

				adr_size = sizeof(clnt_adr);
				clnt_sock = accept(server.getSocket(), (struct sockaddr *)&clnt_adr, &adr_size);

				changeList.changeEvent(clnt_sock, EVFILT_READ, EV_ADD);
				std::cerr << "connected client: " << clnt_sock << std::endl;
			}
			else
			{
				udata *clientData = static_cast<udata *>(eventList[i].udata);
				str_len = read(eventList[i].ident, clientData->buf, BUFFER_SIZE);
				std::cout << Colors::Magenta << "received message\n" << clientData->buf << Colors::Reset << std::endl;
				if (str_len == 0) // close request
				{
					changeList.changeEvent(eventList[i].ident, EVFILT_READ, EV_DELETE);
					close(eventList[i].ident);
					printf("closed client: %lu\n", eventList[i].ident);
				}
				else
				{
					parseBuf(clientData->buf, clientRequest);
					/* client 요청 분석 */
					if (clientRequest.method == "GET")
					{
						handleRequestGET(clientRequest, eventList[i]);
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

