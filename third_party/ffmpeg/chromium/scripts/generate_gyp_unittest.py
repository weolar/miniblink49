#!/usr/bin/python
#
# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Unittest for generate_gyp_py.

It's tough to test the lower-level GetSourceFiles() and GetObjectFiles()
functions, so this focuses on the higher-level functions assuming those two
functions are working as intended (i.e., producing lists of files).
"""

import generate_gyp as gg
from generate_gyp import SourceSet, SourceListCondition
import string
import unittest

class ModuleUnittest(unittest.TestCase):
  def testGetObjectToSourceMapping(self):
    srcs = [
        'a.c',
        'b.asm',
        'c.cc',
    ]
    expected = {
        'a.o': 'a.c',
        'b.o': 'b.asm',
        'c.o': 'c.cc',
    }
    self.assertEqual(expected, gg.GetObjectToSourceMapping(srcs))

  def testGetSourceFileSet(self):
    objs_to_srcs = {
        'a.o': 'a.c',
        'b.o': 'b.asm',
        'c.o': 'c.cc',
    }
    objs = [
        'a.o',
        'c.o',
    ]
    expected = set(['a.c', 'c.cc'])
    self.assertEqual(expected, gg.GetSourceFileSet(objs_to_srcs, objs))

  def testGetSourceFileSet_NotFound(self):
    objs_to_srcs = {
        'a.o': 'a.c',
        'b.o': 'b.asm',
        'c.o': 'c.cc',
    }
    objs = [
        'd.o',
    ]
    self.assertRaises(KeyError, gg.GetSourceFileSet, objs_to_srcs, objs)


class SourceSetUnittest(unittest.TestCase):
  def testEquals(self):
    a = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '3')]))
    b = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '3')]))
    c = SourceSet(set(['c', 'd']), set([SourceListCondition('1', '2', '3')]))
    d = SourceSet(set(['a', 'b']), set([SourceListCondition('0', '2', '3')]))
    e = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '0', '3')]))
    f = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '0')]))

    self.assertEqual(a, b)
    self.assertNotEqual(a, c)
    self.assertNotEqual(a, d)
    self.assertNotEqual(a, e)
    self.assertNotEqual(a, f)

  def testIntersect_Exact(self):
    a = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '3')]))
    b = SourceSet(set(['a', 'b']), set([SourceListCondition('3', '4', '6')]))

    c = a.Intersect(b)

    self.assertEqual(c.sources, set(['a', 'b']))
    self.assertEqual(c.conditions, set([SourceListCondition('1', '2', '3'),
                                        SourceListCondition('3', '4', '6')]))
    self.assertFalse(c.IsEmpty())

  def testIntersect_Disjoint(self):
    a = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '3')]))
    b = SourceSet(set(['c', 'd']), set([SourceListCondition('3', '4', '6')]))

    c = a.Intersect(b)

    self.assertEqual(c.sources, set())
    self.assertEqual(c.conditions, set([SourceListCondition('1', '2', '3'),
                                        SourceListCondition('3', '4', '6')]))
    self.assertTrue(c.IsEmpty())

  def testIntersect_Overlap(self):
    a = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '3')]))
    b = SourceSet(set(['b', 'c']), set([SourceListCondition('3', '4', '6')]))

    c = a.Intersect(b)

    self.assertEqual(c.sources, set(['b']))
    self.assertEqual(c.conditions, set([SourceListCondition('1', '2', '3'),
                                        SourceListCondition('3', '4', '6')]))
    self.assertFalse(c.IsEmpty())

  def testDifference_Exact(self):
    a = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '3')]))
    b = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '3')]))

    c = a.Difference(b)

    self.assertEqual(c.sources, set())
    self.assertEqual(c.conditions, set([SourceListCondition('1', '2', '3')]))
    self.assertTrue(c.IsEmpty())

  def testDifference_Disjoint(self):
    a = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '3')]))
    b = SourceSet(set(['c', 'd']), set([SourceListCondition('3', '4', '6')]))

    c = a.Difference(b)

    self.assertEqual(c.sources, set(['a', 'b']))
    self.assertEqual(c.conditions, set())
    self.assertTrue(c.IsEmpty())

  def testDifference_Overlap(self):
    a = SourceSet(set(['a', 'b']), set([SourceListCondition('1', '2', '5')]))
    b = SourceSet(set(['b', 'c', 'd']),
        set([SourceListCondition('1', '2', '5'),
             SourceListCondition('3', '4', '6')]))

    c = a.Difference(b)

    self.assertEqual(c.sources, set(['a']))
    self.assertEqual(c.conditions, set([SourceListCondition('1', '2', '5')]))
    self.assertFalse(c.IsEmpty())

  def testGenerateGypStanza(self):
    # ia32 should just be ia32.  Win should appear as an OS restriction.
    a = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('ia32', 'Chromium', 'win')]))
    a_stanza = a.GenerateGypStanza()
    string.index(a_stanza, 'target_arch == "ia32"')
    string.index(a_stanza, 'OS == "win"')

    # x64 should just be x64.  Linux should appear as an OS restriction.
    b = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('x64', 'Chromium', 'linux')]))
    b_stanza = b.GenerateGypStanza()
    string.index(b_stanza, 'target_arch == "x64"')
    string.index(b_stanza, 'OS == "linux"')

    # arm should just be arm.
    c = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('arm', 'Chromium', 'linux')]))
    c_stanza = c.GenerateGypStanza()
    string.index(c_stanza, 'target_arch == "arm"')

    # arm-neon should be arm and flip the arm_neon switch.
    d = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('arm-neon', 'Chromium', 'linux')]))
    d_stanza = d.GenerateGypStanza()
    string.index(d_stanza, 'target_arch == "arm" and arm_neon == 1')

    # Multiple conditions
    e = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('arm', 'Chrome', 'win'),
                       SourceListCondition('x64', 'Chromium', 'linux')]))
    e_stanza = e.GenerateGypStanza()
    string.index(e_stanza, ('OS == "win" and target_arch == "arm"'
        ' and ffmpeg_branding == "Chrome"'))
    string.index(e_stanza, ('OS == "linux" and target_arch == "x64"'
        ' and ffmpeg_branding == "Chromium"'))

  def testGenerateGnStanza(self):
    # ia32 should be x86.  Win should appear as an OS restriction.
    a = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('ia32', 'Chromium', 'win')]))
    a_stanza = a.GenerateGnStanza()
    string.index(a_stanza, 'current_cpu == "x86"')
    string.index(a_stanza, 'is_win')

    # x64 should just be x64.  Linux should appear as an OS restriction.
    b = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('x64', 'Chromium', 'linux')]))
    b_stanza = b.GenerateGnStanza()
    string.index(b_stanza, 'current_cpu == "x64"')
    string.index(b_stanza, 'is_linux')

    # arm should just be arm.
    c = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('arm', 'Chromium', 'linux')]))
    c_stanza = c.GenerateGnStanza()
    string.index(c_stanza, 'current_cpu == "arm"')

    # arm-neon should be arm and flip the arm_neon switch.
    d = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('arm-neon', 'Chromium', 'linux')]))
    d_stanza = d.GenerateGnStanza()
    string.index(d_stanza, 'current_cpu == "arm" && arm_use_neon')

    # Multiple conditions
    e = SourceSet(set(['a', 'b']),
                  set([SourceListCondition('arm', 'Chrome', 'win'),
                       SourceListCondition('x64', 'Chromium', 'linux')]))
    e_stanza = e.GenerateGnStanza()
    string.index(e_stanza, ('is_win && current_cpu == "arm"'
        ' && ffmpeg_branding == "Chrome"'))
    string.index(e_stanza, ('is_linux && current_cpu == "x64"'
        ' && ffmpeg_branding == "Chromium"'))

  def testComplexSourceListConditions(self):
    # Create 2 sets with intersecting source 'a', but setup such that 'a'
    # is only valid for combinations (x86 && windows) || (x64 && linux). The
    # generated gyp stanza should then not allow for inclusion of the 'a' file
    # for combinations like x86 && linux.
    a = SourceSet(set(['a']), set([SourceListCondition('x86', 'c', 'win')]))
    b = SourceSet(set(['a']), set([SourceListCondition('x64', 'c', 'linux')]))
    disjoint_sets = gg.CreatePairwiseDisjointSets([a, b])

    # This condition is bad because x86 && linux would pass. Admittedly a very
    # fragile way to test this, but evaulating gn stanzas is hard, and it at
    # least serves to document the motivation for the associated changes to
    # our generate_gyp.py
    bad_condition = ('(current_cpu == "x86" || current_cpu == "x64")'
                     ' && (ffmpeg_branding == "c")'
                     ' && (is_win || is_linux)')

    # Expect only a single set since the two original sets have the same source
    # list.
    self.assertEqual(1, len(disjoint_sets))

    stanza = disjoint_sets[0].GenerateGnStanza()
    self.assertEqual(string.find(stanza, bad_condition), -1)

  def assertEqualSets(self, expected, actual):
    # Do pairwise checks for easier debugging.
    for a in actual:
      self.assertTrue(a in expected, msg='Unexpected set: %s' % a)
    for e in expected:
      self.assertTrue(e in actual, msg='Did not find expected set: %s' % e)

  def testCreatePairwiseDisjointSets_Pair(self):
    a = SourceSet(set(['common', 'intel']),
                  set([SourceListCondition('ia32', 'Chromium', 'win')]))
    b = SourceSet(set(['common', 'intel', 'chrome']),
                  set([SourceListCondition('ia32', 'Chrome', 'win')]))

    expected = []
    expected.append(
        SourceSet(set(['common', 'intel']),
                  set([SourceListCondition('ia32', 'Chromium', 'win'),
                       SourceListCondition('ia32', 'Chrome', 'win')])))
    expected.append(
        SourceSet(set(['chrome']),
                  set([SourceListCondition('ia32', 'Chrome', 'win')])))

    sets = gg.CreatePairwiseDisjointSets([a, b])
    self.assertEqualSets(expected, sets)

  def testCreatePairwiseDisjointSets_Triplet(self):
    a = SourceSet(set(['common', 'intel']),
                  set([SourceListCondition('ia32', 'Chromium', 'win')]))
    b = SourceSet(set(['common', 'intel', 'chrome']),
                  set([SourceListCondition('x64', 'Chrome', 'win')]))
    c = SourceSet(set(['common', 'arm']),
                  set([SourceListCondition('arm', 'Chromium', 'win')]))

    expected = []
    expected.append(
        SourceSet(set(['common']),
                  set([SourceListCondition('ia32', 'Chromium', 'win'),
                       SourceListCondition('x64', 'Chrome', 'win'),
                       SourceListCondition('arm', 'Chromium', 'win')])))
    expected.append(
        SourceSet(set(['intel']),
                  set([SourceListCondition('ia32', 'Chromium', 'win'),
                       SourceListCondition('x64', 'Chrome', 'win')])))
    expected.append(
        SourceSet(set(['chrome']),
                  set([SourceListCondition('x64', 'Chrome', 'win')])))
    expected.append(
        SourceSet(set(['arm']),
                  set([SourceListCondition('arm', 'Chromium', 'win')])))

    sets = gg.CreatePairwiseDisjointSets([a, b, c])
    self.assertEqualSets(expected, sets)

  def testCreatePairwiseDisjointSets_Multiple(self):
    a = SourceSet(set(['common', 'intel']),
                  set([SourceListCondition('ia32', 'Chromium', 'linux')]))
    b = SourceSet(set(['common', 'intel', 'chrome']),
                  set([SourceListCondition('ia32', 'Chrome', 'linux')]))
    c = SourceSet(set(['common', 'intel']),
                  set([SourceListCondition('x64', 'Chromium', 'linux')]))
    d = SourceSet(set(['common', 'intel', 'chrome']),
                  set([SourceListCondition('x64', 'Chrome', 'linux')]))
    e = SourceSet(set(['common', 'arm']),
                  set([SourceListCondition('arm', 'Chromium', 'linux')]))
    f = SourceSet(set(['common', 'arm-neon', 'chrome', 'chromeos']),
                  set([SourceListCondition('arm-neon', 'ChromeOS', 'linux')]))

    expected = []
    expected.append(SourceSet(set(['common']), set([
        SourceListCondition('ia32', 'Chromium', 'linux'),
        SourceListCondition('ia32', 'Chrome', 'linux'),
        SourceListCondition('x64', 'Chromium', 'linux'),
        SourceListCondition('x64', 'Chrome', 'linux'),
        SourceListCondition('arm', 'Chromium', 'linux'),
        SourceListCondition('arm-neon', 'ChromeOS', 'linux')])))
    expected.append(SourceSet(set(['intel']), set([
        SourceListCondition('ia32', 'Chromium', 'linux'),
        SourceListCondition('ia32', 'Chrome', 'linux'),
        SourceListCondition('x64', 'Chromium', 'linux'),
        SourceListCondition('x64', 'Chrome', 'linux')])))
    expected.append(SourceSet(set(['arm']), set([
        SourceListCondition('arm', 'Chromium', 'linux')])))
    expected.append(SourceSet(set(['chrome']), set([
        SourceListCondition('ia32', 'Chrome', 'linux'),
        SourceListCondition('x64', 'Chrome', 'linux'),
        SourceListCondition('arm-neon', 'ChromeOS', 'linux')])))
    expected.append(SourceSet(set(['arm-neon', 'chromeos']), set([
        SourceListCondition('arm-neon', 'ChromeOS', 'linux')])))

    sets = gg.CreatePairwiseDisjointSets([a, b, c, d, e, f])
    self.assertEqualSets(expected, sets)

  def testReduceConditions(self):
    # Set conditions span all of the supported architectures for linux.
    a = SourceSet(set(['foo.c']),
                  set([SourceListCondition('ia32', 'Chromium', 'linux'),
                       SourceListCondition('x64', 'Chromium', 'linux'),
                       SourceListCondition('arm', 'Chromium', 'linux'),
                       SourceListCondition('arm64', 'Chromium', 'linux'),
                       SourceListCondition('arm-neon', 'Chromium', 'linux'),
                       SourceListCondition('mipsel', 'Chromium', 'linux')]))
    gg.ReduceConditionalLogic(a)

    # Conditions should reduce to a single condition with wild-card for
    expected = set([SourceListCondition('*', 'Chromium', 'linux')])
    self.assertEqualSets(expected, a.conditions)

    # Set conditions span all of the supported architectures for windows.
    b = SourceSet(set(['foo.c']),
                  set([SourceListCondition('ia32', 'Chromium', 'win'),
                       SourceListCondition('x64', 'Chromium', 'win')]))
    gg.ReduceConditionalLogic(b)

    # Conditions should reduce to a single condition with wild-card for
    expected = set([SourceListCondition('*', 'Chromium', 'win')])
    self.assertEqualSets(expected, b.conditions)

    # Set conditions span all supported architectures and brandings for windows.
    b = SourceSet(set(['foo.c']),
                  set([SourceListCondition('ia32', 'Chromium', 'win'),
                       SourceListCondition('x64', 'Chromium', 'win'),
                       SourceListCondition('ia32', 'Chrome', 'win'),
                       SourceListCondition('x64', 'Chrome', 'win')]))
    gg.ReduceConditionalLogic(b)
    expected = set([SourceListCondition('*', '*', 'win')])
    self.assertEqualSets(expected, b.conditions)

    # Set conditions span all supported platforms.
    c = SourceSet(set(['foo.c']),
                  set([SourceListCondition('x64', 'Chromium', 'win'),
                       SourceListCondition('x64', 'Chromium', 'mac'),
                       SourceListCondition('x64', 'Chromium', 'linux'),
                       SourceListCondition('x64', 'Chromium', 'android')]))
    gg.ReduceConditionalLogic(c)
    expected = set([SourceListCondition('x64', 'Chromium', '*')])
    self.assertEqualSets(expected, c.conditions)

  def testReduceConditions_fullSpan(self):
    # Build SourceSet with conditions spanning every combination of attributes.
    ss = SourceSet(set(['foo.c']), set())
    for arch in gg.SUPPORT_MATRIX[gg.Attr.ARCHITECTURE]:
      for target in gg.SUPPORT_MATRIX[gg.Attr.TARGET]:
        for platform in gg.SUPPORT_MATRIX[gg.Attr.PLATFORM]:
          ss.conditions.add(SourceListCondition(arch, target, platform))

    gg.ReduceConditionalLogic(ss)
    expected = set([SourceListCondition('*', '*', '*')])
    self.assertEqualSets(expected, ss.conditions)

  def testGenerateGypStanzaWildCard(self):
    a = SourceSet(set(['foo.c']),
                  set([SourceListCondition('x64', 'Chromium', '*')]))
    gyp_stanza = a.GenerateGypStanza()
    gn_stanza = a.GenerateGnStanza()
    for stanza in [gyp_stanza, gn_stanza]:
        string.index(stanza, '== "x64"')
        string.index(stanza, 'ffmpeg_branding == "Chromium"')
        # OS is wild-card, so it should not be mentioned in the stanza.
        self.assertEqual(-1, string.find(stanza, 'OS =='))

if __name__ == '__main__':
  unittest.main()
