#ifndef DEBUG_H
#define DEBUG_H

#include <stddef.h>

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

#endif
