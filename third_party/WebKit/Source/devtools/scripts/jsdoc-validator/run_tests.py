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

java_exec = 'java -Xms1024m -server -XX:+TieredCompilation'
tests_dir = 'tests'
jar_name = 'jsdoc-validator.jar'
script_path = os.path.dirname(os.path.abspath(__file__))
tests_path = rel_to_abs(tests_dir)
validator_jar_file = rel_to_abs(jar_name)
golden_file = os.path.join(tests_path, 'golden.dat')

test_files = [os.path.join(tests_path, f) for f in os.listdir(tests_path) if f.endswith('.js') and os.path.isfile(os.path.join(tests_path, f))]

validator_command = "%s -jar %s %s" % (java_exec, validator_jar_file, " ".join(sorted(test_files)))


def run_and_communicate(command, error_template):
    proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    (out, _) = proc.communicate()
    if proc.returncode:
        print >> sys.stderr, error_template % proc.returncode
        sys.exit(proc.returncode)
    return out


def help():
    print 'usage: %s [option]' % os.path.basename(__file__)
    print 'Options:'
    print '--generate-golden: Re-generate golden file'
    print '--dump: Dump the test results to stdout'


def main():
    need_golden = False
    need_dump = False
    if len(sys.argv) > 1:
        if sys.argv[1] == '--generate-golden':
            need_golden = True
        elif sys.argv[1] == '--dump':
            need_dump = True
        else:
            help()
            return

    result = run_and_communicate(validator_command, "Error running validator: %d")
    result = result.replace(script_path, "")  # pylint: disable=E1103
    if need_dump:
        print result
        return

    if need_golden:
        with open(golden_file, 'wt') as golden:
            golden.write(result)
    else:
        with open(golden_file, 'rt') as golden:
            golden_text = golden.read()
            if golden_text == result:
                print 'OK'
            else:
                print 'ERROR: Golden output mismatch'

if __name__ == '__main__':
    main()
