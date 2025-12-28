#ifndef SYSY_AST_H
#define SYSY_AST_H

#include <iostream>
#include <vector>
#include <string>
#include "Type.h"

class Visitor;

class Node {
public:
    virtual ~Node() = default;
    virtual void accept(Visitor& visitor) = 0;
    void setLine(int line) { line_ = line; }
    int getLine() const { return line_; }
protected:
    int line_ = 0;
};

class Exp : public Node {
public:
    Type* type_ = nullptr; 
};

class Stmt : public Node {};

class LVal : public Exp {
public:
    LVal(std::string name, std::vector<Exp*> dims) 
        : name_(name), dims_(dims) {}
    void accept(Visitor& visitor) override;
    std::string name_;
    std::vector<Exp*> dims_;
};

class BinaryExp : public Exp {
public:
    enum Op { ADD, SUB, MUL, DIV, MOD, LT, GT, LE, GE, EQ, NE, AND, OR };
    BinaryExp(Op op, Exp* lhs, Exp* rhs) 
        : op_(op), lhs_(lhs), rhs_(rhs) {}
    void accept(Visitor& visitor) override;
    Op op_;
    Exp *lhs_, *rhs_;
};

class UnaryExp : public Exp {
public:
    enum Op { PLUS, MINUS, NOT };
    UnaryExp(Op op, Exp* operand) : op_(op), operand_(operand) {}
    void accept(Visitor& visitor) override;
    Op op_;
    Exp* operand_;
};

class IntConst : public Exp {
public:
    explicit IntConst(int val) : val_(val) {}
    void accept(Visitor& visitor) override;
    int val_;
};

class FloatConst : public Exp {
public:
    explicit FloatConst(float val) : val_(val) {}
    void accept(Visitor& visitor) override;
    float val_;
};

class CallExp : public Exp {
public:
    CallExp(std::string func, std::vector<Exp*> args) 
        : func_(func), args_(args) {}
    void accept(Visitor& visitor) override;
    std::string func_;
    std::vector<Exp*> args_;
};

class BlockStmt : public Stmt {
public:
    explicit BlockStmt(std::vector<Node*> stmts) : stmts_(stmts) {}
    void accept(Visitor& visitor) override;
    std::vector<Node*> stmts_; 
};

class ExpStmt : public Stmt {
public:
    explicit ExpStmt(Exp* exp) : exp_(exp) {}
    void accept(Visitor& visitor) override;
    Exp* exp_;
};

class AssignStmt : public Stmt {
public:
    AssignStmt(LVal* lval, Exp* exp) : lval_(lval), exp_(exp) {}
    void accept(Visitor& visitor) override;
    LVal* lval_;
    Exp* exp_;
};

class IfStmt : public Stmt {
public:
    IfStmt(Exp* cond, Stmt* thenStmt, Stmt* elseStmt = nullptr)
        : cond_(cond), thenStmt_(thenStmt), elseStmt_(elseStmt) {}
    void accept(Visitor& visitor) override;
    Exp* cond_;
    Stmt *thenStmt_, *elseStmt_;
};

class WhileStmt : public Stmt {
public:
    WhileStmt(Exp* cond, Stmt* body) : cond_(cond), body_(body) {}
    void accept(Visitor& visitor) override;
    Exp* cond_;
    Stmt* body_;
};

class BreakStmt : public Stmt {
public:
    void accept(Visitor& visitor) override;
};

class ContinueStmt : public Stmt {
public:
    void accept(Visitor& visitor) override;
};

class ReturnStmt : public Stmt {
public:
    explicit ReturnStmt(Exp* retVal = nullptr) : retVal_(retVal) {}
    void accept(Visitor& visitor) override;
    Exp* retVal_;
};

class VarDef : public Node {
public:
    struct InitVal {
        bool isList; 
        Exp* expr;
        std::vector<InitVal*> list;
        InitVal() : isList(false), expr(nullptr) {}
    };
    VarDef(std::string name, std::vector<Exp*> dims, InitVal* init)
        : name_(name), dims_(dims), init_(init) {}
    void accept(Visitor& visitor) override;
    std::string name_;
    std::vector<Exp*> dims_; 
    InitVal* init_;          
};

class Decl : public Node {
public:
    Decl(bool isConst, Type* type, std::vector<VarDef*> defs)
        : isConst_(isConst), type_(type), defs_(defs) {}
    void accept(Visitor& visitor) override;
    bool isConst_;
    Type* type_; 
    std::vector<VarDef*> defs_;
};

class FuncFParam : public Node {
public:
    FuncFParam(Type* type, std::string name, bool isArray, std::vector<Exp*> dims)
        : type_(type), name_(name), isArray_(isArray), dims_(dims) {}
    void accept(Visitor& visitor) override;
    Type* type_; 
    std::string name_;
    bool isArray_; 
    std::vector<Exp*> dims_; 
};

class FuncDef : public Node {
public:
    FuncDef(Type* retType, std::string name, std::vector<FuncFParam*> params, BlockStmt* body)
        : retType_(retType), name_(name), params_(params), body_(body) {}
    void accept(Visitor& visitor) override;
    Type* retType_;
    std::string name_;
    std::vector<FuncFParam*> params_;
    BlockStmt* body_;
};

class CompUnit : public Node {
public:
    explicit CompUnit(std::vector<Node*> children) : children_(children) {}
    void accept(Visitor& visitor) override;
    std::vector<Node*> children_; 
};

class Visitor {
public:
    virtual void visit(CompUnit* node) = 0;
    virtual void visit(Decl* node) = 0;
    virtual void visit(VarDef* node) = 0;
    virtual void visit(FuncDef* node) = 0;
    virtual void visit(FuncFParam* node) = 0;
    virtual void visit(BlockStmt* node) = 0;
    virtual void visit(ExpStmt* node) = 0;
    virtual void visit(AssignStmt* node) = 0;
    virtual void visit(IfStmt* node) = 0;
    virtual void visit(WhileStmt* node) = 0;
    virtual void visit(BreakStmt* node) = 0;
    virtual void visit(ContinueStmt* node) = 0;
    virtual void visit(ReturnStmt* node) = 0;
    virtual void visit(BinaryExp* node) = 0;
    virtual void visit(UnaryExp* node) = 0;
    virtual void visit(LVal* node) = 0;
    virtual void visit(IntConst* node) = 0;
    virtual void visit(FloatConst* node) = 0;
    virtual void visit(CallExp* node) = 0;
};

#endif