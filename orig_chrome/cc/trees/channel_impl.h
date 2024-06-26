// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_CHANNEL_IMPL_H_
#define CC_TREES_CHANNEL_IMPL_H_

#include "cc/base/cc_export.h"

namespace cc {

// Channel used to send commands to and receive commands from ProxyMain.
// The ChannelImpl implementation creates and owns ProxyImpl on receiving the
// InitializeImpl call from ChannelMain.
// See channel_main.h
class CC_EXPORT ChannelImpl {
public:
    // Interface for commands sent to ProxyMain
    virtual void DidCompleteSwapBuffers() = 0;

protected:
    virtual ~ChannelImpl() { }
};

} // namespace cc

#endif // CC_TREES_CHANNEL_IMPL_H_
