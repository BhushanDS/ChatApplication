#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <tchar.h>
#pragma comment(lib,"ws2_32.lib")


/// intialize winsock
/// create socket
/// connect to server
/// rend/recv
/// close the socket

bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMessages(SOCKET s)
{
	std::cout << "Enter Your Chat Name : ";
	std::string name;
	std::getline(std::cin,name);
	std::string msg;
	while (1)
	{
		std::getline(std::cin, msg);
		msg += " : " + name;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);

		if (bytesent == SOCKET_ERROR)
		{
			std::cout << "Error Sending msg \n";
			break;
		}

		//write condition for quite
	}
	closesocket(s);
	WSACleanup();
}

void ReceiveMessages(SOCKET s)
{
	char buffer[4096];
	int revlen = 0;
	std::string msg;
	while (1)
	{
		int bytrecv = recv(s, buffer, sizeof(buffer), 0);
		if (bytrecv == SOCKET_ERROR)
		{
			std::cout << "Disconnected from the server \n";
			break;
		}
		else
		{
			msg = std::string(buffer, bytrecv);
			std::cout << msg << "\n";
		}
	}
	closesocket(s);
	WSACleanup();
}

int main()
{

	if (!Initialize())
		return 1;

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET)
		return 1;

	int port = 12345;
	std::string serveraddr1 = "127.0.0.1";
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	inet_pton(AF_INET, serveraddr1.c_str(), &serveraddr.sin_addr);

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		closesocket(s);
		WSACleanup();
		return 1;
	}
	std::cout << "Successfully connected to server \n";

	std::thread senderThread(SendMessages,s);
	std::thread receiverThread(ReceiveMessages,s);

	senderThread.join();
	receiverThread.join();

	return 0;
}