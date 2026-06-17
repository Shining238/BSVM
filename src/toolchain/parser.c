#include "parser.h"
#include "instructions.h"
#include "lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define CHK_LEXER_STATUS(prog, status, tok) do {\
    if (status != LEXER_OK){\
        printLexerStatus(prog, status, tok);\
        exit(EXIT_FAILURE);\
    }\
} while (0)

#define CHK_PARSER_STATUS(prog, status, tok) do {\
    if ((status != PARSER_OK) && (status != PARSER_EMPTY)){\
        printParserStatus(prog, status, tok);\
        exit(EXIT_FAILURE);\
    }\
} while (0)


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
    {.mnemonic="TRAP", .op=OP_TRAP},
};


static const DirMap dir_map[DIR_COUNT] = {
    {.mnemonic="string", .dir=DIR_STRING},
    {.mnemonic="addr", .dir=DIR_ADDR},
    {.mnemonic="entry", .dir=DIR_ENTRY},
    {.mnemonic="word", .dir=DIR_WORD},
    {.mnemonic="byte", .dir=DIR_BYTE},
    {.mnemonic="text", .dir=DIR_TEXT},
    {.mnemonic="data", .dir=DIR_DATA}
};

const char *getDirStr(DirType dir){
    for (size_t i = 0; i < DIR_COUNT; i++){
        if (dir == dir_map[i].dir){
            return dir_map[i].mnemonic;
        }
    }
    return NULL;
}

const char *getOpStr(OP_CODE op){
    for (size_t i = 0; i < OP_COUNT; i++){
        if (op == op_map[i].op){
            return op_map[i].mnemonic;
        }
    }
    return NULL;
}

void free_IRNode(struct IR_Node *IR){
    if (IR->type == IR_DIR){
        if ((IR->directive.string.type == OPERAND_STRING) || (IR->directive.string.type == OPERAND_IDENT)){
            free(IR->directive.string.ident.str);
        }
    }
    else if (IR->type == IR_LABEL){
        free(IR->label.name.ident.str);
    }
    else if (IR->type == IR_INSTR){
        if (IR->instr.a.type == OPERAND_STRING){
            free(IR->instr.a.ident.str);
        }
        else if (IR->instr.a.type == OPERAND_MEM_IDX_IDENT){
            free(IR->instr.a.mem.ident.str);
        }
        else if ((IR->instr.a.type == OPERAND_IDENT) || (IR->instr.a.type == OPERAND_MEM_IDENT)){
            free(IR->instr.a.ident.str);
        }
        if (IR->instr.b.type == OPERAND_STRING){
            free(IR->instr.b.ident.str);
        }
        else if (IR->instr.b.type == OPERAND_MEM_IDX_IDENT){
            free(IR->instr.b.mem.ident.str);
        }
        else if ((IR->instr.b.type == OPERAND_IDENT) || (IR->instr.b.type == OPERAND_MEM_IDENT)){
            free(IR->instr.b.ident.str);
        }
    }
}

void printinstrOPE(Operand *ope){
    switch (ope->type){
        case OPERAND_NONE: break;
        case OPERAND_IDENT: printf("%s ", ope->ident.str); break;
        case OPERAND_REG: printf("R%hhu", ope->reg); break;
        case OPERAND_VALUE: printf("%ld", ope->value); break;
        case OPERAND_MEM_IDENT: printf("[%s]", ope->mem.ident.str); break;
        case OPERAND_MEM_REG: printf("[R%hhu]", ope->reg); break;
        case OPERAND_MEM_VALUE: printf("[%ld]", ope->value); break;
        case OPERAND_MEM_IDX: printf("[R%hhu+%ld]", ope->mem.base_reg, ope->mem.offset); break;
        case OPERAND_MEM_IDX_IDENT: printf("[%s+%ld]", ope->mem.ident.str, ope->mem.offset); break;
        default: printf("*");
    }
}

