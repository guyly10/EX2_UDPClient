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
	int numOfFiles = 0;	
	char secondWord[255];
	int flagCount = 0;

	cout << "Please choose one of the following commands: GETAll--GET*File Name*--PUT*File Name*.\n";
	//allows to scanf input with spaces from the user
	scanf("%[^\n]", sendBuff);
	

	//splits the string into separate words. divided by space
	char *token = strtok(sendBuff, " ");
	
	while (token != NULL)
	{
		flagCount++;
		strcpy(secondWord, token);
		token = strtok(NULL, " ");
	}

	if (strcmp(sendBuff, "GET") == 0 && strcmp(secondWord, "All") == 0 && flagCount < 3)
	{		
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

		//in the for loop we know how many files we are going to receive from the server
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

	//a command which is consisted from less than 2 words and is not GET All is sent to the server
	else if (flagCount < 3)
	{		
		bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)& server, sizeof(server));
		bytesSent2 = sendto(connSocket, secondWord, (int)strlen(secondWord), 0, (const sockaddr*)& server, sizeof(server));

		if (SOCKET_ERROR == bytesSent || bytesSent2 == SOCKET_ERROR)
		{
			cout << "Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return(-1);
		}

		bytesRecv = recv(connSocket, recvBuff, 255, 0);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return(-1);
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Client: " << recvBuff << ".\n";
	}
	
	//commands which are more than 3 words are unknown commands
	else
	{
		strcpy_s(sendBuff, "Error");
		strcpy_s(secondWord, "Error");
		bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)& server, sizeof(server));
		bytesSent2 = sendto(connSocket, secondWord, (int)strlen(secondWord), 0, (const sockaddr*)& server, sizeof(server));

		if (SOCKET_ERROR == bytesSent || bytesSent2 == SOCKET_ERROR)
		{
			cout << "Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return(-1);
		}

		bytesRecv = recv(connSocket, recvBuff, 255, 0);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return(-1);
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Client: " << recvBuff << ".\n";
	}

	// Closing connections and Winsock.
	cout << "Client: Closing Connection.\n";
	closesocket(connSocket);
}