#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "lexer.h"
#include "utils.h"
#include "vector.h"

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

// Checks if a character can be ignored by lexer (newline, return, space)
bool ignorable(char c)
{
    return c == ' ' || c == '\n' || c == '\r';
}

// Returns the Token for the corresponding keyword
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

// Determines if a lexeme is an identifier or a keyword
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

// Debugging string repr of token
char* tokenstr(const LexItem* li)
{
    switch (li->token)
    {
        case ICONST:    return "ICONST";
        case FCONST:    return "FCONST";
        case SCONST:    return "SCONST";
        case CCONST:    return "CCONST";
        case HEXCONST:  return "HEXCONST";
        case BINCONST:  return "BINCONST";
        case IDENT:     return "IDENT";
        case AUTO:      return "AUTO";
        case BREAK:     return "BREAK";
        case CASE:      return "CASE";
        case CHAR:      return "CHAR";
        case CONST:     return "CONST";
        case CONTINUE:  return "CONTINUE";
        case DEFAULT:   return "DEFAULT";
        case DO:        return "DO";
        case DOUBLE:    return "DOUBLE";
        case ELSE:      return "ELSE";
        case ENUM:      return "ENUM";
        case EXTERN:    return "EXTERN";
        case FLOAT:     return "FLOAT";
        case FOR:       return "FOR";
        case GOTO:      return "GOTO";
        case IF:        return "IF";
        case INT:       return "INT";
        case LONG:      return "LONG";
        case REGISTER:  return "REGISTER";
        case RETURN:    return "RETURN";
        case SHORT:     return "SHORT";
        case SIGNED:    return "SIGNED";
        case SIZEOF:    return "SIZEOF";
        case STATIC:    return "STATIC";
        case STRUCT:    return "STRUCT";
        case SWITCH:    return "SWITCH";
        case TYPEDEF:   return "TYPEDEF";
        case UNION:     return "UNION";
        case UNSIGNED:  return "UNSIGNED";
        case VOID:      return "VOID";
        case VOLATILE:  return "VOLATILE";
        case WHILE:     return "WHILE";
        case PLUS:      return "PLUS";
        case MINUS:     return "MINUS";
        case MULT:      return "MULT";
        case DIV:       return "DIV";
        case ASSOP:     return "ASSOP";
        case MODULO:    return "MODULO";
        case PLUSEQ:    return "PLUSEQ";
        case MINUSEQ:   return "MINUSEQ";
        case MULTEQ:    return "MULTEQ";
        case DIVEQ:     return "DIVEQ";
        case MODULOEQ:  return "MODULOEQ";
        case INCRE:     return "INCRE";
        case DECRE:     return "DECRE";
        case BWAND:     return "BWAND";
        case BWOR:      return "BWOR";
        case BWXOR:     return "BWXOR";
        case BWNOT:     return "BWNOT";
        case BWLSHIFT:  return "BWLSHIFT";
        case BWRSHIFT:  return "BWRSHIFT";
        case EQUAL:     return "EQUAL";
        case NEQUAL:    return "NEQUAL";
        case LTHAN:     return "LTHAN";
        case GTHAN:     return "GTHAN";
        case LTHANEQ:   return "LTHANEQ";
        case GTHANEQ:   return "GTHANEQ";
        case AND:       return "AND";
        case OR:        return "OR";
        case NOT:       return "NOT";
        case LPAREN:    return "LPAREN";
        case RPAREN:    return "RPAREN";
        case LBRACK:    return "LBRACK";
        case RBRACK:    return "RBRACK";
        case LBRACE:    return "LBRACE";
        case RBRACE:    return "RBRACE";
        case COLON:     return "COLON";
        case SEMICOLON: return "SEMICOLON";
        case COMMA:     return "COMMA";
        case ARROW:     return "ARROW";
        case DOT:       return "DOT";
        default:        return "FERR";
    }
    return "FERR";
}

// Extract char from FileVec stream and increment position
char vgetc(FileVec *fv)
{
    char c;
    size_t curlinelen = fv->lines[fv->line]->size_actual;

    // Go to next line
    if (fv->col >= curlinelen)
    {
        fv->col = 0;
        fv->line += 1;
    }

    // At the end of the stream
    if (fv->line >= fv->line_count)
        c = 0;
    else
    {
        c = fv->lines[fv->line]->data[fv->col];
        fv->col += 1;
    }

    return c;
}

// Put back char into FileVec stream (move back internal position)
void vungetc(FileVec *fv)
{
    // Beginning of text stream
    if (fv->line == 0 && fv->col == 0)
    {
        // Do nothing
    }
    else if (fv->col == 0)
    {
        // Go to end of previous line
        fv->line -= 1;
        fv->col = fv->lines[fv->line]->size_actual - 1;
    }
    else
        fv->col -= 1;
}

// Get next char from FileVec stream without extraction
char vpeekc(FileVec *fv)
{
    char c;
    c = fv->lines[fv->line]->data[fv->col];
    return c;
}