void printIRNode(struct IR_Node *IR){
    if (IR->type == IR_DIR){
        printf("DIR : .%s ", getDirStr(IR->directive.type));
        if ((IR->directive.string.type == OPERAND_STRING) || (IR->directive.string.type == OPERAND_IDENT)){
            printf("%s", IR->directive.string.ident.str);
            printf("\n");
        }
        else if (IR->directive.value.type == OPERAND_VALUE){
            printf("%ld\n", IR->directive.value.value);
        }
        else if (IR->directive.string.type != OPERAND_NONE){
            printf("wrong operand type\n");
        }
        else {
            printf("no argument\n");
        }
        return;
    }
    if (IR->type == IR_LABEL){
        if (IR->label.name.type == OPERAND_IDENT){
            printf("LABEL : ");
            fwrite(IR->label.name.ident.str, 1, IR->label.name.ident.length, stdout);
            printf("\n");
        }
        else {
            printf("wrong operand type\n");
        }
        return;
    }
    if (IR->type == IR_INSTR){
        printf("OP : %s ", getOpStr(IR->instr.op));
        if (IR->instr.a.type == OPERAND_NONE){
            printf("\n");
            return;
        }
        printinstrOPE(&IR->instr.a);
        if (IR->instr.b.type == OPERAND_NONE){
            printf("\n");
            return;
        }
        printf(", ");
        printinstrOPE(&IR->instr.b);
        printf("\n");
    }
}


//str must be null-terminated
OP_CODE op_lookup(char *str, size_t len){
    
    for (size_t i = 0; i < OP_COUNT; i++){
        if (!strncmp(op_map[i].mnemonic, str, len)){
            return op_map[i].op;
        }
    }
    return OP_COUNT;
}

//str must be null-terminated
DirType dir_lookup(char *str, size_t len){

    for (size_t i = 0; i < DIR_COUNT; i++){
        if (!strncmp(dir_map[i].mnemonic, str, len)){
            return dir_map[i].dir;
        }
    }
    return DIR_COUNT;
}

void printParserStatus(char *prog, ParserError status, struct Token *tok){
    switch (status){
        case PARSER_OK: printf("%s OK\n", prog); break;
        case PARSER_EMPTY: printf("\n"); break;
        case PARSER_INVALID_REGISTER: printf("%s:%lu.%lu : register number invalid\n", prog, tok->line+1, tok->column+1); break;
        case PARSER_TOO_MANY_OPE: printf("%s:%lu.%lu : too many arguments\n", prog, tok->line+1, tok->column+1); break;
        case PARSER_SYNTAX_ERROR: printf("%s:%lu.%lu : syntax error\n", prog, tok->line+1, tok->column+1); break;
        case PARSER_TOO_FEW_OPE: printf("%s:%lu.%lu : too few arguments\n", prog, tok->line+1, tok->column+1); break;
        case PARSER_UNKNOWN_INSTR: printf("%s:%lu.%lu : unknown instruction\n", prog, tok->line+1, tok->column+1); break;
        case PARSER_UNKNOWN_DIR: printf("%s:%lu.%lu : unknown directive\n", prog, tok->line+1, tok->column+1); break;
        case PARSER_UNKNOWN_OP: printf("%s:%lu.%lu : unknown operation\n", prog, tok->line+1, tok->column+1); break;
        case PARSER_INVALID_ARGS: printf("%s:%lu.%lu : invalid argument\n", prog, tok->line+1, tok->column+1); break;
        default: break;
    }
}

void printLexerStatus(char *prog, LexerError status, struct Token *tok){
    switch (status){
        case LEXER_OK: printf("%s OK\n", prog); break;
        case LEXER_ERROR_MISSING_QUOTE: printf("%s:%lu.%lu : missing quote\n", prog, tok->line+1, tok->column+1); break;
        case LEXER_INVALID_TOKEN: printf("%s:%lu.%lu : invalid token\n", prog, tok->line+1, tok->column+1); break;
        case LEXER_UNKNOWN_ESCAPE_SEQUENCE: printf("%s:%lu.%lu : unknown escape sequence\n", prog, tok->line+1, tok->column+1); break;
        default: break;
    }
}

int isSyntaxElement(struct Token *tok){
    switch (tok->type){
        case TOKEN_COMMA:
        case TOKEN_DOT:
        case TOKEN_OPEN_BRACKET:
        case TOKEN_CLOSE_BRACKET:
        case TOKEN_COLON:
            return 1;
        default:
            return 0;
    }
}

int isEndOfInstr(struct Token *tok){
    return (tok->type == TOKEN_EOF) || (tok->type == TOKEN_NEWLINE);
}

