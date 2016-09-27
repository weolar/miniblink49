#!/usr/bin/env python
# Copyright (c) 2012 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from modular_build import read_file, write_file
import os
import os.path as path
import generate_injected_script_externs
import generate_protocol_externs
import modular_build
import re
import shutil
import subprocess
import sys
import tempfile
try:
    import simplejson as json
except ImportError:
    import json


if len(sys.argv) == 2 and sys.argv[1] == '--help':
    print("Usage: %s [module_names]" % path.basename(sys.argv[0]))
    print("  module_names    list of modules for which the Closure compilation should run.")
    print("                  If absent, the entire frontend will be compiled.")
    sys.exit(0)

is_cygwin = sys.platform == 'cygwin'


def popen(arguments):
    return subprocess.Popen(arguments, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

def to_platform_path(filepath):
    if not is_cygwin:
        return filepath
    return re.sub(r'^/cygdrive/(\w)', '\\1:', filepath)


def to_platform_path_exact(filepath):
    if not is_cygwin:
        return filepath
    output, _ = popen(['cygpath', '-w', filepath]).communicate()
    # pylint: disable=E1103
    return output.strip().replace('\\', '\\\\')

scripts_path = path.dirname(path.abspath(__file__))
devtools_path = path.dirname(scripts_path)
inspector_path = path.join(path.dirname(devtools_path), 'core', 'inspector')
devtools_frontend_path = path.join(devtools_path, 'front_end')
patched_es6_externs_file = to_platform_path(path.join(devtools_frontend_path, 'es6.js'))
global_externs_file = to_platform_path(path.join(devtools_frontend_path, 'externs.js'))
protocol_externs_file = path.join(devtools_frontend_path, 'protocol_externs.js')
injected_script_source_name = path.join(inspector_path, 'InjectedScriptSource.js')
injected_script_externs_file = path.join(inspector_path, 'injected_script_externs.js')

jsmodule_name_prefix = 'jsmodule_'
runtime_module_name = '_runtime'

type_checked_jsdoc_tags_list = ['param', 'return', 'type', 'enum']
type_checked_jsdoc_tags_or = '|'.join(type_checked_jsdoc_tags_list)

# Basic regex for invalid JsDoc types: an object type name ([A-Z][A-Za-z0-9.]+[A-Za-z0-9]) not preceded by '!', '?', ':' (this, new), or '.' (object property).
invalid_type_regex = re.compile(r'@(?:' + type_checked_jsdoc_tags_or + r')\s*\{.*(?<![!?:.A-Za-z0-9])([A-Z][A-Za-z0-9.]+[A-Za-z0-9])[^/]*\}')
invalid_type_designator_regex = re.compile(r'@(?:' + type_checked_jsdoc_tags_or + r')\s*.*(?<![{: ])([?!])=?\}')
invalid_non_object_type_regex = re.compile(r'@(?:' + type_checked_jsdoc_tags_or + r')\s*\{.*(![a-z]+)[^/]*\}')
error_warning_regex = re.compile(r'WARNING|ERROR')
loaded_css_regex = re.compile(r'(?:registerRequiredCSS|WebInspector\.View\.createStyleElement)\s*\(\s*"(.+)"\s*\)')

java_build_regex = re.compile(r'^\w+ version "(\d+)\.(\d+)')
errors_found = False

generate_protocol_externs.generate_protocol_externs(protocol_externs_file, path.join(devtools_path, 'protocol.json'))


# Based on http://stackoverflow.com/questions/377017/test-if-executable-exists-in-python.
def which(program):
    def is_exe(fpath):
        return path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for part in os.environ["PATH"].split(os.pathsep):
            part = part.strip('"')
            exe_file = path.join(part, program)
            if is_exe(exe_file):
                return exe_file
    return None


def log_error(message):
    print 'ERROR: ' + message

def error_excepthook(exctype, value, traceback):
    print 'ERROR:'
    sys.__excepthook__(exctype, value, traceback)
sys.excepthook = error_excepthook

application_descriptors = ['inspector.json', 'toolbox.json']
loader = modular_build.DescriptorLoader(devtools_frontend_path)
descriptors = loader.load_applications(application_descriptors)
modules_by_name = descriptors.modules


def hasErrors(output):
    return re.search(error_warning_regex, output) != None


def verify_jsdoc_extra(additional_files):
    files = [to_platform_path(file) for file in descriptors.all_compiled_files() + additional_files]
    file_list = tempfile.NamedTemporaryFile(mode='wt', delete=False)
    try:
        file_list.write('\n'.join(files))
    finally:
        file_list.close()
    return popen(java_exec + ['-jar', jsdoc_validator_jar, '--files-list-name', to_platform_path_exact(file_list.name)]), file_list


def verify_jsdoc(additional_files):
    def file_list():
        return descriptors.all_compiled_files() + additional_files

    errors_found = False
    for full_file_name in file_list():
        lineIndex = 0
        with open(full_file_name, 'r') as sourceFile:
            for line in sourceFile:
                line = line.rstrip()
                lineIndex += 1
                if not line:
                    continue
                if verify_jsdoc_line(full_file_name, lineIndex, line):
                    errors_found = True
    return errors_found


def verify_jsdoc_line(fileName, lineIndex, line):
    def print_error(message, errorPosition):
        print '%s:%s: ERROR - %s%s%s%s%s%s' % (fileName, lineIndex, message, os.linesep, line, os.linesep, ' ' * errorPosition + '^', os.linesep)

    known_css = {}
    errors_found = False
    match = re.search(invalid_type_regex, line)
    if match:
        print_error('Type "%s" nullability not marked explicitly with "?" (nullable) or "!" (non-nullable)' % match.group(1), match.start(1))
        errors_found = True

    match = re.search(invalid_non_object_type_regex, line)
    if match:
        print_error('Non-object type explicitly marked with "!" (non-nullable), which is the default and should be omitted', match.start(1))
        errors_found = True

    match = re.search(invalid_type_designator_regex, line)
    if match:
        print_error('Type nullability indicator misplaced, should precede type', match.start(1))
        errors_found = True

    match = re.search(loaded_css_regex, line)
    if match:
        file = path.join(devtools_frontend_path, match.group(1))
        exists = known_css.get(file)
        if exists is None:
            exists = path.isfile(file)
            known_css[file] = exists
        if not exists:
            print_error('Dynamically loaded CSS stylesheet is missing in the source tree', match.start(1))
            errors_found = True
    return errors_found


def find_java():
    required_major = 1
    required_minor = 7
    exec_command = None
    has_server_jvm = True
    java_path = which('java')
    if not java_path:
        java_path = which('java.exe')

    if not java_path:
        print 'NOTE: No Java executable found in $PATH.'
        sys.exit(1)

    is_ok = False
    java_version_out, _ = popen([java_path, '-version']).communicate()
    # pylint: disable=E1103
    match = re.search(java_build_regex, java_version_out)
    if match:
        major = int(match.group(1))
        minor = int(match.group(2))
        is_ok = major >= required_major and minor >= required_minor
    if is_ok:
        exec_command = [java_path, '-Xms1024m', '-server', '-XX:+TieredCompilation']
        check_server_proc = popen(exec_command + ['-version'])
        check_server_proc.communicate()
        if check_server_proc.returncode != 0:
            # Not all Java installs have server JVMs.
            exec_command = exec_command.remove('-server')
            has_server_jvm = False

    if not is_ok:
        print 'NOTE: Java executable version %d.%d or above not found in $PATH.' % (required_major, required_minor)
        sys.exit(1)
    print 'Java executable: %s%s' % (java_path, '' if has_server_jvm else ' (no server JVM)')
    return exec_command

java_exec = find_java()

closure_compiler_jar = to_platform_path(path.join(scripts_path, 'closure', 'compiler.jar'))
closure_runner_jar = to_platform_path(path.join(scripts_path, 'compiler-runner', 'closure-runner.jar'))
jsdoc_validator_jar = to_platform_path(path.join(scripts_path, 'jsdoc-validator', 'jsdoc-validator.jar'))

modules_dir = tempfile.mkdtemp()
common_closure_args = [
    '--summary_detail_level', '3',
    '--jscomp_error', 'visibility',
    '--compilation_level', 'SIMPLE_OPTIMIZATIONS',
    '--warning_level', 'VERBOSE',
    '--language_in=ES6_STRICT',
    '--language_out=ES5_STRICT',
    '--accept_const_keyword',
    '--extra_annotation_name', 'suppressReceiverCheck',
    '--extra_annotation_name', 'suppressGlobalPropertiesCheck',
    '--module_output_path_prefix', to_platform_path_exact(modules_dir + path.sep)
]

worker_modules_by_name = {}
dependents_by_module_name = {}

for module_name in descriptors.application:
    module = descriptors.modules[module_name]
    if descriptors.application[module_name].get('type', None) == 'worker':
        worker_modules_by_name[module_name] = module
    for dep in module.get('dependencies', []):
        list = dependents_by_module_name.get(dep)
        if not list:
            list = []
            dependents_by_module_name[dep] = list
        list.append(module_name)


def check_conditional_dependencies():
    for name in modules_by_name:
        for dep_name in modules_by_name[name].get('dependencies', []):
            dependency = modules_by_name[dep_name]
            if dependency.get('experiment') or dependency.get('condition'):
                log_error('Module "%s" may not depend on the conditional module "%s"' % (name, dep_name))
                errors_found = True

check_conditional_dependencies()


def verify_worker_modules():
    for name in modules_by_name:
        for dependency in modules_by_name[name].get('dependencies', []):
            if dependency in worker_modules_by_name:
                log_error('Module "%s" may not depend on the worker module "%s"' % (name, dependency))
                errors_found = True

verify_worker_modules()


def check_duplicate_files():

    def check_module(module, seen_files, seen_modules):
        name = module['name']
        seen_modules[name] = True
        for dep_name in module.get('dependencies', []):
            if not dep_name in seen_modules:
                check_module(modules_by_name[dep_name], seen_files, seen_modules)
        for source in module.get('scripts', []):
            referencing_module = seen_files.get(source)
            if referencing_module:
                log_error('Duplicate use of %s in "%s" (previously seen in "%s")' % (source, name, referencing_module))
            seen_files[source] = name

    for module_name in worker_modules_by_name:
        check_module(worker_modules_by_name[module_name], {}, {})

print 'Checking duplicate files across modules...'
check_duplicate_files()


def module_arg(module_name):
    return ' --module ' + jsmodule_name_prefix + module_name


def modules_to_check():
    if len(sys.argv) == 1:
        return descriptors.sorted_modules()
    print 'Compiling only these modules: %s' % sys.argv[1:]
    return [module for module in descriptors.sorted_modules() if module in set(sys.argv[1:])]


def dump_module(name, recursively, processed_modules):
    if name in processed_modules:
        return ''
    processed_modules[name] = True
    module = modules_by_name[name]
    skipped_scripts = set(module.get('skip_compilation', []))

    command = ''
    dependencies = module.get('dependencies', [])
    if recursively:
        for dependency in dependencies:
            command += dump_module(dependency, recursively, processed_modules)
    command += module_arg(name) + ':'
    filtered_scripts = descriptors.module_compiled_files(name)
    command += str(len(filtered_scripts))
    firstDependency = True
    for dependency in dependencies + [runtime_module_name]:
        if firstDependency:
            command += ':'
        else:
            command += ','
        firstDependency = False
        command += jsmodule_name_prefix + dependency
    for script in filtered_scripts:
        command += ' --js ' + to_platform_path(path.join(devtools_frontend_path, name, script))
    return command

print 'Compiling frontend...'

compiler_args_file = tempfile.NamedTemporaryFile(mode='wt', delete=False)
try:
    platform_protocol_externs_file = to_platform_path(protocol_externs_file)
    runtime_js_path = to_platform_path(path.join(devtools_frontend_path, 'Runtime.js'))
    checked_modules = modules_to_check()
    for name in checked_modules:
        closure_args = ' '.join(common_closure_args)
        closure_args += ' --externs ' + to_platform_path(patched_es6_externs_file)
        closure_args += ' --externs ' + to_platform_path(global_externs_file)
        closure_args += ' --externs ' + platform_protocol_externs_file
        runtime_module = module_arg(runtime_module_name) + ':1 --js ' + runtime_js_path
        closure_args += runtime_module + dump_module(name, True, {})
        compiler_args_file.write('%s %s%s' % (name, closure_args, os.linesep))
finally:
    compiler_args_file.close()

modular_compiler_proc = popen(java_exec + ['-jar', closure_runner_jar, '--compiler-args-file', to_platform_path_exact(compiler_args_file.name)])


def unclosure_injected_script(sourceFileName, outFileName):

    source = read_file(sourceFileName)

    def replace_function(matchobj):
        return re.sub(r'@param', 'param', matchobj.group(1) or '') + '\n//' + matchobj.group(2)

    # Comment out the closure function and its jsdocs
    source = re.sub(r'(/\*\*(?:[\s\n]*\*\s*@param[^\n]+\n)+\s*\*/\s*)?\n(\(function)', replace_function, source, count=1)

    # Comment out its return statement
    source = re.sub(r'\n(\s*return\s+[^;]+;\s*\n\}\)\s*)$', '\n/*\\1*/', source)

    # Replace the "var Object" override with a "self.Object" one
    source = re.sub(r'\nvar Object =', '\nself.Object =', source, count=1)

    write_file(outFileName, source)

injectedScriptSourceTmpFile = to_platform_path(path.join(inspector_path, 'InjectedScriptSourceTmp.js'))

unclosure_injected_script(injected_script_source_name, injectedScriptSourceTmpFile)

print 'Compiling InjectedScriptSource.js...'

spawned_compiler_command = java_exec + [
    '-jar',
    closure_compiler_jar
] + common_closure_args

command = spawned_compiler_command + [
    '--externs', to_platform_path_exact(injected_script_externs_file),
    '--externs', to_platform_path_exact(protocol_externs_file),
    '--module', jsmodule_name_prefix + 'injected_script' + ':1',
    '--js', to_platform_path(injectedScriptSourceTmpFile)
]

injectedScriptCompileProc = popen(command)

print 'Compiling devtools.js...'

command = spawned_compiler_command + [
    '--externs', to_platform_path(global_externs_file),
    '--externs', to_platform_path(path.join(devtools_frontend_path, 'host', 'InspectorFrontendHostAPI.js')),
    '--module', jsmodule_name_prefix + 'devtools_js' + ':1',
    '--js', to_platform_path(path.join(devtools_frontend_path, 'devtools.js'))
]
devtoolsJSCompileProc = popen(command)

print 'Verifying JSDoc comments...'
additional_jsdoc_check_files = [injectedScriptSourceTmpFile]
errors_found |= verify_jsdoc(additional_jsdoc_check_files)
jsdocValidatorProc, jsdocValidatorFileList = verify_jsdoc_extra(additional_jsdoc_check_files)

print 'Validating InjectedScriptSource.js...'
injectedscript_check_script_path = path.join(scripts_path, "check_injected_script_source.py")
validateInjectedScriptProc = popen([sys.executable, injectedscript_check_script_path, injected_script_source_name])

print

(jsdocValidatorOut, _) = jsdocValidatorProc.communicate()
if jsdocValidatorOut:
    print ('JSDoc validator output:%s%s' % (os.linesep, jsdocValidatorOut))
    errors_found = True

os.remove(jsdocValidatorFileList.name)

(moduleCompileOut, _) = modular_compiler_proc.communicate()
print 'Modular compilation output:'

start_module_regex = re.compile(r'^@@ START_MODULE:(.+) @@$')
end_module_regex = re.compile(r'^@@ END_MODULE @@$')

in_module = False
skipped_modules = {}
error_count = 0

def skip_dependents(module_name):
    for skipped_module in dependents_by_module_name.get(module_name, []):
        skipped_modules[skipped_module] = True

has_module_output = False

# pylint: disable=E1103
for line in moduleCompileOut.splitlines():
    if not in_module:
        match = re.search(start_module_regex, line)
        if not match:
            continue
        in_module = True
        has_module_output = True
        module_error_count = 0
        module_output = []
        module_name = match.group(1)
        skip_module = skipped_modules.get(module_name)
        if skip_module:
            skip_dependents(module_name)
    else:
        match = re.search(end_module_regex, line)
        if not match:
            if not skip_module:
                module_output.append(line)
                if hasErrors(line):
                    error_count += 1
                    module_error_count += 1
                    skip_dependents(module_name)
            continue

        in_module = False
        if skip_module:
            print 'Skipping module %s...' % module_name
        elif not module_error_count:
            print 'Module %s compiled successfully: %s' % (module_name, module_output[0])
        else:
            print 'Module %s compile failed: %s errors%s' % (module_name, module_error_count, os.linesep)
            print os.linesep.join(module_output)

if not has_module_output:
    print moduleCompileOut

if error_count:
    print 'Total Closure errors: %d%s' % (error_count, os.linesep)
    errors_found = True

(injectedScriptCompileOut, _) = injectedScriptCompileProc.communicate()
print 'InjectedScriptSource.js compilation output:%s' % os.linesep, injectedScriptCompileOut
errors_found |= hasErrors(injectedScriptCompileOut)

(devtoolsJSCompileOut, _) = devtoolsJSCompileProc.communicate()
print 'devtools.js compilation output:%s' % os.linesep, devtoolsJSCompileOut
errors_found |= hasErrors(devtoolsJSCompileOut)

(validateInjectedScriptOut, _) = validateInjectedScriptProc.communicate()
print 'Validate InjectedScriptSource.js output:%s' % os.linesep, (validateInjectedScriptOut if validateInjectedScriptOut else '<empty>')
errors_found |= hasErrors(validateInjectedScriptOut)

if errors_found:
    print 'ERRORS DETECTED'

os.remove(injectedScriptSourceTmpFile)
os.remove(compiler_args_file.name)
os.remove(protocol_externs_file)
shutil.rmtree(modules_dir, True)
