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
    if(argc!=3){
        printf("to read file is none.\n");
        return 0;
    }

    int rate=atoi(argv[1]);
    int fd=open(argv[2], O_RDONLY, 0644);
    if(fd==-1) die("open");
    
    while(1){
        int n;
        for(int i=0; i<rate; i++){
            short buf[1];
            n=read(fd, buf, 2);
            if(n==-1)die("read");
            else if(n==0) break;

            if(i==0) write(1,buf,2);
        }
        if(n==0)break;
    }
    return 0;
}