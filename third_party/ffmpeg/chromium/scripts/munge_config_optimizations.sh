# Copyright (c) 2009 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This script munges the config.h to mark EBP unavailable so that the ffmpeg
# library can be compiled as WITHOUT -fomit-frame-pointer allowing for
# breakpad to work.
#
# Without this, building without -fomit-frame-pointer on ia32 will result in
# the the inclusion of a number of inline assembly blocks that use too many
# registers for its input/output operands.  This will cause gcc to barf with:
#
#   error: can't find a register in class ‘GENERAL_REGS’ while reloading ‘asm’
#
# This modification should only be required on ia32, and not x64.

set -e

sed -i.orig -e 's/HAVE_EBP_AVAILABLE 1/HAVE_EBP_AVAILABLE 0/g' $1
