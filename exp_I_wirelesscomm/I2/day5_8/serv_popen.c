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

    FILE *p;
    if((p=popen("rec -t raw -b 16 -c 1 -e s -r 44100 -", "r"))==NULL) die("popen");

    int ch;
    while((ch=fgetc(p))!=EOF){
        
        int n2=send(s, &ch, 1, 0);//dataを1byteずつ送信する
        if(n2==-1) die("write");

        int n1=recv(s, data, 1, 0);
        if(n1==-1) die("read");

        //putchar(data[0]);
    }

    pclose(p);
    close(ss);
    close(s);
    
    return 0;
}