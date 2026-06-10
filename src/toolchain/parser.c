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


void free_IRNode(struct IR_Node *IR){
    if (IR->type == IR_DIR){
        if (IR->directive.string.type == OPERAND_STRING){
            free(IR->directive.string.str);
        }
        free(IR->directive.op);
    }
    else if (IR->type == IR_LABEL){
        free(IR->label.name.str);
    }
    else if (IR->type == IR_INSTR){
        if (IR->instr.a.type == OPERAND_STRING){
            free(IR->instr.a.str);
        }
        else if (IR->instr.a.type == OPERAND_MEM_STR){
            free(IR->instr.a.mem.idx.label);
        }
        if (IR->instr.b.type == OPERAND_STRING){
            free(IR->instr.b.str);
        }
        else if (IR->instr.b.type == OPERAND_MEM_STR){
            free(IR->instr.b.mem.idx.label);
        }
    }
}

//str must be null-terminated
OP_CODE op_lookup(char *str){
    
    for (size_t i = 0; i < OP_COUNT; i++){
        if (!strcmp(op_map[i].mnemonic, str)){
            return op_map[i].op;
        }
    }
    return OP_COUNT;
}

int isEndOfInstr(char c){
    return (c == '\n') || (c == '\0');
}

ParserError tryParseDir(char *buffer, struct IR_Node *IR){

    if (buffer[0] != '.'){
        return PARSER_UNKNOWN_INSTR;
    }
    char *op_name = NULL;
    int64_t value = 0;
    int n = 0;
    char *str = NULL;
    char open_quote = 0;
    char close_quote = 0;
    char sep = 0;
    int count = 0;
    if (((count = sscanf(buffer, ".%m[a-z]%c%ld%n", &op_name, &sep, &value, &n)) == 3)){
        if(!isEndOfInstr(buffer[n])){
            return PARSER_TOO_MANY_OPE;
        }
        if (sep != ' '){
            printf(".%s^\n", op_name);
            return PARSER_SYNTAX_ERROR;
        }
        IR->type = IR_DIR;
        IR->directive.type = DIR_VALUE;
        IR->directive.value.type = OPERAND_VALUE;
        IR->directive.value.value = value;
        IR->directive.string.type = OPERAND_NONE;
        IR->directive.op = op_name;
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
        op_name = NULL;
    }
    sep = 0;
    if (((count = sscanf(buffer, ".%m[a-z]%c%c%m[^\"]%c%n", &op_name, &sep, &open_quote, &str, &close_quote, &n)) == 5)){
        if (!isEndOfInstr(buffer[n])){
            free(op_name);
            free(str);
            return PARSER_TOO_MANY_OPE;
        }
        
        if ((sep != ' ') || (open_quote != '\"') || (close_quote != '\"')){
            free(op_name);
            free(str);
            return PARSER_SYNTAX_ERROR;
        }
       
        IR->type = IR_DIR;
        IR->directive.type = DIR_STRING;
        IR->directive.string.str = str;
        IR->directive.value.type = OPERAND_NONE;
        IR->directive.op = op_name;
        return PARSER_OK;
    }
    if (isEndOfInstr(sep)){
        return PARSER_TOO_FEW_OPE;
    }
    if (op_name){
        free(op_name);
    }
    if (str){
        free(str);
    }
    if (count > 1){
        return PARSER_SYNTAX_ERROR;
    }
    return PARSER_UNKNOWN_INSTR;
}

ParserError tryParseLabel(char *buffer, struct IR_Node *IR){
    
    char *op_name = NULL;
    char colon = 0;
    int n = 0;
    int count = 0;
    if (((count = sscanf(buffer, "%m[a-z0-9]%c%n", &op_name, &colon, &n)) == 2) && (colon == ':')){
        if (!isEndOfInstr(buffer[n])){
            free(op_name);
            return PARSER_SYNTAX_ERROR;
        }
        if (colon != ':'){
            free(op_name);
            return PARSER_SYNTAX_ERROR;
        }
        IR->type = IR_LABEL;
        IR->label.name.type = OPERAND_LABEL;
        IR->label.name.str = op_name;
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
    }
    if (count > 1){
        return PARSER_SYNTAX_ERROR;
    }
    return PARSER_UNKNOWN_INSTR;
}

