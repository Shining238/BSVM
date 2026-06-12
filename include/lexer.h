#pragma once
#include <stdlib.h>
#include <stdint.h>

typedef enum {
    TOKEN_OPEN_BRACKET,
    TOKEN_CLOSE_BRACKET,
    
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_COLON,
    
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_IDENT,
    TOKEN_REGISTER,
    TOKEN_MNEMONIC,

    TOKEN_PLUS,
    TOKEN_MINUS,

    TOKEN_NEWLINE,
    TOKEN_EOF
} TokenType;

struct Token{
    TokenType type;
    union {
        int64_t number;
        uint64_t reg;
        struct {
            char *text;
            size_t length;
        };
    };
    size_t line;
    size_t column;
};

typedef enum {
    LEXER_ERROR_MISSING_QUOTE,
    LEXER_INVALID_TOKEN,
    LEXER_UNKNOWN_ESCAPE_SEQUENCE,
    LEXER_OK
} LexerError;

struct Lexer {
    char *cursor;

    size_t line;
    size_t column;

};

struct Token lexer_next_token(struct Lexer *lexer, LexerError *status);

void printToken(struct Token *tok);