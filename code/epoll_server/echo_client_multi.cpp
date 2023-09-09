#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	std::string buffer;
	char message[BUF_SIZE];
	int str_len, recv_len, recv_cnt;
	struct sockaddr_in serv_adr;

	if (argc != 2)
	{
		printf("Usage: %s <Port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("sock() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error");
	else
		puts("Connected..");
		
	while (1)
	{
		fputs("Input message(Q to quit): ", stdout);
		std::cin >> message;
		if (strcmp(message, "Q\n") == 0)
			exit (123);

		str_len = write(sock, message, strlen(message));
		recv_len = 0;
		while (recv_len < str_len)
		{
			recv_len = read(sock, &message[recv_len], BUF_SIZE - 1);
			if (recv_len == -1)
				error_handling("read() error");
			recv_len += recv_cnt;
		}
		message[recv_len] = 0;
		printf("Message from server: %s", message);
	} 
	close(sock);
	return (0);
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}