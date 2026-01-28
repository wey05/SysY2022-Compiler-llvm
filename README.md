# SysY2022 编译器

## 项目简介

这是一个基于 LLVM 的 SysY 语言编译器，能够将 SysY2022 源代码编译为 RISC-V 64位架构的汇编代码。该项目是编译原理课程的大作业实验项目，完整实现了从词法分析到目标代码生成的整个编译过程。

## 项目特性

- **完整的编译器前端**: 词法分析、语法分析、语义分析
- **LLVM IR 生成**: 基于 LLVM 框架构建中间代码
- **RISC-V 目标代码**: 支持 RISC-V 64位架构汇编输出
- **代码优化**: 集成 LLVM 优化 Pass（死代码消除、内存到寄存器转换等）
- **类型系统**: 支持整数(int)和浮点数(float)类型
- **语言特性**: 支持数组、函数、循环、条件语句等
- **测试框架**: 完整的自动化测试系统
- **Web 界面**: 基于 Flask 的可视化测试界面

## 项目结构

```
Compiler_100/
├── CMakeLists.txt              # CMake 构建配置
├── auto_build.py               # 自动 run_single_test.py构建脚本
├──          # 单个测试运行脚本
├── run_tests.py                # 批量测试脚本
├── web_ui.py                   # Web 评测界面
├── include/                    # 头文件目录
│   ├── Ast.h                   # AST 节点定义
│   ├── IrBuilder.h             # IR 构建器接口
│   ├── SymbolTable.h           # 符号表接口
│   └── Type.h                  # 类型系统定义
├── src/                        # 源代码目录
│   ├── main.cpp                # 编译器主程序
│   ├── Ast.cpp                 # AST 实现
│   ├── Type.cpp                # 类型系统实现
│   ├── SymbolTable.cpp         # 符号表实现
│   ├── IrBuilder.cpp           # IR 构建器实现
│   ├── sysy.l                  # Flex 词法分析器
│   └── sysy.y                  # Bison 语法分析器
├── sylib.c                     # 系统库源码
├── test.sy                     # 示例测试文件
├── tests_fuctional_1/          # 功能测试用例集1
├── tests_fuctional_2/          # 功能测试用例集2
├── tests_performance_1/        # 性能测试用例集1
├── tests_performance_2/        # 性能测试用例集2
└── templates/                  # Web 界面模板
    └── index.html              # Web 界面 HTML 模板
```

## 编译要求

### 系统要求
- **操作系统**: Linux（推荐 Ubuntu 22.04 或 24.04）
- **C++ 编译器**: 支持 C++17 (如 GCC 7+, Clang 5+)
- **CMake**: 推荐版本 4.2.1 
- **LLVM**: 推荐版本 14.0 （更新版本可能不兼容）
- **Flex**: 词法分析器生成器
- **Bison**: 语法分析器生成器

### 依赖安装

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install cmake build-essential flex bison llvm-dev clang
sudo apt install gcc-riscv64-linux-gnu qemu-user-static  # RISC-V 工具链
```

## 编译方法

### 方法一：使用自动构建脚本（推荐）
```bash
python3 auto_build.py
```

### 方法二：手动构建
```bash
# 1. 创建构建目录
mkdir build && cd build

# 2. 配置 CMake
cmake ..

# 3. 编译
make 

# 4. 返回项目根目录
cd ..
```

编译成功后，会在 `build/` 目录下生成 `compiler` 可执行文件。

## 使用方法

### 基本用法
```bash
# 编译 SysY 源文件，生成汇编代码
./build/compiler input.sy -o output.s

# 默认输出文件名
./build/compiler input.sy  # 生成 input.s
```

### 测试运行

#### 单个测试
```bash
# 运行单个测试文件
python3 run_single_test.py