ParserError tryParseIDX(char *buffer, struct IR_Node *IR){
    char *op_name = NULL;
    uint8_t a = 0;
    uint8_t b = 0;
    char sign = 0;
    char sep1 = 0;
    char sep2 = 0;
    char open_bracket = 0;
    char close_bracket = 0;
    char comma = 0;
    char r1 = 0;
    char r2 = 0;
    int n = 0;
    int count = 0;
    int64_t offset = 0;
    if ((count = sscanf(buffer, "%m[A-Z]%c%c%hhu%c%c%c%c%hhu%c%ld%c%n", &op_name, &sep1, &r1, &a, &comma, &sep2, &open_bracket, &r2, &b, &sign, &offset, &close_bracket, &n)) == 12){
        if (!isEndOfInstr(buffer[n])){
            free(op_name);
            return PARSER_TOO_MANY_OPE;
        }
        if ((open_bracket == 'R') || (sign == ']')){
            free(op_name);
            return PARSER_UNKNOWN_INSTR;
        }
        if (sign == '-'){
            offset = -offset;
        }
        if ((sep1 != ' ') || (sep2 != ' ') || (comma != ',') || ((sign != '+') && (sign != '-')) || (open_bracket != '[') || (close_bracket != ']') || (r1 != 'R') || (r2 != 'R')){
            free(op_name);
            return PARSER_SYNTAX_ERROR;
        }
        IR->type = IR_INSTR;
        IR->instr.op = op_lookup(op_name);
        IR->instr.mode = MODE_IDX;
        IR->instr.a.type = OPERAND_REG;
        IR->instr.a.reg = a;
        IR->instr.b.type = OPERAND_MEM_IDX;
        IR->instr.b.mem.idx.base_reg = b;
        IR->instr.b.mem.idx.offset = offset;
        free(op_name);
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
    }
    return PARSER_UNKNOWN_INSTR;
}

ParserError tryParseIND(char *buffer, struct IR_Node *IR){
    char *op_name = NULL;
    uint8_t a = 0;
    uint8_t b =0;
    char sep1 = 0;
    char sep2 = 0;
    char comma = 0;
    char r1 = 0;
    char r2 = 0;
    char open_bracket = 0;
    char close_bracket = 0;
    int n = 0;
    int count = 0;
    if ((count = sscanf(buffer, "%m[A-Z]%c%c%hhu%c%c%c%c%hhu%c%n", &op_name, &sep1, &r1, &a, &comma, &sep2, &open_bracket, &r2, &b, &close_bracket, &n)) == 10){
        if (!isEndOfInstr(buffer[n])){
            free(op_name);
            return PARSER_TOO_MANY_OPE;
        }

        if (open_bracket == 'R'){
            return PARSER_UNKNOWN_INSTR;
        }
        if ((sep1 != ' ') || (r1 != 'R') || (comma != ',') || (r2 != 'R') || (sep2 != ' ') || (open_bracket != '[') || (close_bracket != ']')){
            free(op_name);
            return PARSER_SYNTAX_ERROR;
        }
        
        IR->type = IR_INSTR;
        IR->instr.op = op_lookup(op_name);
        IR->instr.mode = MODE_IND;
        IR->instr.a.type = OPERAND_REG;
        IR->instr.b.type = OPERAND_REG;
        IR->instr.a.reg = a;
        IR->instr.b.reg = b;
        free(op_name);
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
    }
    return PARSER_UNKNOWN_INSTR;
}

