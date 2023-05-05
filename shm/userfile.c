
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbg_api.h>

char *randstring(size_t length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";        
    char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * (length +1));

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

int main() {
    char* shm_name = "/bokundebug";
    int shm_size = 1024;

    shm_struct shm = dbginit(shm_name, shm_size);
    if (shm.ptr == NULL) {
        printf("Failed to initialize shared memory.\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        char* message = randstring(10);
        dbgwrite(shm, message);
        printf("Wrote message: %s\n", message);

        sleep(1);
    }

    dbgclose(shm);

    return 0;
}
