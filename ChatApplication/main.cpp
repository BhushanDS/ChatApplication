#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <tchar.h>
#include <vector>
#pragma comment(lib,"ws2_32.lib")

//Initialize winsoc lib
// Create socket
// get ip and port
// bind  ip
//listen on the socket
//accept
//rev and send
//cleanup and send

bool Initialization()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void InteractWithClient(SOCKET clientSocket, std::vector<SOCKET> &clients)
{
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Invalid client socket \n";
	}
	std::cout << "Client connected \n";

	char buffer[4096];
	while (1)
	{
		int byterecv = recv(clientSocket, buffer, sizeof(buffer), 0);
		
		if (byterecv <= 0)
		{
			std::cout << "Client dis-connected \n";
			break;
		}

		std::string msg(buffer, byterecv);
		std::cout << "Message from client : " << msg << "\n";

		for (auto client : clients)
		{
			if(client != clientSocket)
				send(client, msg.c_str(), msg.length(), 0);
		}
	}

	auto it = std::find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end())
		clients.erase(it);

	closesocket(clientSocket);
}

int main()
{
	if (!Initialization())
		return 1;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET)
		return 1;

	// create address structure
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//convert the ipadress (0.0.0.0) put it inside the sin_family in binary format
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
	{
		std::cout << "setting address structure failed \n";
	}

	//bind 
	if (bind(listenSocket, reinterpret_cast<sockaddr*> (&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed \n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "listen failed \n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Server has started listening on port "<<port<<"\n";

	//accept

	std::vector<SOCKET> clients;

	while (1)
	{
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "Invalid Socket \n";
		}

		clients.push_back(clientSocket);

		std::thread t1(InteractWithClient, clientSocket, std::ref(clients));

		t1.detach();
	}
	closesocket(listenSocket);

	WSACleanup();
	return 0;
}