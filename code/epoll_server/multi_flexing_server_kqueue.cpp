#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
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
