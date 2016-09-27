#!/usr/bin/python

import hashlib
import operator
import os
import shutil
import stat
import subprocess
import sys
import tempfile


def rel_to_abs(rel_path):
    return os.path.join(script_path, rel_path)


java_bin_path = os.getenv('JAVA_HOME', '')
if java_bin_path:
    java_bin_path = os.path.join(java_bin_path, 'bin')

main_class = 'org.chromium.devtools.jsdoc.JsDocValidator'
jar_name = 'jsdoc-validator.jar'
hashes_name = 'hashes'
src_dir = 'src'
script_path = os.path.dirname(os.path.abspath(__file__))
closure_jar_relpath = os.path.join('..', 'closure', 'compiler.jar')
src_path = rel_to_abs(src_dir)
hashes_path = rel_to_abs(hashes_name)


def get_file_hash(file, blocksize=65536):
    sha = hashlib.sha256()
    buf = file.read(blocksize)
    while len(buf) > 0:
        sha.update(buf)
        buf = file.read(blocksize)
    return sha.hexdigest()


def traverse(hasher, path):
    abs_path = rel_to_abs(path)
    info = os.lstat(abs_path)
    quoted_name = repr(path.replace('\\', '/'))
    if stat.S_ISDIR(info.st_mode) and not os.path.basename(path).startswith('.'):
        hasher.update('d ' + quoted_name + '\n')
        for entry in sorted(os.listdir(abs_path)):
            traverse(hasher, os.path.join(path, entry))
    elif stat.S_ISREG(info.st_mode) and path.endswith('.java'):
        hasher.update('r ' + quoted_name + ' ')
        hasher.update(str(info.st_size) + ' ')
        with open(abs_path, 'Ur') as file:
            f_hash = get_file_hash(file)
            hasher.update(f_hash + '\n')


def get_src_dir_hash(dir):
    sha = hashlib.sha256()
    traverse(sha, dir)
    return sha.hexdigest()


def get_actual_hashes():
    hashed_files = [(jar_name, True)]
    hashes = {}
    for (file_name, binary) in hashed_files:
        try:
            hash = get_file_hash(open(file_name, 'rb' if binary else 'r'))
            hashes[file_name] = hash
        except IOError:
            hashes[file_name] = '0'
    hashes[src_dir] = get_src_dir_hash(src_dir)
    return hashes


def get_expected_hashes():
    try:
        with open(hashes_path, 'r') as file:
            return {file_name: hash for (file_name, hash) in [(name.strip(), hash.strip()) for (hash, name) in [line.split(' ', 1) for line in file]]}
    except:
        return None


def run_and_communicate(command, error_template):
    proc = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
    proc.communicate()
    if proc.returncode:
        print >> sys.stderr, error_template % proc.returncode
        sys.exit(proc.returncode)


def build_artifacts():
    print 'Compiling...'
    java_files = []
    for root, dirs, files in sorted(os.walk(src_path)):
        for file_name in files:
            if file_name.endswith('.java'):
                java_files.append(os.path.join(root, file_name))

    bin_path = tempfile.mkdtemp()
    manifest_file = tempfile.NamedTemporaryFile(mode='wt', delete=False)
    try:
        manifest_file.write('Class-Path: %s\n' % closure_jar_relpath)
        manifest_file.close()
        javac_path = os.path.join(java_bin_path, 'javac')
        javac_command = '%s -d %s -cp %s %s' % (javac_path, bin_path, rel_to_abs(closure_jar_relpath), ' '.join(java_files))
        run_and_communicate(javac_command, 'Error: javac returned %d')

        print 'Building jar...'
        artifact_path = rel_to_abs(jar_name)
        jar_path = os.path.join(java_bin_path, 'jar')
        jar_command = '%s cvfme %s %s %s -C %s .' % (jar_path, artifact_path, manifest_file.name, main_class, bin_path)
        run_and_communicate(jar_command, 'Error: jar returned %d')
    finally:
        os.remove(manifest_file.name)
        shutil.rmtree(bin_path, True)


def update_hashes():
    print 'Updating hashes...'
    with open(hashes_path, 'w') as file:
        file.writelines(['%s %s\n' % (hash, name) for (name, hash) in get_actual_hashes().iteritems()])


def hashes_modified():
    expected_hashes = get_expected_hashes()
    if not expected_hashes:
        return [('<no expected hashes>', 1, 0)]
    actual_hashes = get_actual_hashes()
    results = []
    for name, expected_hash in expected_hashes.iteritems():
        actual_hash = actual_hashes.get(name)
        if expected_hash != actual_hash:
            results.append((name, expected_hash, actual_hash))
    return results


def help():
    print 'usage: %s [option]' % os.path.basename(__file__)
    print 'Options:'
    print '--force-rebuild: Rebuild classes and jar even if there are no source file changes'
    print '--no-rebuild: Do not rebuild jar, just update hashes'


def main():
    no_rebuild = False
    force_rebuild = False

    if len(sys.argv) > 1:
        if sys.argv[1] == '--help':
            help()
            return
        no_rebuild = sys.argv[1] == '--no-rebuild'
        force_rebuild = sys.argv[1] == '--force-rebuild'

    if not hashes_modified() and not force_rebuild:
        print 'No modifications found, rebuild not required.'
        return
    if not no_rebuild:
        build_artifacts()

    update_hashes()
    print 'Done.'

if __name__ == '__main__':
    main()
