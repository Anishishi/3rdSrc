#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
    if(argc != 3){
        printf("the number of parameter is wrong.\n");
        return 0;
    }
    
    int A=atoi(argv[1]), n=atoi(argv[2]);
    double f[n];
    f[0]=523.251;
    for(int i=1; i<n; i++){
        if(i%7==0 || i%7==3){
            f[i]=f[i-1]*pow(2, 0.0833333333333);
        }else{
            f[i]=f[i-1]*pow(2, 0.166666666667);
        }
    }

    short xt[n*13230];  
    double t[n*13230];
    for(int i=0; i<n*13230; i++){
        t[i]=(double)i/44100;
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<13230; j++){
            xt[13230*i+j]=(short)(A*(double)sin(2 * M_PI * f[i] * (double)t[13230*i+j]));
        }
    }

    for(int i=0; i<n*13230; i++){
        write(1, xt+i, 2);
    }

    return 0;
}