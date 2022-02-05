#include <iostream>

#include <WinSock2.h>	// Функции сокетов
#include <WS2tcpip.h>	// Для работы с ip адресами

#define PORT 800	// порт
#define QUEUE_MESSAGE 0x01	// ограничивает количество одновременно обрабатываемых соединений сервером

#define BUFER_SIZE 1024

#pragma comment(lib, "Ws2_32.lib")	// Необходимая диначимеская библиотека

using std::cerr;

int main()
{
	char bufer[BUFER_SIZE];

	std::cout << "Program is runned..." << std::endl;

	if (WSAStartup(0x0202, (WSADATA*)&bufer[0]) != 0)	// Загрузка dll библиотеки в память (инициалиация)
	{
		cerr << "Error load dll";
		return -1;
	}
	
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, NULL);	// Создание экземпляра сокета
	if (_socket < 0)
	{
		cerr << "Error create socket";
		WSACleanup();
		return -2;
	}

	sockaddr_in local_addr;	// Экземпляр с данными о сервере, ниже инициализация полей
	local_addr.sin_family = AF_INET;	// семейство протоколов (как правило AF_INET == для ip-адресов v4)
	local_addr.sin_port = htons(PORT);	// порт
	local_addr.sin_addr.S_un.S_addr = 0;	// поле с адресом
	
	if (bind(_socket, (sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)	// Связывание порта с ip-фдресом (чтобы он мог принимать входящие соединения)
	{
		cerr << "Error bind socket: " << WSAGetLastError();
		closesocket(_socket);
		WSACleanup();
		return -3;
	}

	if (listen(_socket, QUEUE_MESSAGE) == SOCKET_ERROR)	// Подготовка сокета к приему входящих соединений от клиентов (сервер переходит в режим ожидания подключений)
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

	while ((client_socket = accept(_socket, (sockaddr*)&client_addr, &size_client_addr)) >= 0)	// Цикл обработки поступающих запросов от клиентов
	{
		HOSTENT* hst;
		hst = gethostbyaddr((char*)&client_addr.sin_addr.S_un.S_addr, 4, AF_INET);	// Получение имени клиента по адресу
		std::cout << "New user connected: " << hst->h_name << " [" << inet_ntoa(client_addr.sin_addr) << "]" << std::endl;	// Выввод в консоль инфы о новом клиенте
		std::cout << "Len data == " << strlen(message) << std::endl;
		send(client_socket, message, strlen(message), 0);	// Отправка клиенту
		/*Здесь возможно создание отдельных потоков и уже там обрабатывать поступающих клиентов*/

		int size_mess;
		while ((size_mess = recv(client_socket, bufer, sizeof(bufer), 0)) > 0)	// Условия выхода объединены в одно > 0, так как: Если < 0, то SOCKET_ERROR, иначе если == 0, то соединение было корректно закрыто
		{
			bufer[size_mess] = '\n';
			std::cout << "\nClietn send === " << bufer << std::endl;
			if (send(client_socket, bufer, size_mess, 0) <= 0)
				break;
		}
		std::cout << "\nclient is disconnected\n" << std::endl;

		break;
	}

	std::cout << "Server is close..." << std::endl;

	shutdown(client_socket, SD_BOTH);	// Выборочное закрытие соединений: и 'сервер -> клиент',и 'клиент -> сервер'
	closesocket(client_socket);		// Закрываем сокет
	WSACleanup();	// Выгружаем из памяти библиотеку dll

	std::cout << "Server is stopped\nProgram finished!!!" << std::endl;

	system("pause");

	return 0;
}