ParserError parse_dir(char *prog, struct Lexer *lexer, struct IR_Node *IR, struct Token *tok){
    LexerError status;

    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
    if (tok->type != TOKEN_IDENT){
        return PARSER_SYNTAX_ERROR;
    }
    IR->directive.type = dir_lookup(tok->text, tok->length);
    if (IR->directive.type == DIR_COUNT){
        return PARSER_UNKNOWN_DIR;
    }

    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
    if (tok->type == TOKEN_NUMBER){
        IR->directive.value.type = OPERAND_VALUE;
        IR->directive.value.value = tok->number;
    }
    else if (tok->type == TOKEN_MINUS){
        *tok = lexer_next_token(lexer, &status);
        CHK_LEXER_STATUS(prog, status, tok);
        if (tok->type == TOKEN_NUMBER){
            IR->directive.value.type = OPERAND_VALUE;
            IR->directive.value.value = -tok->number;
        }
        else {
            return PARSER_SYNTAX_ERROR;
        }
    }
    else if ((tok->type == TOKEN_IDENT) || (tok->type == TOKEN_STRING)){
        IR->directive.string.ident.str = malloc(tok->length+1);
        memcpy(IR->directive.string.ident.str, tok->text, tok->length); ///------A changer : malloc du string dans lexer
        IR->directive.string.ident.str[tok->length] = '\0';
        IR->directive.string.type = OPERAND_IDENT;
        if (tok->type == TOKEN_STRING){
            IR->directive.string.type = OPERAND_STRING;
            free(tok->text);
        }
        IR->directive.string.ident.length = tok->length+1;
    }
    else if (isEndOfInstr(tok)){
        IR->directive.string.type = OPERAND_NONE;
        return PARSER_OK;
    }
    else if (isSyntaxElement(tok)){
        return PARSER_SYNTAX_ERROR;
    }
    else {
        return PARSER_INVALID_ARGS;
    }

    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
    if (!isEndOfInstr(tok) && isSyntaxElement(tok)){
        return PARSER_SYNTAX_ERROR;
    }
    else if (!isEndOfInstr(tok)){
        return PARSER_TOO_MANY_OPE;
    }
    return PARSER_OK;
}

ParserError parse_label(char *prog, struct Lexer *lexer, struct IR_Node *IR, struct Token *tok){
    LexerError status;
    IR->label.name.ident.str = malloc(tok->length+1);
    memcpy(IR->label.name.ident.str, tok->text, tok->length);
    IR->label.name.ident.str[tok->length] = '\0';
    IR->label.name.ident.length = tok->length+1;
    IR->label.name.type = OPERAND_IDENT;

    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
   
    if (tok->type != TOKEN_COLON){
        if (isEndOfInstr(tok)){
            return PARSER_SYNTAX_ERROR;
        }
        else {    
            return PARSER_TOO_MANY_OPE;
        }
    }

    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
    if (!isEndOfInstr(tok) && isSyntaxElement(tok)){
        return PARSER_SYNTAX_ERROR;
    }
    else if (!isEndOfInstr(tok)){
        return PARSER_TOO_MANY_OPE;
    }
    return PARSER_OK;
}

