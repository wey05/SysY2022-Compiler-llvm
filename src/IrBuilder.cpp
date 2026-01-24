#include "IrBuilder.h"
#include <iostream>
#include <vector>
#include <functional>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <map>

// 全局静态常量表（存储编译阶段就能确定值的常量）
static std::map<std::string, double> GlobalConstTable;

class ConstEvaluator : public Visitor {
public:
    double val_ = 0;
    void visit(IntConst* node) override { val_ = (double)node->val_; }
    void visit(FloatConst* node) override { val_ = (double)node->val_; }
    void visit(UnaryExp* node) override {
        node->operand_->accept(*this);
        double v = val_;
        if (node->op_ == UnaryExp::MINUS) val_ = -v;
        else if (node->op_ == UnaryExp::PLUS) val_ = v;
        else if (node->op_ == UnaryExp::NOT) val_ = !v;
    }
    void visit(BinaryExp* node) override {
        node->lhs_->accept(*this);
        double l = val_;
        node->rhs_->accept(*this);
        double r = val_;
        switch(node->op_) {
            case BinaryExp::ADD: val_ = l + r; break;
            case BinaryExp::SUB: val_ = l - r; break;
            case BinaryExp::MUL: val_ = l * r; break;
            case BinaryExp::DIV: val_ = (r != 0) ? l / r : 0; break;
            case BinaryExp::MOD: val_ = (int)l % (int)r; break;
            case BinaryExp::LT: val_ = l < r; break;
            case BinaryExp::GT: val_ = l > r; break;
            case BinaryExp::LE: val_ = l <= r; break;
            case BinaryExp::GE: val_ = l >= r; break;
            case BinaryExp::EQ: val_ = l == r; break;
            case BinaryExp::NE: val_ = l != r; break;
            case BinaryExp::AND: val_ = l && r; break;
            case BinaryExp::OR:  val_ = l || r; break;
        }
    }
    void visit(CompUnit*) override {}
    void visit(Decl*) override {}
    void visit(VarDef*) override {}
    void visit(FuncDef*) override {}
    void visit(FuncFParam*) override {}
    void visit(BlockStmt*) override {}
    void visit(ExpStmt*) override {}
    void visit(AssignStmt*) override {}
    void visit(IfStmt*) override {}
    void visit(WhileStmt*) override {}
    void visit(BreakStmt*) override {}
    void visit(ContinueStmt*) override {}
    void visit(ReturnStmt*) override {}
    void visit(LVal* node) override { 
        if (GlobalConstTable.count(node->name_)) {
            val_ = GlobalConstTable[node->name_];
        } else {
            val_ = 0; 
        }
    } 
    void visit(CallExp*) override {}
};

static double evalConst(Exp* exp) {
    ConstEvaluator evaluator;
    exp->accept(evaluator);
    return evaluator.val_;
}

// 递归计算嵌套维度
static int get_total_dims(Type* type) {
    if (!type) return 0;
    if (type->isPointer()) {
        return 1 + get_total_dims(((PointerType*)type)->getTargetType());
    }
    if (type->isArray()) {
        return 1 + get_total_dims(((ArrayType*)type)->getElementType());
    }
    return 0;
}

