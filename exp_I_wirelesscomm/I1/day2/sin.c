#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
    if(argc != 4){
        printf("the number of parameter is wrong.\n");
        return 0;
    }

    int A=atoi(argv[1]), f=atoi(argv[2]), n=atoi(argv[3]);
    short xt[n];  
    double t[n];
    for(int i=0; i<n; i++){
        t[i]=(double)i/44100;
    }
    for(int i=0; i<n; i++){
        xt[i]=A*(double)sin(2 * M_PI * f * (double)t[i]);
    }

    for(int i=0; i<n; i++){
        write(1, xt+i, 2);
    }

    return 0;
}