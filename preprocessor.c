#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "preprocessor.h"
#include "vector.h"
#include "lexer.h"

char *pp_keywords[9] = {
    "include"  ,  "define"  ,  "undef"  ,  "if"    ,
    "ifdef"    ,  "ifndef"  ,  "endif"  ,  "error" ,
    "defined",
};


// Returns the token of a pre-processor directive
PPToken pp_get_kw_tok(char* kw)
{
    if (!strcmp(kw, "include")) return PPINCLUDE;
    if (!strcmp(kw, "define"))  return PPDEFINE;
    if (!strcmp(kw, "undef"))   return PPUNDEF;
    if (!strcmp(kw, "if"))      return PPIF;
    if (!strcmp(kw, "ifdef"))   return PPIFDEF;
    if (!strcmp(kw, "ifndef"))  return PPIFNDEF;
    if (!strcmp(kw, "endif"))   return PPENDIF;
    if (!strcmp(kw, "error"))   return PPERROR;
    if (!strcmp(kw, "defined")) return PPDEFINED;

    return PPERR;
}

char* pp_tokenstr(PPToken tok)
{
    switch (tok)
    {
        case PPINCLUDE: return "PPINCLUDE";
        case PPDEFINE:  return "PPDEFINE";
        case PPUNDEF:   return "PPUNDEF";
        case PPIF:      return "PPIF";
        case PPIFDEF:   return "PPIFDEF";
        case PPIFNDEF:  return "PPIFNDEF";
        case PPERROR:   return "PPERROR";
        case PPENDIF:   return "PPENDIF";
        case PPDEFINED: return "PPDEFINED";
        case PPIDENT:   return "PPIDENT";
        case PPFUNCTION:return "PPFUNCTION";
        case PPNUM:     return "PPNUM";
        case PPSTRING:  return "PPSTRING";
        case PPPUNC:    return "PPPUNC";
        case PPOTHER:   return "PPOTHER";
        case PPCOMMENT: return "PPCOMMENT";
        case PPHASH:    return "PPHASH";
        case PPCONCAT:  return "PPCONCAT";
        case PPERR:     return "PPERR";
        case PPDONE:    return "PPDONE";
        default:        return "TOKENSTRERR";
    }
}

PPToken pp_id_or_kw(char* lexeme)
{
    PPToken res = PPIDENT;
    for (int i = 0; i < LEN(pp_keywords); i++)
    {
        if (strcmp(pp_keywords[i], lexeme) == 0)
        {
            res = pp_get_kw_tok(pp_keywords[i]);
            break;
        }
    }
    return res;
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

// Returns a heap allocated vector of strings
// consisting of the lines of the file
vec_char** file_into_lines(FILE *in, bool merge)
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
        vec_char_push(lines[curline], c);
        if (c == '\n')
        {
            vec_char_push(lines[curline], 0);
            lines[curline]->size_actual--; // push increments size
            curline++;
        }
    }
    fseek(in, 0, SEEK_SET);

    return lines;
}

