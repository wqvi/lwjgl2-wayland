#!/usr/bin/env python3

import sys;
import json;
from pathlib import Path

"""
Some poorly written script I made to automate using this library a bit more.
You just need to right click on the instance of your choice.
Then click edit.
Then go to versions.
Then click on "LWJGL 2". And on the right side of that click customize.
"""

if len(sys.argv) >= 0 and len(sys.argv) <= 2:
    print('Invalid argument(s)', file=sys.stderr)
    print('First argument must be a valid Minecraft instance directory.', file=sys.stderr)
    print('Second argument must be valid lwjgl2-wayland repository, that has been compiled', file=sys.stderr);
    exit(1)

inst_dir = Path(sys.argv[1])
if not inst_dir.is_dir():
    print('Please provide a valid path to a Minecraft instance directory.', file=sys.stderr)
    exit(1)

inst_libs = inst_dir / 'libraries/'
inst_symlink_jar = inst_libs / 'lwjgl-wayland.jar'
if not inst_libs.is_dir():
    print('Creating library dir to place lwjgl-wayland.jar.')
    inst_libs.mkdir()

org_lwjgl_json = inst_dir / 'patches/org.lwjgl.json'
if not org_lwjgl_json.is_file():
    print('Please make sure you set the "LWJGL 2" version to customized in the Prism Launcher UI', file=sys.stderr)
    exit(1)

git_dir = Path(sys.argv[2])
if not git_dir.is_dir():
    print('Please provide a valid path to the lwjgl2-wayland git repository. This has to be cloned to your machine', file=sys.stderr)
    exit(1)

git_jar = (git_dir / 'libs/lwjgl.jar').resolve()
if not git_jar.is_file():
    print('Make sure to compile the lwjgl2-wayland repo.', file=sys.stderr)
    exit(1)

if inst_symlink_jar.exists() or inst_symlink_jar.is_symlink():
    inst_symlink_jar.unlink()

inst_symlink_jar.symlink_to(git_jar)
print(f'symlink created at: {inst_symlink_jar}')

with open(org_lwjgl_json.resolve(), 'r') as file:
    data = json.load(file)

data['libraries'][4].clear()

custom_data = {
        'name': 'org.lwjgl.lwjgl:lwjgl:wayland',
        'MMC-hint': 'local'
        }

data['libraries'][4].update(custom_data)

with org_lwjgl_json.open('w') as file:
    json.dump(data, file, indent=4)
