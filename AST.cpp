#include "AST.h"

//Global LLVM Variables and initialization
std::unique_ptr<llvm::LLVMContext> context = std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("KaleidoscopeJIT", *context);
std::unique_ptr<llvm::IRBuilder<>> builder = std::make_unique<llvm::IRBuilder<>>(*context);

/*
std::unique_ptr<llvm::orc::KaleidoscopeJIT> jit = std::make_unique<llvm::orc::KaleidoscopeJIT>();
module->setDataLayout(jit->setDataLayout());

std::unique_ptr<llvm::FunctionPassManager> fpm = std::make_unique<llvm::FunctionPassManager>();
std::unique_ptr<llvm::LoopAnalysisManager> lam = std::make_unique<llvm::LoopAnalysisManager>();
std::unique_ptr<llvm::FunctionAnalysisManager> fam = std::make_unique<llvm::FunctionAnalysisManager>();
std::unique_ptr<llvm::CGSCCAnalysisManager> cgam = std::make_unique<llvm::CGSCCAnalysisManager>();
std::unique_ptr<llvm::ModuleAnalysisManager> mam = std::make_unique<llvm::ModuleAnalysisManager>();
std::unique_ptr<llvm::PassInstrumentationCallbacks> pic = std::make_unique<llvm::PassInstrumentationCallbacks>();
std::unique_ptr<llvm::StandardInstrumentations> si = std::make_unique<llvm::StandardInstrumentations>(*context, true);

si->registerCallbacks(*pic, mam.get());

//Passes added 
fpm->addPass(llvm::InstCombinePass());
fpm->addPass(llvm::ReassociatePass());
fpm->addPass(llvm::SimplifyCFGPass());

llvm::PassBuilder pb;
llvm::ExitOnError exit_on_err;
pb.registerModuleAnalyses(*mam);
pb.registerFunctionAnalyses(*fam);
pb.crossRegisterProxies(*lam, *fam, *cgam, *mam);
*/


//Small symbol table
std::map<std::string, llvm::Value*> named_values;
//std::map<std::string, std::unique_ptr<FunctionPrototypeAST>> function_protos;


llvm::Value* NumberExprAST::codegen(){
    return llvm::ConstantFP::get(*context, llvm::APFloat(val));
}

llvm::Value* VariableExprAST::codegen(){
    llvm::Value* value = named_values[name];
    if(not value){
        log_error("Unknown variable name");
        return nullptr;
    }
    else return value;
}

llvm::Value * BinaryExprAST::codegen(){
    llvm::Value* L = LHS->codegen();
    llvm::Value* R = RHS->codegen();
    if(not L or not R) return nullptr;

    switch (operation){
        case '+':
            return builder->CreateFAdd(L,R,"addtmp");
        case '-':
            return builder->CreateFSub(L,R,"subtmp");
        case '*':
            return builder->CreateFMul(L,R,"multmp");
        case '<':
            L = builder->CreateFCmpULT(L,R,"cmptmp");
            return builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*context));
        default:
            log_error("Invalid operator");
            return nullptr;
    }
}

llvm::Value * CallExprAST::codegen(){
    llvm::Function* callee_f = module->getFunction(callee);
    //Function lookup in the module (Global table)
    if(not callee_f){
        log_error("Unknown function referenced");
        return nullptr;
    }
    else{
        //Check for mismatch in arguments
        if(callee_f->arg_size() != args.size()){
            log_error("Incorrect # arguments passed");
            return nullptr;
        }
        
        std::vector<llvm::Value*> args_values;
        for(unsigned i = 0, e = args.size(); i != e; ++i){
            args_values.push_back(args[i]->codegen());
            if(not args_values.back()) return nullptr;
        }
        return builder->CreateCall(callee_f, args_values, "calltmp");
    }
}

llvm::Function* FunctionPrototypeAST::codegen(){
    std::vector<llvm::Type*> doubles(args.size(), llvm::Type::getDoubleTy(*context));
    llvm::FunctionType * ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context), doubles, false);
    llvm::Function * f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, module.get());

    unsigned i  = 0;
    for(auto &arg: f->args()){
        arg.setName(args[i++]);
    }
    return f;
}

llvm::Function* FunctionAST::codegen(){
    //Check for existing function from previous Extern
    llvm::Function* func = module->getFunction(proto->get_name());

    if(not func){
        func = proto->codegen();
    }
    if(not func){
        return nullptr;
    }
    
    if(not func->empty()){
        log_error("Function cannot be redefined.");
        return (llvm::Function*) nullptr;
    }

    std::vector<std::string> arguments = this->proto->get_args();
    if(arguments.size() != func->arg_size()){
        log_error("Function arguments size does not match");
        return (llvm::Function*) nullptr;
    }

    //Creation of a basic block
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(BB);

    //Record function arguments in the named_values map
    named_values.clear();
    for(auto &arg: func->args()){
        named_values[std::string(arg.getName())] = &arg;
    }

    llvm::Value* ret_val = body->codegen();
    if(ret_val){
        builder->CreateRet(ret_val);
        llvm::verifyFunction(*func);

        //fpm->run(*func, *fam);

        return func;
    }
    else{
        func->eraseFromParent();
        return nullptr;
    }



}




