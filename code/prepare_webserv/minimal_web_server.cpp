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

void	initKqueue(int& kq)
{
	kq = kqueue();
	if (kq == -1)
	{
		perror("kqueue() error");
		exit(1);
	}
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
	struct kevent eventList[MAX_NEVENTS]; 
	// 일반적으로 MacOS는 Kqueue 최대 개수에 제한을 두지 않는다. 설정할 수 있게 해야 함.
	int occurEventNum; // kevent에서 발생한 이벤트 개수
	
	// udata에 각 소켓의 버퍼, 설정, 에러 등을 담아서 이벤트 발생 시 해당 부분을 핸들링 할 수 있도록 한다.
	changeList.changeEvent(server.getSocket(), EVFILT_READ, EV_ADD);
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
				changeList.changeEvent(clnt_sock, EVFILT_READ | EVFILT_WRITE, EV_ADD);
				fcntl(clnt_sock, F_SETFL, O_NONBLOCK);
				std::cerr << "connected client: " << clnt_sock << std::endl;
			}
			else
			{
				UserData *currentEventUdata = static_cast<UserData*>(eventList[i].udata);
				str_len = currentEventUdata->recvFromClient(eventList[i].ident);
				if (str_len == 0) // close request -> write 이벤트도 등록해야 한다.
				{
					changeList.changeEvent(eventList[i].ident, EVFILT_READ | EVFILT_WRITE, EV_DELETE);
					close(eventList[i].ident);
					printf("close client: %lu\n", eventList[i].ident);						
				}
				else if (str_len < 0)
				{
					close(eventList[i].ident);
					printf("ERROR: recv error: force close client: %lu\n", eventList[i].ident);
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