ParserError tryParseDIR(char *buffer, struct IR_Node *IR){
    
    char *op_name;
    uint8_t a = 0;
    uint64_t addr = 0;
    char r = 0;
    char sep1 = 0;
    char sep2 = 0;
    char open_bracket = 0;
    char close_bracket = 0;
    char comma = 0;
    int count = 0;
    int n = 0;
    if ((count = sscanf(buffer, "%m[A-Z]%c%c%hhu%c%c%c%lu%c%n", &op_name, &sep1, &r, &a, &comma, &sep2, &open_bracket, &addr, &close_bracket, &n)) == 9){
        if (!isEndOfInstr(buffer[n])){
            free(op_name);
            return PARSER_TOO_MANY_OPE;
        }
        if (open_bracket == 'R'){
            free(op_name);
            return PARSER_UNKNOWN_INSTR;
        }

        if ((r != 'R') || (sep1 != ' ') || (sep2 != ' ') || (open_bracket != '[') || (close_bracket != ']') || (comma != ',')){
            free(op_name);
            return PARSER_SYNTAX_ERROR;
        }
        
        IR->type = IR_INSTR;
        IR->instr.op = op_lookup(op_name);
        IR->instr.mode = MODE_DIR;
        IR->instr.a.type = OPERAND_REG;
        IR->instr.a.reg = a;
        IR->instr.b.type = OPERAND_VALUE;
        IR->instr.b.value = (int64_t) addr;
        free(op_name);
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
    }
    return PARSER_UNKNOWN_INSTR;
}


ParserError tryParseREG(char *buffer, struct IR_Node *IR){

    char *op_name;
    char sep1 = 0;
    char sep2 = 0;
    char r1 = 0;
    char r2 = 0;
    char comma = 0;
    int count = 0;
    int n = 0;
    uint8_t a = 0;
    uint8_t b = 0;

    if ((count = sscanf(buffer, "%m[A-Z]%c%c%hhu%c%c%c%hhu%n", &op_name, &sep1, &r1, &a, &comma, &sep2, &r2, &b, &n)) == 8){
        if (!isEndOfInstr(buffer[n])){
            free(op_name);
            return PARSER_TOO_MANY_OPE;
        }
        if (r2 == '['){
            free(op_name);
            return PARSER_UNKNOWN_INSTR;
        }

        if ((r1 != 'R') || (r2 != 'R') || (sep1 != ' ') || (sep2 != ' ') || (comma != ',')){
            free(op_name);
            return PARSER_SYNTAX_ERROR;
        }
        
        IR->type = IR_INSTR;
        IR->instr.op = op_lookup(op_name);
        IR->instr.mode = MODE_REG;
        IR->instr.a.type = OPERAND_REG;
        IR->instr.a.reg = a;
        IR->instr.b.type = OPERAND_REG;
        IR->instr.b.reg = b;
        free(op_name);
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
    }
    return PARSER_UNKNOWN_INSTR;
}

ParserError tryParseIMM(char *buffer, struct IR_Node *IR){

    char *op_name;
    char sep1 = 0;
    char sep2 = 0;
    char r1 = 0;
    char comma = 0;
    int count = 0;
    int n = 0;
    uint8_t a = 0;
    int64_t imm = 0;

    if ((count = sscanf(buffer, "%m[A-Z]%c%c%hhu%c%c%ld%n", &op_name, &sep1, &r1, &a, &comma, &sep2, &imm, &n)) == 7){
        if (!isEndOfInstr(buffer[n])){
            free(op_name);
            return PARSER_TOO_MANY_OPE;
        }

        if ((r1 != 'R') || (sep1 != ' ') || (sep2 != ' ') || (comma != ',')){
            free(op_name);
            return PARSER_SYNTAX_ERROR;
        }
        
        IR->type = IR_INSTR;
        IR->instr.op = op_lookup(op_name);
        IR->instr.mode = MODE_IMM;
        IR->instr.a.type = OPERAND_REG;
        IR->instr.a.reg = a;
        IR->instr.b.type = OPERAND_VALUE;
        IR->instr.b.value = imm;
        free(op_name);
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
    }
    return PARSER_UNKNOWN_INSTR;
}

