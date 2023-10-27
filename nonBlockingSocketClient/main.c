#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

int sendDataToServer(SOCKET ConnectSocket, char* sendData, int numSends, int sendSize) {
    for (int i = 0; i < numSends; i++) {
        int iResult = send(ConnectSocket, sendData, sendSize, 0);
        if (iResult == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK) {
                // The socket is not ready to send, wait and try again
                i--;
                continue;
            } else {
                printf("send failed: %d\n", err);
                return -1;
            }
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        fprintf(stderr, "Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "connect failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    // Make the socket non-blocking
    u_long mode = 1;
    ioctlsocket(ConnectSocket, FIONBIO, &mode);

    int numSends, sendSize;

    printf("Enter the number of sends: ");
    if (scanf("%d", &numSends) != 1) {
        printf("Invalid input. Exiting.\n");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Enter the size of each send: ");
    if (scanf("%d", &sendSize) != 1) {
        printf("Invalid input. Exiting.\n");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    char* sendData = (char*)malloc(sendSize);
    if (sendData == NULL) {
        perror("Memory allocation error");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    for (int i = 0; i < sendSize; i++) {
        sendData[i] = 'A' + (i % 26);
    }

    clock_t start_send_time = clock();
    if (sendDataToServer(ConnectSocket, sendData, numSends, sendSize) == -1) {
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    clock_t end_send_time = clock();

    double time_elapsed_ms = (double)(end_send_time - start_send_time) / (CLOCKS_PER_SEC / 1000.0);
    double packets_per_second = numSends / (time_elapsed_ms / 1000.0);
    double bytes_per_second = (numSends * sendSize) / (time_elapsed_ms / 1000.0);

    printf("Time to send all data: %.3f seconds\n", time_elapsed_ms / 1000.0);
    printf("Packets per second: %.2f\n", packets_per_second);
    printf("Bytes per second: %.2f\n", bytes_per_second);

    printf("Sent %d sends of %d bytes each.\n", numSends, sendSize);

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "shutdown failed: %d\n", WSAGetLastError());
    }

    closesocket(ConnectSocket);
    WSACleanup();

    free(sendData);

    return 0;
}
