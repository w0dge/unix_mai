#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


#define EXPORT __attribute__((visibility("default")))

typedef struct {
    void* ptr;
    int fd;
    int size;
    char* name;
} shm_struct;


EXPORT shm_struct dbginit(char* shm_name, int shm_size);
EXPORT int dbgwrite(shm_struct shm, char* message);
EXPORT void dbgclose(shm_struct shm);

shm_struct dbginit(char* shm_name, int shm_size) {
    int shm_fd;
    void* shm_ptr;

    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        shm_struct shm = { NULL, -1, -1, NULL };
        return shm;
    }

    if (ftruncate(shm_fd, shm_size) == -1) {
        close(shm_fd);
        shm_struct shm = { NULL, -1, -1, NULL };
        return shm;
    }

    shm_ptr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        close(shm_fd);
        shm_struct shm = { NULL, -1, -1, NULL };
        return shm;
    }

    shm_struct shm = { shm_ptr, shm_fd, shm_size, shm_name };
    return shm;
}

int dbgwrite(shm_struct shm, char* message) {
    size_t len;

    len = strlen(message);
    if (len >= shm.size) {
        fprintf(stderr, "message too long for shared memory\n");
        return -1;
    }

    memcpy(shm.ptr, message, len);
    memset(shm.ptr + len, '\0', shm.size - len);
    return 0;
}

void dbgclose(shm_struct shm) {
    munmap(shm.ptr, shm.size);
    close(shm.fd);
    shm_unlink(shm.name);
}
