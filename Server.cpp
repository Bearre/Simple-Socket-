#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")


int main()
{
	
	//Key constants
	const char IP_SERV[] = "";			// Enter local Server IP address
	const uint16_t PORT_NUM = 0;			// Enter Open working server port
	const short BUFF_SIZE = 1024;			// Maximum size of buffer for exchange info between server and client

	
	int erStat;					// Keeps socket errors status

	//IP in string format to numeric format for socket functions
	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);
	
	if (erStat <= 0) {
		std::cout << "Error in IP translation to special numeric format" << '\n';
		return 1;
	}
	

	// WinSock initialization
	WSADATA wsData;
		
	erStat = WSAStartup(MAKEWORD(2,2), &wsData);
	
	if ( erStat != 0 ) {
		std::cout << "Error WinSock version initializaion #";
		std::cout << WSAGetLastError();
		return 1;
	} else {
		std::cout << "WinSock initialization is OK" << '\n';
	}
	
	// Server socket initialization
	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << '\n'; 
		closesocket(ServSock);
		WSACleanup();
		return 1;
	} else {
		std::cout << "Server socket initialization is OK" << '\n';
	}

	// Server socket binding
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure
				
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;	
	servInfo.sin_port = htons(PORT_NUM);

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if ( erStat != 0 ) {
		std::cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		WSACleanup();
		return 1;
	} else {
		std::cout << "Binding socket to Server info is OK" << '\n';
	}
	
	//Starting to listen to any Clients
	erStat = listen(ServSock, SOMAXCONN);

	if ( erStat != 0 ) {
		std::cout << "Can't start to listen to. Error # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Listening..." << '\n';
	}

	//Client socket creation and acception in case of connection
	sockaddr_in clientInfo; 
	ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) {
		std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	} else {
		std::cout << "Connection to a client established successfully" << '\n';
		char clientIP[22];

		inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);	 	  // Convert connected client's IP to standard string format

		std::cout << "Client connected with IP address " << clientIP << '\n';

	}

	std::vector<char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);				 // Creation of buffers for sending and receiving data
	short packet_size = 0;									 // The size of sending / receiving packet in bytes
	
	
	// Receiving packet from client. Program is waiting (system pause) until receive
	while (true) {
		packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);		 
		std::cout << "Client's message: " << servBuff.data() << '\n'; 

		cout << "Your (host) message: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		// Check whether server would like to stop chatting 
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(ClientConn, SD_BOTH);
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			return 0;
		}

		packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << '\n';
			closesocket(ServSock);
			closesocket(ClientConn);
			WSACleanup();
			return 1;
		}

	}

	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();

	return 0;

}
