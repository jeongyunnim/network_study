#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void)
{
    //1. 클라이언트 소켓 생성
    /**
     * 클라이언트는 OS가 임의로 TCP port를 열어준다.
     */
    int hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (hSocket == -1)
    {
        std::cout << "Error: client socket" << std::endl;
        return (1);
    }
    //2. 포트 바인딩 및 연결
    sockaddr_in serveraddr = {0};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(25000);
    if (inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr) <= 0)
    {
        std::cout << "Error: invalid server ID: " << serveraddr.sin_addr.s_addr << std::endl;
        return (1);
    }
    if (::connect(hSocket, (sockaddr*)&serveraddr, sizeof(serveraddr)) == -1)
    {
        std::cout << "Error: can not connect with server" << std::endl;
        return (1);
    }

    //3. chatting massage send/recv
    char szBuffer[128] = {0};
    while(1)
    {
        std::cin >> szBuffer;
        if (strcmp(szBuffer, "EXIT") == 0)
            break ;
        ::send(hSocket, szBuffer, strlen(szBuffer) + 1, 0);
        memset(szBuffer, 0, sizeof(szBuffer));
        ::recv(hSocket, szBuffer, sizeof(szBuffer), 0);
        std::cout << "From server: " << szBuffer << std::endl;
    }
    std::cout << "close server" << std::endl;
    shutdown(hSocket, SHUT_RDWR);
    close(hSocket);
    return (0);
}