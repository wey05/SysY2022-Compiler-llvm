#!/usr/bin/env python3
import os
import subprocess
import sys
import shutil

def main():
    # 项目根目录
    root_dir = os.path.dirname(os.path.abspath(__file__))
    build_dir = os.path.join(root_dir, 'build')
    
    # 检查并删除现有build目录
    if os.path.exists(build_dir):
        print(f"Removing existing build directory: {build_dir}")
        try:
            shutil.rmtree(build_dir)
            print(f"Successfully removed {build_dir}")
        except Exception as e:
            print(f"Error removing build directory: {e}")
            sys.exit(1)
    
    # 创建新的build目录
    print(f"Creating build directory: {build_dir}")
    os.makedirs(build_dir)
    
    try:
        # 执行cmake ..
        print("Running cmake..")
        subprocess.run(['cmake', '..'], cwd=build_dir, check=True)
        
        # 执行make
        print("Running make")
        subprocess.run(['make'], cwd=build_dir, check=True)
        
        print("\nBuild completed successfully!")
        
        # 检查生成的编译器是否存在
        compiler_path = os.path.join(build_dir, 'compiler')
        if os.path.exists(compiler_path):
            print(f"\nCompiler generated at: {compiler_path}")
        else:
            print(f"\nWarning: Compiler not found at expected path: {compiler_path}")
            
    except subprocess.CalledProcessError as e:
        print(f"Build failed with error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()