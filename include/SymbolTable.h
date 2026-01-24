#ifndef SYSY_SYMBOLTABLE_H
#define SYSY_SYMBOLTABLE_H

#include <string>
#include <map>
#include "Type.h"

// 前置声明 LLVM 类
namespace llvm {
    class Value;
}

class SymbolEntry {
public:
    enum Kind { GLOBAL, LOCAL, PARAM, FUNCTION };
    SymbolEntry(Type* type, Kind kind, llvm::Value* val) 
        : type_(type), kind_(kind), val_(val) {}
    
    Type* getType() const { return type_; }
    Kind getKind() const { return kind_; }
    llvm::Value* getValue() const { return val_; }

private:
    Type* type_; 
    Kind kind_;
    llvm::Value* val_; 
};

class SymbolTable {
public:
    SymbolTable() : parent_(nullptr) {}
    explicit SymbolTable(SymbolTable* parent) : parent_(parent) {}

    SymbolEntry* lookup(const std::string& name);
    void insert(const std::string& name, SymbolEntry* entry);
    SymbolTable* getParent() const { return parent_; }

private:
    SymbolTable* parent_;
    std::map<std::string, SymbolEntry*> table_;
};

#endif