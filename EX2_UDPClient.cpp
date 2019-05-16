#include <iostream>
using namespace std;
// Don't forget to include "ws2_32.lib" in the library list.
#include <winsock2.h> 
#include <Ws2tcpip.h>
#include <string.h>
#pragma warning(disable : 4996)

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Error: no server port provided\n");
		exit(-1);

	}

	int SERVER_PORT = atoi(argv[1]);
	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Client: Error at WSAStartup()\n";
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return(-1);
	}

	// For a client to communicate on a network, it must connect to a server.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	//server.sin_addr.s_addr = inet_addr("127.0.0.1");
	InetPton(AF_INET, L"127.0.0.1", &server.sin_addr.s_addr);
	server.sin_port = htons(SERVER_PORT);

	// Send and receive data.

	int bytesSent = 0;
	int bytesSent2 = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];
	char command[255];
	bool flag = true;
	int numOfFiles = 0;
	int getCount = 0;
	char get[] = "GET ";
	char secondWord[255];
	int flagCount = 0;

	cout << "Please choose one of the following commands: GETAll--GET*File Name*--PUT*File Name*.\n";
	scanf("%[^\n]", sendBuff);
	//cin >> sendBuff >> secondWord;

	char *token = strtok(sendBuff, " ");
	
	while (token != NULL)
	{
		flagCount++;
		strcpy(secondWord, token);
		token = strtok(NULL, " ");
	}

	if (strcmp(sendBuff, "GET") == 0 && strcmp(secondWord, "All") == 0 && flagCount < 3)
	{
		// Asks the server what's the currnet time.
		// The send function sends data on a connected socket.
		// The buffer to be sent and its size are needed.
		// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
		// The two last arguments hold the details of the server to communicate with. 
		// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
		bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)& server, sizeof(server));
		bytesSent2 = sendto(connSocket, secondWord, (int)strlen(secondWord), 0, (const sockaddr*)& server, sizeof(server));

		if (SOCKET_ERROR == bytesSent || SOCKET_ERROR == bytesSent2)
		{
			cout << "Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return(-1);
		}		

		// Gets the server's answer using simple recieve (no need to hold the server's address).
		bytesRecv = recv(connSocket, recvBuff, 255, 0);

		numOfFiles = recvBuff[0];

		for (int i = 0; i < numOfFiles; i++)
		{
			bytesRecv = recv(connSocket, recvBuff, 255, 0);
			if (SOCKET_ERROR == bytesRecv)
			{
				cout << "Client: Error at recv(): " << WSAGetLastError() << endl;
				closesocket(connSocket);
				WSACleanup();
				return(-1);
			}

			recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
			if (strcmp(recvBuff, ".") == 0 || strcmp(recvBuff, "..") == 0)
			{
				continue;
			}	
			cout << "Client: Recieved: " << recvBuff << ".\n";
		}
	}

	else if (strcmp(sendBuff, "GET") == 0 && flagCount < 3)
	{
		// Asks the server what's the currnet time.
		// The send function sends data on a connected socket.
		// The buffer to be sent and its size are needed.
		// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
		// The two last arguments hold the details of the server to communicate with. 
		// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
		bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)& server, sizeof(server));
		bytesSent2 = sendto(connSocket, secondWord, (int)strlen(secondWord), 0, (const sockaddr*)& server, sizeof(server));

		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return(-1);
		}

		bytesRecv = recv(connSocket, recvBuff, 255, 0);

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Client: " << recvBuff << ".\n";
	}
	
	else if (strcmp(sendBuff, "PUT") == 0 && flagCount < 3)
	{
		cout << "Not Implemented yet.\n";
	}

	else
	{
		cout << "500: Unknown Error.\n";
	}


	// Closing connections and Winsock.
	cout << "Client: Closing Connection.\n";
	closesocket(connSocket);
}