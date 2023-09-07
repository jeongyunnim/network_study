#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int clnt_sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr;
    FILE *readfp;
    FILE *writefp;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return (1);
    }
    clnt_sock = socket(AF_INET, SOCK_STREAM, 0); // AF_INET과 PF_INET은 같은 값을 가진다.
    if (clnt_sock == -1)
        error_handling("sock() error");
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = (atoi(argv[1]));
    if (connect(clnt_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");
    else   
        puts("connected with server\b");
    readfp = fdopen(clnt_sock, "r");
    writefp = fdopen(clnt_sock, "w");
    while (1)
    {
        fgets(message, BUF_SIZE, stdin);
        if (strcmp(message, "Q\n") == 0)
            break ;
        fputs(message, writefp);
        fflush(writefp);
        fgets(message, BUF_SIZE, readfp);
        printf("from server: %s", message);
    }
    fclose(readfp);
    fclose(writefp);
    close(clnt_sock);
    return (0);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit (1);
}