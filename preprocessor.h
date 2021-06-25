#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "vector.h"
#include "lexer.h"

char *pp_keywords[] = {
    "include"  ,  "define"  ,  "undef"  ,  "if"    ,
    "ifdef"    ,  "ifndef"  ,  "endif"  ,  "error" ,
};

typedef enum {
    // Pre-processor directives
    PPINCLUDE, PPDEFINE, PPUNDEF,
    PPIF, PPIFDEF, PPIFNDEF, PPERROR, PPENDIF,

    // Misc
    PPHASH, PPPUNC, PPERR, PPDONE,
} PPToken;

typedef struct {
    PPToken token;
    char lexeme[BUF_SIZE];
} PPItem;

// Returns the token of a pre-processor directive
PPToken get_pp_kw_tok(char* kw)
{
    if (!strcmp(kw, "include")) return PPINCLUDE;
    if (!strcmp(kw, "define"))  return PPDEFINE;
    if (!strcmp(kw, "undef"))   return PPUNDEF;
    if (!strcmp(kw, "if"))      return PPIF;
    if (!strcmp(kw, "ifdef"))   return PPIFDEF;
    if (!strcmp(kw, "ifndef"))  return PPIFNDEF;
    if (!strcmp(kw, "endif"))   return PPENDIF;
    if (!strcmp(kw, "error"))   return PPERROR;

    return PPERR;
}

// Returns number of lines in file 
// Escaped new lines can be counted as one merged line
size_t file_line_count(FILE* in, bool merge)
{
    char c, peek;
    size_t line_count = 0;

    fseek(in, 0, SEEK_SET);

    // Get the amount of lines (escaped lines count as one)
    while ( (c = fgetc(in)) != EOF )
    {
        peek = peekch(in);
        if (merge && c == '\\' && peek == '\n')
        {
            c = fgetc(in); // grab escape
            c = fgetc(in); // grab newline
        }
        if (c == '\n')
            line_count++;
    }
    fseek(in, 0, SEEK_SET);

    return line_count;
}

vec_char** file_into_lines(FILE* in, bool merge)
{
    vec_char** lines;
    char c, peek;
    size_t line_count, curline = 0;

    // Get the amount of lines (escaped lines count as one)
    line_count = file_line_count(in, merge);

    // Create array of lines
    lines = (vec_char**)malloc(sizeof(vec_char*) * line_count);
    for (size_t i = 0; i < line_count; i++)
        lines[i] = vec_char_create();

    // Initialize line content
    while ( (c = fgetc(in)) != EOF )
    {
        peek = peekch(in);
        if (c == '\\' && peek == '\n')
        {
            c = fgetc(in); // grab escape
            c = fgetc(in); // grab newline
        }
        if (c == '\n')
        {
            vec_char_push(lines[curline], 0);
            curline++;
        }
        else
            vec_char_push(lines[curline], c);
    }

    return lines;
}

PPItem pp_get_next_token(vec_char** lines, size_t line_count, size_t line, size_t col)
{
    enum {
        START, 
        TOK_INID, TOK_INPUNC, TOK_INNUM, TOK_INSTRING, TOK_OTHER,
    } pp_state = START;
    char c, peek;
    int pos = 0;

    PPItem ppitem;
    char lexeme[BUF_SIZE];

    for (size_t i = 0; i < line_count; i++)
    {
        for (size_t j = 0; j < lines[i]->size_actual; j++)
        {
            c = lines[i]->data[j];
            peek = j + 1 < lines[i]->size_actual ? lines[i]->data[j + 1] : 0;

            /* Check if in directive line or statement line */
            switch (pp_state)
            {
            case START:
                if (c == ' ' || c == '\t')
                    continue;

                lexeme[pos++] = c;
                
                if (isalpha(c))
                    pp_state = TOK_INID;
                else if (c == '"' || c == '\'')
                    pp_state = TOK_INSTRING;
                else if (isdigit(c))
                    pp_state = TOK_INNUM;
                else
                {
                    PPToken t = PPPUNC;
                    switch (c)
                    {
                    case '+':
                        if (peek == '=' || peek == '+')       // plus compound assign
                            lexeme[pos++] = c;
                        break;
                    case '-':                  // minus
                        if (peek == '=' || peek == '-')       // minus compound assign
                            lexeme[pos++] = c;
                        break;
                    case '*':                  // mult
                        if (peek == '=')       // mult compound assign
                            lexeme[pos++] = c;
                        break;
                    case '/':                  // div
                        if (peek == '=')       // div compound assign
                            lexeme[pos++] = c;
                        break;
                    case '=':                  // assignment
                        if (peek == '=')       // equality
                            lexeme[pos++] = c;
                        break;
                    case '<':                  // less than
                        if (peek == '=' || peek == '<')       // less than or equal
                            lexeme[pos++] = c;
                        break;
                    case '>':                  // greater than
                        if (peek == '=' || peek == '>')       // greater than or equal
                            lexeme[pos++] = c;
                        break;
                    case '!':                  // not
                        if (peek == '=')       // not equal
                            lexeme[pos++] = c;
                        break;
                    case '~':                  // bitwise not
                        if (peek == '=')       // not equal
                            lexeme[pos++] = c;
                        break;
                    case '&':                  // bitwise and
                        if (peek == '&')       // and
                            lexeme[pos++] = c;
                        break;
                    case '|':                  // bitwise or
                        if (peek == '|')       // or
                            lexeme[pos++] = c;
                        break;
                    case '(':                  // left paren
                    case ')':                  // right paren
                    case '[':                  // left bracket
                    case ']':                  // right bracket
                    case '{':                  // left brace
                    case '}':                  // right brace
                    case ':':                  // colon
                    case ';':                  // semi-colon
                    case ',':                  // comma
                        lexeme[pos++] = c;
                        break;
                    }
                    ppitem.token = t;
                    lexeme[pos] = 0;
                    strcpy(ppitem.lexeme, lexeme);
                    return ppitem;
                }
                break;
            case TOK_INID:
                break;

            case TOK_INSTRING:
                break;

            case TOK_INNUM:
                break;
                
            }

        }
    }

    for (int i = 0; i < line_count; i++)
        printf("%s\n", lines[i]->data);
}

#endif /* PREPROCESSOR_H */