LexItem get_next_token(FileVec *fv)
{
    enum TokState {
        START, INID, INCOMMENT,
        ININT, INFLOAT, 
        INHEXA, INBINARY,
        INCHAR, INSTRING,
    } lexstate = START;

    LexItem li;
    char c, peek, lexeme[BUF_SIZE];
    int pos = 0, comment_type = 0;

    // Grab a char
    while ((c = vgetc(fv)) != 0)
    {
        // Grab the peek as well
        peek = vpeekc(fv);

        switch (lexstate)
        {
        case START:
            if (ignorable(c))
                continue;
            
            // Add char to the lexeme
            lexeme[pos++] = c;

            if (isalpha(c) || c == '_')
                lexstate = INID;
            else if (c == '"')
                lexstate = INSTRING;
            else if (c == '\'')
                lexstate = INCHAR;
            else if (c == '0' && (tolower(peek) == 'x' || tolower(peek) == 'b'))
            {
                lexstate = tolower(peek) == 'x' ? INHEXA : INBINARY;
                c = vgetc(fv);
                lexeme[pos++] = c;
            }
            else if (isdigit(c))
                lexstate = ININT;
            else if (c == '/' && (peek == '*' || peek == '/'))
            {
                vungetc(fv);
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
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = PLUSEQ;
                        break;
                    }
                    else if (peek == '+')  // increment
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = INCRE;
                        break;
                    }
                    t = PLUS;
                    break;
                case '-':                  // minus
                    if (peek == '=')       // minus compound assign
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = MINUSEQ;
                        break;
                    }
                    else if (peek == '-')  // decrement
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = DECRE;
                        break;
                    }
                    else if (peek == '>')  // arrow
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = ARROW;
                        break;
                    }
                    t = MINUS;
                    break;
                case '*':                  // mult
                    if (peek == '=')       // mult compound assign
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = MULTEQ;
                        break;
                    }
                    t = MULT;
                    break;
                case '/':                  // div
                    if (peek == '=')       // div compound assign
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = DIVEQ;
                        break;
                    }
                    t = DIV;
                    break;
                case '=':                  // assignment
                    if (peek == '=')       // equality
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = EQUAL;
                        break;
                    }
                    t = ASSOP;
                    break;
                case '<':                  // less than
                    if (peek == '=')       // less than or equal
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = LTHANEQ;
                        break;
                    }
                    else if (peek == '<')  // bitwise left shift
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = BWLSHIFT;
                        break;
                    }
                    t = LTHAN;
                    break;
                case '>':                  // greater than
                    if (peek == '=')       // greater than or equal
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = GTHANEQ;
                        break;
                    }
                    else if (peek == '>')  // bitwise right
                    {
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = BWRSHIFT;
                        break;
                    }
                    t = GTHAN;
                    break;
                case '!':                  // not
                    if (peek == '=')       // not equal
                    {
                        c = vgetc(fv);
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
                        c = vgetc(fv);
                        lexeme[pos++] = c;
                        t = AND;
                        break;
                    }
                    t = BWAND;
                    break;
                case '|':                  // bitwise or
                    if (peek == '|')       // or
                    {
                        c = vgetc(fv);
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
                case '.':                  // dot
                    t = DOT;
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
                vungetc(fv);
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
                c = vgetc(fv);
                lexeme[pos++] = c;
                li.token = CCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            else if (c == '\\')
            {
                lexeme[pos++] = c; // add '\'
                c = vgetc(fv); // char after '\'
                peek = vpeekc(fv); // closing '
                if (peek != '\'')
                {
                    li.token = ERR;
                    lexeme[pos] = 0;
                    return li;
                }
                lexeme[pos++] = c;
                c = vgetc(fv);
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
                vungetc(fv);
                lexstate = INFLOAT;
            } 
            else 
            {
                vungetc(fv);
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
                vungetc(fv);
                li.token = FCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INHEXA:
            if (isxdigit(c))
            {
                lexeme[pos++] = c;
            }
            else
            {
                vungetc(fv);
                li.token = HEXCONST;
                lexeme[pos] = 0;
                strcpy(li.lexeme, lexeme);
                return li;
            }
            break;

        case INBINARY:
            if (c == '0' || c == '1')
            {
                lexeme[pos++] = c;
            }
            else
            {
                vungetc(fv);
                li.token = BINCONST;
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
                c = vgetc(fv);
            }
            if (comment_type == MULTI_LINE)
            {
                if (c == '*' && peek == '/')
                {
                    c = vgetc(fv);
                    li.token = COMMENT;
                    strcpy(li.lexeme, "MULIT-LINE-COMMENT");
                    return li;
                }
            }
            if (comment_type == SINGLE_LINE)
            {
                if (c == '\n')
                {
                    vungetc(fv);
                    li.token = COMMENT;
                    strcpy(li.lexeme, "SINGLE-LINE-COMMENT");
                    return li;
                }
            }
            break;

        }

    }// end of while

    if (c == 0)
    {
        li.token = DONE;
        strcpy(li.lexeme, "");
        return li;
    }

    return li;
}

