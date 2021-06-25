#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "utils.h"

#define MULTI_LINE  1
#define SINGLE_LINE 2

typedef enum Token {
    // Literal consts
    ICONST, FCONST, SCONST, CCONST,

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

    // Misc. tokens
    COMMENT, ERR, DONE,
} Token;

/* Buffer for storing lexemes */
#define BUF_SIZE 24

/* LexItem */
typedef struct {
    Token token;
    char lexeme[BUF_SIZE];
} LexItem;

bool ignorable(char c)
{
    return c == ' ' || c == '\n' || c == '\r';
}

char *keywords[32] = {
    "auto"     ,  "break"    ,  "case"     ,  "char",
    "const"    ,  "continue" ,  "default"  ,  "do",
    "double"   ,  "else"     ,  "enum"     ,  "extern",
    "float"    ,  "for"      ,  "goto"     ,  "if",
    "int"      ,  "long"     ,  "register" ,  "return",
    "short"    ,  "signed"   ,  "sizeof"   ,  "static",
    "struct"   ,  "switch"   ,  "typedef"  ,  "union",
    "unsigned" ,  "void"     ,  "volatile" ,  "while",
};

Token get_kw_tok(char* kw)
{
    if (!strcmp(kw, "auto"))     return AUTO;
    if (!strcmp(kw, "break"))    return BREAK;
    if (!strcmp(kw, "case"))     return CASE;
    if (!strcmp(kw, "char"))     return CHAR;
    if (!strcmp(kw, "const"))    return CONST;
    if (!strcmp(kw, "continue")) return CONTINUE;
    if (!strcmp(kw, "default"))  return DEFAULT;
    if (!strcmp(kw, "do"))       return DO;
    if (!strcmp(kw, "double"))   return DOUBLE;
    if (!strcmp(kw, "else"))     return ELSE;
    if (!strcmp(kw, "enum"))     return ENUM;
    if (!strcmp(kw, "extern"))   return EXTERN;
    if (!strcmp(kw, "float"))    return FLOAT;
    if (!strcmp(kw, "for"))      return FOR;
    if (!strcmp(kw, "goto"))     return GOTO;
    if (!strcmp(kw, "if"))       return IF;
    if (!strcmp(kw, "int"))      return INT;
    if (!strcmp(kw, "long"))     return LONG;
    if (!strcmp(kw, "register")) return REGISTER;
    if (!strcmp(kw, "return"))   return RETURN;
    if (!strcmp(kw, "short"))    return SHORT;
    if (!strcmp(kw, "signed"))   return SIGNED;
    if (!strcmp(kw, "sizeof"))   return SIZEOF;
    if (!strcmp(kw, "static"))   return STATIC;
    if (!strcmp(kw, "struct"))   return STRUCT;
    if (!strcmp(kw, "switch"))   return SWITCH;
    if (!strcmp(kw, "typedef"))  return TYPEDEF;
    if (!strcmp(kw, "union"))    return UNION;
    if (!strcmp(kw, "unsigned")) return UNSIGNED;
    if (!strcmp(kw, "void"))     return VOID;
    if (!strcmp(kw, "volatile")) return VOLATILE;
    if (!strcmp(kw, "while"))    return WHILE;

    return ERR;
}

LexItem id_or_kw(const char* lexeme)
{
    LexItem li;
    strcpy(li.lexeme, lexeme);
    li.token = IDENT;
    for (int i = 0; i < LEN(keywords); i++)
    {
        if (strcmp(keywords[i], lexeme) == 0)
        {
            li.token = get_kw_tok(keywords[i]);
        }
    }
    return li;
}

void printlexitem(const LexItem* li)
{
    char* tokenstr;
    switch (li->token)
    {
        case FOR:
            tokenstr = "FOR";   break;
        case IF:
            tokenstr = "IF";    break;
        case WHILE:
            tokenstr = "WHILE"; break;
        case ELSE:
            tokenstr = "ELSE";  break;
        case INT:
            tokenstr = "INT";   break;
        case FLOAT:
            tokenstr = "FLOAT"; break;
        case CHAR:
            tokenstr = "CHAR";  break;
        default:
            tokenstr = "IDENT"; break;
    }
    printf("%s(%s)\n", tokenstr, li->lexeme);
}


