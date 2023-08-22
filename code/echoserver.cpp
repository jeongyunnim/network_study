#include <iostream>
#include <cstdlib>
#include <unistd.h>
/* win32 대체 */
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, std::string argv[])
{
    //1. 접속대기 소켓 생성 hSocket == socket handle
    int hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    /*
        [1] AF == address family -> L3가 IP Protocol이라는 것.
        [2] SOCK_STREAM -> 이 소켓에서의 데이터 단위가 stream이다. == L4가 TCP 이다.
        [3] 이 2가지 정보로 이미 TCP/IP 프로토콜이라는 것을 알았기 때문에 0을 준다.
    */

    return (0);
}