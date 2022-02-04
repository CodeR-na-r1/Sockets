#include <iostream>

#include <WinSock2.h>	// ������� �������
#include <WS2tcpip.h>	// ��� ������ � ip ��������

#define PORT 800	// ����
#define ADDRESS__SERVER "127.0.0.1"	// ����� ������� (� ���������� ����� ����� ��� ��������)

#define BUFER_SIZE 1024

#pragma comment(lib, "Ws2_32.lib")	// ����������� ������������ ����������

using std::cerr;

int main()
{
	char bufer[BUFER_SIZE];

	std::cout << "Program is start..." << std::endl;

	if (WSAStartup(0x0202, (WSADATA*)&bufer[0]) != 0)	// �������� dll ���������� � ������ (������������)
	{
		cerr << "Error load dll";
		system("pause");
		return -1;
	}

	SOCKET _socket = socket(AF_INET, SOCK_STREAM, NULL);	// �������� ���������� ������
	if (_socket < 0)
	{
		cerr << "Error create socket";
		WSACleanup();
		system("pause");
		return -2;
	}

	sockaddr_in server_addr;	// ��������� � ������� � �������, ���� ������������� ����� (�������� ������ � ����� �������)
	server_addr.sin_family = AF_INET;	// ��������� ���������� (��� ������� AF_INET == ��� ip-������� v4)
	server_addr.sin_port = htons(PORT);	// ����
	server_addr.sin_addr.S_un.S_addr = 0;	// ���� � �������

	HOSTENT *hst;

	if (inet_addr(ADDRESS__SERVER) != INADDR_NONE)
	{
		server_addr.sin_addr.S_un.S_addr = inet_addr(ADDRESS__SERVER);
	}
	else
	{
		hst = gethostbyname(ADDRESS__SERVER);
		if (hst != nullptr)
		{
			//server_addr.sin_addr.S_un.S_addr = inet_addr(hst->h_name);
			(unsigned long)server_addr.sin_addr.S_un.S_addr = ((unsigned long**)hst->h_addr_list)[0][0];
		}
		else
		{
			cerr << GetLastError();
			closesocket(_socket);
			WSACleanup();
			system("pause");
			return -3;
		}
	}

	std::cout << "Client try connecting..." << std::endl;

	if (connect(_socket, (sockaddr*)&server_addr, sizeof(server_addr)) != 0)
	{
		cerr << GetLastError();
		closesocket(_socket);
		WSACleanup();
		system("pause");
		return -4;
	}

	std::cout << "\nConnected to: " << inet_ntoa(server_addr.sin_addr) << std::endl << std::endl << std::endl;

	int len_message = recv(_socket, bufer, sizeof(bufer), 0);

	std::cout << "Data from server == " << bufer << std::endl;
	std::cout << "Len data == " << len_message << std::endl;

	std::cout << "Client is close..." << std::endl;

	shutdown(_socket, SD_BOTH);		// ���������� �������� ����������: � '������ -> ������',� '������ -> ������'
	closesocket(_socket);		// ��������� �����
	WSACleanup();	// ��������� �� ������ ���������� dll

	std::cout << "Client is stopped\nProgram finished!!!" << std::endl;

	system("pause");

	return 0;
}