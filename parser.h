#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "AST.h"
#include <memory>
#include <vector>
#include <map>
#include "global.h"
/*
    The Kaleidoscope language is caractherized by the following grammar

    Language production
    top: definition | external | top_level_expression ';'
    
    definition: 'def' prototype expression
    top_level_expr: expression
    external: 'extern' function_prototype
    function_prototype: id '(' id* ')'
    expression: literal bin_op_rhs

    literal: number_expr 
        | paren_expr
        | identifier_expr

    bin_op_rhs: ('+' literal)*

    number_expr: number 
    paren_expr: '(' expression ')' 
    indentifier_expr: identifier | identifier '(' expression (, expression)* ')' | identifier '(' ')'

    ------------------------------

    Tokens are kind of obvious so it will be an exercise to the reader to guess them correctly
    
    Each parsing function can be imagined as the traversing of the ASL tree which we are trying to build up
*/

//Token auxiliar functions, buffer and operator precedence map
int get_next_token();
int get_token_precedence();
static int cur_tok;

//Parsing functions
std::unique_ptr<ExprAST> parse_number_expr();
std::unique_ptr<ExprAST> parse_paren_expr();
std::unique_ptr<ExprAST> parse_identifier_expr();
std::unique_ptr<ExprAST> parse_literal();

std::unique_ptr<ExprAST> parse_bin_op_rhs(int expr_precedence,std::unique_ptr<ExprAST> LHS);
std::unique_ptr<ExprAST> parse_expression();
std::unique_ptr<FunctionPrototypeAST> parse_function_prototype();
std::unique_ptr<FunctionAST> parse_definition();
std::unique_ptr<FunctionPrototypeAST> parse_extern();
std::unique_ptr<FunctionAST> parse_top_level_expr(); 

//Handling functions 
void handle_definition();
void handle_extern();
void handle_top_level_expression();

//Main code for running the frontend
void main_loop();

#endif 
