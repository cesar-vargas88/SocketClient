// TCP-Client.cpp : Defines the entry point for the console application.

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 1024

#include <fstream>
#include <ctime>
#include <string>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <string>

#include <chrono>
#include <thread>
#include <iostream>

std::string const path = "C:\\Users\\varga\\source\\repos\\iMSTK\\Examples\\Aescape\\massages\\";
std::string const massageName = "massage1";

int __cdecl main(int argc, char** argv)
{

#pragma region Load CSV file

    std::vector<std::string> lines;

    std::ifstream myFile(path + massageName + ".csv");

    if (myFile.is_open())
    {
		std::cout << "Loading the file" << std::endl;

		std::string line;
		getline(myFile, line);

        while (getline(myFile, line))
			lines.push_back(line);

        std::cout << "Closing the file" << std::endl;

        myFile.close();
    }
    else
        std::cout << "Unable to open the file" << std::endl;

#pragma endregion

	std::vector<std::string> log;

	for (int i = 0; i < lines.size(); i++) {
		std::cout << "\n\n**************************\n*    CLIENT    *\n**************************\n\n";

		//Initialize Winsock
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			std::cout << "WSAStartup Failed with error: " << iResult << std::endl;
			return 1;
		}
		std::cout << "Initialize Winsock" << std::endl;

		//Create a SOCKET for listening for incoming connections request
		SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET) {
			std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}
		std::cout << "Create a SOCKET for listening for incoming connections request" << std::endl;

		// The sockaddr_in structure specifies the address family,
		// IP address, and port for the socket that is being bound.
		sockaddr_in addrServer;
		addrServer.sin_family = AF_INET;
		InetPton(AF_INET, "127.0.0.1", &addrServer.sin_addr.s_addr);
		addrServer.sin_port = htons(6666);
		memset(&(addrServer.sin_zero), '\0', 8);

		// Connect to server.
		std::cout << "Connecting..." << std::endl;
		iResult = connect(ConnectSocket, (SOCKADDR*)&addrServer, sizeof(addrServer));
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			std::cout << "Unable to connect to server: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}
		std::cout << "Connect to server" << std::endl;

		//variables
		char messageToSend[DEFAULT_BUFLEN] = { 0 };

		for (int j = 0; j < lines[i].size(); j++)
			messageToSend[j] = lines[i][j];
	
		const char* sendbuf = messageToSend;
		char messageReceived[DEFAULT_BUFLEN];
		int messageReceivedLength = DEFAULT_BUFLEN;

		// Send file name
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		std::cout << "Bytes Sent: " << iResult << std::endl;
		//std::cout << "Message Sent: " << lines[i] << std::endl;

		// Receive until the peer closes the connection
		do {
			iResult = recv(ConnectSocket, messageReceived, messageReceivedLength, 0);

			if (iResult > 0) {

				std::wcout << "Bytes received: " << iResult << std::endl;

				//Create string
				std::string inputString;

				for (int i = 0; i < iResult; i++)
					inputString += messageReceived[i];

				std::cout << "Message received: " << inputString << std::endl;	

				log.push_back(inputString);
			}
			else if (iResult == 0)
				std::cout << "Connection closed" << std::endl;
			else
				std::cout << "recv failed with error: " << WSAGetLastError();

		} while (iResult > 0);

		// shutdown the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		std::cout << "Shutdown the connection since no more data will be sent" << std::endl;

		// cleanup
		closesocket(ConnectSocket);
		WSACleanup();
	}

	return 0;
}