ParserError tryParseREGU(char *buffer, struct IR_Node *IR){
 
    char *op_name;
    char sep = 0;
    char r = 0;
    int count = 0;
    int n = 0;
    uint8_t a = 0;

    if ((count = sscanf(buffer, "%m[A-Z]%c%c%hhu%n", &op_name, &sep, &r, &a, &n)) == 4){
        if (!isEndOfInstr(buffer[n])){
            free(op_name);
            return PARSER_TOO_MANY_OPE;
        }

        if ((r != 'R') || (sep != ' ')){
            free(op_name);
            return PARSER_SYNTAX_ERROR;
        }
        
        IR->type = IR_INSTR;
        IR->instr.op = op_lookup(op_name);
        IR->instr.mode = MODE_REGU;
        IR->instr.a.type = OPERAND_REG;
        IR->instr.b.type = OPERAND_NONE;
        IR->instr.a.reg = a;
        free(op_name);
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
    }
    return PARSER_UNKNOWN_INSTR; 
}

ParserError tryParseNONE(char *buffer, struct IR_Node *IR){
    char *op_name = NULL;
    int n = 0;
    int count = 0;
    if ((count = sscanf(buffer, "%m[A-Z]%n", &op_name, &n)) == 1){
         if (!isEndOfInstr(buffer[n])){
            free(op_name);
            return PARSER_TOO_MANY_OPE;
        }

        IR->type = IR_INSTR;
        IR->instr.op = op_lookup(op_name);
        IR->instr.mode = MODE_NONE;
        IR->instr.a.type = OPERAND_NONE;
        IR->instr.b.type = OPERAND_NONE;
        free(op_name);
        return PARSER_OK;
    }
    if (op_name){
        free(op_name);
    }
    return PARSER_UNKNOWN_INSTR; 
}


ParserError tryParseOp(char *buffer, struct IR_Node *IR){

    ParserError status;
    status = tryParseIMM(buffer, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    status = tryParseDIR(buffer, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    status = tryParseREG(buffer, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    status = tryParseIDX(buffer, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    status = tryParseIND(buffer, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    status = tryParseREGU(buffer, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    status = tryParseNONE(buffer, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    return PARSER_UNKNOWN_INSTR;
}


ParserError parse_line(char *buffer, struct IR_Node *IR){
    IR->type = IR_UNDEF;
    ParserError status;
    size_t i = 0;
    size_t n = strlen(buffer);
    while ((i < n) && (buffer[i] != '.') && !isalnum(buffer[i])) i++;
    if ((buffer[i] == '\n') || (buffer[i] == '\0')){
        return PARSER_EMPTY;
    }
    status = tryParseDir(buffer + i, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    status = tryParseLabel(buffer + i, IR);
    if (status != PARSER_UNKNOWN_INSTR){
        return status;
    }
    status = tryParseOp(buffer + i, IR);
    return status;
}

void printStatus(ParserError status, size_t line){
    switch (status){
            case PARSER_OK: printf("OK\n"); break;
            case PARSER_TOO_MANY_OPE: printf("Error : too many arguments on line %lu\n", line); break;
            case PARSER_SYNTAX_ERROR: printf("Syntax error on line %lu\n", line); break;
            case PARSER_TOO_FEW_OPE: printf("Error : too few arguments on line %lu\n", line); break;
            case PARSER_UNKNOWN_INSTR: printf("Error : unknown instruction on line %lu\n", line);
            default: break;
    }
}

struct IR_Node *parser(char *filename, size_t *n){

    FILE *file = fopen(filename, "r");

    if (!file){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    size_t prog_size = 128;
    size_t i = 0;
    size_t line = 0;
    struct IR_Node *IRList = malloc(prog_size * sizeof(*IRList));
    struct IR_Node *tmp;
    //+1 for \0
    char str[INSTR_LEN + 1] = {0};
    ParserError status;

    while (fgets(str, INSTR_LEN, file)){
        printf("line : %lu |%s|", line+1, str);
        if (i == prog_size){
            tmp = realloc(IRList, 2 * prog_size);
            prog_size *= 2;
            if (!tmp){
                free(IRList);
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
        status = parse_line(str, &IRList[i]);
        if (status == PARSER_EMPTY){
            line++;
            continue;
        }
        if (status != PARSER_OK){
            printStatus(status, line+1);
            exit(EXIT_FAILURE);
        }
        i++;
        line++;
    }
    *n = i;
    return IRList;
}