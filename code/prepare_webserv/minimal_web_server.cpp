#include <algorithm>
#include <vector>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include "Server.hpp"
#include "ChangeList.hpp"

#define BUFFER_SIZE 1024 // OS socket 버퍼 사이즈를 알아내서 컨트롤 해주는 것이 좋을까?
#define PORT_NUM 8080
#define MAX_NEVENTS 10
#define ERROR -1
#define CRLF "\r\n"

void setSocketKeepAlive(int clientSocket, int cnt, int idle, int interval)
{
	int on = true;

	setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on));
	setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt));
	setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPALIVE, &idle, sizeof(idle));
	setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
}

int main(void)
{
	Server server;

	/* 서버 소켓 활성화(listen) */
	server.initServerAddress(PORT_NUM);
	if (server.initServerListeningSocket() == -1)
		return (1);
	std::cout << Colors::GreenString("server socket is now listen.") << std::endl;

	int clnt_sock;
	struct sockaddr_in clnt_adr;
	socklen_t adr_size;
	int str_len, i;

	int kq = kqueue();
	if (kq == -1)
		exit(1);

	ChangeList changeList;
	struct kevent eventList[MAX_NEVENTS];
	int occurEventNum; // kevent에서 발생한 이벤트 개수
	
	changeList.changeEvent(server.getSocket(), EVFILT_READ, EV_ADD);
	int value = true;
	setsockopt(server.getSocket(), SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
	setsockopt(server.getSocket(), SOL_SOCKET, SO_REUSEPORT, &value, sizeof(value));
	
	struct linger lingerVal;
	lingerVal.l_onoff = true;
	lingerVal.l_linger = true;

	while (1)
	{
		occurEventNum = kevent(kq, &changeList.getKeventVector()[0], changeList.getKeventVector().size(), eventList, MAX_NEVENTS, NULL);
		if (occurEventNum == -1)
		{
			perror("kevent() error");
			break;
		}
		changeList.clearEvent();
		for (i = 0; i < occurEventNum; i++)
		{
			UserData *currentEventUdata = static_cast<UserData*>(eventList[i].udata);
			if (eventList[i].ident == server.getSocket()) // 연결 요청을 받음.
			{
				adr_size = sizeof(clnt_adr);
				clnt_sock = accept(server.getSocket(), (struct sockaddr *)&clnt_adr, &adr_size);
				changeList.changeEvent(clnt_sock, EVFILT_READ, EV_ADD);
				setsockopt(clnt_sock, SOL_SOCKET, SO_LINGER, &lingerVal, sizeof(lingerVal));
				setSocketKeepAlive(clnt_sock, 60, 5, 5);
				fcntl(clnt_sock, F_SETFL, O_NONBLOCK);
				std::cerr << "connected client: " << clnt_sock << std::endl;
			}
			else if (eventList[i].filter & EVFILT_READ)
			{
				str_len = currentEventUdata->recvFromClient(eventList[i].ident);
				if (str_len == 0 || (eventList[i].fflags & EV_EOF) == true) // close request -> write 이벤트도 등록해야 한다.
				{
					std::cout << Colors::BoldGreen << "close client:" << eventList[i].ident << Colors::Reset << std::endl;
					delete currentEventUdata;
					changeList.changeEvent(eventList[i].ident, EVFILT_READ | EVFILT_WRITE, EV_DELETE);
					close(eventList[i].ident);
				}
				else if (str_len < 0 || (eventList[i].fflags & EV_ERROR) == true)
				{
					close(eventList[i].ident);
					std::cout << "ERROR: recv error: force close client: " << eventList[i].ident << std::endl;
				}
				else
				{
					if (currentEventUdata->generateResponse() == ERROR)
						return (404);
				}
			}
			else if (eventList[i].filter & EVFILT_WRITE)
			{
				std::cout << "도대체 WRITE 언제..?" << std::endl;
			}
		}
	}
	close(server.getSocket());
	close(kq);
	return (0);
}
