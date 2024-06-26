#!/bin/bash -e

# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Use this to copy all config files into the tree.
for os in android linux linux-noasm mac win; do
  for target in Chromium ChromiumOS Chrome ChromeOS; do
    # Copy config files for various architectures:
    #   - ia32/x64 have config.asm, config.h
    #   - arm/arm-neon have config.h
    for arch in arm arm-neon arm64 ia32 x64 mipsel mips64el; do
      # Don't waste time on non-existent configs, if no config.h then skip.
      [ ! -e "build.$arch.$os/$target/config.h" ] && continue
      for f in config.h config.asm libavutil/avconfig.h libavutil/ffversion.h; do
        FROM="build.$arch.$os/$target/$f"
        TO="chromium/config/$target/$os/$arch/$f"
        if [ "$(dirname $f)" != "" ]; then mkdir -p $(dirname $TO); fi
        [ -e $FROM ] && cp -v $FROM $TO
      done
    done
  done
done

echo "Copied all existing newer configs successfully."
