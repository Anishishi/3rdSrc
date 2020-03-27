#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

void die(char *s){
    perror(s);
    exit(1);
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

    while(1){
        char data[256];

        int n1=recv(s, data, 1, 0);
        if(n1==-1) die("read");//recieve_version
        //if(n1==0) break;//n1==0をしていしないと無限ループする

        //int n0 = read(fd, data, 1);//標準入力のフィルを読み込む
        //if(n0==-1) die("open");
        //if(n0==0) break;


        //int n2=send(s, data, 1, 0);//dataを1byteずつ送信する
        //if(n2==-1) die("write");

        putchar(data[0]);
    }

    //close(fd);
    close(s);

    return 0;
}