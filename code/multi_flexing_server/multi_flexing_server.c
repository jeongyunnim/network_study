#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <signal.h>
#define BUF_SIZE 100

void signal_handler(int signo)
{
	exit (123);
}

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	signal(SIGINT, signal_handler);
	socklen_t adr_size;
	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];

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
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	/**
	 * fd_max를 서버 소켓으로 초기화 하는 이유
	 * 서버 소켓도 읽어야 하니까.
	 */
	fd_max = serv_sock;

	while (1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
			break ;
		if (fd_num == 0)
			continue ;
		for (i = 0; i < fd_max + 1; i++)
		{
			if (FD_ISSET(i, &cpy_reads))
			{
				if (i == serv_sock)
				{
					adr_size = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_size);
					FD_SET(clnt_sock, &reads);
					if (fd_max < clnt_sock)
						fd_max = clnt_sock;
					printf("connected client: %d\n", clnt_sock);
				}
				else
				{
					str_len = read(i, buf, BUF_SIZE);
					if (str_len == 0)
					{
						FD_CLR(i, &reads);
						close(i);
						printf("closed client: %d\n", i);
					}
					else
					{
						write(i, buf, str_len);
						write(1, buf, str_len);
					}
				}
			}
		}
	}
	close(serv_sock);
	return (0);
}