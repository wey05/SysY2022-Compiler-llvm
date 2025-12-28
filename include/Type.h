#ifndef SYSY_TYPE_H
#define SYSY_TYPE_H

#include <string>
#include <vector>

class Type {
public:
    enum TypeID { VOID, INT, FLOAT, PTR, ARRAY, FUNC };
    explicit Type(TypeID id) : id_(id) {}
    virtual ~Type() = default;
    
    TypeID getTypeID() const { return id_; }
    bool isVoid() const { return id_ == VOID; }
    bool isInt() const { return id_ == INT; }
    bool isFloat() const { return id_ == FLOAT; }
    bool isPointer() const { return id_ == PTR; }
    bool isArray() const { return id_ == ARRAY; }
    bool isFunc() const { return id_ == FUNC; }

    virtual std::string toStr() = 0;

    static Type* getVoidType();
    static Type* getIntType();
    static Type* getFloatType();
    static Type* getIntPtrType(); 
    static Type* getFloatPtrType();

private:
    TypeID id_;
};

class IntType : public Type {
public:
    IntType() : Type(Type::INT) {}
    std::string toStr() override { return "i32"; }
};

class FloatType : public Type {
public:
    FloatType() : Type(Type::FLOAT) {}
    std::string toStr() override { return "float"; }
};

class VoidType : public Type {
public:
    VoidType() : Type(Type::VOID) {}
    std::string toStr() override { return "void"; }
};

class ArrayType : public Type {
public:
    ArrayType(Type* elemType, int length) 
        : Type(Type::ARRAY), elemType_(elemType), length_(length) {}
    std::string toStr() override { return "[" + std::to_string(length_) + " x " + elemType_->toStr() + "]"; }
    Type* getElementType() const { return elemType_; }
    int getLength() const { return length_; }
private:
    Type* elemType_;
    int length_;
};

class PointerType : public Type {
public:
    explicit PointerType(Type* targetType) 
        : Type(Type::PTR), targetType_(targetType) {}
    std::string toStr() override { return targetType_->toStr() + "*"; }
    Type* getTargetType() const { return targetType_; }
private:
    Type* targetType_;
};

class FunctionType : public Type {
public:
    FunctionType(Type* retType, std::vector<Type*> params)
        : Type(Type::FUNC), retType_(retType), params_(params) {}
    std::string toStr() override { return "func"; }
    Type* getRetType() const { return retType_; }
    const std::vector<Type*>& getParams() const { return params_; }
private:
    Type* retType_;
    std::vector<Type*> params_;
};

#endif