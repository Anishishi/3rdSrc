#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void die(char *s){
    perror(s);
    exit(1);
}

int main(int argc, char **argv){
    if(argc<2){
        printf("filename is necessary.");
        return 0;
    }
    
    int fd=open(argv[1], O_RDONLY, 0644);
    if(fd==-1) die("open");
    
    int cnt=0;
    while(1){
        unsigned char data[256];
        int n = read(fd, data, 1);
        if(n==-1) die("open");
        if(n==0) break;

        printf("%d %d\n", cnt, data[0]);
        cnt++;
    }
    
    close(fd);

    return 0;
}