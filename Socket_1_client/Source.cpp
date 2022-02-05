#include <iostream>

#include <WinSock2.h>	// Функции сокетов
#include <WS2tcpip.h>	// Для работы с ip адресами

#define PORT 800	// порт
#define ADDRESS__SERVER "127.0.0.1"	// Адрес сервера (в дальнейшем можно брать как параметр)

#define BUFER_SIZE 1024

#pragma comment(lib, "Ws2_32.lib")	// Необходимая диначимеская библиотека

using std::cerr;

int main()
{
	char bufer[BUFER_SIZE];

	std::cout << "Program is start..." << std::endl;

	if (WSAStartup(0x0202, (WSADATA*)&bufer[0]) != 0)	// Загрузка dll библиотеки в память (инициалиация)
	{
		cerr << "Error load dll";
		system("pause");
		return -1;
	}

	SOCKET _socket = socket(AF_INET, SOCK_STREAM, NULL);	// Создание экземпляра сокета. Параметры: AF_INET == для ip-адресов v4, SOCK_STREAM == связь с установлением соединения, NULL == параметры используемого протокола по умолчанию.
	if (_socket < 0)
	{
		cerr << "Error create socket";
		WSACleanup();
		system("pause");
		return -2;
	}

	sockaddr_in server_addr;	// Экземпляр с данными о сервере, ниже инициализация полей (указание адреса и порта сервера)
	server_addr.sin_family = AF_INET;	// семейство протоколов (как правило AF_INET == для ip-адресов v4)
	server_addr.sin_port = htons(PORT);	// порт
	server_addr.sin_addr.S_un.S_addr = 0;	// поле с адресом

	HOSTENT *hst;

	if (inet_addr(ADDRESS__SERVER) != INADDR_NONE)	// Если адрес сервера задан в виде строки с ip, то преоразуем его в цифровой формат
	{
		server_addr.sin_addr.S_un.S_addr = inet_addr(ADDRESS__SERVER);
	}
	else	// Если ошибка, то  либо адрес неверный, либо там домен. Пробуем преобразовать домен в ip адрес сервера в вид строки
	{
		hst = gethostbyname(ADDRESS__SERVER);
		if (hst != nullptr)	// Если успешно то забираем его
		{
			//server_addr.sin_addr.S_un.S_addr = inet_addr(hst->h_name);
			(unsigned long)server_addr.sin_addr.S_un.S_addr = ((unsigned long**)hst->h_addr_list)[0][0];
		}
		else	// Ошибка - адрес неверный
		{
			cerr << GetLastError();
			closesocket(_socket);
			WSACleanup();
			system("pause");
			return -3;
		}
	}

	std::cout << "Client try connecting..." << std::endl;

	if (connect(_socket, (sockaddr*)&server_addr, sizeof(server_addr)) != 0)	// Коннектимся с сервером
	{
		cerr << GetLastError();
		closesocket(_socket);
		WSACleanup();
		system("pause");
		return -4;
	}

	std::cout << "\nConnected to: " << inet_ntoa(server_addr.sin_addr) << std::endl << std::endl << std::endl;

	int len_message = recv(_socket, bufer, sizeof(bufer), 0);	// Получение данных с сервера + еще хорошо бы проверку (пример проверки есть в файле с реализацией сервера)

	std::cout << "Data from server == " << bufer << std::endl;
	std::cout << "Len data == " << len_message << std::endl;
	
	strcpy_s(bufer, "Hello, server!");
	//std::cout << bufer << std::endl;
	send(_socket, bufer, strlen(bufer), 0);		// Отправка данных серверу

	std::cout << "Client is close..." << std::endl;

	shutdown(_socket, SD_BOTH);		// Выборочное закрытие соединений: и 'сервер -> клиент',и 'клиент -> сервер'
	closesocket(_socket);		// Закрываем сокет
	WSACleanup();	// Выгружаем из памяти библиотеку dll

	std::cout << "Client is stopped\nProgram finished!!!" << std::endl;

	system("pause");

	return 0;
}