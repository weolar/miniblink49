# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Pull latest revisions of the W3C test repos and update our DEPS entries."""

import argparse
import re

from webkitpy.common.webkit_finder import WebKitFinder


class DepsUpdater(object):
    def __init__(self, host):
        self.host = host
        self.executive = host.executive
        self.fs = host.filesystem
        self.finder = WebKitFinder(self.fs)
        self.verbose = False
        self.allow_local_blink_commits = False
        self.keep_w3c_repos_around = False

    def main(self, argv=None):
        self.parse_args(argv)

        self.cd('')
        if not self.checkout_is_okay():
            return 1

        self.print_('## noting the current Blink commitish')
        blink_commitish = self.run(['git', 'show-ref', 'HEAD'])[1].split()[0]

        wpt_import_text = self.update('web-platform-tests',
                                      'https://chromium.googlesource.com/external/w3c/web-platform-tests.git')

        for resource in ['testharnessreport.js', 'vendor-prefix.js']:
            source = self.path_from_webkit_base('LayoutTests', 'resources', resource)
            destination = self.path_from_webkit_base('LayoutTests', 'imported', 'web-platform-tests', 'resources', resource)
            self.copyfile(source, destination)
            self.run(['git', 'add', destination])

        css_import_text = self.update('csswg-test',
                                      'https://chromium.googlesource.com/external/w3c/csswg-test.git')

        self.commit_changes_if_needed(blink_commitish, css_import_text, wpt_import_text)

        return 0

    def parse_args(self, argv):
        parser = argparse.ArgumentParser()
        parser.description = __doc__
        parser.add_argument('-v', '--verbose', action='store_true',
                            help='log what we are doing')
        parser.add_argument('--allow-local-blink-commits', action='store_true',
                            help='allow script to run even if we have local blink commits')
        parser.add_argument('--keep-w3c-repos-around', action='store_true',
                            help='leave the w3c repos around that were imported previously.')

        args = parser.parse_args(argv)
        self.allow_local_blink_commits = args.allow_local_blink_commits
        self.keep_w3c_repos_around = args.keep_w3c_repos_around
        self.verbose = args.verbose

    def checkout_is_okay(self):
        if self.run(['git', 'diff', '--quiet', 'HEAD'], exit_on_failure=False)[0]:
            self.print_('## blink checkout is dirty, aborting')
            return False

        local_blink_commits = self.run(['git', 'log', '--oneline', 'origin/master..HEAD'])[1]
        if local_blink_commits and not self.allow_local_blink_commits:
            self.print_('## blink checkout has local commits, aborting')
            return False

        if self.fs.exists(self.path_from_webkit_base('web-platform-tests')):
            self.print_('## web-platform-tests repo exists, aborting')
            return False

        if self.fs.exists(self.path_from_webkit_base('csswg-test')):
            self.print_('## csswg-test repo exists, aborting')
            return False

        return True

    def update(self, repo, url):
        self.print_('## cloning %s' % repo)
        self.cd('')
        self.run(['git', 'clone', url])
        self.cd(re.compile('.*/([^/]+)\.git').match(url).group(1))
        self.run(['git', 'submodule', 'update', '--init', '--recursive'])

        self.print_('## noting the revision we are importing')
        master_commitish = self.run(['git', 'show-ref', 'origin/master'])[1].split()[0]

        self.print_('## cleaning out tests from LayoutTests/imported/%s' % repo)
        dest_repo = self.path_from_webkit_base('LayoutTests', 'imported', repo)
        files_to_delete = self.fs.files_under(dest_repo, file_filter=self.is_not_baseline)
        for subpath in files_to_delete:
            self.remove('LayoutTests', 'imported', subpath)

        self.print_('## importing the tests')
        src_repo = self.path_from_webkit_base(repo)
        import_path = self.path_from_webkit_base('Tools', 'Scripts', 'import-w3c-tests')
        self.run([self.host.executable, import_path, '-d', 'imported', src_repo])

        self.cd('')
        self.run(['git', 'add', '--all', 'LayoutTests/imported/%s' % repo])

        self.print_('## deleting manual tests')
        files_to_delete = self.fs.files_under(dest_repo, file_filter=self.is_manual_test)
        for subpath in files_to_delete:
            self.remove('LayoutTests', 'imported', subpath)

        self.print_('## deleting any orphaned baselines')
        previous_baselines = self.fs.files_under(dest_repo, file_filter=self.is_baseline)
        for subpath in previous_baselines:
            full_path = self.fs.join(dest_repo, subpath)
            if self.fs.glob(full_path.replace('-expected.txt', '*')) == [full_path]:
                self.fs.remove(full_path)

        if not self.keep_w3c_repos_around:
            self.print_('## deleting %s repo' % repo)
            self.cd('')
            self.rmtree(repo)

        return 'imported %s@%s' % (repo, master_commitish)

    def commit_changes_if_needed(self, blink_commitish, css_import_text, wpt_import_text):
        if self.run(['git', 'diff', '--quiet', 'HEAD'], exit_on_failure=False)[0]:
            self.print_('## commiting changes')
            commit_msg = ('update-w3c-deps import using blink %s:\n'
                          '\n'
                          '%s\n'
                          '%s\n' % (blink_commitish, css_import_text, wpt_import_text))
            path_to_commit_msg = self.path_from_webkit_base('commit_msg')
            if self.verbose:
                self.print_('cat > %s <<EOF' % path_to_commit_msg)
                self.print_(commit_msg)
                self.print_('EOF')
            self.fs.write_text_file(path_to_commit_msg, commit_msg)
            self.run(['git', 'commit', '-a', '-F', path_to_commit_msg])
            self.remove(path_to_commit_msg)
            self.print_('## Done: changes imported and committed')
        else:
            self.print_('## Done: no changes to import')

    def is_manual_test(self, fs, dirname, basename):
        return basename.endswith('-manual.html') or basename.endswith('-manual.htm')

    def is_baseline(self, fs, dirname, basename):
        return basename.endswith('-expected.txt')

    def is_not_baseline(self, fs, dirname, basename):
        return not self.is_baseline(fs, dirname, basename)

    def run(self, cmd, exit_on_failure=True):
        if self.verbose:
            self.print_(' '.join(cmd))

        proc = self.executive.popen(cmd, stdout=self.executive.PIPE, stderr=self.executive.PIPE)
        out, err = proc.communicate()
        if proc.returncode or self.verbose:
            self.print_('# ret> %d' % proc.returncode)
            if out:
                for line in out.splitlines():
                    self.print_('# out> %s' % line)
            if err:
                for line in err.splitlines():
                    self.print_('# err> %s' % line)
        if exit_on_failure and proc.returncode:
            self.host.exit(proc.returncode)
        return proc.returncode, out

    def cd(self, *comps):
        dest = self.path_from_webkit_base(*comps)
        if self.verbose:
            self.print_('cd %s' % dest)
        self.fs.chdir(dest)

    def copyfile(self, source, destination):
        if self.verbose:
            self.print_('cp %s %s' % (source, destination))
        self.fs.copyfile(source, destination)

    def remove(self, *comps):
        dest = self.path_from_webkit_base(*comps)
        if self.verbose:
            self.print_('rm %s' % dest)
        self.fs.remove(dest)

    def rmtree(self, *comps):
        dest = self.path_from_webkit_base(*comps)
        if self.verbose:
            self.print_('rm -fr %s' % dest)
        self.fs.rmtree(dest)

    def path_from_webkit_base(self, *comps):
        return self.finder.path_from_webkit_base(*comps)

    def print_(self, msg):
        self.host.print_(msg)
