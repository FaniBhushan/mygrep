import os
import random
import stat

# ==== Configuration ====
pattern = "test"
output_dir = "test_files"
num_small_files = 10000
num_pattern_lines_large_file = 100000
large_file_path = "large_text_file.txt"

# ==== Step 1: Create 10,000 small text files ====
os.makedirs(output_dir, exist_ok=True)

print(f"Creating {num_small_files} text files in '{output_dir}'...")

for i in range(num_small_files):
    file_path = os.path.join(output_dir, f"file_{i}.txt")

    include_pattern = random.random() < 0.3  # 30% of files include 'test'

    if include_pattern:
        content = f"This file contains the pattern: {pattern}\nSome more content here.\n"
    else:
        content = "This file does not contain the keyword.\nJust some dummy content.\n"

    with open(file_path, "w") as f:
        f.write(content)

print(f"✅ Created {num_small_files} files in '{output_dir}'.")

# ==== Step 2: Create a large file with 100,000 pattern lines ====
print(f"Creating large file '{large_file_path}' with {num_pattern_lines_large_file} lines containing '{pattern}'...")

with open(large_file_path, "w") as f:
    for i in range(num_pattern_lines_large_file):
        f.write(f"This is line {i} with the pattern {pattern}.\n")

print(f"✅ Created large file: '{large_file_path}' with {num_pattern_lines_large_file} pattern lines.")

# ==== Step 3: Create a restricted folder ====
# Folder to create
folder_name = "restricted_folder"

# Create the folder
os.makedirs(folder_name, exist_ok=True)

# Remove read, write, and execute permissions for everyone
os.chmod(folder_name, 0)

print(f"✅ Folder '{folder_name}' created with no permissions.")

# ==== Step 4: Create a restricted file ====

# File name
file_name = "restricted_file.txt"

# Create the file
with open(file_name, "w") as f:
    f.write("This file is private and now locked down.\n")

# Remove all permissions (no read/write/execute for anyone)
os.chmod(file_name, 0)

print(f"✅ Created file '{file_name}' with no permissions.")