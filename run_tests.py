#!/usr/bin/env python3
import os
import subprocess
import glob
import sys

COMPILER = "./build/compiler"
SYLIB_SRC = "sylib.c"
SYLIB_OBJ = "sylib.o"
TEST_DIR = "tests_fuctional"
RISCV_GCC = "riscv64-unknown-linux-gnu-gcc"
QEMU = "qemu-riscv64"

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    RESET = '\033[0m'

def compile_sylib():
    cmd = [RISCV_GCC, "-c", SYLIB_SRC, "-o", SYLIB_OBJ]
    subprocess.run(cmd, check=True)

def run_single_test(sy_file):
    base_name = os.path.splitext(sy_file)[0]
    asm_file = base_name + ".s"
    exe_file = base_name + ".exe"
    in_file = base_name + ".in"
    out_file = base_name + ".out"

    compile_cmd = [COMPILER, sy_file, "-o", asm_file]
    res_compile = subprocess.run(compile_cmd, capture_output=True, text=True)
    if res_compile.returncode != 0:
        return False, "Compile Error", res_compile.stderr

    link_cmd = [RISCV_GCC, "-static", asm_file, SYLIB_OBJ, "-o", exe_file]
    res_link = subprocess.run(link_cmd, capture_output=True, text=True)
    if res_link.returncode != 0:
        return False, "Link Error", res_link.stderr

    input_content = None
    if os.path.exists(in_file):
        with open(in_file, 'rb') as f:
            input_content = f.read()

    try:
        res_run = subprocess.run(
            [QEMU, exe_file], 
            input=input_content, 
            capture_output=True, 
            timeout=5
        )
    except subprocess.TimeoutExpired:
        return False, "Runtime Error", "Timeout"
    
    if not os.path.exists(out_file):
        return False, "Missing .out", ""

    with open(out_file, 'rb') as f:
        expected_output = f.read().strip().replace(b'\r\n', b'\n')
    
    actual_output = res_run.stdout.strip().replace(b'\r\n', b'\n')

    if os.path.exists(asm_file): os.remove(asm_file)
    if os.path.exists(exe_file): os.remove(exe_file)

    if actual_output == expected_output:
        return True, "Pass", ""
    else:
        try:
            exp_str = expected_output.decode('utf-8')
            act_str = actual_output.decode('utf-8')
        except:
            exp_str = str(expected_output)
            act_str = str(actual_output)
        err_msg = f"\n{Colors.YELLOW}Expected:{Colors.RESET}\n{exp_str}\n{Colors.YELLOW}Got:{Colors.RESET}\n{act_str}"
        return False, "Wrong Answer", err_msg

def main():
    if not os.path.exists(COMPILER):
        print("Compiler not found.")
        sys.exit(1)
    compile_sylib()
    sy_files = sorted(glob.glob(os.path.join(TEST_DIR, "*.sy")))
    passed_count = 0
    failed_count = 0
    failed_cases = []

    for sy_file in sy_files:
        test_name = os.path.basename(sy_file)
        print(f"Testing {test_name:30s} ... ", end='', flush=True)
        success, status, message = run_single_test(sy_file)
        if success:
            print(f"{Colors.GREEN}[PASS]{Colors.RESET}")
            passed_count += 1
        else:
            print(f"{Colors.RED}[FAIL] ({status}){Colors.RESET}")
            print(message)
            failed_count += 1
            failed_cases.append(test_name)

    print(f"\nTotal: {len(sy_files)}, Passed: {passed_count}, Failed: {failed_count}")
    if failed_cases:
        sys.exit(1)

if __name__ == "__main__":
    main()