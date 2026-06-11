#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>


struct Token lexer_next_token(struct Lexer *lexer, LexerError *status){

    while ((*lexer->cursor == ' ') || (*lexer->cursor == '\t')){
        lexer->cursor++;
        lexer->column++;
    }
    size_t len = 0;
    const char *start = lexer->cursor;
    size_t column = 0;
    uint64_t reg = 0;
    int64_t value = 0;

    //ponctuation
    switch (*lexer->cursor){
        case '.':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_DOT, .line=lexer->line, .column=lexer->column++};
        case ',':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_COMMA, .line=lexer->line, .column=lexer->column++};
        case '[':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_OPEN_BRACKET, .line=lexer->line, .column=lexer->column++};
        case ']':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_CLOSE_BRACKET, .line=lexer->line, .column=lexer->column++};
        case ':':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_COLON, .line=lexer->line, .column=lexer->column++};
        case '+':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_PLUS, .line=lexer->line, .column=lexer->column++};
        case '-':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_MINUS, .line=lexer->line, .column=lexer->column++};
        case '\n':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_NEWLINE, .line=lexer->line, .column=lexer->column++};
        case '\0':
            lexer->cursor++;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_EOF, .line=lexer->line, .column=lexer->column++};
    }
    //register
    if (*lexer->cursor == 'R'){
        lexer->cursor++;
        while (*lexer->cursor && isdigit(*lexer->cursor)){
            if ((__int128_t)(reg * 10) > INT64_MAX){
                *status = LEXER_INVALID_TOKEN;
                lexer->column += len+2;
                return (struct Token) {.type=TOKEN_EOF, .line=lexer->line, .column=lexer->column};
            }
            reg = reg * 10 + (*lexer->cursor - '0');
            lexer->cursor++;
            len++;
        }
        if (len > 0){
            column = lexer->column;
            lexer->column += len+1;
            *status = LEXER_OK;
            return (struct Token) {.type=TOKEN_REGISTER, .reg=reg, .line=lexer->line, .column=column};
        }
        lexer->cursor--;
    }
    //nombre
    if (isdigit(*lexer->cursor)){
        value = (*lexer->cursor - '0'); 
        lexer->cursor++;
        while (*lexer->cursor && isdigit(*lexer->cursor)){
            if ((__int128_t)(value * 10) > INT64_MAX){
                *status = LEXER_INVALID_TOKEN;
                lexer->column += len+2;
                return (struct Token) {.type=TOKEN_EOF, .line=lexer->line, .column=lexer->column};
            }
            value = value * 10 + (*lexer->cursor - '0');
            len++;
            lexer->cursor++;
        }
        column = lexer->column;
        lexer->column += len+1;
        *status = LEXER_OK;
        return (struct Token) {.type=TOKEN_NUMBER, .number=value, .line=lexer->line, .column=column};
    }

    //string
    if (*lexer->cursor == '\"'){
        start++;
        lexer->cursor++;
        while (*lexer->cursor && (*lexer->cursor != '\"')){
            *lexer->cursor++;
            len++;
        }
        if (*lexer->cursor != '\"'){
            *status = LEXER_ERROR_MISSING_QUOTE;
            lexer->column += len+1;
            return (struct Token) {.type=TOKEN_EOF, .line=lexer->line, .column=lexer->column};
        }
        lexer->cursor++;
        column = lexer->column;
        lexer->column += len + 2;
        *status = LEXER_OK;
        return (struct Token) {.type=TOKEN_STRING, .text=start, .length=len, .line=lexer->line, .column=column};
    }

    //identifier
    if (islower(*lexer->cursor)){
        lexer->cursor++;
        len++;
        while (*lexer->cursor && ((isalnum(*lexer->cursor)) || (*lexer->cursor == '_'))){
            lexer->cursor++;
            len++;
        }
        column = lexer->column;
        lexer->column += len;
        *status = LEXER_OK;
        return (struct Token) {.type=TOKEN_IDENT, .text=start, .length=len, .line=lexer->line, .column=column};
    }

    //mnemonic
    if (isupper(*lexer->cursor)){
        lexer->cursor++;
        len++;
        while (*lexer->cursor && isupper(*lexer->cursor)){
            lexer->cursor++;
            len++;
        }
        column = lexer->column;
        lexer->column += len;
        *status = LEXER_OK;
        return (struct Token) {.type=TOKEN_MNEMONIC, .text=start, .length=len, .line=lexer->line, .column=column};
    }
    *status = LEXER_INVALID_TOKEN;
    return (struct Token) {.type=TOKEN_EOF, .line=lexer->line, .column=lexer->column};
}

void printToken(struct Token *tok){
    char *str = NULL;
    switch (tok->type){
        case TOKEN_CLOSE_BRACKET: str = "CLOSE_BRACKET"; break;
        case TOKEN_OPEN_BRACKET: str = "OPEN_BRACKET"; break;
        case TOKEN_COMMA: str = "COMMA"; break;
        case TOKEN_DOT: str = "DOT"; break;
        case TOKEN_COLON: str = "COLON"; break;
        case TOKEN_MINUS: str = "MOINS"; break;
        case TOKEN_PLUS: str = "PLUS"; break;
        case TOKEN_NEWLINE: str = "NEWLINE"; break;
        case TOKEN_EOF: str = "EOF"; break;
        case TOKEN_NUMBER: printf("NUMBER(%ld)\n", tok->number); break;
        case TOKEN_REGISTER: printf("REG(%lu)\n", tok->reg); break;
        case TOKEN_STRING: printf("STR(");
            for(size_t i = 0; i < tok->length; i++){
                printf("%c", tok->text[i]);
            }
            printf(")\n");
            break;
        case TOKEN_MNEMONIC: printf("MNEMONIC(");
            for(size_t i = 0; i < tok->length; i++){
                printf("%c", tok->text[i]);
            }
            printf(")\n");
            break;
        case TOKEN_IDENT: printf("IDENTIFIER(");
            for(size_t i = 0; i < tok->length; i++){
                printf("%c", tok->text[i]);
            }
            printf(")\n");
            break;
    }
    if (str){
        printf("%s\n", str);
    }
}

#ifdef TEST
int main(){

    char buffer[256];
    struct Token tok;
    LexerError status;
    struct Lexer lexer = {.column=0, .line=0, .cursor=buffer};
    while (fgets(buffer, 256, stdin)){
        lexer.cursor = buffer;
        tok = lexer_next_token(&lexer, &status);
        while ((status == LEXER_OK) && (tok.type != TOKEN_NEWLINE) && (tok.type != TOKEN_EOF)){
            printToken(&tok);
            tok = lexer_next_token(&lexer, &status);
        }
        if (status != LEXER_OK){
            printf("error : %lu.%lu : invalid token\n", tok.line+1, tok.column+1);
        }
        lexer.column = 0;
        lexer.line++;
        printf("NEWLINE implicite\n");
        memset(buffer, 0, 256);
    }
    return 0;
}
#endif