IrBuilder::IrBuilder() {
    context = std::make_unique<llvm::LLVMContext>();
    // IR容器
    module_ = std::make_unique<llvm::Module>("sysy", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module_->setTargetTriple("riscv64-unknown-linux-gnu");
    module_->addModuleFlag(llvm::Module::Error, "target-abi", 
                          llvm::MDString::get(*context, "lp64d"));
    curSymTable_ = new SymbolTable(nullptr); 
    GlobalConstTable.clear(); 
}

void IrBuilder::pushSymTable() {
    curSymTable_ = new SymbolTable(curSymTable_);
}

void IrBuilder::popSymTable() {
    SymbolTable* old = curSymTable_;
    curSymTable_ = curSymTable_->getParent();
    delete old;
}

llvm::Type* IrBuilder::mapType(Type* t) {
    if (t->isVoid()) return llvm::Type::getVoidTy(*context);
    if (t->isInt()) return llvm::Type::getInt32Ty(*context);
    if (t->isFloat()) return llvm::Type::getFloatTy(*context);
    if (t->isPointer()) {
        llvm::Type* base = mapType(((PointerType*)t)->getTargetType());
        return base->getPointerTo();
    }
    if (t->isArray()) {
        ArrayType* at = (ArrayType*)t;
        llvm::Type* elem = mapType(at->getElementType());
        return llvm::ArrayType::get(elem, at->getLength());
    }
    return llvm::Type::getVoidTy(*context);
}

void IrBuilder::checkAndCast(llvm::Value*& val, llvm::Type* target) {
    llvm::Type* src = val->getType();
    if (src == target) return;
    // 仅改变类型解释，不修改二进制位
    if (src->isPointerTy() && target->isPointerTy()) {
        val = builder->CreateBitCast(val, target);
        return;
    }
    if (src->isIntegerTy(32) && target->isFloatTy()) {
        val = builder->CreateSIToFP(val, target);
    } else if (src->isFloatTy() && target->isIntegerTy(32)) {
        val = builder->CreateFPToSI(val, target);
    } else if (src->isIntegerTy(1) && target->isIntegerTy(32)) {
        val = builder->CreateZExt(val, target);
    } else if (src->isIntegerTy(32) && target->isIntegerTy(1)) {
        val = builder->CreateICmpNE(val, llvm::ConstantInt::get(src, 0));
    } else if (src->isFloatTy() && target->isIntegerTy(1)) {
        val = builder->CreateFCmpONE(val, llvm::ConstantFP::get(src, 0.0));
    }
}

void IrBuilder::visit(CompUnit* node) {
    auto declareLib = [&](std::string name, llvm::Type* ret, std::vector<llvm::Type*> params) {
        llvm::FunctionType* ft = llvm::FunctionType::get(ret, params, false);
        // ExternalLinkage：链接属性，表示这个函数是外部定义的
        llvm::Function* func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, module_.get());
        /*
            +m：乘法扩展
            +a：原子操作扩展
            +f：单精度浮点扩展
            +d：双精度浮点扩展
            +c：压缩指令扩展
        */
        func->addFnAttr("target-features", "+m,+a,+f,+d,+c");
        Type* dummyType = Type::getVoidType(); 
        SymbolEntry* entry = new SymbolEntry(dummyType, SymbolEntry::FUNCTION, module_->getFunction(name));
        curSymTable_->insert(name, entry);
    };
    llvm::Type* voidTy = llvm::Type::getVoidTy(*context);
    llvm::Type* i32Ty = llvm::Type::getInt32Ty(*context);
    llvm::Type* floatTy = llvm::Type::getFloatTy(*context);
    llvm::Type* i32PtrTy = llvm::Type::getInt32PtrTy(*context);
    llvm::Type* floatPtrTy = llvm::Type::getFloatPtrTy(*context);
    // 声明运行时函数
    declareLib("getint", i32Ty, {});
    declareLib("getch", i32Ty, {});
    declareLib("getfloat", floatTy, {});
    declareLib("putint", voidTy, {i32Ty});
    declareLib("putch", voidTy, {i32Ty});
    declareLib("putfloat", voidTy, {floatTy});
    declareLib("getarray", i32Ty, {i32PtrTy});
    declareLib("getfarray", i32Ty, {floatPtrTy});
    declareLib("putarray", voidTy, {i32Ty, i32PtrTy});
    declareLib("putfarray", voidTy, {i32Ty, floatPtrTy});
    declareLib("starttime", voidTy, {});
    declareLib("stoptime", voidTy, {});
    for (auto child : node->children_) {
        child->accept(*this);
    }
}

static Type* currentDeclType = nullptr;
static bool currentDeclIsConst = false;

void IrBuilder::visit(Decl* node) {
    currentDeclType = node->type_;
    currentDeclIsConst = node->isConst_;
    for (auto def : node->defs_) def->accept(*this);
}

