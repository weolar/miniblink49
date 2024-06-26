// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_CHANNEL_MAIN_H_
#define CC_TREES_CHANNEL_MAIN_H_

#include "cc/base/cc_export.h"

namespace cc {

// ChannelMain and ChannelImpl provide an abstract communication layer for
// the main and impl side of the compositor.
//
// The communication sequence between the 2 sides is:
//
// LayerTreeHost<-->ProxyMain<-->ChannelMain
//                                      |
//                                      |
//                               ChannelImpl<-->ProxyImpl<-->LayerTreeHostImpl

class CC_EXPORT ChannelMain {
public:
    // Interface for commands sent to the ProxyImpl
    virtual void SetThrottleFrameProductionOnImpl(bool throttle) = 0;
    virtual void SetLayerTreeHostClientReadyOnImpl() = 0;

    virtual ~ChannelMain() { }
};

} // namespace cc

#endif // CC_TREES_CHANNEL_MAIN_H_
