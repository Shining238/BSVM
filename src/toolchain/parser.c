#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


struct IR_Node *parser(char *filename){

    FILE *file = fopen(filename, "r");
    if(!file)((void)filename);
    return NULL;
}