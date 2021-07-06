#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "utils.h" // Defines BUF_SIZE
#include "lexer.h"
#include "vector.h"

extern char *pp_keywords[9];

typedef enum {
    // Directive keywords
    PPINCLUDE, PPDEFINE, PPUNDEF, PPIF, 
    PPIFDEF, PPIFNDEF, PPERROR, PPENDIF,
    PPDEFINED,

    PPIDENT, PPNUM, PPSTRING, PPPUNC, PPOTHER,
    PPFUNCTION,

    // Misc
    PPCOMMENT,
    PPHASH, PPCONCAT, PPERR, PPDONE,
} PPToken;

typedef struct {
    PPToken token;
    Token punc_type;
    size_t line;
    size_t col;
    size_t length;
    char lexeme[BUF_SIZE];
} PPItem;

// Returns the token of a pre-processor directive
extern PPToken pp_get_kw_tok(char *kw);

//extern PPItem pp_id_or_kw(char* lexeme);
extern PPToken pp_id_or_kw(char* lexeme);

// Returns the enum token as a string repr
extern char* pp_tokenstr(PPToken tok);

// Returns number of lines in file 
// Escaped new lines can be counted as one merged line
extern size_t file_line_count(FILE *in, bool merge);

// Returns a heap allocated vector of strings
// consisting of the lines of the file
extern vec_char **file_into_lines(FILE* in, bool merge);

extern PPItem pp_get_next_token(FileVec *fv);

extern PPItem dir_get_next_token(LineInfo *line);

extern char ligetc(LineInfo *line);
extern void liungetc(LineInfo *line);
extern char lipeekc(LineInfo *line);

// Preprocessing parsers
extern bool parse_directive(LineInfo *li);
extern bool pp_parse_define(LineInfo *li);
extern bool pp_parse_if(LineInfo *li);
extern bool pp_parse_ifdef(LineInfo *li);
extern bool pp_parse_ifndef(LineInfo *li);
extern bool pp_parse_undef(LineInfo *li);
extern bool pp_parse_endif(LineInfo *li);
extern bool pp_parse_include(LineInfo *li);

#endif /* PREPROCESSOR_H */

