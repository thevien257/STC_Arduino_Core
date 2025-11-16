import sys
import subprocess
import os
import shutil

# Get SDCC path and normalize
sdcc = sys.argv[1].replace('/', '\\')
if sdcc.endswith('sdcc'):
    sdcc += '.exe'

# Process all other arguments
args = []
for arg in sys.argv[2:]:
    # Normalize all paths (replace forward slashes)
    normalized_arg = arg.replace('/', '\\')
    
    if normalized_arg.endswith('.o'):
        # Convert .o to .rel
        rel_file = normalized_arg.replace('.o', '.rel')
        if not os.path.exists(rel_file) and os.path.exists(normalized_arg):
            shutil.copy2(normalized_arg, rel_file)
        args.append(rel_file)
    elif normalized_arg.endswith('.a'):
        # Convert .a to .lib
        lib_file = normalized_arg.replace('.a', '.lib')
        if not os.path.exists(lib_file) and os.path.exists(normalized_arg):
            shutil.copy2(normalized_arg, lib_file)
        args.append(lib_file)
    else:
        args.append(normalized_arg)

# Debug: Write command to a file
with open(os.path.join(os.environ.get('TEMP', 'C:\\'), 'sdcc_link_debug.txt'), 'w') as f:
    f.write("Command:\n")
    f.write(sdcc + '\n')
    for a in args:
        f.write(a + '\n')
    f.write('\n')
    
    # Check if library path exists
    for a in args:
        if a.startswith('-L'):
            lib_path = a[2:].strip('"')
            f.write(f"Library path: {lib_path}\n")
            f.write(f"Exists: {os.path.exists(lib_path)}\n")
            if os.path.exists(lib_path):
                f.write(f"Contents: {os.listdir(lib_path)}\n")

# Build and execute command
cmd = [sdcc] + args
result = subprocess.run(cmd)

sys.exit(result.returncode)