#include <algorithm>
#include <vector>
#include <fcntl.h>
#include "Server.hpp"
#include "ChangeList.hpp"

#define BUFFER_SIZE 1024 // OS socket 버퍼 사이즈를 알아내서 컨트롤 해주는 것이 좋을까?
#define PORT_NUM 8080
#define MAX_NEVENTS 10
#define ERROR -1
#define CRLF "\r\n"

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

	int 	kq = kqueue();
	if (kq == -1)
		exit(1);

	ChangeList changeList;
	struct kevent eventList[MAX_NEVENTS]; 
	int occurEventNum; // kevent에서 발생한 이벤트 개수
	
	// udata에 각 소켓의 버퍼, 설정, 에러 등을 담아서 이벤트 발생 시 해당 부분을 핸들링 할 수 있도록 한다.
	changeList.changeEvent(server.getSocket(), EVFILT_READ, EV_ADD);
	int value = true;
	setsockopt(server.getSocket(), SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &value, sizeof(sockaddr_in));
	headerInfo clientRequest;

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
			if (eventList[i].ident == server.getSocket()) // 연결 요청을 받음.
			{
				adr_size = sizeof(clnt_adr);
				clnt_sock = accept(server.getSocket(), (struct sockaddr *)&clnt_adr, &adr_size);
				changeList.changeEvent(clnt_sock, EVFILT_READ, EV_ADD);
				fcntl(clnt_sock, F_SETFL, O_NONBLOCK);
				std::cerr << "connected client: " << clnt_sock << std::endl;
			}
			else
			{
				UserData *currentEventUdata = static_cast<UserData*>(eventList[i].udata);
				str_len = currentEventUdata->recvFromClient(eventList[i].ident);
				if (str_len == 0) // close request -> write 이벤트도 등록해야 한다.
				{
					changeList.changeEvent(eventList[i].ident, EVFILT_READ, EV_DELETE);
					close(eventList[i].ident);
					std::cout << "close client:" << eventList[i].ident << std::endl;
				}
				else if (str_len < 0)
				{
					close(eventList[i].ident);
					std::cout << "ERROR: recv error: force close client: " << eventList[i].ident << std::endl;
				}
				else
				{
					currentEventUdata->generateResponse();
				}
			}
		}
	}
	close(server.getSocket());
	close(kq);
	return (0);
}

