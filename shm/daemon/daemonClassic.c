#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define SHARED_MEM_NAME "/bokundebug" // Name of the shared memory file
#define SHARED_MEM_SIZE 1024           // Size of the shared memory

void run_daemon() {
    int fd;
    char* shared_mem;

    // Open the shared memory file for reading and writing
    fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory
    if (ftruncate(fd, SHARED_MEM_SIZE) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory into the process address space
    shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Close the file descriptor for the shared memory file, as it is no longer needed
    close(fd);

    // Read and write data to the shared memory
    char last_msg[SHARED_MEM_SIZE] = {0};
    while (1) {
        char* msg = shared_mem;
        if (strcmp(msg, last_msg) != 0) {
            printf("Received message: %s\n", msg);
            strcpy(last_msg, msg);
        }

        // Suspend execution for 100 milliseconds
        usleep(100 * 1000);
    }

    // Remove the mapping of the shared memory
    if (munmap(shared_mem, SHARED_MEM_SIZE) == -1) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    // Remove the shared memory file
    if (shm_unlink(SHARED_MEM_NAME) == -1) {
        perror("shm_unlink failed");
        exit(EXIT_FAILURE);
    }
}
