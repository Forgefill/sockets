#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#define MAX_CLIENTS 3

void HandleData(SOCKET clientSocket, char* data, int dataSize) {
    // Handle received data from the client
    printf("Data from client received\n");
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
        fprintf(stderr, "getaddrinfo failed\n");
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        fprintf(stderr, "Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        fprintf(stderr, "bind failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        fprintf(stderr, "listen failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    SOCKET clientSockets[MAX_CLIENTS] = {INVALID_SOCKET};
    int numClients = 0;

    while (1) {
        // Check for incoming connections and handle connected clients
        for (int i = 0; i < numClients; ++i) {
            SOCKET ClientSocket = clientSockets[i];
            if (ClientSocket == INVALID_SOCKET) continue;

            char recvbuf[DEFAULT_BUFLEN];
            int recvbuflen = DEFAULT_BUFLEN;

            int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0) {
                HandleData(ClientSocket, recvbuf, iResult);
            } else if (iResult == 0) {
                // Connection closed by the client
                printf("Client disconnected\n");
                closesocket(ClientSocket);
                clientSockets[i] = INVALID_SOCKET;
            } else {
                int err = WSAGetLastError();
                if (err != WSAEWOULDBLOCK) {
                    // Handle other socket errors
                    printf("recv failed: %d\n", err);
                    closesocket(ClientSocket);
                    clientSockets[i] = INVALID_SOCKET;
                }
            }
        }

        // Accept new client connections
        if (numClients < MAX_CLIENTS) {
            SOCKET NewClientSocket = accept(ListenSocket, NULL, NULL);
            if (NewClientSocket != INVALID_SOCKET) {
                // Make the new socket non-blocking
                u_long mode = 1;
                ioctlsocket(NewClientSocket, FIONBIO, &mode);

                // Add the new client socket to the array
                clientSockets[numClients] = NewClientSocket;
                numClients++;
                printf("New client connected\n");
            }
        }
    }

    // Clean up and close the listening socket
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
