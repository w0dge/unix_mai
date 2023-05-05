#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SHM_NAME "/bokundebug" // Name of the shared memory file
#define SHM_SIZE 1024           // Size of the shared memory
#define TCP_IP "localhost"      // IP address of the TCP server
#define TCP_PORT 9999           // Port number of the TCP server

void send_data() {
    int shm_fd, sock_fd;
    char* shm;
    struct sockaddr_in serv_addr;

    // Open the shared memory file for reading and writing
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory into the process address space
    shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Create a TCP socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);
    if (inet_pton(AF_INET, TCP_IP, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    // Connect to the TCP server
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    // Read and send data from the shared memory to the TCP server
    while (1) {
        char* msg = shm;
        if (*msg != '\0') {
            printf("Sending message: %s\n", msg);
            send(sock_fd, msg, strlen(msg), 0);
            *msg = '\0';
        }

        // Suspend execution for 100 milliseconds
        usleep(100 * 1000);
    }

    // Remove the mapping of the shared memory
    if (munmap(shm, SHM_SIZE) == -1) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    // Remove the shared memory file
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink failed");
        exit(EXIT_FAILURE);
    }

    // Close the TCP socket
    close(sock_fd);
}

int main() {
    send_data();
    return 0;
}
