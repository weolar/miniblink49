// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/animation/animation.h"

#include "base/mac/mac_util.h"
#include "base/message_loop/message_loop.h"

namespace gfx {

// static
bool Animation::ScrollAnimationsEnabledBySystem() {
  // Because of sandboxing, OS settings should only be queried from the browser
  // process.
  DCHECK(base::MessageLoopForUI::IsCurrent() ||
      base::MessageLoopForIO::IsCurrent());

  bool enabled = false;
  id value = nil;
  if (base::mac::IsOSMountainLionOrLater()) {
    value = [[NSUserDefaults standardUserDefaults]
        objectForKey:@"NSScrollAnimationEnabled"];
  } else {
    value = [[NSUserDefaults standardUserDefaults]
        objectForKey:@"AppleScrollAnimationEnabled"];
  }
  if (value)
    enabled = [value boolValue];
  return enabled;
}

} // namespace gfx
