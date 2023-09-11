#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")


void ErrorHandler(const char *pszMessage)
{
	printf("ERROR: %s\n", pszMessage);
	::WSACleanup();
	exit(1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		ErrorHandler("������ �ʱ�ȭ �� �� �����ϴ�.");

	FILE *fp = NULL;
	errno_t nResult = fopen_s(&fp, "Sleep Away.zip", "rb");
	if (nResult != 0)
		ErrorHandler("������ ������ ������ �� �����ϴ�.");

	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandler("���� ��� ������ ������ �� �����ϴ�.");

	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
		ErrorHandler("���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");

	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandler("���� ���·� ��ȯ�� �� �����ϴ�.");
	puts("���ϼ۽ż����� �����մϴ�.");

	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = ::accept(hSocket,
						(SOCKADDR*)&clientaddr, &nAddrLen);
	if (hClient == INVALID_SOCKET)
		ErrorHandler("Ŭ���̾�Ʈ ��� ������ ������ �� �����ϴ�.");
	puts("Ŭ���̾�Ʈ�� ����Ǿ����ϴ�.");

	char byBuffer[65536];
	int nRead, nSent, i = 0;
	while ((nRead = fread(byBuffer, sizeof(char), 65536, fp)) > 0)
	{
		nSent = send(hClient, byBuffer, nRead, 0);
		printf("[%04d] ���۵� ������ ũ��: %d\n", ++i, nSent);
		fflush(stdout);
	}

	::Sleep(100);

	::closesocket(hSocket);
	::closesocket(hClient);
	puts("Ŭ���̾�Ʈ ������ ������ϴ�.");

	fclose(fp);
	::WSACleanup();
	return 0;
}
