// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_MAC_SCOPED_COCOA_DISABLE_SCREEN_UPDATES_H_
#define UI_GFX_MAC_SCOPED_COCOA_DISABLE_SCREEN_UPDATES_H_

#import <Cocoa/Cocoa.h>

#include "base/mac/mac_util.h"
#include "base/macros.h"

namespace gfx {

// A stack-based class to disable Cocoa screen updates. When instantiated, it
// disables screen updates and enables them when destroyed. Update disabling
// can be nested, and there is a time-maximum (about 1 second) after which
// Cocoa will automatically re-enable updating. This class doesn't attempt to
// overrule that.
class ScopedCocoaDisableScreenUpdates {
public:
    ScopedCocoaDisableScreenUpdates()
    {
        if (base::mac::IsOSElCapitanOrLater()) {
            // Beginning with OS X 10.11, [NSAnimationContext beginGrouping] is the
            // preferred way of disabling screen updates. Use of
            // NSDisableScreenUpdates() is discouraged.
            [NSAnimationContext beginGrouping];
        } else {
            NSDisableScreenUpdates();
        }
    }
    ~ScopedCocoaDisableScreenUpdates()
    {
        if (base::mac::IsOSElCapitanOrLater()) {
            [NSAnimationContext endGrouping];
        } else {
            NSEnableScreenUpdates();
        }
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ScopedCocoaDisableScreenUpdates);
};

} // namespace gfx

#endif // UI_GFX_MAC_SCOPED_COCOA_DISABLE_SCREEN_UPDATES_H_
