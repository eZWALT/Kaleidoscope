#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <vector>
#include "global.h"

/**
 * THIS FILE CONTAINS THE CLASS IMPLEMENTATIONS OF AST NODES WITH ITS OWN LLVM CODE GENERATION
*/
//Base class for expressions
class ExprAST{
    //This class would need a type field
    public:
        virtual ~ExprAST() = default;
        virtual llvm::Value* codegen() = 0;
};

//Literal numeric expressions
class NumberExprAST: public ExprAST{
    double val;

    public:
        NumberExprAST(double val): val(val) {}
        virtual llvm::Value* codegen() override;
};

//Variable referencing expression
class VariableExprAST: public ExprAST {
    std::string name;
    public:
        VariableExprAST(const std::string &name): name(name) {}
        virtual llvm::Value* codegen() override;
};

//Class for binary operations such as addition
class BinaryExprAST: public ExprAST {
    char operation;
    std::unique_ptr<ExprAST> LHS, RHS;
    public:
        BinaryExprAST(char operation, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS): 
            operation(operation), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
        virtual llvm::Value* codegen() override;
};

// Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string callee;
  std::vector<std::unique_ptr<ExprAST>> args;

    public:
        CallExprAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args):
         callee(callee), args(std::move(args)) {}
        virtual llvm::Value* codegen() override;
};

class FunctionPrototypeAST{
    std::string name;
    std::vector<std::string> args;
    public:
        FunctionPrototypeAST(const std::string & name, std::vector<std::string> args):
            name(name), args(args) {}

        const std::string& get_name() const {return name;}
        const std::vector<std::string>& get_args() const {return args;}
        llvm::Function* codegen();
};

class FunctionAST{
    std::unique_ptr<FunctionPrototypeAST> proto;
    std::unique_ptr<ExprAST> body;
    public:
        FunctionAST(std::unique_ptr<FunctionPrototypeAST> proto, std::unique_ptr<ExprAST> body):
            proto(std::move(proto)), body(std::move(body)) {}
        llvm::Function* codegen();
};

#endif 