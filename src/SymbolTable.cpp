#include "SymbolTable.h"

SymbolEntry* SymbolTable::lookup(const std::string& name) {
    auto it = table_.find(name);
    if (it != table_.end()) {
        return it->second;
    }
    if (parent_) {
        return parent_->lookup(name);
    }
    return nullptr;
}

void SymbolTable::insert(const std::string& name, SymbolEntry* entry) {
    table_[name] = entry;
}