void IrBuilder::visit(VarDef* node) {
    Type* baseSysY = currentDeclType;
    Type* userType = baseSysY; 
    llvm::Type* baseType = mapType(baseSysY);
    llvm::Type* finalType = baseType;
    std::vector<int> dims;
    // 反向遍历数组维度，自内而外构建数组
    for (auto it = node->dims_.rbegin(); it != node->dims_.rend(); ++it) {
        int d = (int)evalConst(*it);
        if (d <= 0) d = 1; 
        finalType = llvm::ArrayType::get(finalType, d);
        userType = new ArrayType(userType, d);
        dims.insert(dims.begin(), d);
    }
    if (currentDeclIsConst && node->init_ && !node->init_->isList && dims.empty()) {
        double val = evalConst(node->init_->expr);
        if (baseSysY->isInt()) GlobalConstTable[node->name_] = (int)val;
        else GlobalConstTable[node->name_] = val;
    }
    // 全局变量
    if (curSymTable_->getParent() == nullptr) {
        module_->getOrInsertGlobal(node->name_, finalType);
        llvm::GlobalVariable* gVar = module_->getNamedGlobal(node->name_);
        gVar->setLinkage(llvm::GlobalValue::ExternalLinkage);
        // 4字节对齐
        gVar->setAlignment(llvm::MaybeAlign(4));
        llvm::Constant* initVal = nullptr;
        if (dims.empty()) {
            // 非数组变量
            double v = 0;
            if (node->init_) v = evalConst(node->init_->expr);
            if (baseSysY->isInt()) initVal = llvm::ConstantInt::get(baseType, (int)v);
            else initVal = llvm::ConstantFP::get(baseType, v);
        } else {
            // 数组变量
            int totalElements = 1;
            for(int d : dims) totalElements *= d;
            // 不论数组维数，统一先设置为一维数组，方便初始化初值
            std::vector<llvm::Constant*> flatValues;
            llvm::Constant* zero = baseSysY->isInt() 
                ? (llvm::Constant*)llvm::ConstantInt::get(baseType, 0)
                : (llvm::Constant*)llvm::ConstantFP::get(baseType, 0.0);
            flatValues.resize(totalElements, zero);
            if (node->init_) {
                int cursor = 0;
                // 递归填充数组元素
                std::function<void(VarDef::InitVal*, int)> fill = 
                    [&](VarDef::InitVal* init, int depth) {
                    if (!init->isList) {
                        double v = evalConst(init->expr);
                        if (baseSysY->isInt()) flatValues[cursor++] = llvm::ConstantInt::get(baseType, (int)v);
                        else flatValues[cursor++] = llvm::ConstantFP::get(baseType, v);
                        return;
                    }
                    int step = 1;
                    for (size_t i = depth + 1; i < dims.size(); ++i) step *= dims[i];
                    int startCursor = cursor;
                    for (auto child : init->list) {
                        if (child->isList) {
                            int offset = cursor - startCursor;
                            if (offset % step != 0) cursor += step - (offset % step);
                            fill(child, depth + 1);
                        } else {
                            double v = evalConst(child->expr);
                            if (baseSysY->isInt()) flatValues[cursor++] = llvm::ConstantInt::get(baseType, (int)v);
                            else flatValues[cursor++] = llvm::ConstantFP::get(baseType, v);
                        }
                    }
                };
                fill(node->init_, 0);
            }
            // 将一维列表重构为多维数组
            std::function<llvm::Constant*(int, int)> reshape = 
                [&](int depth, int offset) -> llvm::Constant* {
                if (depth == (int)dims.size()) return flatValues[offset];
                std::vector<llvm::Constant*> childValues;
                int step = 1;
                for(size_t i = depth + 1; i < dims.size(); ++i) step *= dims[i];
                int limit = dims[depth];
                if (limit == 0) limit = 1;
                for (int i = 0; i < limit; ++i) {
                    childValues.push_back(reshape(depth + 1, offset + i * step));
                }
                if (childValues.empty()) return llvm::ConstantArray::get(llvm::ArrayType::get(baseType, 0), childValues);
                llvm::ArrayType* arrTy = llvm::ArrayType::get(childValues[0]->getType(), childValues.size());
                return llvm::ConstantArray::get(arrTy, childValues);
            };
            initVal = reshape(0, 0);
        }
        gVar->setInitializer(initVal);
        gVar->setConstant(currentDeclIsConst);
        SymbolEntry* entry = new SymbolEntry(userType, SymbolEntry::GLOBAL, gVar);
        curSymTable_->insert(node->name_, entry);
    } else {
        // 局部变量
        llvm::Function* func = builder->GetInsertBlock()->getParent();
        llvm::IRBuilder<> tmpBuilder(&func->getEntryBlock(), func->getEntryBlock().begin());
        llvm::AllocaInst* alloca = tmpBuilder.CreateAlloca(finalType, nullptr, node->name_);
        SymbolEntry* entry = new SymbolEntry(userType, SymbolEntry::LOCAL, alloca);
        curSymTable_->insert(node->name_, entry);
        if (node->init_) {
            if (dims.empty()) {
                node->init_->expr->accept(*this);
                checkAndCast(curVal_, baseType);
                builder->CreateStore(curVal_, alloca);
            } else {
                uint64_t size = module_->getDataLayout().getTypeAllocSize(finalType);
                builder->CreateMemSet(alloca, llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), 0), size, llvm::MaybeAlign(4));
                llvm::Value* basePtr = builder->CreateBitCast(alloca, baseType->getPointerTo());
                int cursor = 0;
                std::function<void(VarDef::InitVal*, int)> emitInit = 
                    [&](VarDef::InitVal* init, int depth) {
                    if (!init->isList) {
                        init->expr->accept(*this);
                        checkAndCast(curVal_, baseType);
                        llvm::Value* idx = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), cursor);
                        llvm::Value* ptr = builder->CreateInBoundsGEP(baseType, basePtr, idx);
                        builder->CreateStore(curVal_, ptr);
                        cursor++;
                        return;
                    }
                    int step = 1;
                    for (size_t i = depth + 1; i < dims.size(); ++i) step *= dims[i];
                    int currentCapacity = dims[depth] * step;
                    int startCursor = cursor;
                    for (auto child : init->list) {
                        if (child->isList) {
                            int offset = cursor - startCursor;
                            if (offset % step != 0) cursor += step - (offset % step);
                            emitInit(child, depth + 1);
                        } else {
                            child->expr->accept(*this);
                            checkAndCast(curVal_, baseType);
                            llvm::Value* idx = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), cursor);
                            llvm::Value* ptr = builder->CreateInBoundsGEP(baseType, basePtr, idx);
                            builder->CreateStore(curVal_, ptr);
                            cursor++;
                        }
                    }
                    if (cursor < startCursor + currentCapacity) cursor = startCursor + currentCapacity;
                };
                emitInit(node->init_, 0);
            }
        }
    }
}

