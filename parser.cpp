#include "parser.h"


int get_next_token(){
    return cur_tok = get_token();
}

int get_token_precedence(){
    if(not isascii((char) cur_tok)) return -1;
    else{
        int tok_pre = bin_op_precedence[cur_tok];
        if(tok_pre <= 0) return -1;
        else return tok_pre;
    }
}

//Number expression (literal production)
std::unique_ptr<ExprAST> parse_number_expr(){
    auto result = std::make_unique<NumberExprAST>(num_val);
    get_next_token();
    //Return ExprAST pointer 
    return std::move(result);
}

//Parenthesis expression (literal production)
std::unique_ptr<ExprAST> parse_paren_expr(){
    get_next_token();
    auto v = parse_expression();
    //If no expression is parsed then there's an error
    if(!v) return nullptr;

    if((char) cur_tok != ')'){
        log_error("Expected parenthesis token ')' \n");
        return nullptr;
    }
    get_next_token(); //Get )
    return v;
}

//Both function call's and variables
std::unique_ptr<ExprAST> parse_identifier_expr(){
    std::string id = identifier_str;
    get_next_token(); //get past the id 
    //2 possible cases: variable call and variable reference
    if((char)cur_tok != '('){
        return std::make_unique<VariableExprAST>(id);
    }
    else{
        get_next_token(); //get past the ( 
        std::vector<std::unique_ptr<ExprAST>> args;
        if((char)cur_tok != ')'){
            while(true){
                auto arg = parse_expression();
                //The argument is empty, no expression was parsed, then this call is invalid
                if(arg == nullptr){
                    return nullptr;
                }
                else{
                    args.push_back(std::move(arg));
                }

                if((char)cur_tok == ')'){
                    break;
                } 

                if ((char)cur_tok != ','){
                    log_error("Expected ')' or ',' in argument list");
                    return nullptr;
                }
                get_next_token();
            }
        }
        get_next_token(); //get past the )
        return std::make_unique<CallExprAST>(id, std::move(args));
    }
}

std::unique_ptr<ExprAST> parse_literal(){
    switch(cur_tok){
        default:
            log_error("Unexpected token when expecting an expression");
            return nullptr;
            break;
        case tok_identifier:
            return parse_identifier_expr();
            break;
        case tok_number: 
            return parse_number_expr();
            break;
        case '(':
            return parse_paren_expr();
            break;
    }
}

//Parsing of a whole expression: literal bin_op_rhs
std::unique_ptr<ExprAST> parse_expression(){
    std::unique_ptr<ExprAST> LHS = parse_literal();
    if(! LHS){
        return nullptr;
    }
    else{
        return parse_bin_op_rhs(0, std::move(LHS));
    }
}

//Parsing a right hand side binary operation: ('+' | '-' | '*' | '/') literal
std::unique_ptr<ExprAST> parse_bin_op_rhs(int expr_precedence, std::unique_ptr<ExprAST> LHS){
    while(true){
        int tok_precedence = get_token_precedence();

        //If we wound an operation that is as important as the current one or more, we are done parisng an expression
        if(tok_precedence < expr_precedence) return LHS;

        //Get the operator
        int bin_op = cur_tok;
        get_next_token();

        auto RHS = parse_literal();
        //Wrongly parsed literal
        if(not RHS) return nullptr;

        int next_precedence = get_token_precedence();
        if(tok_precedence < next_precedence){
            RHS = parse_bin_op_rhs(tok_precedence+1, std::move(RHS));
            if(not RHS) return nullptr;
        }
        //Merge both sides 
        LHS = std::make_unique<BinaryExprAST>(bin_op, std::move(RHS), std::move(LHS));

    }
}

//This will parse a function call which has the form
std::unique_ptr<FunctionPrototypeAST> parse_function_prototype(){
    if(cur_tok != tok_identifier){
        log_error("Expected function name in prototype");
        return nullptr;
    }

    std::string function_name = identifier_str;
    get_next_token(); //get past the identifier

    //The name of the function is pending...
    if((char) cur_tok != '('){
        log_error("Expected opening '(' in function prototype");
        return nullptr;
    }

    std::vector<std::string> arg_names;
    while(get_next_token() == tok_identifier) arg_names.push_back(identifier_str);
    if((char) cur_tok != ')'){
        log_error("Expected closing ')' in function prototype");
        return nullptr;
    }

    get_next_token(); //Get past the closing parenthesis
    return std::make_unique<FunctionPrototypeAST>(function_name, std::move(arg_names));
    
}

//Parse a whole function
std::unique_ptr<FunctionAST> parse_definition(){
    get_next_token(); //get past the def keyword
    auto prototype = parse_function_prototype();
    if(not prototype) return nullptr;

    auto expr = parse_expression();
    if(not expr) return nullptr;
    else return std::make_unique<FunctionAST>(std::move(prototype), std::move(expr));
}

std::unique_ptr<FunctionPrototypeAST> parse_extern(){
    get_next_token(); //get past the extern keyword
    return parse_function_prototype();
}

//Top level expressions will be treated as lambda functions with no parameters
std::unique_ptr<FunctionAST> parse_top_level_expr(){
    auto expr = parse_expression();
    if(not expr) return nullptr;
    else{
        auto prototype = std::make_unique<FunctionPrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(prototype), std::move(expr));
    }
} 

 void main_loop(){
    while (true){
        fprintf(stderr, "ready> ");
        switch(cur_tok){
            case tok_eof:
                return;
            case ';':
                get_next_token();
                break;
            case tok_def:
                handle_definition();
                break;
            case tok_extern: 
                handle_extern();
                break;
            default: 
                handle_top_level_expression();
                break;
        }
    }
}

 void handle_definition(){
    auto function_ast = parse_definition();
    if(function_ast){
        auto * function_ir = function_ast->codegen();
        if(function_ir){
            fprintf(stderr, "read function definition:");
            function_ir->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    }
    else get_next_token(); //Skip token for error recovery
}

 void handle_extern(){
    auto proto_ast = parse_extern();
    if(proto_ast){
        auto * function_ir = proto_ast->codegen();
        if(function_ir){
            fprintf(stderr, "read extern:");
            function_ir->print(llvm::errs());
            fprintf(stderr, "\n");
        } 
    } 
    else get_next_token(); //Skip token for error recovery
}

 void handle_top_level_expression(){
    auto function_ast = parse_top_level_expr();
    if(function_ast){
        auto * function_ir = function_ast->codegen();
        if(function_ir){
            fprintf(stderr, "read top-level expression:");
            function_ir->print(llvm::errs());
            fprintf(stderr, "\n");
            function_ir->eraseFromParent();
        }

    }
    else get_next_token(); //Skip token for error recovery
}






