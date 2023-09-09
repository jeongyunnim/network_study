// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <sys/time.h>
// #include <sys/epoll.h>
// #define BUF_SIZE 100
// #define EPOLL_SIZE 50

// int main(int argc, char *argv[])
// {
// 	int serv_sock, clnt_sock;
// 	struct sockaddr_in serv_adr, clnt_adr;
// 	socklen_t adr_size;
// 	int str_len, i;
// 	char buf[BUF_SIZE];

// 	struct epoll_event *ep_events;
// 	struct epoll_event event;
// 	int epfd, event_cnt;

// 	if (argc != 2)
// 	{
// 		printf("Usage: %s <port>\n", argv[0]);
// 		return (1);
// 	}
// 	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
// 	memset(&serv_adr, 0, sizeof(serv_adr));
// 	serv_adr.sin_family = AF_INET;
// 	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
// 	serv_adr.sin_port = htons(atoi(argv[1]));

// 	if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
// 	{
// 		printf("bind() error\n");
// 		exit(1);
// 	}
// 	if (listen(serv_sock, 3) == -1)
// 	{
// 		printf("listen() error\n");
// 		exit(1);
// 	}
// 	else
// 	{
// 		printf("server socket now listen.\n");
// 	}

// 	epfd = epoll_create(EPOLL_SIZE);
// 	event.data.fd = serv_sock;
// 	epoll_ctl(epfd, EPOLL_CRL_ADD, serv_sock, &event);


// 	while (1)
// 	{
// 		evnet_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
// 		if (event_cnt == -1)
// 		{
// 			puts("epoll_wait() error");
// 			break ;
// 		}
// 		for (i = 0; i < event_cnt + 1; i++)
// 		{
// 			if (ep_events[i].data.fd == serv_sock);
// 			{
// 				adr_size = sizeof(clnt_adr);
// 				clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &adr_size);
// 				event.events = EPOLLIN;
// 				event.data.fd = clnt_sock;
// 				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
// 				printf("connected client: %d\n", clnt_sock);
// 			}
// 			else
// 			{
// 				str_len = read(ep_events[i].data.fd, BUF_SIZE);
// 				if (str_len == 0) // close request
// 				{
// 					epoll_ctl(
// 							epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
// 					close(ep_events[i].data.fd);
// 					printf("closed client: %d\n", ep_events[i].data.fd);
// 				}
// 				else
// 				{
// 					write(ep_events[i].data.fd, buf, str_len); // echo
// 				}
// 			}
// 		}
// 	}
// 	close(serv_sock);
// 	close(epfd);
// 	return (0);
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/event.h> // Include kqueue library
#define BUF_SIZE 4
#define MAX_EVENTS 50 // 새로운 상수로 이벤트 수를 나타냅니다

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t adr_size;
    int str_len, i;
    char buf[BUF_SIZE];

    int kq;
    struct kevent changelist[2], *eventlist;
    int nev;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return (1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    {
        printf("bind() error\n");
        exit(1);
    }
    if (listen(serv_sock, 3) == -1)
    {
        printf("listen() error\n");
        exit(1);
    }
    else
    {
        printf("server socket now listen.\n");
    }

    kq = kqueue();
    if (kq == -1)
    {
        perror("kqueue() error");
        exit(1);
    }

    EV_SET(&changelist[0], serv_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, changelist, 1, NULL, 0, NULL) == -1)
    {
        perror("kevent() error");
        exit(1);
    }

    // eventlist 배열 동적 할당
    eventlist = (struct kevent *)malloc(sizeof(struct kevent) * MAX_EVENTS);

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
            if (eventlist[i].ident == serv_sock)
            {
                adr_size = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_size);

                EV_SET(&changelist[0], clnt_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, changelist, 1, NULL, 0, NULL) == -1)
                {
                    perror("kevent() error");
                    exit(1);
                }

                printf("connected client: %d\n", clnt_sock);
            }
            else
            {
                str_len = read(eventlist[i].ident, buf, BUF_SIZE);
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
                    write(eventlist[i].ident, buf, str_len); // echo
                }
            }
        }
    }

    free(eventlist); // 동적으로 할당한 메모리 해제
    close(serv_sock);
    close(kq);
    return (0);
}