// Debug：函数参数传递时数组必须转换为指针类型
void IrBuilder::visit(FuncDef* node) {
    std::vector<llvm::Type*> paramTypes;
    std::vector<Type*> userParamTypes;
    for (auto param : node->params_) {
        // 前后端类型同步
        Type* userT = param->type_;
        llvm::Type* t = mapType(param->type_);
        if (param->isArray_) {
            llvm::Type* elemType = t;
            Type* userElem = userT;
            for (auto it = param->dims_.rbegin(); it != param->dims_.rend(); ++it) {
                // 数组维度值为整型常量
                int d = (int)evalConst(*it);
                elemType = llvm::ArrayType::get(elemType, d);
                userElem = new ArrayType(userElem, d);
            }
            t = elemType->getPointerTo(); 
            userT = new PointerType(userElem);
        }
        paramTypes.push_back(t);
        userParamTypes.push_back(userT);
    }
    llvm::Type* retType = mapType(node->retType_);
    // false：固定参数
    llvm::FunctionType* ft = llvm::FunctionType::get(retType, paramTypes, false);
    std::string funcName = node->name_;
    if (funcName == "main") funcName = "__sysy_main";
    llvm::Function* func = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, funcName, module_.get());
    func->addFnAttr("target-features", "+m,+a,+f,+d,+c");
    SymbolEntry* entry = new SymbolEntry(new FunctionType(node->retType_, {}), SymbolEntry::FUNCTION, func);
    curSymTable_->insert(node->name_, entry);
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(entryBB);
    pushSymTable();
    int argIdx = 0;
    for (auto& arg : func->args()) {
        auto paramNode = node->params_[argIdx];
        llvm::AllocaInst* alloca = builder->CreateAlloca(arg.getType(), nullptr, paramNode->name_);
        builder->CreateStore(&arg, alloca);
        curSymTable_->insert(paramNode->name_, new SymbolEntry(userParamTypes[argIdx], SymbolEntry::PARAM, alloca));
        argIdx++;
    }
    node->body_->accept(*this);
    // 检查当前代码生成的基本块末尾是否有终止指令，处理函数返回值
    if (builder->GetInsertBlock()->getTerminator() == nullptr) {
        if (retType->isVoidTy()) {
            builder->CreateRetVoid();
        } else if (retType->isIntegerTy()) {
            builder->CreateRet(llvm::ConstantInt::get(retType, 0));
        } else if (retType->isFloatingPointTy()) {
            builder->CreateRet(llvm::ConstantFP::get(retType, 0.0));
        } else {
             builder->CreateRet(llvm::UndefValue::get(retType));
        }
    }
    popSymTable();
}