//sauf bracket
ParserError parse_instrOPE(char *prog, struct Lexer *lexer, Operand *ope, struct Token *tok){
    LexerError status;

    struct Token suiv;
    int neg = 0;
    //identifier
    if (tok->type == TOKEN_IDENT){
        ope->ident.str = malloc(tok->length+1);
        memcpy(ope->ident.str, tok->text, tok->length);
        ope->ident.str[tok->length] = '\0';
        ope->ident.length = tok->length+1;
        ope->type = OPERAND_IDENT;
        return PARSER_OK;
    }

    //-x
    else if (tok->type == TOKEN_MINUS){
        *tok = lexer_next_token(lexer, &status);
        CHK_LEXER_STATUS(prog, status, tok);
        if (tok->type == TOKEN_NUMBER){
            ope->value = -tok->number;
            ope->type = OPERAND_VALUE;
            return PARSER_OK;
        }
        return PARSER_SYNTAX_ERROR;
    }

    //x
    else if (tok->type == TOKEN_NUMBER){
        ope->value = tok->number;
        ope->type = OPERAND_VALUE;
        return PARSER_OK;
    }

    //RX
    else if (tok->type == TOKEN_REGISTER){
        if (tok->reg > UINT8_MAX){
            return PARSER_INVALID_REGISTER;
        }
        ope->type = OPERAND_REG;
        ope->reg = (uint8_t) tok->reg;
        return PARSER_OK;
    }

    //[..]
    else if (tok->type == TOKEN_OPEN_BRACKET){
        *tok = lexer_next_token(lexer, &status);
        CHK_LEXER_STATUS(prog, status, tok);
        suiv = lexer_next_token(lexer, &status);
        CHK_LEXER_STATUS(prog, status, tok);

        if ((isEndOfInstr(tok) || isSyntaxElement(tok)) || isEndOfInstr(&suiv)){
            return PARSER_SYNTAX_ERROR;
        }
        //nombre
        if ((tok->type == TOKEN_MINUS) || (tok->type == TOKEN_NUMBER)){
            if (tok->type == TOKEN_MINUS){
                if (suiv.type != TOKEN_NUMBER){
                    memcpy(tok, &suiv, sizeof(struct Token));
                    return PARSER_SYNTAX_ERROR;
                }
                neg = 1;
            }
            ope->type = OPERAND_MEM_VALUE;
            ope->value = neg ? -tok->number : tok->number;
            if (suiv.type != TOKEN_CLOSE_BRACKET){
                memcpy(tok, &suiv, sizeof(struct Token));
                return PARSER_SYNTAX_ERROR;
            }
            return PARSER_OK;
        }
        else if (tok->type == TOKEN_IDENT){
            if ((suiv.type == TOKEN_MINUS) || (suiv.type == TOKEN_PLUS)){
                ope->type = OPERAND_MEM_IDX_IDENT;
                ope->ident.str = malloc(tok->length+1);
                memcpy(ope->mem.ident.str, tok->text, tok->length);
                ope->mem.ident.str[tok->length] = '\0';
                ope->mem.ident.length = tok->length+1;
                memcpy(tok, &suiv, sizeof(struct Token));
                neg = (tok->type == TOKEN_MINUS) ? 1 : 0;
            }
            else if (suiv.type != TOKEN_CLOSE_BRACKET){
                memcpy(tok, &suiv, sizeof(struct Token));
                return PARSER_SYNTAX_ERROR;
            }
            else {
                ope->type = OPERAND_MEM_IDENT;
                ope->ident.str = malloc(tok->length+1);
                memcpy(ope->ident.str, tok->text, tok->length);
                ope->ident.str[tok->length] = '\0';
                ope->ident.length = tok->length+1;
                
                *tok = lexer_next_token(lexer, &status);
                CHK_LEXER_STATUS(prog, status, tok);
                if (!isEndOfInstr(tok) && isSyntaxElement(tok)){
                    return PARSER_SYNTAX_ERROR;
                }
                else if (!isEndOfInstr(tok)){
                    return PARSER_TOO_MANY_OPE;
                }
                return PARSER_OK;
            }
        } 
        else if (tok->type == TOKEN_REGISTER){
            if ((suiv.type == TOKEN_MINUS) || (suiv.type == TOKEN_PLUS)){
                ope->type = OPERAND_MEM_IDX;
                ope->mem.base_reg = tok->reg;
                memcpy(tok, &suiv, sizeof(struct Token));
                neg = (tok->type == TOKEN_MINUS) ? 1 : 0;
            }
            else if (suiv.type != TOKEN_CLOSE_BRACKET){
                memcpy(tok, &suiv, sizeof(struct Token));
                return PARSER_SYNTAX_ERROR;
            }
            else {
                ope->type = OPERAND_MEM_REG;
                ope->reg = tok->reg;
                memcpy(tok, &suiv, sizeof(struct Token));
                return PARSER_OK;
            }
        }

        *tok = lexer_next_token(lexer, &status);
        if (tok->type != TOKEN_NUMBER){
            return PARSER_SYNTAX_ERROR;
        }
        ope->mem.offset = neg ? -tok->number : tok->number;

        *tok = lexer_next_token(lexer, &status);
        if (tok->type != TOKEN_CLOSE_BRACKET){
            return PARSER_SYNTAX_ERROR;
        }
    }
    else {
        return PARSER_INVALID_ARGS;
    }
    return PARSER_OK;
}


