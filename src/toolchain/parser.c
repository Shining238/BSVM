#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


struct IR_Node *parser(char *filename){

    FILE *file = fopen(filename, "r");

    if (!file){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct IR_Node *IR = malloc(128 * sizeof(*IR));
    //+1 for \0
    char str[INSTR_LEN + 1] = {0};
    size_t consumed = 0;

    while (fgets(str, INSTR_LEN, file)){
    }

}