void IrBuilder::visit(BlockStmt* node) {
    pushSymTable();
    for (auto item : node->stmts_) {
        if (builder->GetInsertBlock()->getTerminator()) {
            break;
        }
        item->accept(*this);
    }
    popSymTable();
}

void IrBuilder::visit(AssignStmt* node) {
    node->exp_->accept(*this);
    llvm::Value* rhs = curVal_;
    isLVal_ = true;
    node->lval_->accept(*this);
    isLVal_ = false;
    llvm::Value* lhsPtr = curVal_;
    // 不能给空指针赋右值
    if (!lhsPtr->getType()->isPointerTy()) {
        std::cerr << "Assign to non-pointer!" << std::endl;
        exit(1);
    }
    llvm::Type* target = lhsPtr->getType()->getPointerElementType();
    checkAndCast(rhs, target);
    builder->CreateStore(rhs, lhsPtr);
}

void IrBuilder::visit(ExpStmt* node) {
    if (node->exp_) node->exp_->accept(*this);
}

// 通过trueBB和falseBB来实现if-else语句
void IrBuilder::visit(IfStmt* node) {
    if (builder->GetInsertBlock()->getTerminator()) return;
    llvm::Function* func = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(*context, "true", func);
    llvm::BasicBlock* falseBB = node->elseStmt_ ? llvm::BasicBlock::Create(*context, "false", func) : nullptr;
    llvm::BasicBlock* nextBB = llvm::BasicBlock::Create(*context, "next"); 
    node->cond_->accept(*this);
    checkAndCast(curVal_, llvm::Type::getInt1Ty(*context));
    builder->CreateCondBr(curVal_, trueBB, falseBB ? falseBB : nextBB);
    builder->SetInsertPoint(trueBB);
    node->thenStmt_->accept(*this);
    if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(nextBB);
    if (falseBB) {
        builder->SetInsertPoint(falseBB);
        node->elseStmt_->accept(*this);
        if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(nextBB);
    }
    func->getBasicBlockList().push_back(nextBB);
    builder->SetInsertPoint(nextBB);
}

void IrBuilder::visit(WhileStmt* node) {
    if (builder->GetInsertBlock()->getTerminator()) return;
    llvm::Function* func = builder->GetInsertBlock()->getParent();
    // 条件判断基本块
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "cond", func);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "body", func);
    llvm::BasicBlock* endBB = llvm::BasicBlock::Create(*context, "end", func);
    builder->CreateBr(condBB);
    builder->SetInsertPoint(condBB);
    loopStack_.push({condBB, endBB});
    node->cond_->accept(*this);
    checkAndCast(curVal_, llvm::Type::getInt1Ty(*context));
    builder->CreateCondBr(curVal_, bodyBB, endBB);
    builder->SetInsertPoint(bodyBB);
    node->body_->accept(*this);
    if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(condBB);
    builder->SetInsertPoint(endBB);
    loopStack_.pop();
}

void IrBuilder::visit(BreakStmt* node) {
    if (!loopStack_.empty()) builder->CreateBr(loopStack_.top().endBB);
}

void IrBuilder::visit(ContinueStmt* node) {
    if (!loopStack_.empty()) builder->CreateBr(loopStack_.top().condBB);
}

void IrBuilder::visit(ReturnStmt* node) {
    if (node->retVal_) {
        node->retVal_->accept(*this);
        llvm::Function* func = builder->GetInsertBlock()->getParent();
        checkAndCast(curVal_, func->getReturnType());
        builder->CreateRet(curVal_);
    } else {
        builder->CreateRetVoid();
    }
}

