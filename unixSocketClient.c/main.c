#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "/tmp/unixSocketServer"
#define DEFAULT_BUFLEN 64

int main(int argc, char* argv[]) {
    int sockfd;
    struct sockaddr_un server_addr;
    char sendBuffer[DEFAULT_BUFLEN];
    int numSends, sendSize;

    // Measure the start time for the connection in milliseconds
    clock_t start_connection_time = clock();

    // Create a socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Initialize server_addr structure
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    // Calculate connection time in milliseconds
    clock_t end_connection_time = clock();
    double connection_time_ms = (double)(end_connection_time - start_connection_time) / (CLOCKS_PER_SEC / 1000.0);
    printf("Connection time: %.2f milliseconds\n", connection_time_ms);

    // Input the number of sends and the size of each send
    printf("Enter the number of sends: ");
    if (scanf("%d", &numSends) != 1) {
        printf("Invalid input. Exiting.\n");
        close(sockfd);
        return 1;
    }

    printf("Enter the size of each send: ");
    if (scanf("%d", &sendSize) != 1) {
        printf("Invalid input. Exiting.\n");
        close(sockfd);
        return 1;
    }

    // Prepare data to send
    memset(sendBuffer, 'A', sendSize);

    // Measure time before sending in milliseconds
    clock_t start_send_time = clock();

    // Send data to the server
    for (int i = 0; i < numSends; i++) {
        ssize_t bytesSent = send(sockfd, sendBuffer, sendSize, 0);
        if (bytesSent == -1) {
            perror("send");
            close(sockfd);
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

    // Close the socket
    close(sockfd);

    return 0;
}
