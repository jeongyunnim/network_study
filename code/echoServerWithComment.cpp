#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(void)
{
	//1. 접속대기 소켓 생성 hSocket == socket handle
	int hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	/*
		[1] AF == address family -> L3가 IP Protocol이라는 것.
		[2] SOCK_STREAM -> 이 소켓에서의 데이터 단위가 stream이다. == L4가 TCP 이다.
		[3] 이 2가지 정보로 이미 TCP/IP 프로토콜이라는 것을 알았기 때문에 0을 준다.
	*/
	if (hSocket == -1)
	{
		std::cout << "Error: socket." << std::endl;
		return (1);
	}
	//2. 포트 바인딩
	sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	/*
	 * htons(), htonl()
	 * 기존 network 개발자들이 big endian을 사용하여 개발을 했다. 
	 * 하지만 intel mac에서는 little endian을 쓰기 때문에 이를 big endian으로 변환해주는 함수를 사용하는 것. 
	 * big endian을 사용한다면 굳이 이 함수를 호출할 필요가 없다.
	*/ 
	svraddr.sin_port = htons(25000); // host to network short 
	svraddr.sin_addr.s_addr = htonl(INADDR_ANY); // host to network long
	/**
	 * bind의 실패
	 * port 번호 오류: 다른 프로세스가 이미 해당 TCP port를 소유 중인 경우
	 * ip 오류: 사용할 수 없는 IP주소를 할당하려는 경우
	 * 
	 * port 번호가 식별하는 것.
	 * end-point 서버/클라이언트 개발자에게 port 번호는 소켓에 대한 식별자이자 프로세스를 구별하는 식별자이다.
	 * -> socket을 프로세스가 열기 때문에
	*/
	int opt = true;
	if (setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
		std::cout << "nono" << std::endl;
	}
	// if (setsockopt(hSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	// {
	// 	std::cout << "nono" << std::endl;
	// }
	if (::bind(hSocket, (sockaddr*)&svraddr, sizeof(svraddr)) == -1)
	{
		std::cout << "Error: 소켓에 IP주소와 port를 바인드할 수 없음." << std::endl;
		return (1);
	}

	//3. 접속대기 상태로 전환
	/**
	 * [1] SOCKET s
	 *		: listen 상태로 바꿔줄 소켓 핸들
	 * [2] int backlog
	 * 		: 여러 클라이언트가 동시에 서버에 접근하려고 할 수 있다. 
	 * 		서버가 listen에서 accept되고 서버 통신 소켓을 오픈하기 까지의 순간 동시에 connect 하려는 클라이언트의 요청이 무시되는 문제가 있었음.
	 * 		backlog queue -> 현재는 OS 레벨에서 모두 처리해준다(SOMAXCONN: 대기열의 최대 연결 요청 수 설정)
	 */
	if (::listen(hSocket, SOMAXCONN) == SO_ERROR)
	{
		std::cout << "Error: listen." << std::endl;
		return (1);
	}
	/*====================================================================*/

	//4. 클라이언트 접속 처리 및 대응
	/**
	 * 클라이언트와 통신할 서버 통신 소켓을 여는 과정.
	 */
	sockaddr_in clientaddr = { 0 };
	socklen_t nAddrLen = sizeof(clientaddr);
	int hClient = 0;
	char szBuffer[128] = { 0 };
	int nReceive = 0;

	//4.1 클라이언트 연결을 받아들이고 새로운 소켓 생성(개방)
	while ((hClient = ::accept(hSocket, (sockaddr*)&clientaddr, &nAddrLen)) != -1)
	{
		//4.2 클라이언트로부터 문자열을 수신
		/**
		 * @brief Construct a new while object
		 * 동기모드로 wait이 걸려있다가 데이터가 수신이 되면 반환을 하게된다.
		 * 클라이언트가 연결을 끊을 때, 소켓이 닫히고, recv가 최종적으로 0을 반환하려 해당 반복문을 빠져나온다.
		 */
		while ((nReceive = ::recv(hClient, szBuffer, sizeof(szBuffer), 0)) != -1)
		{
			//4.3 수신한 문자열을 그대로 반향전송
			::send(hClient, szBuffer, sizeof(szBuffer), 0);
			std::cout << szBuffer << std::endl;
			memset(szBuffer, 0, sizeof(szBuffer));
		}
		//4.4 클라이언트가 연결을 종료
		shutdown(hClient, SHUT_RDWR);
		close(hClient);
	}
	close(hSocket);
	return (0);
}