void IrBuilder::visit(BinaryExp* node) {
    if (node->op_ == BinaryExp::AND || node->op_ == BinaryExp::OR) {
        llvm::Function* func = builder->GetInsertBlock()->getParent();
        llvm::BasicBlock* rhsBB = llvm::BasicBlock::Create(*context, "rhs", func);
        llvm::BasicBlock* endBB = llvm::BasicBlock::Create(*context, "end", func);
        node->lhs_->accept(*this);
        checkAndCast(curVal_, llvm::Type::getInt1Ty(*context));
        llvm::Value* lhsVal = curVal_;
        llvm::BasicBlock* lhsBB = builder->GetInsertBlock();
        if (node->op_ == BinaryExp::AND) 
            builder->CreateCondBr(lhsVal, rhsBB, endBB); 
        else 
            builder->CreateCondBr(lhsVal, endBB, rhsBB); 
        builder->SetInsertPoint(rhsBB);
        node->rhs_->accept(*this);
        checkAndCast(curVal_, llvm::Type::getInt1Ty(*context));
        llvm::Value* rhsVal = curVal_;
        llvm::BasicBlock* rhsEndBB = builder->GetInsertBlock();
        builder->CreateBr(endBB);
        builder->SetInsertPoint(endBB);
        // getInt1Ty是布尔类型，在SysY中用32位整数表示
        llvm::PHINode* phi = builder->CreatePHI(llvm::Type::getInt1Ty(*context), 2);
        if (node->op_ == BinaryExp::AND) {
            phi->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), 0), lhsBB);
            phi->addIncoming(rhsVal, rhsEndBB);
        } else {
            phi->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), 1), lhsBB);
            phi->addIncoming(rhsVal, rhsEndBB);
        }
        curVal_ = builder->CreateZExt(phi, llvm::Type::getInt32Ty(*context));
        return;
    }
    node->lhs_->accept(*this);
    llvm::Value* L = curVal_;
    node->rhs_->accept(*this);
    llvm::Value* R = curVal_;
    bool isFloat = L->getType()->isFloatTy() || R->getType()->isFloatTy();
    llvm::Type* target = isFloat ? llvm::Type::getFloatTy(*context) : llvm::Type::getInt32Ty(*context);
    checkAndCast(L, target);
    checkAndCast(R, target);
    if (isFloat) {
        switch(node->op_) {
            case BinaryExp::ADD: curVal_ = builder->CreateFAdd(L, R); break;
            case BinaryExp::SUB: curVal_ = builder->CreateFSub(L, R); break;
            case BinaryExp::MUL: curVal_ = builder->CreateFMul(L, R); break;
            case BinaryExp::DIV: curVal_ = builder->CreateFDiv(L, R); break;
            case BinaryExp::LT: curVal_ = builder->CreateFCmpOLT(L, R); break;
            case BinaryExp::GT: curVal_ = builder->CreateFCmpOGT(L, R); break;
            case BinaryExp::LE: curVal_ = builder->CreateFCmpOLE(L, R); break;
            case BinaryExp::GE: curVal_ = builder->CreateFCmpOGE(L, R); break;
            case BinaryExp::EQ: curVal_ = builder->CreateFCmpOEQ(L, R); break;
            case BinaryExp::NE: curVal_ = builder->CreateFCmpONE(L, R); break;
            default: break;
        }
    } else {
        switch(node->op_) {
            case BinaryExp::ADD: curVal_ = builder->CreateAdd(L, R); break;
            case BinaryExp::SUB: curVal_ = builder->CreateSub(L, R); break;
            case BinaryExp::MUL: curVal_ = builder->CreateMul(L, R); break;
            case BinaryExp::DIV: curVal_ = builder->CreateSDiv(L, R); break;
            case BinaryExp::MOD: curVal_ = builder->CreateSRem(L, R); break;
            case BinaryExp::LT: curVal_ = builder->CreateICmpSLT(L, R); break;
            case BinaryExp::GT: curVal_ = builder->CreateICmpSGT(L, R); break;
            case BinaryExp::LE: curVal_ = builder->CreateICmpSLE(L, R); break;
            case BinaryExp::GE: curVal_ = builder->CreateICmpSGE(L, R); break;
            case BinaryExp::EQ: curVal_ = builder->CreateICmpEQ(L, R); break;
            case BinaryExp::NE: curVal_ = builder->CreateICmpNE(L, R); break;
            default: break;
        }
    }
    if (curVal_->getType()->isIntegerTy(1)) {
        curVal_ = builder->CreateZExt(curVal_, llvm::Type::getInt32Ty(*context));
    }
}

