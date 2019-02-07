#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
    char buf[10];

    fgets(buf, sizeof(buf), stdin);

    //printf("buf:%x\n",buf);
    //printf("count:%x\n", "100");
    printf("strcmp:%d\n",strcmp(buf,"100\n"));

    if (strcmp(buf, "100\n")==0){
        printf("aaa\n");
    }else{
        printf("bbbb\n");
    }

    return 0;
}