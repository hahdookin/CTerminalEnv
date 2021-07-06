#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "utils.h" // Defines BUF_SIZE
#include "vector.h"

#define MULTI_LINE  1
#define SINGLE_LINE 2

extern char *keywords[32];

typedef enum Token {
    // Literal consts
    ICONST, FCONST, SCONST, CCONST,

    // Different number constants
    HEXCONST, BINCONST,

    // Identifier
    IDENT,

    // Keywords
    AUTO     ,  BREAK    ,  CASE     ,  CHAR,
    CONST    ,  CONTINUE ,  DEFAULT  ,  DO,
    DOUBLE   ,  ELSE     ,  ENUM     ,  EXTERN,
    FLOAT    ,  FOR      ,  GOTO     ,  IF,
    INT      ,  LONG     ,  REGISTER ,  RETURN,
    SHORT    ,  SIGNED   ,  SIZEOF   ,  STATIC,
    STRUCT   ,  SWITCH   ,  TYPEDEF  ,  UNION,
    UNSIGNED ,  VOID     ,  VOLATILE ,  WHILE,

    // Arithmetic operators
    PLUS, MINUS, MULT, DIV,
    ASSOP, MODULO,

    // Compound operators
    PLUSEQ, MINUSEQ, MULTEQ, DIVEQ,
    MODULOEQ, INCRE, DECRE,

    // Bitwise operators
    BWAND, BWOR, BWXOR, BWNOT,
    BWLSHIFT, BWRSHIFT,

    // Compound bitwise operators
    BWANDEQ, BWOREQ, BWXOREQ, BWNOTEQ,
    BWLSHIFTEQ, BWRSHIFTEQ,

    // Logical operators
    EQUAL, NEQUAL,
    LTHAN, GTHAN,
    LTHANEQ, GTHANEQ,
    AND, OR, NOT,

    // Parens, colons, semicolons, etc
    LPAREN, RPAREN, // ()
    LBRACK, RBRACK, // []
    LBRACE, RBRACE, // {}
    COLON, SEMICOLON, COMMA,
    ARROW, DOT,

    // Misc. tokens
    COMMENT, ERR, DONE,
} Token;

/* LexItem */
typedef struct {
    Token token;
    size_t line;
    size_t col;
    char lexeme[BUF_SIZE];
} LexItem;

typedef struct {
    vec_char **lines;
    size_t line_count;

    // Current position
    size_t line;
    size_t col;
} FileVec;

typedef struct {
    vec_char *line;
    size_t col;
    size_t line_num;
} LineInfo;

// Checks if a character can be ignored by lexer (newline, return, space)
extern bool ignorable(char c);

// Returns the Token for the corresponding keyword
extern Token get_kw_tok(char *kw);

// Determines if a lexeme is an identifier or a keyword
extern LexItem id_or_kw(const char *lexeme);

// Debugging string repr of token
extern char *tokenstr(const LexItem *li);

/* Lexer */
//extern LexItem fgetNextToken(FILE *in);
extern LexItem get_next_token(FileVec *fv);

// FileVec stream processing
extern char vgetc(FileVec *fv);
extern void vungetc(FileVec *fv);
extern char vpeekc(FileVec *fv);

#endif /* LEXER_H */
