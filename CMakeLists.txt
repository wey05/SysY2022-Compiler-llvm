cmake_minimum_required(VERSION 3.10)
project(SysYCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fno-rtti")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# 2. Flex & Bison
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(BISON_HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/sysy.tab.h")
set(BISON_SRC_FILE "${CMAKE_CURRENT_BINARY_DIR}/sysy.tab.cpp")
set(FLEX_SRC_FILE "${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp")

BISON_TARGET(
    sysy_parser 
    src/sysy.y 
    ${BISON_SRC_FILE}
    COMPILE_FLAGS "--defines=${BISON_HEADER_FILE} -o ${BISON_SRC_FILE}"
)

FLEX_TARGET(
    sysy_lexer 
    src/sysy.l 
    ${FLEX_SRC_FILE}
    COMPILE_FLAGS "-o${FLEX_SRC_FILE}"
)

ADD_FLEX_BISON_DEPENDENCY(sysy_lexer sysy_parser)

include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

# 3. 映射 LLVM 组件
# 修改点：添加了 scalaropts, instcombine, transformutils, analysis, ipo 以支持优化 Pass
llvm_map_components_to_libnames(llvm_libs 
    core 
    support 
    codegen 
    target 
    riscvcodegen 
    riscvdesc 
    riscvinfo 
    riscvasmparser
    mc
    scalaropts
    instcombine
    transformutils
    analysis
    ipo
)

add_executable(compiler
    src/main.cpp
    src/Ast.cpp
    src/Type.cpp
    src/SymbolTable.cpp
    src/IrBuilder.cpp
    ${BISON_SRC_FILE}
    ${FLEX_SRC_FILE}
)

target_link_libraries(compiler ${llvm_libs} pthread tinfo dl)