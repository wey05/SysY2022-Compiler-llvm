import os
import shutil
import subprocess
import glob
import json
import re  # 引入正则模块
from flask import Flask, render_template, jsonify, request, Response

app = Flask(__name__)

# --- 配置路径 ---
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
CURRENT_TEST_DIR = os.path.join(ROOT_DIR, 'tests') 

FILES = {
    'source': 'test.sy',
    'input':  'test.in',
    'output': 'test.out',
    'asm':    'test.s',
    'exe':    'test.exe'
}

@app.route('/')
def index():
    return render_template('index.html')

# --- 1. 目录扫描 ---
@app.route('/api/list_dirs')
def list_dirs():
    all_items = os.listdir(ROOT_DIR)
    dirs = [d for d in all_items if os.path.isdir(os.path.join(ROOT_DIR, d)) and d.startswith('tests')]
    dirs.sort()
    return jsonify(dirs)

@app.route('/api/change_dir', methods=['POST'])
def change_dir():
    global CURRENT_TEST_DIR
    target = request.json.get('dir')
    new_path = os.path.join(ROOT_DIR, target)
    if os.path.exists(new_path) and os.path.isdir(new_path):
        CURRENT_TEST_DIR = new_path
        return jsonify({'success': True})
    return jsonify({'success': False, 'msg': 'Directory not found'})

@app.route('/api/list_tests')
def list_tests():
    if not os.path.exists(CURRENT_TEST_DIR): return jsonify([])
    files = sorted([os.path.basename(f) for f in glob.glob(os.path.join(CURRENT_TEST_DIR, "*.sy"))])
    return jsonify(files)

# --- 2. 编译构建 ---
@app.route('/api/build', methods=['POST'])
def build_compiler():
    try:
        result = subprocess.run(['python3', 'auto_build.py'], cwd=ROOT_DIR, capture_output=True, text=True)
        return jsonify({'success': result.returncode == 0, 'log': result.stdout + result.stderr})
    except Exception as e:
        return jsonify({'success': False, 'log': str(e)})

# --- 3. 核心评测逻辑 ---

def clean_string_output(text):
    """
    清洗字符串：
    使用正则删除 Time...s 片段。
    """
    if not text:
        return ""
    
    lines = text.splitlines()
    cleaned_lines = []
    
    # 核心正则：忽略大小写，匹配 Time 开头到第一个 s 结尾
    pattern = re.compile(r"(?i)Timer.*?s")
    
    for line in lines:
        cleaned_line = pattern.sub("", line).strip()
        if cleaned_line:
            cleaned_lines.append(cleaned_line)
        
    return "\n".join(cleaned_lines).strip()

def compare_output(expected, actual):
    """
    双向清洗后比对
    """
    exp_clean = clean_string_output(expected)
    act_clean = clean_string_output(actual)

    # 如果预期输出本来是空（或清洗后为空），且实际输出清洗后也是空，则认为通过
    # (例如 void main() {} 什么都不输出，但可能有Time)
    if not exp_clean and not act_clean:
        return True

    return exp_clean == act_clean

