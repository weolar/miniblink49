#!/bin/bash
#
# Copyright 2017 The PDFium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set -e
set -o pipefail
set -u

REVIEWERS=`paste -s -d, third_party/freetype/OWNERS`
roll-dep -r "${REVIEWERS}" "$@" third_party/freetype/src/
FTVERSION=`git -C third_party/freetype/src/ describe --long`
FTCOMMIT=`git -C third_party/freetype/src/ rev-parse HEAD`

# Make sure our copy of pstables.h matches the one in freetype/src.
# May need to --bypass-hooks to prevent formatting of this file.
cp third_party/freetype/src/src/psnames/pstables.h \
  third_party/freetype/include/pstables.h

sed -i "s/^Version: .*\$/Version: ${FTVERSION%-*}/" \
  third_party/freetype/README.pdfium
sed -i "s/^Revision: .*\$/Revision: ${FTCOMMIT}/" \
  third_party/freetype/README.pdfium

git add third_party/freetype/README.pdfium
git add third_party/freetype/include/pstables.h
git commit --quiet --amend --no-edit
