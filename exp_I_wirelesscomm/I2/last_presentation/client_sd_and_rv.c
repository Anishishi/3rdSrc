#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

void die(char *s){
    perror(s);
    exit(1);
}

void *exe_recv(void *tmp){
    char data;
    int* s=(int *)tmp;
    while(1){
        int n2=recv(*s, &data, 1, 0);
        if(n2==-1) die("read");
         putchar(data);
    }
}

void exe_send(int s){
    FILE *p;
    if((p=popen("rec -t raw -b 16 -c 1 -e s -r 44100 -", "r"))==NULL) die("popen");

    int ch;
    while((ch=fgetc(p))!=EOF){
        int n1=send(s, &ch, 1, 0);
        if(n1==-1) die("write");
    }
}

int main(int argc, char **argv){
    if(argc!=3){
        printf("The number of argument is wrong.\n");
        return 0;
    }

    //int fd=open(argv[3], O_RDONLY, 0644);
    //if(fd==-1) die("file_open");

    int port_num = atoi(argv[2]);
    int s = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;//IPv4のアドレス
    inet_aton(argv[1], &addr.sin_addr);//IPアドレス
    addr.sin_port = htons(port_num);//port
    int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));//connect
    
    if(ret==-1) perror("open");

    pthread_t pthread;

    pthread_create(&pthread, NULL, &exe_recv, &s);
    exe_send(s);
    pthread_join(pthread, NULL);

    close(s);

    return 0;
}