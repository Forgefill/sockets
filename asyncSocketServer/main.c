#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#pragma comment(lib, "Ws2_32.lib")

int nextClientID = 1; // Counter for assigning unique IDs to clients

// Define a structure to store per-client data
typedef struct {
    SOCKET clientSocket;
    HANDLE completionPort;
    int clientID;
} ClientData;

DWORD WINAPI HandleClient(LPVOID lpParam) {
    ClientData* clientData = (ClientData*)lpParam;
    SOCKET clientSocket = clientData->clientSocket;
    HANDLE completionPort = clientData->completionPort;
    int clientID = clientData->clientID;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int num_packets = 0;
    int total_bytes = 0;

    printf("Client %d connected.\n", clientID);

    while (1) {
        int iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            num_packets++;
            total_bytes += iResult;
        } else if (iResult == 0) {
            printf("Client %d disconnected.\n", clientID);
            break;
        } else {
            printf("recv failed for Client %d: %d\n", clientID, WSAGetLastError());
            break;
        }
    }

    closesocket(clientSocket);
    free(clientData);

    return 0;
}

int main() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a socket for the server
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Bind the socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    // Listen for incoming connections
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    while (1) {
        // Accept a client socket
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        // Create a structure to store client data
        ClientData* clientData = (ClientData*)malloc(sizeof(ClientData));
        clientData->clientSocket = ClientSocket;
        clientData->completionPort = NULL; // You can set the completion port if needed
        clientData->clientID = nextClientID++; // Assign a unique ID to the client

        // Create a new thread to handle the client
        HANDLE hThread = CreateThread(NULL, 0, HandleClient, clientData, 0, NULL);
        if (hThread == NULL) {
            printf("Failed to create a client handling thread\n");
            closesocket(ClientSocket);
            free(clientData);
        }
    }

    // Cleanup and close the listening socket (not reached in this code)
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
