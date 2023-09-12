#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(const char *message);

int main(int argc, char *argv[])
{
	int sock;
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
		puts("*** Start downloading file ***");
	
    FILE *fp = fopen("download_test.txt", "wb");
    if (fp == NULL)
    {
        std::cerr << "open error" << std::endl;
        exit (1);
    }
    char byteBuffer[65536];
    int nRecv;
    /**
     *  압축파일은 내부적으로 crc를 사용하는데, 파일에 대한 해시값을 같이 보내는 것이 정석이다. 
     **/
	while ((nRecv = ::recv(sock, byteBuffer, 1024, 0)) > 0)
	{
        fwrite(byteBuffer, nRecv, 1, fp);
        putchar('#');
	}
	shutdown(sock, SHUT_RDWR);
	close(sock);
    std::cout << "download conplete." << std::endl;
	return (0);
}

void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}