import sys
import subprocess
import os

# Get arguments
sdcc = sys.argv[1].replace('/', '\\')
if sdcc.endswith('sdcc'):
    sdcc += '.exe'
    
src = sys.argv[2]
obj = sys.argv[3]
mark = sys.argv[4]
flags = sys.argv[5:]

# Check if .cpp
cpp_flags = []
if src.lower().endswith('.cpp'):
    cpp_flags = ['-x', 'c', '--include', 'dummy_variable_main.h']

# Build command
cmd = [sdcc] + flags + cpp_flags + [src, '-o', obj]

# Execute
result = subprocess.run(cmd)

# Copy .o to .rel
if os.path.exists(obj):
    rel = obj.replace('.o', '.rel')
    if obj != rel:
        try:
            import shutil
            shutil.copy2(obj, rel)
        except:
            pass

sys.exit(result.returncode)