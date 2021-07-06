#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "utils.h"
#include "vector.h"
#include "dict.h"
#include "preprocessor.h"
#include "lexer.h"

static dict *define_vals;

FileVec *file_vec_create(FILE *fp)
{
    FileVec *res;

    res = (FileVec*)malloc(sizeof(FileVec));

    res->lines = file_into_lines(fp, true);
    res->line_count = file_line_count(fp, true);
    res->line = 0;
    res->col = 0;

    return res;
}

//bool isdirective(vec_char *line)
bool isdirective(LineInfo *li)
{
    char c;
    for (int i = 0; i < li->line->size_actual; i++)
    {
        c = li->line->data[i];
        if (c == ' ' || c == '\t')
            continue;
        if (c == '#') return true;
        else return false;
    }
}

bool pp_parse_define(LineInfo *li)
{
    PPItem ppitem;
    vec_char *ident, *cur_lexeme, *define_val;
    define_val = vec_char_create();

    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPIDENT && ppitem.token != PPFUNCTION) 
    { 
        printf("missing ident in define\n"); 
        vec_char_delete(define_val);
        return false;
    }

    ident = vec_char_from(ppitem.lexeme, ppitem.length);

    // Object like macro
    if (ppitem.token == PPIDENT)
    {
        ppitem = dir_get_next_token(li);
        if (ppitem.token == PPDONE) 
        { 
            printf("%s defined but not set\n", ident->data);
            vec_char_delete(ident);
            vec_char_delete(define_val);
            return true;
        }
        else 
        {
            while (ppitem.token != PPDONE)
            {
                cur_lexeme = vec_char_from(ppitem.lexeme, ppitem.length);
                vec_char_splice(define_val, cur_lexeme, define_val->size_actual);
                vec_char_push(define_val, ' ');
                vec_char_delete(cur_lexeme);
                ppitem = dir_get_next_token(li);
            }
            printf("%s defined as %s\n", ident->data, define_val->data);
        }
    }
    // Function like macro
    else if (ppitem.token == PPFUNCTION)
    {
        vec_char *params;
        ppitem = dir_get_next_token(li); // grab (
        if (ppitem.lexeme[0] != '(') 
        { 
            printf("missing ("); 
            vec_char_delete(ident);
            vec_char_delete(define_val);
            return false;
        }
        ppitem = dir_get_next_token(li); // grab first param or )
        params = vec_char_from(ppitem.lexeme, ppitem.length);
        if (ppitem.token != PPIDENT && ppitem.lexeme[0] != ')') 
        { 
            printf("missing param in %s\n", ident->data); 
            vec_char_delete(ident);
            vec_char_delete(define_val);
            vec_char_delete(params);
            return false;
        }
        ppitem = dir_get_next_token(li);
        if (ppitem.lexeme[0] == ',')
        {
            while (ppitem.lexeme[0] != ')')
            {
                ppitem = dir_get_next_token(li);
                if (ppitem.token != PPIDENT) 
                { 
                    printf("missing ident in param list\n"); 
                    vec_char_delete(ident);
                    vec_char_delete(define_val);
                    vec_char_delete(params);
                    return false;
                }
                vec_char *cur_param = vec_char_from(ppitem.lexeme, ppitem.length);
                vec_char_splice(params, cur_param, params->size_actual);
                vec_char_delete(cur_param);
                ppitem = dir_get_next_token(li);
                if (ppitem.lexeme[0] != ',' && ppitem.lexeme[0] != ')')
                { 
                    printf("missing comma in param list\n"); 
                    vec_char_delete(ident);
                    vec_char_delete(define_val);
                    vec_char_delete(params);
                    return false;
                }
            }
        }
        if (ppitem.lexeme[0] == ')')
        {
            ppitem = dir_get_next_token(li);
            if (ppitem.token == PPDONE) 
            { 
                printf("%s defined but not set\n", ident->data);
                vec_char_delete(ident);
                vec_char_delete(define_val);
                vec_char_delete(params);
                return false;
            }
            else 
            {
                while (ppitem.token != PPDONE)
                {
                    cur_lexeme = vec_char_from(ppitem.lexeme, ppitem.length);
                    vec_char_splice(define_val, cur_lexeme, define_val->size_actual);
                    vec_char_push(define_val, ' ');
                    vec_char_delete(cur_lexeme);
                    ppitem = dir_get_next_token(li);
                }
                printf(
                    "%s with params: [%s] defined as %s\n", 
                    ident->data, 
                    params->data, 
                    define_val->data
                );
            }
        }
        else 
        { 
            printf("error in macro fn param list\n"); 
            vec_char_delete(ident);
            vec_char_delete(define_val);
            return false;
        }
        vec_char_delete(params);
    }
    vec_char_delete(ident);
    vec_char_delete(define_val);
    return true;
}
bool pp_parse_if(LineInfo *li)
{
    PPItem ppitem;
    vec_char *cur_lexeme, *define_val;
    define_val = vec_char_create();
    ppitem = dir_get_next_token(li);
    if (ppitem.token == PPDONE) 
    { 
        printf("missing expression in if\n"); 
        vec_char_delete(define_val);
        return false; 
    }
    while (ppitem.token != PPDONE)
    {
        cur_lexeme = vec_char_from(ppitem.lexeme, ppitem.length);
        vec_char_splice(define_val, cur_lexeme, define_val->size_actual);
        vec_char_delete(cur_lexeme);
        ppitem = dir_get_next_token(li);
    }
    printf("if condition: %s\n", define_val->data);
    vec_char_delete(define_val);
    return true;
}
bool pp_parse_ifdef(LineInfo *li)
{
    PPItem ppitem;
    vec_char *ident;
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPIDENT) 
    { 
        printf("missing ident in ifdef\n"); 
        return false; 
    }
    ident = vec_char_from(ppitem.lexeme, ppitem.length);
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPDONE) 
    { 
        printf("too many arguments in ifdef\n"); 
        vec_char_delete(ident);
        return false; 
    }
    printf("ifdef condition: %s\n", ident->data);
    vec_char_delete(ident);
    return true;
}
bool pp_parse_ifndef(LineInfo *li)
{
    PPItem ppitem;
    vec_char *ident;
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPIDENT) 
    { 
        printf("missing ident in ifndef\n"); 
        return false; 
    }
    ident = vec_char_from(ppitem.lexeme, ppitem.length);
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPDONE) 
    { 
        printf("too many arguments in ifndef\n"); 
        vec_char_delete(ident);
        return false; 
    }
    printf("ifndef condition: %s\n", ident->data);
    vec_char_delete(ident);
    return true;
}
bool pp_parse_undef(LineInfo *li)
{
    PPItem ppitem;
    vec_char *ident;
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPIDENT && ppitem.token != PPFUNCTION) 
    { 
        printf("missing ident in undef\n"); 
        return false; 
    }
    ident = vec_char_from(ppitem.lexeme, ppitem.length);
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPDONE) 
    { 
        printf("too many arguments in undef\n"); 
        vec_char_delete(ident);
        return false; 
    }
    printf("undefining: %s\n", ident->data);
    vec_char_delete(ident);
    return true;
}
bool pp_parse_endif(LineInfo *li)
{
    PPItem ppitem;
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPDONE) 
    { 
        printf("too many arguments in endif\n"); 
        return false; 
    }
    printf("endif here\n");
    return true;
}
bool pp_parse_include(LineInfo *li)
{
    PPItem ppitem;
    vec_char *ident;
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPSTRING) 
    { 
        printf("missing string in include\n"); 
        return false; 
    }
    ident = vec_char_from(ppitem.lexeme, ppitem.length);
    ppitem = dir_get_next_token(li);
    if (ppitem.token != PPDONE) 
    { 
        printf("too many arguments in include\n"); 
        vec_char_delete(ident);
        return false;
    }
    printf("including: %s\n", ident->data);
    vec_char_delete(ident);
    return true;
}