void IrBuilder::visit(UnaryExp* node) {
    node->operand_->accept(*this);
    if (node->op_ == UnaryExp::PLUS) return;
    if (node->op_ == UnaryExp::MINUS) {
        if (curVal_->getType()->isFloatTy()) curVal_ = builder->CreateFNeg(curVal_);
        else curVal_ = builder->CreateNeg(curVal_);
    } else if (node->op_ == UnaryExp::NOT) {
        if (curVal_->getType()->isFloatTy()) {
            curVal_ = builder->CreateFCmpOEQ(curVal_, llvm::ConstantFP::get(curVal_->getType(), 0.0));
        } else {
            curVal_ = builder->CreateICmpEQ(curVal_, llvm::ConstantInt::get(curVal_->getType(), 0));
        }
        curVal_ = builder->CreateZExt(curVal_, llvm::Type::getInt32Ty(*context));
    }
}

void IrBuilder::visit(LVal* node) {
    SymbolEntry* entry = curSymTable_->lookup(node->name_);
    if (!entry) {
        std::cerr << "Error: Undefined variable: " << node->name_ << std::endl;
        exit(1);
    }
    llvm::Value* addr = entry->getValue();
    Type* sysyType = entry->getType();
    bool isParamPtr = (entry->getKind() == SymbolEntry::PARAM && sysyType->isPointer());
    // 如果为指针参数：先加载指针指向的实际地址，参数传递的是指针的指针
    if (isParamPtr) {
        addr = builder->CreateLoad(addr->getType()->getPointerElementType(), addr);
    }
    // GEP 指令的下标列表，用于计算数组元素的内存偏
    std::vector<llvm::Value*> indices;
    if (!isParamPtr) {
        indices.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0));
    }
    for (auto dim : node->dims_) {
        bool oldIsLVal = isLVal_;
        isLVal_ = false;   
        // 递归求值下标表达式
        dim->accept(*this);
        checkAndCast(curVal_, llvm::Type::getInt32Ty(*context));
        indices.push_back(curVal_);
        isLVal_ = oldIsLVal;
    }
    int totalDims = get_total_dims(sysyType);
    int providedDims = node->dims_.size();
    bool isArrayDecayed = false;
    if (providedDims < totalDims) {
        // 说明数组退化为指针，补0下标
        indices.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0));
        //  isArrayDecayed：标记数组是否退化
        isArrayDecayed = true;
    }
    llvm::Type* basePointeeType = addr->getType()->getPointerElementType();
    if (indices.size() > 0 && !(isParamPtr && indices.empty())) {
         addr = builder->CreateInBoundsGEP(basePointeeType, addr, indices);
    }
    if (isLVal_) {
        curVal_ = addr;
    } else {
        llvm::Type* finalType = addr->getType()->getPointerElementType();
        if (finalType->isArrayTy() || isArrayDecayed) {
            curVal_ = addr;
        } else {
            curVal_ = builder->CreateLoad(finalType, addr);
        }
    }
}

void IrBuilder::visit(CallExp* node) {
    SymbolEntry* entry = curSymTable_->lookup(node->func_);
    llvm::Function* func = (llvm::Function*)entry->getValue();
    std::vector<llvm::Value*> args;
    for (size_t i = 0; i < node->args_.size(); ++i) {
        node->args_[i]->accept(*this);
        llvm::Type* target = func->getFunctionType()->getParamType(i);
        checkAndCast(curVal_, target);
        args.push_back(curVal_);
    }
    curVal_ = builder->CreateCall(func, args);
}

void IrBuilder::visit(IntConst* node) {
    curVal_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), node->val_);
}

void IrBuilder::visit(FloatConst* node) {
    curVal_ = llvm::ConstantFP::get(llvm::Type::getFloatTy(*context), node->val_);
}

void IrBuilder::visit(FuncFParam* node) {}
