#!/usr/bin/env python3
import os
import subprocess
import sys

COMPILER = "./build/compiler"
SYLIB_SRC = "sylib.c"
SYLIB_OBJ = "sylib.o"
TEST_FILE = "test.sy"
TEST_IN = "test.in"
TEST_OUT = "test.out"
RISCV_GCC = "riscv64-unknown-linux-gnu-gcc"
QEMU = "qemu-riscv64"

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    RESET = '\033[0m'

def compile_sylib():
    """编译系统库"""
    cmd = [RISCV_GCC, "-c", SYLIB_SRC, "-o", SYLIB_OBJ]
    subprocess.run(cmd, check=True)

def run_test():
    """运行单个测试"""
    base_name = os.path.splitext(TEST_FILE)[0]
    asm_file = base_name + ".s"
    exe_file = base_name + ".exe"
    
    # 检查编译器是否存在
    if not os.path.exists(COMPILER):
        print(f"{Colors.RED}Error: Compiler not found at {COMPILER}{Colors.RESET}")
        sys.exit(1)
    
    # 编译系统库
    print(f"Compiling {SYLIB_SRC}...")
    try:
        compile_sylib()
    except subprocess.CalledProcessError as e:
        print(f"{Colors.RED}Error compiling {SYLIB_SRC}: {e}{Colors.RESET}")
        sys.exit(1)
    
    # 编译测试代码
    print(f"Compiling {TEST_FILE}...")
    compile_cmd = [COMPILER, TEST_FILE, "-o", asm_file]
    res_compile = subprocess.run(compile_cmd, capture_output=True, text=True)
    if res_compile.returncode != 0:
        print(f"{Colors.RED}Compile Error: {Colors.RESET}")
        print(res_compile.stderr)
        sys.exit(1)
    
    # 链接生成可执行文件
    print(f"Linking {asm_file}...")
    link_cmd = [RISCV_GCC, "-static", asm_file, SYLIB_OBJ, "-o", exe_file]
    res_link = subprocess.run(link_cmd, capture_output=True, text=True)
    if res_link.returncode != 0:
        print(f"{Colors.RED}Link Error: {Colors.RESET}")
        print(res_link.stderr)
        sys.exit(1)
    
    # 读取输入文件
    input_content = None
    if os.path.exists(TEST_IN):
        print(f"Reading input from {TEST_IN}...")
        with open(TEST_IN, 'rb') as f:
            input_content = f.read()
    
    # 执行程序
    print(f"Running {exe_file}...")
    try:
        res_run = subprocess.run(
            [QEMU, exe_file], 
            input=input_content, 
            capture_output=True, 
            timeout=10
        )
    except subprocess.TimeoutExpired:
        print(f"{Colors.RED}Runtime Error: Timeout{Colors.RESET}")
        sys.exit(1)
    
    # 处理输出
    actual_output = res_run.stdout.strip().replace(b'\r\n', b'\n')
    
    # 打印输出
    print(f"\n{Colors.YELLOW}Program Output:{Colors.RESET}")
    try:
        output_str = actual_output.decode('utf-8')
        print(output_str)
    except UnicodeDecodeError:
        print(f"Raw output: {actual_output}")
    
    # 保存输出到test.out
    print(f"\nSaving output to {TEST_OUT}...")
    with open(TEST_OUT, 'wb') as f:
        f.write(actual_output + b'\n')
    

    
    print(f"\n{Colors.GREEN}Test completed successfully!{Colors.RESET}")
    print(f"Output saved to {TEST_OUT}")

if __name__ == "__main__":
    run_test()