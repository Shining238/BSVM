#include "parser.h"

#include <stdio.h>

void usage(char *prog){
    printf("%s <file.bsm>\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){

    if (argc != 2){
        usage(argv[0]);
    }

    printf("Hello world !\n");
    size_t prog_size = 0;
    struct IR_Node *IRList = parser(argv[1], &prog_size);

    for (size_t i = 0; i < prog_size; i++){
        free_IRNode(&IRList[i]);
    }


    exit(EXIT_SUCCESS);
}