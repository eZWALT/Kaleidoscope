#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <string>
#include "global.h"

//Different kind of tokens the Kaleidoscope language has
enum Token{
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5, 
    tok_invalid_num = -6,
};

int get_token();
#endif 
