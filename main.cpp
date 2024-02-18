#include "parser.h"
#include "lexer.h"
#include "AST.h"
#include <stdio.h>

std::map<char, int> bin_op_precedence;


int main(){

    bin_op_precedence['>'] = 10;
    bin_op_precedence['<'] = 10;
    bin_op_precedence['+'] = 20;
    bin_op_precedence['-'] = 20;
    bin_op_precedence['*'] = 40;

    fprintf(stderr, "ready> ");
    get_next_token();
    

    main_loop();
}