/*

while (true) {

        if (!thNewPosition) {

            std::cout << "\n\n********************************\n*    Socket server callback    *\n********************************\n\n";

            //Initialize Winsock
            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != NO_ERROR) {
                std::cout << "WSAStartup failed with error: " << iResult << std::endl;
                return;
            }
            std::cout << "Initialize Winsock" << std::endl;

            //Create a SOCKET for listening for incoming connections request
            SOCKET ServerSocket, ClientSocket;
            ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (ServerSocket == INVALID_SOCKET) {
                std::cout << "Socket failed with error: " << WSAGetLastError() << std::endl;
                WSACleanup();
                return;
            }
            std::cout << "Create a SOCKET for listening for incoming connections request" << std::endl;

            //The sockaddr_in structure specifies the address family,
            //IP address, and port for the socket that is being bound
            sockaddr_in addrServer;
            addrServer.sin_family = AF_INET;
            InetPton(AF_INET, ipAddress, &addrServer.sin_addr.s_addr);
            addrServer.sin_port = htons(port);
            memset(&(addrServer.sin_zero), '\0', 8);

            //Bind socket
            if (bind(ServerSocket, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR) {
                std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
                closesocket(ServerSocket);
                WSACleanup();
                return;
            }
            std::cout << "Bind socket" << std::endl;

            //Listen for incomin connection requests on the created socket
            if (listen(ServerSocket, 5) == SOCKET_ERROR) {
                std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
                closesocket(ServerSocket);
                WSACleanup();
                return;
            }
            std::cout << "Listen for incomin connection requests on the created socket" << std::endl;

            // Accept a client socket
            ClientSocket = accept(ServerSocket, NULL, NULL);
            if (ClientSocket == INVALID_SOCKET) {
                std::cout << "Accept failed with error: " << WSAGetLastError() << std::endl;
                closesocket(ServerSocket);
                WSACleanup();
                return;
            }
            std::cout << "Accept a client socket" << std::endl;

            closesocket(ServerSocket);

            //Variables for recieve
            int iSendResult;
            char messageReceived[DEFAULT_BUFLEN];
            int messageReceivedLength = DEFAULT_BUFLEN;

            // Receive until the peer shuts down the connection
            iResult = recv(ClientSocket, messageReceived, messageReceivedLength, 0);

            if (iResult > 0) {

                std::wcout << "Bytes received: " << iResult << std::endl;

                //Create string
                std::string inputString;

                for (int i = 0; i < iResult; i++)
                    inputString += messageReceived[i];

                std::cout << "Message received: " << inputString << std::endl;

#pragma region Parse message and send Log

                thNewPosition = parsePositionMessage(inputString,
                    thTpLeftPosition,
                    thTpLeftRotation,
                    thTpLeftHumanTraingleID,
                    thTpLeftForceZ,
                    thTpRightPosition,
                    thTpRightRotation,
                    thTpRightHumanTraingleID,
                    thTpRightForceZ);

                if (thNewPosition)
                    std::cout << "New postion done" << std::endl;
                else
                    std::cout << "Position parsing error" << std::endl;

                thWaitingLog = true;

                while (!thLogReady)
                    std::this_thread::sleep_for(50ms);

                //Convert to char array
                std::string outputString = thLogMessage;

#pragma endregion

                char messageToSend[DEFAULT_BUFLEN];

                for (int i = 0; i < outputString.length(); i++)
                    messageToSend[i] = outputString[i];

                const char* sendbuf = messageToSend;

                // Send response to client
                iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
                if (iSendResult == SOCKET_ERROR) {
                    std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;
                    closesocket(ClientSocket);
                    WSACleanup();
                    return;
                }
                std::cout << "Bytes sent: " << iSendResult << std::endl;
                //std::cout << "Message sent: " << messageToSend << std::endl;
            }
            else if (iResult == 0)
                std::cout << "Connection closing" << std::endl;
            else {
                std::cout << "Recieve failed with error: " << WSAGetLastError() << std::endl;
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }

            std::cout << "Connection closing" << std::endl;

            //Shutdown the connection since we're done
            iResult = shutdown(ClientSocket, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                printf("shutdown failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }
            std::cout << "Connection closed" << std::endl;

            // cleanup
            closesocket(ClientSocket);
            WSACleanup();
        }

        std::this_thread::sleep_for(50ms);



*/