#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TCP_IP "localhost"  // IP address to listen on
#define TCP_PORT 9999       // Port number to listen on

void receive_data() {
    int sock_fd, conn_fd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[1024];

    // Create a TCP socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(TCP_PORT);

    // Bind the socket to the server address
    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    listen(sock_fd, 1);
    clilen = sizeof(cli_addr);

    // Accept an incoming connection
    if ((conn_fd = accept(sock_fd, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Read data from the TCP socket and print it to the console
    while (1) {
        int bytes_read = recv(conn_fd, buffer, sizeof(buffer), 0);
        if (bytes_read < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        } else if (bytes_read == 0) {
            printf("Connection closed by client.\n");
            break;
        } else {
            buffer[bytes_read] = '\0';
            printf("Received message: %s\n", buffer);
        }
    }

    // Close the TCP socket
    close(conn_fd);
    close(sock_fd);
}

int main() {
    receive_data();
    return 0;
}
