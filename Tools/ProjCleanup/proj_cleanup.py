import os
import shutil
from datetime import datetime
import yaml

# === Determine directory paths ===
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TARGET_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))  # One level up
BACKUP_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "..", ".."))  # Two levels up
ALLOWED_RESOURCES_YAML = os.path.join(SCRIPT_DIR, "allowed_resources.yaml")  # YAML config in same dir as script

# === Create timestamped backup directory ===
timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
BASE_BACKUP_DIR = os.path.join(BACKUP_ROOT, "backup")
TIMESTAMPED_BACKUP_DIR = os.path.join(BASE_BACKUP_DIR, timestamp)
os.makedirs(TIMESTAMPED_BACKUP_DIR, exist_ok=True)

# === Load allowed files and directories from YAML ===
with open(ALLOWED_RESOURCES_YAML, "r") as f:
    config = yaml.safe_load(f)

allowed_files = set(config.get("files", []))
allowed_dirs = set(config.get("directories", []))

# === Backup allowed files ===
for filename in allowed_files:
    source_path = os.path.join(TARGET_DIR, filename)
    dest_path = os.path.join(TIMESTAMPED_BACKUP_DIR, filename)

    if os.path.isfile(source_path):
        os.makedirs(os.path.dirname(dest_path), exist_ok=True)
        shutil.copy2(source_path, dest_path)
        print(f"Backed up file: {filename}")

# === Backup allowed directories ===
for dirname in allowed_dirs:
    source_path = os.path.join(TARGET_DIR, dirname)
    dest_path = os.path.join(TIMESTAMPED_BACKUP_DIR, dirname)

    if os.path.isdir(source_path):
        shutil.copytree(source_path, dest_path)
        print(f"Backed up directory: {dirname}")

# === Cleanup: Delete everything not explicitly allowed ===
for item in os.listdir(TARGET_DIR):
    if item in allowed_files or item in allowed_dirs or item == "backup":
        continue

    item_path = os.path.join(TARGET_DIR, item)
    try:
        if os.path.isfile(item_path) or os.path.islink(item_path):
            os.remove(item_path)
            print(f"Deleted file: {item}")
        elif os.path.isdir(item_path):
            shutil.rmtree(item_path)
            print(f"Deleted directory: {item}")
    except Exception as e:
        print(f"Failed to delete {item}: {e}")