/* Lexer */
#if 0
LexItem vgetNextToken(FILE *in)
{
    enum TokState {
        START, INID, INCOMMENT,
        ININT, INFLOAT, 
        INCHAR, INSTRING,
    } lexstate = START;

    LexItem li;
    char c, peek, lexeme[BUF_SIZE];
    int pos = 0, comment_type = 0;

    // Grab a char
    while ((c = fgetc(in)) != EOF)
    {
        // Grab the peek as well
        peek = peekch(in);

        switch (lexstate)
        {
        case START:
            if (ignorable(c))
                continue;
            
            // Add char to the lexeme
            lexeme[pos++] = c;

            if (isalpha(c))
                lexstate = INID;
            else if (c == '"')
                lexstate = INSTRING;
            else if (c == '\'')
                lexstate = INCHAR;
            else if (isdigit(c))
                lexstate = ININT;
            else if (c == '/' && (peek == '*' || peek == '/'))
            {
                ungetc(c, in);
                lexstate = INCOMMENT;
            }
            else
            {
                Token t = ERR;
                switch (c)
                {
                case '+':                  // plus
                    if (peek == '=')       // plus compound assign
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = PLUSEQ;
                        break;
                    }
                    else if (peek == '+')  // increment
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = INCRE;
                        break;
                    }
                    t = PLUS;
                    break;
                case '-':                  // minus
                    if (peek == '=')       // minus compound assign
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = MINUSEQ;
                        break;
                    }
                    else if (peek == '-')  // decrement
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = DECRE;
                        break;
                    }
                    t = MINUS;
                    break;
                case '*':                  // mult
                    if (peek == '=')       // mult compound assign
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = MULTEQ;
                        break;
                    }
                    t = MULT;
                    break;
                case '/':                  // div
                    if (peek == '=')       // div compound assign
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = DIVEQ;
                        break;
                    }
                    t = DIV;
                    break;
                case '=':                  // assignment
                    if (peek == '=')       // equality
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = EQUAL;
                        break;
                    }
                    t = ASSOP;
                    break;
                case '<':                  // less than
                    if (peek == '=')       // less than or equal
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = LTHANEQ;
                        break;
                    }
                    else if (peek == '<')  // bitwise left shift
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = BWLSHIFT;
                        break;
                    }
                    t = LTHAN;
                    break;
                case '>':                  // greater than
                    if (peek == '=')       // greater than or equal
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = GTHANEQ;
                        break;
                    }
                    else if (peek == '>')  // bitwise right
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = BWRSHIFT;
                        break;
                    }
                    t = GTHAN;
                    break;
                case '!':                  // not
                    if (peek == '=')       // not equal
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = NEQUAL;
                        break;
                    }
                    t = NOT;
                    break;
                case '~':                  // bitwise not
                    t = BWNOT;
                    break;
                case '&':                  // bitwise and
                    if (peek == '&')       // and
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = AND;
                        break;
                    }
                    t = BWAND;
                    break;
                case '|':                  // bitwise or
                    if (peek == '|')       // or
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = OR;
                        break;
                    }
                    t = BWOR;
                    break;
                case '(':                  // left paren
                    t = LPAREN;
                    break;
                case ')':                  // right paren
                    t = RPAREN;
                    break;
                case '[':                  // left bracket
                    t = LBRACK;
                    break;
                case ']':                  // right bracket
                    t = RBRACK;
                    break;
                case '{':                  // left brace
                    t = LBRACE;
                    break;
                case '}':                  // right brace
                    t = RBRACE;
                    break;
                case ':':                  // colon
                    t = COLON;
                    break;
                case ';':                  // semi-colon
                    t = SEMICOLON;
                    break;
                case ',':                  // comma
                    t = COMMA;
                    break;
                }
                li.token = t;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INID:
            if (isalpha(c) || isdigit(c) || c == '_')
            {
                lexeme[pos++] = c;
            }
            else 
            {
                ungetc(c, in);
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return id_or_kw(lexeme);
            }
            break;

        case INSTRING:
            if (c == '\n') 
            {
                li.token = ERR;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            lexeme[pos++] = c;
            if (c == '"')
            {
                // Need to remove the beginning and ending '"'
                li.token = SCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INCHAR:
            if (isalpha(c) && peek == '\'')
            {
                lexeme[pos++] = c;
                c = fgetc(in);
                lexeme[pos++] = c;
                li.token = CCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            else if (c == '\\')
            {
                lexeme[pos++] = c; // add '\'
                c = fgetc(in); // char after '\'
                peek = peekch(in); // closing '
                if (peek != '\'')
                {
                    li.token = ERR;
                    lexeme[pos] = 0;
                    return li;
                }
                lexeme[pos++] = c;
                c = fgetc(in);
                lexeme[pos++] = c;
                lexeme[pos] = 0;
                li.token = CCONST;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            else
            {
                li.token = ERR;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case ININT:
            if (isdigit(c))
            {
                lexeme[pos++] = c;
            }
            else if (c == '.')
            {
                ungetc(c, in);
                lexstate = INFLOAT;
            } 
            else 
            {
                ungetc(c, in);
                li.token = ICONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INFLOAT:
            if (c == '.' && isdigit(peek))
            {
                lexeme[pos++] = c;
            }
            else if (isdigit(c))
            {
                lexeme[pos++] = c;
            }
            else if (c == '.' && !isdigit(peek))
            {
                li.token = ERR;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            else
            {
                ungetc(c, in);
                li.token = FCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INCOMMENT:
            if (c == '/' && !comment_type)
            {
                /* Multi-line */
                if (peek == '*')
                    comment_type = MULTI_LINE; 
                // Single line
                if (peek == '/')
                    comment_type = SINGLE_LINE;
                c = fgetc(in);
            }
            if (comment_type == MULTI_LINE)
            {
                if (c == '*' && peek == '/')
                {
                    c = fgetc(in);
                    li.token = COMMENT;
                    strcpy(li.lexeme, "MULIT-LINE-COMMENT");
                    return li;
                }
            }
            if (comment_type == SINGLE_LINE)
            {
                if (c == '\n')
                {
                    c = fgetc(in);
                    li.token = COMMENT;
                    strcpy(li.lexeme, "SINGLE-LINE-COMMENT");
                    return li;
                }
            }
            break;

        }// end of while

    }

    if (feof(in))
    {
        li.token = DONE;
        strcpy(li.lexeme, "");
        return li;
    }


    return li;
}
#endif

//vec_(char);

#if 0
LexItem getNextToken(vec_char* buf)
{
    enum TokState {
        START, INID, INCOMMENT,
        ININT, INFLOAT, 
        INCHAR, INSTRING,
    } lexstate = START;

    LexItem li;
    char c, peek, lexeme[BUF_SIZE];
    int pos = 0, comment_type = 0;

    // Grab a char
    while ((c = fgetc(in)) != EOF)
    {
        // Grab the peek as well
        peek = peekch(in);

        switch (lexstate)
        {
        case START:
            if (ignorable(c))
                continue;
            
            // Add char to the lexeme
            lexeme[pos++] = c;

            if (isalpha(c))
                lexstate = INID;
            else if (c == '"')
                lexstate = INSTRING;
            else if (c == '\'')
                lexstate = INCHAR;
            else if (isdigit(c))
                lexstate = ININT;
            else if (c == '/' && (peek == '*' || peek == '/'))
            {
                ungetc(c, in);
                lexstate = INCOMMENT;
            }
            else
            {
                Token t = ERR;
                switch (c)
                {
                case '+':                  // plus
                    if (peek == '=')       // plus compound assign
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = PLUSEQ;
                        break;
                    }
                    else if (peek == '+')  // increment
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = INCRE;
                        break;
                    }
                    t = PLUS;
                    break;
                case '-':                  // minus
                    if (peek == '=')       // minus compound assign
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = MINUSEQ;
                        break;
                    }
                    else if (peek == '-')  // decrement
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = DECRE;
                        break;
                    }
                    t = MINUS;
                    break;
                case '*':                  // mult
                    if (peek == '=')       // mult compound assign
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = MULTEQ;
                        break;
                    }
                    t = MULT;
                    break;
                case '/':                  // div
                    if (peek == '=')       // div compound assign
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = DIVEQ;
                        break;
                    }
                    t = DIV;
                    break;
                case '=':                  // assignment
                    if (peek == '=')       // equality
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = EQUAL;
                        break;
                    }
                    t = ASSOP;
                    break;
                case '<':                  // less than
                    if (peek == '=')       // less than or equal
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = LTHANEQ;
                        break;
                    }
                    else if (peek == '<')  // bitwise left shift
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = BWLSHIFT;
                        break;
                    }
                    t = LTHAN;
                    break;
                case '>':                  // greater than
                    if (peek == '=')       // greater than or equal
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = GTHANEQ;
                        break;
                    }
                    else if (peek == '>')  // bitwise right
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = BWRSHIFT;
                        break;
                    }
                    t = GTHAN;
                    break;
                case '!':                  // not
                    if (peek == '=')       // not equal
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = NEQUAL;
                        break;
                    }
                    t = NOT;
                    break;
                case '~':                  // bitwise not
                    t = BWNOT;
                    break;
                case '&':                  // bitwise and
                    if (peek == '&')       // and
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = AND;
                        break;
                    }
                    t = BWAND;
                    break;
                case '|':                  // bitwise or
                    if (peek == '|')       // or
                    {
                        c = fgetc(in);
                        lexeme[pos++] = c;
                        t = OR;
                        break;
                    }
                    t = BWOR;
                    break;
                case '(':                  // left paren
                    t = LPAREN;
                    break;
                case ')':                  // right paren
                    t = RPAREN;
                    break;
                case '[':                  // left bracket
                    t = LBRACK;
                    break;
                case ']':                  // right bracket
                    t = RBRACK;
                    break;
                case '{':                  // left brace
                    t = LBRACE;
                    break;
                case '}':                  // right brace
                    t = RBRACE;
                    break;
                case ':':                  // colon
                    t = COLON;
                    break;
                case ';':                  // semi-colon
                    t = SEMICOLON;
                    break;
                case ',':                  // comma
                    t = COMMA;
                    break;
                }
                li.token = t;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INID:
            if (isalpha(c) || isdigit(c) || c == '_')
            {
                lexeme[pos++] = c;
            }
            else 
            {
                ungetc(c, in);
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return id_or_kw(lexeme);
            }
            break;

        case INSTRING:
            if (c == '\n') 
            {
                li.token = ERR;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            lexeme[pos++] = c;
            if (c == '"')
            {
                // Need to remove the beginning and ending '"'
                li.token = SCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INCHAR:
            if (isalpha(c) && peek == '\'')
            {
                lexeme[pos++] = c;
                c = fgetc(in);
                lexeme[pos++] = c;
                li.token = CCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            else if (c == '\\')
            {
                lexeme[pos++] = c; // add '\'
                c = fgetc(in); // char after '\'
                peek = peekch(in); // closing '
                if (peek != '\'')
                {
                    li.token = ERR;
                    lexeme[pos] = 0;
                    return li;
                }
                lexeme[pos++] = c;
                c = fgetc(in);
                lexeme[pos++] = c;
                lexeme[pos] = 0;
                li.token = CCONST;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            else
            {
                li.token = ERR;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case ININT:
            if (isdigit(c))
            {
                lexeme[pos++] = c;
            }
            else if (c == '.')
            {
                ungetc(c, in);
                lexstate = INFLOAT;
            } 
            else 
            {
                ungetc(c, in);
                li.token = ICONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INFLOAT:
            if (c == '.' && isdigit(peek))
            {
                lexeme[pos++] = c;
            }
            else if (isdigit(c))
            {
                lexeme[pos++] = c;
            }
            else if (c == '.' && !isdigit(peek))
            {
                li.token = ERR;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            else
            {
                ungetc(c, in);
                li.token = FCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INCOMMENT:
            if (c == '/' && !comment_type)
            {
                /* Multi-line */
                if (peek == '*')
                    comment_type = MULTI_LINE; 
                // Single line
                if (peek == '/')
                    comment_type = SINGLE_LINE;
                c = fgetc(in);
            }
            if (comment_type == MULTI_LINE)
            {
                if (c == '*' && peek == '/')
                {
                    c = fgetc(in);
                    li.token = COMMENT;
                    strcpy(li.lexeme, "MULIT-LINE-COMMENT");
                    return li;
                }
            }
            if (comment_type == SINGLE_LINE)
            {
                if (c == '\n')
                {
                    c = fgetc(in);
                    li.token = COMMENT;
                    strcpy(li.lexeme, "SINGLE-LINE-COMMENT");
                    return li;
                }
            }
            break;

        }// end of while

    }

    if (feof(in))
    {
        li.token = DONE;
        strcpy(li.lexeme, "");
        return li;
    }


    return li;
}
#endif

#endif /* LEXER_H */
