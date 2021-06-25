#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "utils.h"
#include "vector.h"
#include "preprocessor.h"
#include "lexer.h"

void vec_char_print(vec_char* vec)
{
    printf("[");
    for (int i = 0; i < vec->size_actual; i++)
    {
        if (i == vec->size_actual - 1) printf("%c", vec->data[i]);
        else printf("%c, ", vec->data[i]);
    }
    printf("]\n");
}

int main(int argc, char *argv[])
{
    FILE *fp;
    char c, peek;
    size_t line_count;
    vec_char** lines;

    fp = fopen("input", "r");

    line_count = file_line_count(fp, true);
    lines = file_into_lines(fp, true);  

    for (size_t i = 0; i < line_count; i++)
        printf("%s\n", lines[i]->data);

    //preprocess(lines, line_count, 0, 0);


    /* Clean up! */
    for (size_t i = 0; i < line_count; i++)
        vec_char_delete(lines[i]);
    free(lines);
    
    return 0;
}
