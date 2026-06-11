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
    if (status != PARSER_OK){\
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
};


static const DirMap dir_map[DIR_COUNT] = {
    {.mnemonic="string", .dir=DIR_STRING},
    {.mnemonic="addr", .dir=DIR_ADDR},
    {.mnemonic="org", .dir=DIR_ORG},
    {.mnemonic="word", .dir=DIR_WORD}
};

void free_IRNode(struct IR_Node *IR){
    if (IR->type == IR_DIR){
        if ((IR->directive.string.type == OPERAND_STRING) || (IR->directive.string.type == OPERAND_LABEL)){
            free(IR->directive.string.str);
        }
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
        case PARSER_TOO_MANY_OPE: printf("%s:%lu.%lu : too many arguments\n", prog, tok->line, tok->column); break;
        case PARSER_SYNTAX_ERROR: printf("%s:%lu.%lu : syntax error\n", prog, tok->line, tok->column); break;
        case PARSER_TOO_FEW_OPE: printf("%s:%lu.%lu : too few arguments\n", prog, tok->line, tok->column); break;
        case PARSER_UNKNOWN_INSTR: printf("%s:%lu.%lu : unknown instruction\n", prog, tok->line, tok->column); break;
        case PARSER_UNKNOWN_DIR: printf("%s:%lu.%lu : unknown directive\n", prog, tok->line, tok->column); break;
        case PARSER_UNKNOWN_OP: printf("%s:%lu.%lu : unknown operation\n", prog, tok->line, tok->column); break;
        default: break;
    }
}

void printLexerStatus(char *prog, LexerError status, struct Token *tok){
    switch (status){
        case LEXER_OK: printf("%s OK\n", prog); break;
        case LEXER_ERROR_MISSING_QUOTE: printf("%s:%lu.%lu : missing quote\n", prog, tok->line, tok->column); break;
        case LEXER_INVALID_TOKEN: printf("%s:%lu.%lu : invalid token\n", prog, tok->line, tok->column); break;
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
    CHK_LEXER_STATUS(prog, status, prog);
    if (tok->type == TOKEN_NUMBER){
        IR->directive.value.value = tok->number;
    }
    else if ((tok->type == TOKEN_IDENT) || (tok->type == TOKEN_STRING)){
        IR->directive.string.str = tok->text;
    }
    else if (isEndOfInstr(tok)){
        return PARSER_TOO_FEW_OPE;
    }
    else {
        return PARSER_SYNTAX_ERROR;
    }

    *tok = lexer_next_token(lexer, &status);
    CHK_LEXER_STATUS(prog, status, tok);
    if (!isEndOfInstr(tok) && isSyntaxElement(tok)){
        return PARSER_SYNTAX_ERROR;
    }
    else if (!isEndOfInstr){
        return PARSER_TOO_MANY_OPE;
    }
    else {
        return PARSER_OK;
    }
}

ParserError parse_label(char *prog, struct Lexer *lexer, struct IR_Node *IR, struct Token *tok){
    LexerError status;
    *tok = lexer_next_token(lexer, status);
    CHK_LEXER_STATUS(prog, status, tok);
    
    if (tok->type == TOKEN_COLON){
        return PARSER_OK;
    }
    else if (isEndOfInstr(tok)){
        return PARSER_SYNTAX_ERROR;
    }
    else {    
        return PARSER_TOO_MANY_OPE;
    }
}

ParserError parse_instr(char prog, struct Lexer *lexer, struct IR_Node *IR, struct Token *tok);

ParserError parse_line(char *prog, char *buffer, struct IR_Node *IR, struct Lexer *lexer){

    struct Token *tok;
    LexerError lstatus;
    ParserError pstatus;
    lexer->cursor = buffer;
    *tok = lexer_next_token(lexer, lstatus);
    CHK_LEXER_STATUS(prog, lstatus, tok);
    if (isSyntaxElement(tok)){
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
    size_t line = 0;
    struct IR_Node *IRList = malloc(prog_size * sizeof(*IRList));
    struct IR_Node *tmp;
    //+1 for \0
    char str[INSTR_LEN + 1] = {0};
    ParserError status;

    while (fgets(str, INSTR_LEN, file)){
        if (i == prog_size){
            tmp = realloc(IRList, 2 * prog_size);
            prog_size *= 2;
            if (!tmp){
                free(IRList);
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
        i++;
    }
    *n = i;
    if (fclose(file) != 0){
        perror("fclose");
        exit(EXIT_FAILURE);
    }
    return IRList;
}