bool parse_directive(LineInfo *li)
{
    PPItem ppitem = dir_get_next_token(li);

    if (ppitem.token != PPHASH) 
    { 
        printf("missing hash\n"); 
        return false;
    }

    ppitem = dir_get_next_token(li);
    switch (ppitem.token)
    {
    case PPINCLUDE:
        pp_parse_include(li);
        break;

    case PPIF:
        pp_parse_if(li);
        break;

    case PPIFDEF:
        pp_parse_ifdef(li);
        break;

    case PPIFNDEF:
        pp_parse_ifndef(li);
        break;

    case PPENDIF:
        pp_parse_endif(li);
        break;

    case PPUNDEF:
        pp_parse_undef(li);
        break;

    case PPDEFINE:
        pp_parse_define(li);
        break;

    default:
        printf("unknown directive: %s\n", ppitem.lexeme);
        return false;
    }
    return true;
}

void pp_printtokenstream(LineInfo *li)
{
    PPItem ppitem = dir_get_next_token(li);
    if (ppitem.token == PPDONE) return;
    while (ppitem.token != PPDONE)
    {
        switch (ppitem.token)
        {
            case PPIDENT:
            case PPNUM:
            case PPPUNC:
            case PPSTRING:
            case PPFUNCTION:
                printf("%s(", pp_tokenstr(ppitem.token));
                printf("\033[95m");
                printf("%s", ppitem.lexeme);
                printf("\033[97m");
                printf(") ");
                break;
            default:
                printf("%s ", pp_tokenstr(ppitem.token));
                break;
        }
        ppitem = dir_get_next_token(li);
    }

    printf("\n");
}

void test(FileVec *fv)
{
    LineInfo li;
    while (fv->line < fv->line_count)
    {
        li.line = fv->lines[fv->line];
        li.col = 0;
        li.line_num = fv->line;

        if (isdirective(&li))
        {
            //pp_printtokenstream(&li);
            parse_directive(&li);
        }
        
        fv->line++;
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;
    char c, peek;

    fp = fopen("input2", "r");
    define_vals = dict_create();

    FileVec fv;
    fv.lines = file_into_lines(fp, true);
    fv.line_count = file_line_count(fp, true);
    fv.line = 0;
    fv.col = 0;

    test(&fv);
    for (size_t i = 0; i < define_vals->size_actual; i++)
    {
        printf("%s: %s\n", define_vals->nodes[i]->key->data, define_vals->nodes[i]->value->data);
    }

    /* for (size_t i = 0; i < fv.line_count; i++) */
    /*     for (size_t j = 0; j < fv.lines[i]->size_actual; j++) */
    /*         printf("%c", fv.lines[i]->data[j]); */


    /* PPItem pp = pp_get_next_token(&fv); */
    /* while ( pp.token != PPDONE ) */
    /* { */
    /*     if (pp.token == PPCOMMENT) */
    /*     { */
    /*         printf("%s|%s\n", pp.lexeme, pp_tokenstr(pp.token)); */
    /*     } */
    /*     pp = pp_get_next_token(&fv); */
    /* } */

    /* while ( (c = vgetc(&fv)) != 0 ) */
    /*     printf("%c", c); */

    /* LexItem x = get_next_token(&fv); */
    /* while (x.token != DONE) */
    /* { */
    /*     printf("%s|%s\n", x.lexeme, tokenstr(&x)); */
    /*     x = get_next_token(&fv); */
    /* } */

    /* Clean up! */
    for (size_t i = 0; i < fv.line_count; i++)
        vec_char_delete(fv.lines[i]);
    
    return 0;
}
