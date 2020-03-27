#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    unsigned char buf[256];
    buf[0]=228;
    buf[1]=186;
    buf[2]=186;
    buf[3]=229;
    buf[4]=191;
    buf[5]=151;

    int fd=open("hitoshi", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd==-1) {perror("open"); exit(1);}
    write(fd, buf, 6);
    close(fd);
    return 0;
}