ParserError parse_instr(char *prog, struct Lexer *lexer, struct IR_Node *IR, struct Token *tok){
    LexerError status;
    ParserError pstatus;
    IR->instr.op = op_lookup(tok->text, tok->length);
    if (IR->instr.op == OP_COUNT){
        return PARSER_UNKNOWN_INSTR;
    }

    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);

    //Operand A
    if (isEndOfInstr(tok)){
        IR->instr.a.type = OPERAND_NONE;
        IR->instr.b.type = OPERAND_NONE;
        return PARSER_OK;
    }
    else {
        pstatus = parse_instrOPE(prog, lexer, &IR->instr.a, tok);
        CHK_PARSER_STATUS(prog, pstatus, tok);
    }

    //expected , ou la fin
    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
    if (!isEndOfInstr(tok) && (tok->type != TOKEN_COMMA)){
        if (isSyntaxElement(tok)){
            return PARSER_SYNTAX_ERROR;
        }
        return PARSER_TOO_MANY_OPE;
    }
    else if (isEndOfInstr(tok)){
        IR->instr.b.type = OPERAND_NONE;
        return PARSER_OK;
    }

    //Operand B
    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
    pstatus = parse_instrOPE(prog, lexer, &IR->instr.b, tok);
    CHK_PARSER_STATUS(prog, pstatus, tok);
    
    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
    if (!isEndOfInstr(tok) && isSyntaxElement(tok)){
        return PARSER_SYNTAX_ERROR;
    }
    else if (!isEndOfInstr(tok)){
        return PARSER_TOO_MANY_OPE;
    }
    return PARSER_OK;
}

ParserError parse_line(char *prog, char *buffer, struct IR_Node *IR, struct Lexer *lexer, struct Token *tok){

    LexerError lstatus;
    ParserError pstatus;
    lexer->cursor = buffer;
    *tok = lexer_next_token(lexer, &lstatus);
    CHK_LEXER_STATUS(prog, lstatus, tok);
    if (isEndOfInstr(tok)){
        return PARSER_EMPTY;
    }
    if ((tok->type != TOKEN_DOT) && isSyntaxElement(tok)){
        return PARSER_SYNTAX_ERROR;
    }
    else if (isEndOfInstr(tok)){
        return PARSER_OK;
    }
    switch (tok->type){
        case TOKEN_DOT:IR->type = IR_DIR; pstatus = parse_dir(prog, lexer, IR, tok); break;
        case TOKEN_IDENT:IR->type = IR_LABEL; pstatus = parse_label(prog, lexer, IR, tok); break;
        case TOKEN_MNEMONIC:IR->type = IR_INSTR; pstatus = parse_instr(prog, lexer, IR, tok); break;
        default: return PARSER_UNKNOWN_INSTR;
    }

    CHK_PARSER_STATUS(prog, pstatus, tok);
    IR->line = tok->line+1;
    return PARSER_OK;
}


struct IR_Node *parser(char *prog, size_t *n){

    FILE *file = fopen(prog, "r");

    if (!file){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    size_t prog_size = 128;
    size_t i = 0;
    struct IR_Node *IRList = malloc(prog_size * sizeof(*IRList));
    struct IR_Node *tmp;
    //+1 for \0
    char str[INSTR_LEN + 1] = {0};
    ParserError status;
    struct Lexer lexer = {.cursor=str, .line=0, .column=0};
    struct Token tok = {0};

    while (fgets(str, INSTR_LEN, file)){
        lexer.cursor = str;
        if (i == prog_size){
            tmp = realloc(IRList, 2 * prog_size);
            prog_size *= 2;
            if (!tmp){
                free(IRList);
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
        status = parse_line(prog, str, &IRList[i], &lexer, &tok);
        if (status != PARSER_EMPTY){
            i++;
        }
        else {
            memset(&IRList[i], 0, sizeof(struct IR_Node));
        }
        lexer.line++;
        lexer.column = 0;
    }
    *n = i;
    if (fclose(file) != 0){
        perror("fclose");
        exit(EXIT_FAILURE);
    }
    return IRList;
}

#ifdef TEST
int main(int argc, char *argv[]){

    size_t prog_size = 0;
    struct IR_Node *IRList = parser(argv[1], &prog_size);
    printf("prog size : %lu\n", prog_size);
    for (int i = 0; i < prog_size; i++){
        printIRNode(&IRList[i]);
        free_IRNode(&IRList[i]);
    }
    printf("PARSER OK\n");

    free(IRList);
    return 0;
}
#endif