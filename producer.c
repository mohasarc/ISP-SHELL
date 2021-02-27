#include <stdlib.h>
#include <stdio.h>
#define SIZE 36
int main(int argc, char *argv[])
{
    int M = atoi(argv[1]);
    char alphabet[SIZE] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
                        'h', 'i', 'j', 'k', 'l', 'm', 'n',  
                        'o', 'p', 'q', 'r', 's', 't', 'u', 
                        'v', 'w', 'x', 'y', 'z' , '0', '1', 
                        '2', '3', '4', '5', '6', '7', '8', '9'}; 

    for (int i = 0; i < M; i++){
        printf("%c", alphabet[rand() % SIZE]);
    }
}
