#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/unixSocketServer"  // Update the socket path as needed
#define DEFAULT_BUFLEN 64

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_un);

    // Create a Unix domain socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // Initialize server_addr structure
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    while (1) {
        // Accept a client connection
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            perror("accept");
            close(server_fd);
            return 1;
        }

        char recvbuf[DEFAULT_BUFLEN];
        int num_packets = 0;
        int total_bytes = 0;

        while (1) {
            ssize_t bytesReceived = recv(client_fd, recvbuf, DEFAULT_BUFLEN, 0);
            if (bytesReceived > 0) {
                num_packets++;
                total_bytes += bytesReceived;
            } else if (bytesReceived == 0) {
                printf("Connection closed\n");
                break;
            } else {
                perror("recv");
                break;
            }
        }

        printf("Client connection closed.\n");
        close(client_fd);
    }

    // Close and unlink the Unix domain socket
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
