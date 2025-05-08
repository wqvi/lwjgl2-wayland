#!/usr/bin/env python3

import configparser
import json
from pathlib import Path
import sys


def create_library_symlink(sym_path, jar_path):
    if not (sym_path.exists() or sym_path.is_symlink()):
        sym_path.symlink_to(jar_path)
        print(f'Symlink created at: {sym_path}')
    else:
        print(f'Symlink already exists at: {sym_path}')


def patch_library(path):
    with open(path, 'r') as file:
        data = json.load(file)


    if data['libraries'][4]['name'] != 'org.lwjgl.lwjgl:lwjgl:wayland':
        print('Patching org.lwjgl.json')
        data['libraries'][4].clear()
        data['libraries'][4].update({
            'name': 'org.lwjgl.lwjgl:lwjgl:wayland',
            'MMC-hint': 'local'
            })
        with open(path, 'w') as file:
            json.dump(data, file, indent=4)
    else:
        print('Patch already applied to org.lwjgl.json')


if __name__ == '__main__':
    print('Open the console window for the instance of your choice')
    print('edit -> version -> lwjgl2 -> customize')
    print('')

    if len(sys.argv) >= 0 and len(sys.argv) <= 1:
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

    git_dir = Path.cwd()
    if len(sys.argv) > 2:
        git_dir = Path(sys.argv[2])


    if not git_dir.is_dir():
        print('Please provide a valid path to the lwjgl2-wayland git repository. This has to be cloned to your machine', file=sys.stderr)
        exit(1)

    git_jar = (git_dir / 'libs/lwjgl.jar').resolve()
    if not git_jar.is_file():
        print('Make sure to compile the lwjgl2-wayland repo.', file=sys.stderr)
        exit(1)


    create_library_symlink(inst_symlink_jar, git_jar)
    patch_library(org_lwjgl_json)