def execute_test_case(test_name, dir_path):
    base_name = os.path.splitext(test_name)[0]
    src_sy = os.path.join(dir_path, test_name)
    src_in = os.path.join(dir_path, base_name + '.in')
    src_out = os.path.join(dir_path, base_name + '.out')

    shutil.copy(src_sy, FILES['source'])
    
    input_content = ""
    if os.path.exists(src_in):
        shutil.copy(src_in, FILES['input'])
        with open(src_in, 'r') as f: input_content = f.read()
    else:
        with open(FILES['input'], 'w') as f: f.write("")

    expected_content = ""
    if os.path.exists(src_out):
        with open(src_out, 'r') as f: expected_content = f.read()

    # 调用 run_single_test.py (注意它现在已经包含了清洗逻辑)
    try:
        process = subprocess.run(
            ['python3', 'run_single_test.py'], 
            cwd=ROOT_DIR, 
            capture_output=True, 
            text=True, 
            timeout=10
        )
        log = process.stdout + process.stderr
        ret_code = process.returncode
    except subprocess.TimeoutExpired:
        log = "Timeout Error: Execution time exceeded 10s."
        ret_code = -1
    except Exception as e:
        log = f"System Error: {str(e)}"
        ret_code = -1

    # 获取 run_single_test.py 写入的 cleaned output
    actual_output = ""
    if os.path.exists(FILES['output']):
        with open(FILES['output'], 'r', errors='ignore') as f: actual_output = f.read()
    
    asm_code = ""
    if os.path.exists(FILES['asm']):
        with open(FILES['asm'], 'r') as f: asm_code = f.read()

    is_pass = False
    if ret_code == 0:
        # 即使 run_single_test.py 已经清洗过了，我们在 WebUI 这里再做一次比对逻辑也是安全的
        # 关键是保证 expected_content 也被同样的逻辑清洗
        is_pass = compare_output(expected_content, actual_output)
    
    return {
        'source': open(FILES['source'], 'r').read(),
        'input': input_content,
        'expected': expected_content,
        'actual': actual_output,
        'asm': asm_code,
        'log': log,
        'pass': is_pass,
        'test_name': test_name
    }

# --- 4. 接口路由 ---

@app.route('/api/run_test', methods=['POST'])
def run_test():
    test_name = request.json.get('test_name')
    result = execute_test_case(test_name, CURRENT_TEST_DIR)
    return jsonify(result)

@app.route('/api/run_all_stream')
def run_all_stream():
    target_dir_name = request.args.get('dir', 'tests')
    target_dir = os.path.join(ROOT_DIR, target_dir_name)
    
    if not os.path.exists(target_dir):
        return jsonify({'error': 'Directory not found'})

    files = sorted([os.path.basename(f) for f in glob.glob(os.path.join(target_dir, "*.sy"))])
    total_count = len(files)

    def generate():
        yield f"data: {json.dumps({'type': 'start', 'total': total_count})}\n\n"
        for idx, f in enumerate(files):
            res = execute_test_case(f, target_dir)
            data = {
                'type': 'result',
                'index': idx + 1,
                'name': f,
                'pass': res['pass'],
                'log': res['log'] if not res['pass'] else "OK"
            }
            yield f"data: {json.dumps(data)}\n\n"
        yield f"data: {json.dumps({'type': 'done'})}\n\n"

    return Response(generate(), mimetype='text/event-stream')

@app.route('/api/run_custom', methods=['POST'])
def run_custom():
    data = request.json
    source_code = data.get('source', '')
    input_data = data.get('input', '')

    try:
        with open(FILES['source'], 'w') as f: f.write(source_code)
        with open(FILES['input'], 'w') as f: f.write(input_data)
        if os.path.exists(FILES['output']): os.remove(FILES['output'])
        if os.path.exists(FILES['asm']): os.remove(FILES['asm'])
    except Exception as e:
        return jsonify({'success': False, 'log': f"File Write Error: {str(e)}"})

    try:
        process = subprocess.run(['python3', 'run_single_test.py'], cwd=ROOT_DIR, capture_output=True, text=True, timeout=15)
        log = process.stdout + "\n" + process.stderr
    except subprocess.TimeoutExpired:
        log = "Timeout Error"
    except Exception as e:
        log = str(e)

    asm_code = "; Compilation Failed"
    if os.path.exists(FILES['asm']):
        with open(FILES['asm'], 'r') as f: asm_code = f.read()

    actual_output = ""
    if os.path.exists(FILES['output']):
        with open(FILES['output'], 'r', errors='ignore') as f: actual_output = f.read()

    return jsonify({
        'asm': asm_code,
        'actual': actual_output, # 这已经是 run_single_test.py 清洗后的了
        'log': log
    })

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