# 需要确保测试目录中有 test.sy 文件，同时也可以自己建一个 test.in 文件来输入测试数据
```

#### 批量测试
```bash
# 运行所有指定文件夹下的测试：注意修改 run_tests.py 中的指定文件夹
python3 run_tests.py
```

### Web 界面

启动 Web 评测界面：
```bash
python3 web_ui.py
```

然后在浏览器中访问 `http://127.0.0.1:5000`，可以进行：
- 查看和选择测试用例
- 在线编译和运行测试
- 查看编译结果和汇编代码
- 批量运行测试并查看统计信息

## SysY 语言特性

SysY 是一个类 C 的简单编程语言（其实就是 C 语言的子集拓展），支持以下特性：

### 数据类型
- `int`: 32 位整数
- `float`: 32 位浮点数
- `const`: 常量修饰符

### 控制结构
- `if-else`: 条件语句
- `while`: 循环语句
- `break` / `continue`: 循环控制

### 函数
- 函数定义和调用
- 参数传递
- 返回值

### 数组
- 一维数组
- 数组初始化
- 数组索引访问

### 示例代码

```c
int n;
int bubblesort(int arr[])
{
    int i;
    int j;
    i =0; 
    while(i < n-1){
    // Last i elements are already in place
        j = 0;
        while(j < n-i-1){
            if (arr[j] > arr[j+1]) {
                // swap(&arr[j], &arr[j+1]); 
                int tmp;
                tmp = arr[j+1];
                arr[j+1] = arr[j];
                arr[j] = tmp;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    return 0;
}

int main(){
    n = 10;
    int a[10];
    a[0]=4;a[1]=3;a[2]=9;a[3]=2;a[4]=0;
    a[5]=1;a[6]=6;a[7]=5;a[8]=7;a[9]=8;
    int i;
    i = bubblesort(a);
    while (i < n) {
        int tmp;
        tmp = a[i];
        putint(tmp);
        tmp = 10;
        putch(tmp);
        i = i + 1;
    }
    return 0;
}

```

## 技术实现

### 编译器架构
1. **词法分析**: 使用 Flex 将源代码转换为记号流
2. **语法分析**: 使用 Bison 构建抽象语法树 (AST)
3. **语义分析**: 符号表管理和类型检查
4. **中间代码生成**: 将 AST 转换为 LLVM IR
5. **目标代码生成**: LLVM IR 编译为 RISC-V 汇编

### 优化 Pass
- 内存到寄存器转换 (Mem2Reg)
- 死代码消除 (Dead Code Elimination)
- 标量优化 (Scalar Optimizations)



## 测试用例说明

### 功能测试
- `tests_fuctional_1/`: 基础语法测试（算术运算、条件语句、循环等）
- `tests_fuctional_2/`: 高级功能测试（图算法、数据结构、递归等）

### 性能测试 (没找到的话请在竞赛官网下载)
- `tests_performance_1/`: 性能测试集1
- `tests_performance_2/`: 性能测试集2（矩阵运算、FFT、排序算法等）

### 测试文件命名规则
- `*.sy`: SysY 源代码文件
- `*.in`: 输入文件
- `*.out`: 期望输出文件

### 测试通过情况
- 140个功能测试用例：全部通过
- 119个性能测试用例：通过了115个，3个超时（10s），1个编译错误
- 一共259个测试用例，通过254个

## 常见问题

### 编译错误
1. **LLVM 未找到**: 确保安装了 LLVM 开发包 (`llvm-dev`)
2. **RISC-V 工具链缺失**: 安装 riscv64 交叉编译工具链
3. **Flex/Bison 版本问题**: 确保版本兼容

## 作者
四人略

## 致谢

感谢编译原理课程的王挺和陈振邦老师，以及 LLVM 社区提供的优秀工具和框架，同时部分内容如数组的定义和传参、网页前后端、LLVM调用等代码参考了 C 语言官方文档以及大模型辅助编写。

---

**注意**: 本项目仅用于学习和研究目的，不建议用于生产环境。完整项目在https://github.com/wey05/SysY2022-Compiler-llvm
