# Copyright 2012 the V8 project authors. All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of Google Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
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


import hashlib
import os
import shutil
import sys
import tarfile
import imp

from testrunner.local import statusfile
from testrunner.local import testsuite
from testrunner.local import utils
from testrunner.objects import testcase

# The revision hash needs to be 7 characters?
TEST_262_ARCHIVE_REVISION = "c6ac390"  # This is the 2015-07-06 revision.
TEST_262_ARCHIVE_MD5 = "e1393ef330f38e9cb1bfa4e3eada5ba8"
TEST_262_URL = "https://github.com/tc39/test262/tarball/%s"
TEST_262_HARNESS_FILES = ["sta.js", "assert.js"]

TEST_262_SUITE_PATH = ["data", "test"]
TEST_262_HARNESS_PATH = ["data", "harness"]
TEST_262_TOOLS_PATH = ["data", "tools", "packaging"]

class Test262TestSuite(testsuite.TestSuite):

  def __init__(self, name, root):
    super(Test262TestSuite, self).__init__(name, root)
    self.testroot = os.path.join(self.root, *TEST_262_SUITE_PATH)
    self.harnesspath = os.path.join(self.root, *TEST_262_HARNESS_PATH)
    self.harness = [os.path.join(self.harnesspath, f)
                    for f in TEST_262_HARNESS_FILES]
    self.harness += [os.path.join(self.root, "harness-adapt.js")]
    self.ParseTestRecord = None

  def ListTests(self, context):
    tests = []
    for dirname, dirs, files in os.walk(self.testroot):
      for dotted in [x for x in dirs if x.startswith(".")]:
        dirs.remove(dotted)
      if context.noi18n and "intl402" in dirs:
        dirs.remove("intl402")
      dirs.sort()
      files.sort()
      for filename in files:
        if filename.endswith(".js"):
          testname = os.path.join(dirname[len(self.testroot) + 1:],
                                  filename[:-3])
          case = testcase.TestCase(self, testname)
          tests.append(case)
    return tests

  def GetFlagsForTestCase(self, testcase, context):
    return (testcase.flags + context.mode_flags + self.harness +
            self.GetIncludesForTest(testcase) + ["--harmony"] +
            [os.path.join(self.testroot, testcase.path + ".js")])

  def VariantFlags(self, testcase, default_flags):
    flags = super(Test262TestSuite, self).VariantFlags(testcase, default_flags)
    test_record = self.GetTestRecord(testcase)
    if "noStrict" in test_record:
      return flags
    strict_flags = [f + ["--use-strict"] for f in flags]
    if "onlyStrict" in test_record:
      return strict_flags
    return flags + strict_flags

  def LoadParseTestRecord(self):
    if not self.ParseTestRecord:
      root = os.path.join(self.root, *TEST_262_TOOLS_PATH)
      f = None
      try:
        (f, pathname, description) = imp.find_module("parseTestRecord", [root])
        module = imp.load_module("parseTestRecord", f, pathname, description)
        self.ParseTestRecord = module.parseTestRecord
      except:
        raise ImportError("Cannot load parseTestRecord; you may need to "
                          "--download-data for test262")
      finally:
        if f:
          f.close()
    return self.ParseTestRecord

  def GetTestRecord(self, testcase):
    if not hasattr(testcase, "test_record"):
      ParseTestRecord = self.LoadParseTestRecord()
      testcase.test_record = ParseTestRecord(self.GetSourceForTest(testcase),
                                             testcase.path)
    return testcase.test_record

  def GetIncludesForTest(self, testcase):
    test_record = self.GetTestRecord(testcase)
    if "includes" in test_record:
      includes = [os.path.join(self.harnesspath, f)
                  for f in test_record["includes"]]
    else:
      includes = []
    return includes

  def GetSourceForTest(self, testcase):
    filename = os.path.join(self.testroot, testcase.path + ".js")
    with open(filename) as f:
      return f.read()

  def IsNegativeTest(self, testcase):
    test_record = self.GetTestRecord(testcase)
    return "negative" in test_record

  def IsFailureOutput(self, output, testpath):
    if output.exit_code != 0:
      return True
    return "FAILED!" in output.stdout

  def HasUnexpectedOutput(self, testcase):
    outcome = self.GetOutcome(testcase)
    if (statusfile.FAIL_SLOPPY in testcase.outcomes and
        "--use-strict" not in testcase.flags):
      return outcome != statusfile.FAIL
    return not outcome in (testcase.outcomes or [statusfile.PASS])

  def DownloadData(self):
    revision = TEST_262_ARCHIVE_REVISION
    archive_url = TEST_262_URL % revision
    archive_name = os.path.join(self.root, "tc39-test262-%s.tar.gz" % revision)
    directory_name = os.path.join(self.root, "data")
    directory_old_name = os.path.join(self.root, "data.old")

    # Clobber if the test is in an outdated state, i.e. if there are any other
    # archive files present.
    archive_files = [f for f in os.listdir(self.root)
                     if f.startswith("tc39-test262-")]
    if (len(archive_files) > 1 or
        os.path.basename(archive_name) not in archive_files):
      print "Clobber outdated test archives ..."
      for f in archive_files:
        os.remove(os.path.join(self.root, f))

    if not os.path.exists(archive_name):
      print "Downloading test data from %s ..." % archive_url
      utils.URLRetrieve(archive_url, archive_name)
      if os.path.exists(directory_name):
        if os.path.exists(directory_old_name):
          shutil.rmtree(directory_old_name)
        os.rename(directory_name, directory_old_name)
    if not os.path.exists(directory_name):
      print "Extracting test262-%s.tar.gz ..." % revision
      md5 = hashlib.md5()
      with open(archive_name, "rb") as f:
        for chunk in iter(lambda: f.read(8192), ""):
          md5.update(chunk)
      print "MD5 hash is %s" % md5.hexdigest()
      if md5.hexdigest() != TEST_262_ARCHIVE_MD5:
        os.remove(archive_name)
        print "MD5 expected %s" % TEST_262_ARCHIVE_MD5
        raise Exception("MD5 hash mismatch of test data file")
      archive = tarfile.open(archive_name, "r:gz")
      if sys.platform in ("win32", "cygwin"):
        # Magic incantation to allow longer path names on Windows.
        archive.extractall(u"\\\\?\\%s" % self.root)
      else:
        archive.extractall(self.root)
      os.rename(os.path.join(self.root, "tc39-test262-%s" % revision),
                directory_name)


def GetSuite(name, root):
  return Test262TestSuite(name, root)
