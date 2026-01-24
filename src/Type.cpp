#include "Type.h"

static IntType* intType = new IntType();
static FloatType* floatType = new FloatType();
static VoidType* voidType = new VoidType();
static PointerType* intPtrType = new PointerType(intType);
static PointerType* floatPtrType = new PointerType(floatType);

Type* Type::getVoidType() { return voidType; }
Type* Type::getIntType() { return intType; }
Type* Type::getFloatType() { return floatType; }
Type* Type::getIntPtrType() { return intPtrType; }
Type* Type::getFloatPtrType() { return floatPtrType; }