#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MYDRIVER_GET_MAC_ADDR _IOR('m', 1, char*)

int main(void)
{
    int fd;
    char mac_addr[6];
    fd = open("/dev/mydriver", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (ioctl(fd, MYDRIVER_GET_MAC_ADDR, mac_addr) < 0) {
        perror("ioctl");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    close(fd);
    return 0;
}
