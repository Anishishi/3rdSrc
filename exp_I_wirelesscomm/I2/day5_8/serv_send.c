#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void die(char *s){
    perror(s);
    exit(1);
}

int main(int argc, char **argv){

    int ss=socket(PF_INET, SOCK_STREAM, 0);
    if(argc!=2){
        die("the size of marameter is wrong./n");
    }
    int port_num = atoi(argv[1]);

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;//IPv4
    addr.sin_port=htons(port_num);//port
    addr.sin_addr.s_addr=INADDR_ANY;//どのIPアドレスからでも良いです
    bind(ss, (struct sockaddr *)&addr, sizeof(addr));//待つ

    listen(ss, 10);//接続受付開始宣言

    struct sockaddr_in client_addr;
    socklen_t len=sizeof(struct sockaddr_in);
    int s = accept(ss, (struct sockaddr *)&client_addr, &len);

    if(s==-1) die("open");

    int ch;
    while((ch=getchar())!=EOF){
        //char data[256];
        
        //int n1=recv(s, data, 1, 0);
        //if(n1==-1) die("read");//recieve_version
        //if(n1==0) break;//n1==0をしていしないと無限ループする

        //int n0 = read(fd, data, 1);//標準入力のフィルを読み込む
        //if(n0==-1) d];ie("open");
        //if(n0==0) break;

        int n2=send(s, &ch, 1, 0);//dataを1byteずつ送信する
        if(n2==-1) die("write");

        //putchar(data[0]);
    }

    close(ss);
    close(s);
    
    return 0;
}