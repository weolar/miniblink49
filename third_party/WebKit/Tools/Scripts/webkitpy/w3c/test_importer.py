# Copyright (C) 2013 Adobe Systems Incorporated. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above
#    copyright notice, this list of conditions and the following
#    disclaimer.
# 2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials
#    provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

"""
 This script imports a directory of W3C tests into WebKit.

 This script will import the tests into WebKit following these rules:

    - By default, all tests are imported under LayoutTests/w3c/[repo-name].

    - By default, only reftests and jstest are imported. This can be overridden
      with a -a or --all argument

    - Also by default, if test files by the same name already exist in the
      destination directory, they are overwritten with the idea that running
      this script would refresh files periodically.  This can also be
      overridden by a -n or --no-overwrite flag

    - All files are converted to work in WebKit:
         1. Paths to testharness.js scripts and vendor-prefix.js files are
            modified to point to Webkit's copy of them in LayoutTests/resources,
            using the correct relative path from the new location.
         2. All CSS properties requiring the -webkit-vendor prefix are prefixed
            (the list of what needs prefixes is read from Source/WebCore/CSS/CSSProperties.in).
         3. Each reftest has its own copy of its reference file following
            the naming conventions new-run-webkit-tests expects.
         4. If a reference files lives outside the directory of the test that
            uses it, it is checked for paths to support files as it will be
            imported into a different relative position to the test file
            (in the same directory).
         5. Any tags with the class "instructions" have style="display:none" added
            to them. Some w3c tests contain instructions to manual testers which we
            want to strip out (the test result parser only recognizes pure testharness.js
            output and not those instructions).

     - Upon completion, script outputs the total number tests imported, broken
       down by test type

     - Also upon completion, if we are not importing the files in place, each
       directory where files are imported will have a w3c-import.log file written with
       a timestamp, the W3C Mercurial changeset if available, the list of CSS
       properties used that require prefixes, the list of imported files, and
       guidance for future test modification and maintenance. On subsequent
       imports, this file is read to determine if files have been
       removed in the newer changesets.  The script removes these files
       accordingly.
"""

# FIXME: Change this file to use the Host abstractions rather that os, sys, shutils, etc.

import datetime
import logging
import mimetypes
import optparse
import os
import shutil
import sys

from webkitpy.common.host import Host
from webkitpy.common.webkit_finder import WebKitFinder
from webkitpy.common.system.executive import ScriptError
from webkitpy.layout_tests.models.test_expectations import TestExpectationParser
from webkitpy.w3c.test_parser import TestParser
from webkitpy.w3c.test_converter import convert_for_webkit


CHANGESET_NOT_AVAILABLE = 'Not Available'


_log = logging.getLogger(__name__)


def main(_argv, _stdout, _stderr):
    options, args = parse_args()
    dir_to_import = os.path.normpath(os.path.abspath(args[0]))
    if len(args) == 1:
        top_of_repo = dir_to_import
    else:
        top_of_repo = os.path.normpath(os.path.abspath(args[1]))

    if not os.path.exists(dir_to_import):
        sys.exit('Directory %s not found!' % dir_to_import)
    if not os.path.exists(top_of_repo):
        sys.exit('Repository directory %s not found!' % top_of_repo)
    if top_of_repo not in dir_to_import:
        sys.exit('Repository directory %s must be a parent of %s' % (top_of_repo, dir_to_import))

    configure_logging()
    test_importer = TestImporter(Host(), dir_to_import, top_of_repo, options)
    test_importer.do_import()


def configure_logging():
    class LogHandler(logging.StreamHandler):

        def format(self, record):
            if record.levelno > logging.INFO:
                return "%s: %s" % (record.levelname, record.getMessage())
            return record.getMessage()

    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    handler = LogHandler()
    handler.setLevel(logging.INFO)
    logger.addHandler(handler)
    return handler


