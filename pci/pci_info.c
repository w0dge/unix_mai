#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *fp;
    char buf[1024];

    //fp = popen("sudo lspci -nn | grep Ethernet | sed 's/.*\[\([^]]*\)\].*/\1/'", "r");
    fp = popen("sudo lspci -nn | grep Ethernet | sed 's/.*\\[\\([^]]*\\)\\].*/\\1/'", "r");

    if (fp == NULL) {
        perror("Failed to run command");
        exit(EXIT_FAILURE);
    }

    while (fgets(buf, 1024, fp) != NULL) {
        printf("%s", buf);
    }

    pclose(fp);

    exit(EXIT_SUCCESS);
}
