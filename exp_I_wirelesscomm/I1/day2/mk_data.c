#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
    if(argc<2){
        printf("filename is necessary.");
        return 0;
    }
    char buf[256];
    for(int i=0; i<256; i++){
        buf[i]=(char)i;
    }
    int fd=open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd==-1) {perror("open"); exit(1);}
    write(fd, buf, 256);
    close(fd);

    return 0;
}