//PPItem pp_get_next_token(vec_char** lines, size_t line_count, size_t *line, size_t *col)
PPItem pp_get_next_token(FileVec *fv)
{
    enum {
        START, TOK_INID, 
        TOK_INPUNC, 
        TOK_ININT, TOK_INFLOAT, TOK_INHEXA, TOK_INBINARY,
        TOK_INSTRING, TOK_INCOMMENT, TOK_OTHER,
    } pp_state = START;
    char c, peek;
    int pos = 0, comment_type = 0;
    char closing_quote;

    PPItem ppitem;
    char lexeme[BUF_SIZE];

    while ( (c = vgetc(fv)) != 0 )
    {
        peek = vpeekc(fv);

        /* Check if in directive line or statement line */
        switch (pp_state)
        {
        case START:
            if (c == ' ' || c == '\t' || c == '\n')
                continue;

            lexeme[pos++] = c;

            ppitem.line = fv->line;
            ppitem.col = fv->col;
            
            if (isalpha(c) || c == '_')
                pp_state = TOK_INID;
            else if ( c == '"' || c == '\'' || (c == '<' && isalnum(peek)) )
            {
                switch (c)
                {
                    case '"':  closing_quote = '"'; break;
                    case '\'': closing_quote = '\''; break;
                    case '<':  closing_quote = '>'; break;
                }
                pp_state = TOK_INSTRING;
            }
            else if (c == '0' && (tolower(peek) == 'x' || tolower(peek) == 'b'))
            {
                pp_state = tolower(peek) == 'x' ? TOK_INHEXA : TOK_INBINARY;
                c = vgetc(fv);
                lexeme[pos++] = c;
            }
            else if (isdigit(c))
            {
                pp_state = TOK_ININT;
            }
            else if (c == '/' && (peek == '*' || peek == '/'))
            {
                pp_state = TOK_INCOMMENT;
            }
            else
            {
                Token t; // Token for the operator type
                switch (c)
                {
                case '+':
                    if (peek == '=' || peek == '+')       // plus compound assign
                    {
                        if (peek == '=') t = PLUSEQ;
                        if (peek == '+') t = INCRE;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = PLUS;
                    break;
                case '-':                  // minus
                    if (peek == '=' || peek == '-' || peek == '>')       // minus compound assign
                    {
                        if (peek == '=') t = MINUSEQ;
                        if (peek == '-') t = DECRE;
                        if (peek == '>') t = ARROW;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = MINUS;
                    break;
                case '*':                  // mult
                    if (peek == '=')       // mult compound assign
                    {
                        t = MULTEQ;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = MULT;
                    break;
                case '/':                  // div
                    if (peek == '=')       // div compound assign
                    {
                        t = DIVEQ;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = DIV;
                    break;
                case '=':                  // assignment
                    if (peek == '=')       // equality
                    {
                        t = EQUAL;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = ASSOP;
                    break;
                case '<':                  // less than
                    if (peek == '=' || peek == '<')       // less than or equal
                    {
                        if (peek == '=') t = LTHANEQ;
                        if (peek == '<') t = BWLSHIFT;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = LTHAN;
                    break;
                case '>':                  // greater than
                    if (peek == '=' || peek == '>')       // greater than or equal
                    {
                        if (peek == '=') t = GTHANEQ;
                        if (peek == '>') t = BWRSHIFT;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = GTHAN;
                    break;
                case '!':                  // not
                    if (peek == '=')       // not equal
                    {
                        t = NEQUAL;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = NOT;
                    break;
                case '~':                  // bitwise not
                    if (peek == '=')       // not equal
                    {
                        t = BWNOTEQ;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = BWNOT;
                    break;
                case '&':                  // bitwise and
                    if (peek == '&')       // and
                    {
                        t = AND;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = BWAND;
                    break;
                case '|':                  // bitwise or
                    if (peek == '|')       // or
                    {
                        t = OR;
                        c = vgetc(fv);
                        lexeme[pos++] = peek;
                        break;
                    }
                    t = BWOR;
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
                    break;
                case '#':
                    t = PPHASH;
                    if (peek == '#')
                    {
                        lexeme[pos++] = peek;
                        t = PPCONCAT;
                    }
                    break;
                }
                ppitem.token = PPPUNC;
                ppitem.punc_type = t;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;
        case TOK_INID:
            if (isalpha(c) || isdigit(c) || c == '_')
                lexeme[pos++] = c;
            else 
            {
                vungetc(fv);
                lexeme[pos] = 0;
                ppitem.token = pp_id_or_kw(lexeme);
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INSTRING:
            lexeme[pos++] = c;
            if (c == closing_quote)
            {
                ppitem.token = PPSTRING;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_ININT:
            if (isdigit(c))
            {
                lexeme[pos++] = c;
            }
            else if (c == '.')
            {
                vungetc(fv);
                pp_state = TOK_INFLOAT;
            } 
            else 
            {
                vungetc(fv);
                ppitem.token = PPNUM;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INFLOAT:
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
                ppitem.token = PPERR;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            else
            {
                vungetc(fv);
                ppitem.token = PPNUM;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                return ppitem;
            }
            break;

        case TOK_INHEXA:
            if (isxdigit(c))
            {
                lexeme[pos++] = c;
            }
            else
            {
                vungetc(fv);
                ppitem.token = PPNUM;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INBINARY:
            if (c == '0' || c == '1')
            {
                lexeme[pos++] = c;
            }
            else
            {
                vungetc(fv);
                ppitem.token = PPNUM;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INCOMMENT:
            if (!comment_type)
            {
                /* Multi-line */
                if (c == '*')
                {
                    comment_type = MULTI_LINE; 
                }
                // Single line
                if (c == '/')
                {
                    comment_type = SINGLE_LINE;
                }
            }
            lexeme[pos++] = c;
            if (comment_type == MULTI_LINE)
            {
                if (c == '*' && peek == '/')
                {
                    c = vgetc(fv); // grab peek
                    lexeme[pos++] = c; // add peek

                    lexeme[pos] = 0;
                    ppitem.token = PPCOMMENT;
                    strcpy(ppitem.lexeme, lexeme);
                    ppitem.length = strlen(lexeme);
                    return ppitem;
                }
            }
            if (comment_type == SINGLE_LINE)
            {
                if (c == '\n')
                {
                    lexeme[pos] = 0;
                    ppitem.token = PPCOMMENT;
                    strcpy(ppitem.lexeme, lexeme);
                    ppitem.length = strlen(lexeme);
                    return ppitem;
                }
            }
            break;
        }

    }

    if (c == 0)
    {
        ppitem.token = PPDONE;
        strcpy(ppitem.lexeme, "");
        ppitem.length = 0;
        ppitem.line = fv->line;
        ppitem.col = fv->col;
        return ppitem;
    }

    return ppitem;
}

char ligetc(LineInfo *line)
{
    char c;
    c = line->line->data[line->col++];
    return c;
}
void liungetc(LineInfo *line)
{
    if (line->col > 0) line->col--;
}
char lipeekc(LineInfo *line)
{
    char c;
    c = line->line->data[line->col];
    return c;
}

PPItem dir_get_next_token(LineInfo *line)
{
    enum {
        START, TOK_INID, 
        TOK_INPUNC, 
        TOK_ININT, TOK_INFLOAT, TOK_INHEXA, TOK_INBINARY,
        TOK_INSTRING, TOK_INCOMMENT, TOK_OTHER,
    } pp_state = START;
    char c, peek;
    int pos = 0, comment_type = 0;
    char closing_quote;

    PPItem ppitem;
    char lexeme[BUF_SIZE];

    while ( (c = ligetc(line)) != 0 )
    {
        peek = lipeekc(line);

        /* Check if in directive line or statement line */
        switch (pp_state)
        {
        case START:
            if (c == ' ' || c == '\t' || c == '\n')
                continue;

            lexeme[pos++] = c;
            
            if (isalpha(c) || c == '_')
                pp_state = TOK_INID;
            else if ( c == '"' || c == '\'' || (c == '<' && isalnum(peek)) )
            {
                switch (c)
                {
                    case '"':  closing_quote = '"'; break;
                    case '\'': closing_quote = '\''; break;
                    case '<':  closing_quote = '>'; break;
                }
                pp_state = TOK_INSTRING;
            }
            else if (c == '0' && (tolower(peek) == 'x' || tolower(peek) == 'b'))
            {
                pp_state = tolower(peek) == 'x' ? TOK_INHEXA : TOK_INBINARY;
                c = ligetc(line);
                lexeme[pos++] = c;
            }
            else if (isdigit(c))
            {
                pp_state = TOK_ININT;
            }
            else if (c == '/' && (peek == '*' || peek == '/'))
            {
                pp_state = TOK_INCOMMENT;
            }
            else
            {
                PPToken t = PPPUNC;
                switch (c)
                {
                case '+':
                    if (peek == '=' || peek == '+')       // plus compound assign
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '-':                  // minus
                    if (peek == '=' || peek == '-')       // minus compound assign
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '*':                  // mult
                    if (peek == '=')       // mult compound assign
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '/':                  // div
                    if (peek == '=')       // div compound assign
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '=':                  // assignment
                    if (peek == '=')       // equality
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '<':                  // less than
                    if (peek == '=' || peek == '<')       // less than or equal
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '>':                  // greater than
                    if (peek == '=' || peek == '>')       // greater than or equal
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '!':                  // not
                    if (peek == '=')       // not equal
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '~':                  // bitwise not
                    if (peek == '=')       // not equal
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '&':                  // bitwise and
                    if (peek == '&')       // and
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
                    break;
                case '|':                  // bitwise or
                    if (peek == '|')       // or
                    {
                        c = ligetc(line);
                        lexeme[pos++] = peek;
                    }
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
                    break;
                case '#':
                    t = PPHASH;
                    if (peek == '#')
                    {
                        lexeme[pos++] = peek;
                        t = PPCONCAT;
                    }
                    break;
                }
                ppitem.token = t;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;
        case TOK_INID:
            if (isalpha(c) || isdigit(c) || c == '_')
                lexeme[pos++] = c;
            else 
            {
                lexeme[pos] = 0;
                if (c == '(')
                    ppitem.token = PPFUNCTION;
                else
                    ppitem.token = pp_id_or_kw(lexeme);
                liungetc(line);
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INSTRING:
            lexeme[pos++] = c;
            if (c == closing_quote)
            {
                ppitem.token = PPSTRING;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_ININT:
            if (isdigit(c))
            {
                lexeme[pos++] = c;
            }
            else if (c == '.')
            {
                liungetc(line);
                pp_state = TOK_INFLOAT;
            } 
            else 
            {
                liungetc(line);
                ppitem.token = PPNUM;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INFLOAT:
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
                ppitem.token = PPERR;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            else
            {
                liungetc(line);
                ppitem.token = PPNUM;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INHEXA:
            if (isxdigit(c))
            {
                lexeme[pos++] = c;
            }
            else
            {
                liungetc(line);
                ppitem.token = PPNUM;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INBINARY:
            if (c == '0' || c == '1')
            {
                lexeme[pos++] = c;
            }
            else
            {
                liungetc(line);
                ppitem.token = PPNUM;
                lexeme[pos] = 0;
                strcpy(ppitem.lexeme, lexeme);
                ppitem.length = strlen(lexeme);
                return ppitem;
            }
            break;

        case TOK_INCOMMENT:
            if (!comment_type)
            {
                /* Multi-line */
                if (c == '*')
                {
                    comment_type = MULTI_LINE; 
                }
                // Single line
                if (c == '/')
                {
                    comment_type = SINGLE_LINE;
                }
            }
            lexeme[pos++] = c;
            if (comment_type == MULTI_LINE)
            {
                if (c == '*' && peek == '/')
                {
                    c = ligetc(line); // grab peek
                    lexeme[pos++] = c; // add peek

                    lexeme[pos] = 0;
                    ppitem.token = PPCOMMENT;
                    strcpy(ppitem.lexeme, lexeme);
                    ppitem.length = strlen(lexeme);
                    return ppitem;
                }
            }
            if (comment_type == SINGLE_LINE)
            {
                if (c == '\n')
                {
                    lexeme[pos] = 0;
                    ppitem.token = PPCOMMENT;
                    strcpy(ppitem.lexeme, lexeme);
                    ppitem.length = strlen(lexeme);
                    return ppitem;
                }
            }
            break;
        }

    }

    if (c == 0)
    {
        ppitem.token = PPDONE;
        strcpy(ppitem.lexeme, "");
        ppitem.length = 0;
        return ppitem;
    }

    return ppitem;
}