def parse_args():
    parser = optparse.OptionParser(usage='usage: %prog [options] [dir_to_import] [top_of_repo]')
    parser.add_option('-n', '--no-overwrite', dest='overwrite', action='store_false', default=True,
        help='Flag to prevent duplicate test files from overwriting existing tests. By default, they will be overwritten.')
    parser.add_option('-a', '--all', action='store_true', default=False,
        help='Import all tests including reftests, JS tests, and manual/pixel tests. By default, only reftests and JS tests are imported.')
    parser.add_option('-d', '--dest-dir', dest='destination', default='w3c',
        help='Import into a specified directory relative to the LayoutTests root. By default, files are imported under LayoutTests/w3c.')
    parser.add_option('--ignore-expectations', action='store_true', default=False,
        help='Ignore the W3CImportExpectations file and import everything.')
    parser.add_option('--dry-run', action='store_true', default=False,
        help='Dryrun only (don\'t actually write any results).')

    options, args = parser.parse_args()
    if len(args) > 2:
        parser.error('Incorrect number of arguments')
    elif len(args) == 0:
        args = (os.getcwd(),)
    return options, args


class TestImporter(object):

    def __init__(self, host, dir_to_import, top_of_repo, options):
        self.host = host
        self.dir_to_import = dir_to_import
        self.top_of_repo = top_of_repo
        self.options = options

        self.filesystem = self.host.filesystem
        self.webkit_finder = WebKitFinder(self.filesystem)
        self._webkit_root = self.webkit_finder.webkit_base()
        self.layout_tests_dir = self.webkit_finder.path_from_webkit_base('LayoutTests')
        self.destination_directory = self.filesystem.normpath(self.filesystem.join(self.layout_tests_dir, options.destination,
                                                                                   self.filesystem.basename(self.top_of_repo)))
        self.import_in_place = (self.dir_to_import == self.destination_directory)
        self.dir_above_repo = self.filesystem.dirname(self.top_of_repo)

        self.changeset = CHANGESET_NOT_AVAILABLE

        self.import_list = []

    def do_import(self):
        _log.info("Importing %s into %s", self.dir_to_import, self.destination_directory)
        self.find_importable_tests(self.dir_to_import)
        self.load_changeset()
        self.import_tests()

    def load_changeset(self):
        """Returns the current changeset from mercurial or "Not Available"."""
        try:
            self.changeset = self.host.executive.run_command(['hg', 'tip']).split('changeset:')[1]
        except (OSError, ScriptError):
            self.changeset = CHANGESET_NOT_AVAILABLE

    def find_importable_tests(self, directory):
        # FIXME: use filesystem
        paths_to_skip = self.find_paths_to_skip()

        for root, dirs, files in os.walk(directory):
            cur_dir = root.replace(self.dir_above_repo + '/', '') + '/'
            _log.info('  scanning ' + cur_dir + '...')
            total_tests = 0
            reftests = 0
            jstests = 0

            # Files in 'tools' are not for browser testing (e.g., a script for generating test files).
            # http://testthewebforward.org/docs/test-format-guidelines.html#tools
            DIRS_TO_SKIP = ('.git', '.hg', 'test-plan', 'tools')

            # Need to copy all files in 'support', including HTML without meta data.
            # http://testthewebforward.org/docs/test-format-guidelines.html#support-files
            DIRS_TO_INCLUDE = ('resources', 'support')

            if dirs:
                for d in DIRS_TO_SKIP:
                    if d in dirs:
                        dirs.remove(d)

                for path in paths_to_skip:
                    path_base = path.replace(self.options.destination + '/', '')
                    path_base = path_base.replace(cur_dir, '')
                    path_full = self.filesystem.join(root, path_base)
                    if path_base in dirs:
                        dirs.remove(path_base)
                        if not self.options.dry_run and self.import_in_place:
                            _log.info("  pruning %s" % path_base)
                            self.filesystem.rmtree(path_full)
                        else:
                            _log.info("  skipping %s" % path_base)


            copy_list = []

            for filename in files:
                path_full = self.filesystem.join(root, filename)
                path_base = path_full.replace(directory + '/', '')
                path_base = self.destination_directory.replace(self.layout_tests_dir + '/', '') + '/' + path_base
                if path_base in paths_to_skip:
                    if not self.options.dry_run and self.import_in_place:
                        _log.info("  pruning %s" % path_base)
                        self.filesystem.remove(path_full)
                        continue
                    else:
                        continue
                # FIXME: This block should really be a separate function, but the early-continues make that difficult.

                if filename.startswith('.') or filename.endswith('.pl'):
                    continue  # For some reason the w3c repo contains random perl scripts we don't care about.

                fullpath = os.path.join(root, filename)

                mimetype = mimetypes.guess_type(fullpath)
                if not 'html' in str(mimetype[0]) and not 'application/xhtml+xml' in str(mimetype[0]) and not 'application/xml' in str(mimetype[0]):
                    copy_list.append({'src': fullpath, 'dest': filename})
                    continue

                if os.path.basename(root) in DIRS_TO_INCLUDE:
                    copy_list.append({'src': fullpath, 'dest': filename})
                    continue

                test_parser = TestParser(vars(self.options), filename=fullpath)
                test_info = test_parser.analyze_test()
                if test_info is None:
                    continue

                if 'reference' in test_info.keys():
                    reftests += 1
                    total_tests += 1
                    test_basename = os.path.basename(test_info['test'])

                    # Add the ref file, following WebKit style.
                    # FIXME: Ideally we'd support reading the metadata
                    # directly rather than relying  on a naming convention.
                    # Using a naming convention creates duplicate copies of the
                    # reference files.
                    ref_file = os.path.splitext(test_basename)[0] + '-expected'
                    # Make sure to use the extension from the *reference*, not
                    # from the test, because at least flexbox tests use XHTML
                    # references but HTML tests.
                    ref_file += os.path.splitext(test_info['reference'])[1]

                    copy_list.append({'src': test_info['reference'], 'dest': ref_file, 'reference_support_info': test_info['reference_support_info']})
                    copy_list.append({'src': test_info['test'], 'dest': filename})

                elif 'jstest' in test_info.keys():
                    jstests += 1
                    total_tests += 1
                    copy_list.append({'src': fullpath, 'dest': filename})
                else:
                    total_tests += 1
                    copy_list.append({'src': fullpath, 'dest': filename})

            if copy_list:
                # Only add this directory to the list if there's something to import
                self.import_list.append({'dirname': root, 'copy_list': copy_list,
                    'reftests': reftests, 'jstests': jstests, 'total_tests': total_tests})

    def find_paths_to_skip(self):
        if self.options.ignore_expectations:
            return set()

        paths_to_skip = set()
        port = self.host.port_factory.get()
        w3c_import_expectations_path = self.webkit_finder.path_from_webkit_base('LayoutTests', 'W3CImportExpectations')
        w3c_import_expectations = self.filesystem.read_text_file(w3c_import_expectations_path)
        parser = TestExpectationParser(port, all_tests=(), is_lint_mode=False)
        expectation_lines = parser.parse(w3c_import_expectations_path, w3c_import_expectations)
        for line in expectation_lines:
            if 'SKIP' in line.expectations:
                if line.specifiers:
                    _log.warning("W3CImportExpectations:%s should not have any specifiers" % line.line_numbers)
                    continue
                paths_to_skip.add(line.name)
        return paths_to_skip

    def import_tests(self):
        total_imported_tests = 0
        total_imported_reftests = 0
        total_imported_jstests = 0
        total_prefixed_properties = {}

        for dir_to_copy in self.import_list:
            total_imported_tests += dir_to_copy['total_tests']
            total_imported_reftests += dir_to_copy['reftests']
            total_imported_jstests += dir_to_copy['jstests']

            prefixed_properties = []

            if not dir_to_copy['copy_list']:
                continue

            orig_path = dir_to_copy['dirname']

            subpath = os.path.relpath(orig_path, self.top_of_repo)
            new_path = os.path.join(self.destination_directory, subpath)

            if not(os.path.exists(new_path)):
                os.makedirs(new_path)

            copied_files = []

            for file_to_copy in dir_to_copy['copy_list']:
                # FIXME: Split this block into a separate function.
                orig_filepath = os.path.normpath(file_to_copy['src'])

                if os.path.isdir(orig_filepath):
                    # FIXME: Figure out what is triggering this and what to do about it.
                    _log.error('%s refers to a directory' % orig_filepath)
                    continue

                if not(os.path.exists(orig_filepath)):
                    _log.warning('%s not found. Possible error in the test.', orig_filepath)
                    continue

                new_filepath = os.path.join(new_path, file_to_copy['dest'])
                if 'reference_support_info' in file_to_copy.keys() and file_to_copy['reference_support_info'] != {}:
                    reference_support_info = file_to_copy['reference_support_info']
                else:
                    reference_support_info = None

                if not(os.path.exists(os.path.dirname(new_filepath))):
                    if not self.import_in_place and not self.options.dry_run:
                        os.makedirs(os.path.dirname(new_filepath))

                relpath = os.path.relpath(new_filepath, self.layout_tests_dir)
                if not self.options.overwrite and os.path.exists(new_filepath):
                    _log.info('  skipping %s' % relpath)
                else:
                    # FIXME: Maybe doing a file diff is in order here for existing files?
                    # In other words, there's no sense in overwriting identical files, but
                    # there's no harm in copying the identical thing.
                    _log.info('  %s' % relpath)

                # Only html, xml, or css should be converted
                # FIXME: Eventually, so should js when support is added for this type of conversion
                mimetype = mimetypes.guess_type(orig_filepath)
                if 'html' in str(mimetype[0]) or 'xml' in str(mimetype[0])  or 'css' in str(mimetype[0]):
                    converted_file = convert_for_webkit(new_path, filename=orig_filepath, reference_support_info=reference_support_info)

                    if not converted_file:
                        if not self.import_in_place and not self.options.dry_run:
                            shutil.copyfile(orig_filepath, new_filepath)  # The file was unmodified.
                    else:
                        for prefixed_property in converted_file[0]:
                            total_prefixed_properties.setdefault(prefixed_property, 0)
                            total_prefixed_properties[prefixed_property] += 1

                        prefixed_properties.extend(set(converted_file[0]) - set(prefixed_properties))
                        if not self.options.dry_run:
                            outfile = open(new_filepath, 'wb')
                            outfile.write(converted_file[1].encode('utf-8'))
                            outfile.close()
                else:
                    if not self.import_in_place and not self.options.dry_run:
                        shutil.copyfile(orig_filepath, new_filepath)

                copied_files.append(new_filepath.replace(self._webkit_root, ''))

        _log.info('')
        _log.info('Import complete')
        _log.info('')
        _log.info('IMPORTED %d TOTAL TESTS', total_imported_tests)
        _log.info('Imported %d reftests', total_imported_reftests)
        _log.info('Imported %d JS tests', total_imported_jstests)
        _log.info('Imported %d pixel/manual tests', total_imported_tests - total_imported_jstests - total_imported_reftests)
        _log.info('')

        if total_prefixed_properties:
            _log.info('Properties needing prefixes (by count):')
            for prefixed_property in sorted(total_prefixed_properties, key=lambda p: total_prefixed_properties[p]):
                _log.info('  %s: %s', prefixed_property, total_prefixed_properties[prefixed_property])

    def setup_destination_directory(self):
        """ Creates a destination directory that mirrors that of the source directory """

        new_subpath = self.dir_to_import[len(self.top_of_repo):]

        destination_directory = os.path.join(self.destination_directory, new_subpath)

        if not os.path.exists(destination_directory):
            os.makedirs(destination_directory)

        _log.info('Tests will be imported into: %s', destination_directory)
