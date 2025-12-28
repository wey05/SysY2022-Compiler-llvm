#include <iostream>
#include <fstream>
#include <memory>
#include "Ast.h"
#include "IrBuilder.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/InitializePasses.h>
#include <llvm/PassRegistry.h>
#include <llvm/Transforms/Utils.h> 
#include <llvm/Transforms/Scalar.h>

extern FILE* yyin;
extern int yyparse();
extern CompUnit* root;

extern "C" void LLVMInitializeRISCVTargetInfo();    //基本信息
extern "C" void LLVMInitializeRISCVTarget();        //目标机器
extern "C" void LLVMInitializeRISCVTargetMC();      //机器代码
extern "C" void LLVMInitializeRISCVAsmParser();     //汇编解析器
extern "C" void LLVMInitializeRISCVAsmPrinter();    //汇编打印机

int main(int argc, char** argv) {
    //注册pass
    llvm::PassRegistry &Registry = *llvm::PassRegistry::getPassRegistry();
    llvm::initializeCore(Registry);             //基础
    llvm::initializeScalarOpts(Registry);       //死代码消除
    llvm::initializeTransformUtils(Registry);   //内存到寄存器转换
    llvm::initializeAnalysis(Registry);         //支配树分析
    llvm::initializeTarget(Registry);           //目标代码相关

    LLVMInitializeRISCVTargetInfo();
    LLVMInitializeRISCVTarget();
    LLVMInitializeRISCVTargetMC();
    LLVMInitializeRISCVAsmParser();
    LLVMInitializeRISCVAsmPrinter();

    std::string inputFile;
    std::string outputFile = "a.s";

    //例如./compiler input.sy -o output.s
    //         0        1      2    3
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o") {
            if (i + 1 < argc) outputFile = argv[++i];
        } else {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Usage: compiler input.sy -o output.s" << std::endl;
        return 1;
    }

    yyin = fopen(inputFile.c_str(), "r");
    if (!yyin) {
        std::cerr << "Cannot open file: " << inputFile << std::endl;
        return 1;
    }

    if (yyparse() != 0) return 1;

    //构建LLVM IR
    IrBuilder builder;
    builder.visit(root);
    llvm::Module* module = builder.getModule();

    //验证IR
    if (llvm::verifyModule(*module, &llvm::errs())) {
        std::cerr << "Source Code: " << inputFile << std::endl;
        std::cerr << "[Fatal Error] IR Verification Failed. The generated code is invalid." << std::endl;
        return 1;
    }

    //运行Pass
    std::unique_ptr<llvm::legacy::FunctionPassManager> fpm = std::make_unique<llvm::legacy::FunctionPassManager>(module);//创建pass管理器
    fpm->add(llvm::createPromoteMemoryToRegisterPass());
    fpm->add(llvm::createDeadCodeEliminationPass());
    fpm->doInitialization();//初始化pass管理器
    for (auto &F : *module) {
        if (!F.isDeclaration()) {
            fpm->run(F);
        }
    }

    //设置Target和ABI
    std::string targetTriple = "riscv64-unknown-linux-gnu";
    module->setTargetTriple(targetTriple);

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        std::cerr << "Error: " << error << std::endl;
        return 1;
    }

    std::string cpu = "generic-rv64";
    std::string features = "+m,+a,+f,+d,+c";

    llvm::TargetOptions opt;
    opt.FloatABIType = llvm::FloatABI::Hard; //设置浮点ABI为Hard，确保浮点参数通过fa0-fa7寄存器传递，而不是整数寄存器
    auto rm = llvm::Optional<llvm::Reloc::Model>(); // 默认静态链接
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);//创建目标机器
    module->setDataLayout(targetMachine->createDataLayout());//将目标机器的数据布局信息设置到LLVM模块中

    //生成汇编
    std::error_code ec;
    llvm::raw_fd_ostream dest(outputFile, ec, llvm::sys::fs::OF_None);
    if (ec) {
        std::cerr << "Could not open file: " << ec.message();
        return 1;
    }

    llvm::legacy::PassManager pass;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_AssemblyFile)) {
        std::cerr << "TargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*module);
    dest.flush();

    return 0;
}