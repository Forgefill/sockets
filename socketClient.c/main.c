#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Include time.h for time measurement

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {
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

    // Measure the start time for the connection in milliseconds
    clock_t start_connection_time = clock();

    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("connect failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    // Calculate connection time in milliseconds
    clock_t end_connection_time = clock();
    double connection_time_ms = (double)(end_connection_time - start_connection_time) / (CLOCKS_PER_SEC / 1000.0);
    printf("Connection time: %.2f milliseconds\n", connection_time_ms);

    int numSends, sendSize;

    // Input the number of sends and the size of each send
    printf("Enter the number of sends: ");
    if (scanf("%d", &numSends) != 1) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    printf("Enter the size of each send: ");
    if (scanf("%d", &sendSize) != 1) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    char* sendData = (char*)malloc(sendSize);
    if (sendData == NULL) {
        perror("Memory allocation error");
        return 1;
    }

    // Prepare data to send
    for (int i = 0; i < sendSize; i++) {
        sendData[i] = 'A' + (i % 26);
    }

    // Measure time before sending in milliseconds
    clock_t start_send_time = clock();

    // Send data to the server
    for (int i = 0; i < numSends; i++) {
        iResult = send(ConnectSocket, sendData, sendSize, 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
    }

    // Measure time after sending in milliseconds
    clock_t end_send_time = clock();

    double time_elapsed_ms = (double)(end_send_time - start_send_time) / (CLOCKS_PER_SEC / 1000.0);
    double packets_per_second = numSends / (time_elapsed_ms / 1000.0);
    double bytes_per_second = (numSends * sendSize) / (time_elapsed_ms / 1000.0);

    printf("Time to send all data: %.3f seconds\n", time_elapsed_ms / 1000.0);
    printf("Packets per second: %.2f\n", packets_per_second);
    printf("Bytes per second: %.2f\n", bytes_per_second);

    printf("Sent %d sends of %d bytes each.\n", numSends, sendSize);

    // Shutdown the connection for sending
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Close the socket and clean up Winsock
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
