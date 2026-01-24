#ifndef SYSY_IRBUILDER_H
#define SYSY_IRBUILDER_H

#include "Ast.h"
#include "SymbolTable.h"

// LLVM Headers
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <stack>
#include <memory>

class IrBuilder : public Visitor {
public:
    IrBuilder();
    llvm::Module* getModule() { return module_.get(); }

    void visit(CompUnit* node) override;
    void visit(Decl* node) override;
    void visit(VarDef* node) override;
    void visit(FuncDef* node) override;
    void visit(FuncFParam* node) override;
    void visit(BlockStmt* node) override;
    void visit(ExpStmt* node) override;
    void visit(AssignStmt* node) override;
    void visit(IfStmt* node) override;
    void visit(WhileStmt* node) override;
    void visit(BreakStmt* node) override;
    void visit(ContinueStmt* node) override;
    void visit(ReturnStmt* node) override;
    void visit(BinaryExp* node) override;
    void visit(UnaryExp* node) override;
    void visit(LVal* node) override;
    void visit(IntConst* node) override;
    void visit(FloatConst* node) override;
    void visit(CallExp* node) override;

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    SymbolTable* curSymTable_ = nullptr;
    llvm::Value* curVal_ = nullptr; 
    bool isLVal_ = false;

    struct LoopInfo {
        llvm::BasicBlock* condBB;
        llvm::BasicBlock* endBB;
    };
    std::stack<LoopInfo> loopStack_;

    void pushSymTable();
    void popSymTable();
    llvm::Type* mapType(Type* t); 
    void checkAndCast(llvm::Value*& val, llvm::Type* target);
    
};

#endif