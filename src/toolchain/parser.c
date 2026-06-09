#include "parser.h"
#include "instructions.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

static const OpMap op_map[OP_COUNT] = {
    {.mnemonic="ADD", .op=OP_ADD},
    {.mnemonic="SUB", .op=OP_SUB},
    {.mnemonic="MUL", .op=OP_MUL},
    {.mnemonic="DIV", .op=OP_DIV},
    {.mnemonic="CMP", .op=OP_CMP},
    {.mnemonic="NEG", .op=OP_NEG}, 
    {.mnemonic="AND", .op=OP_AND},
    {.mnemonic="OR", .op=OP_OR},
    {.mnemonic="XOR", .op=OP_XOR},
    {.mnemonic="NOT", .op=OP_NOT},
    {.mnemonic="SHL", .op=OP_SHL},
    {.mnemonic="SHR", .op=OP_SHR},
    {.mnemonic="ST", .op=OP_ST},
    {.mnemonic="LD", .op=OP_LD},
    {.mnemonic="SWP", .op=OP_SWP},
    {.mnemonic="PUSH", .op=OP_PUSH},
    {.mnemonic="POP", .op=OP_POP},
    {.mnemonic="FAS", .op=OP_FAS},
    {.mnemonic="CALL", .op=OP_CALL},
    {.mnemonic="RTN", .op=OP_RTN},
    {.mnemonic="NOP", .op=OP_NOP},
    {.mnemonic="RST", .op=OP_RST},
    {.mnemonic="JMP", .op=OP_JMP},
    {.mnemonic="JEQ", .op=OP_JEQ},
    {.mnemonic="JNE", .op=OP_JNE},
    {.mnemonic="JLE", .op=OP_JLE},
    {.mnemonic="JLT", .op=OP_JLT},
    {.mnemonic="JGE", .op=OP_JGE},
    {.mnemonic="JGT", .op=OP_JGT},
    {.mnemonic="HALT", .op=OP_HALT},
};


OP_CODE op_lookup(char *str, size_t size){
    
    for (size_t i = 0; i < OP_COUNT; i++){
        if (!strncmp(op_map[i].mnemonic, str, size)){
            return op_map[i].op;
        }
    }
    return OP_COUNT;
}

ParserError parse_instr(char *str, char *op_name, size_t line_size, size_t op_name_size, struct IR_Node *IR){
    
    IR->instr.op = op_lookup(op_name, op_name_size);

    if (IR->instr.op == OP_COUNT){
        return PARSER_INVALID_OP;
    }

    size_t i = 0;
    while ((i < line_size - op_name_size) && str[i] != 'R'){
        i++;
    }
}

ParserError parse_line(char *str, struct IR_Node *IR){

    size_t buf_size = 128;
    size_t i = 0;
    size_t name_len = 0;
    IR->type = IR_UNDEF;

    char buffer[128] = {0};
    while ((i < buf_size + 1) && isalpha(str[i])){
        if (str[i] == ':'){
            IR->type = IR_LABEL;
        }
        buffer[i] = str[i];
        i++;
    }
    
    name_len = i;

    if (buffer[0] == '.'){
        if (IR->type != IR_UNDEF){
            return PARSER_INVALID_TYPE;
        }
        IR->type = IR_DIR;
    }

    if (IR->type == IR_UNDEF){
        IR->type = IR_INSTR;
    }

    ParserError status;


}


struct IR_Node *parser(char *filename){

    FILE *file = fopen(filename, "r");

    if (!file){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    size_t i = 0;
    struct IR_Node *IRList = malloc(128 * sizeof(*IRList));
    //+1 for \0
    char str[INSTR_LEN + 1] = {0};

    while (fgets(str, INSTR_LEN, file)){
        parse_line(str, &IRList[i++]);
    }

}