#ifndef GLOBAL_H
#define GLOBAL_H 

#include <vector>
#include <string> 
#include <memory> 
#include <map>
#include <iostream>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <algorithm>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
//#include "llvm/Transform/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"




/**
 * THIS FILE CONTAIN THE DEFINITION OF THE GLOBAL VARIABLES, HELPER FUNCTIONS AND ALL THE IMPORTS NECESSARY
*/

// lexer variables
extern std::string identifier_str;
extern double num_val;

inline void log_error(const char * s){
    fprintf(stderr, "Log error: %s\n", s);
}

// bin_op_precedence - This holds the precedence for each binary operator that is defined.
extern std::map<char, int> bin_op_precedence;

//LLVM global variables
extern std::unique_ptr<llvm::LLVMContext> context;
extern std::unique_ptr<llvm::IRBuilder<>> builder;
extern std::unique_ptr<llvm::Module> module;
/*
extern std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit;
extern std::unique_ptr<llvm::FunctionPassManager> fpm;
extern std::unique_ptr<llvm::LoopAnalysisManager> lam;
extern std::unique_ptr<llvm::FunctionAnalysisManager> fam;
extern std::unique_ptr<llvm::CGSCCAnalysisManager> cgam;
extern std::unique_ptr<llvm::ModuleAnalysisManager> mam;
extern std::unique_ptr<llvm::PassInstrumentationCallbacks> pic;
extern std::unique_ptr<llvm::StandardInstrumentations> si;

extern llvm::PassBuilder pb;
extern llvm::ExitOnError exit_on_err;
*/
//Symbol table and function table
extern std::map<std::string, llvm::Value*> named_values;
//extern std::map<std::string, std::unique_ptr<FunctionPrototypeAST>> function_protos;


#endif