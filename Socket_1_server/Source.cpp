#include <iostream>

#include <WinSock2.h>	// ������� �������
#include <WS2tcpip.h>	// ��� ������ � ip ��������

#define PORT 800	// ����
#define QUEUE_MESSAGE 0x01	// ������������ ���������� ������������ �������������� ���������� ��������

#define BUFER_SIZE 1024

#pragma comment(lib, "Ws2_32.lib")	// ����������� ������������ ����������

using std::cerr;

int main()
{
	char bufer[BUFER_SIZE];

	std::cout << "Program is runned..." << std::endl;

	if (WSAStartup(0x0202, (WSADATA*)&bufer[0]) != 0)	// �������� dll ���������� � ������ (������������)
	{
		cerr << "Error load dll";
		return -1;
	}
	
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, NULL);	// �������� ���������� ������
	if (_socket < 0)
	{
		cerr << "Error create socket";
		WSACleanup();
		return -2;
	}

	sockaddr_in local_addr;	// ��������� � ������� � �������, ���� ������������� �����
	local_addr.sin_family = AF_INET;	// ��������� ���������� (��� ������� AF_INET == ��� ip-������� v4)
	local_addr.sin_port = htons(PORT);	// ����
	local_addr.sin_addr.S_un.S_addr = 0;	// ���� � �������
	
	if (bind(_socket, (sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)	// ���������� ����� � ip-������� (����� �� ��� ��������� �������� ����������)
	{
		cerr << "Error bind socket: " << WSAGetLastError();
		closesocket(_socket);
		WSACleanup();
		return -3;
	}

	if (listen(_socket, QUEUE_MESSAGE) == SOCKET_ERROR)	// ���������� ������ � ������ �������� ���������� �� �������� (������ ��������� � ����� �������� �����������)
	{
		cerr << WSAGetLastError();
		closesocket(_socket);
		WSACleanup();
		return -4;
	}

	std::cout << "\nServer is start listen\n" << std::endl;

	SOCKET client_socket;
	sockaddr_in client_addr;
	int size_client_addr = sizeof(client_addr);

	const char* message = "Hello client!";

	while ((client_socket = accept(_socket, (sockaddr*)&client_addr, &size_client_addr)) >= 0)	// ���� ��������� ����������� �������� �� ��������
	{
		HOSTENT* hst;
		hst = gethostbyaddr((char*)&client_addr.sin_addr.S_un.S_addr, 4, AF_INET);	// ��������� ����� ������� �� ������
		std::cout << "New user connected: " << hst->h_name << " [" << inet_ntoa(client_addr.sin_addr) << "]" << std::endl;	// ������ � ������� ���� � ����� �������
#define MESS "Hello client!\n"
		std::cout << "Len data == " << sizeof(MESS) << std::endl;
		send(client_socket, MESS, 13, 0);	// �������� �������
		/*����� �������� �������� ��������� ������� � ��� ��� ������������ ����������� ��������*/
		break;
	}

	std::cout << "Server is close..." << std::endl;

	shutdown(client_socket, SD_BOTH);	// ���������� �������� ����������: � '������ -> ������',� '������ -> ������'
	closesocket(client_socket);		// ��������� �����
	WSACleanup();	// ��������� �� ������ ���������� dll

	std::cout << "\n\nServer is stopped\nProgram finished!!!" << std::endl;

	system("pause");

	return 0;
}