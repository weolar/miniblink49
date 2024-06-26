// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_PROXY_MAIN_H_
#define CC_TREES_PROXY_MAIN_H_

#include "base/memory/weak_ptr.h"
#include "cc/base/cc_export.h"

namespace cc {
class ThreadedChannel;

// TODO(khushalsagar): The main side of ThreadProxy. It is currently defined as
// an interface with the implementation provided by ThreadProxy and will be
// made an independent class.
// The methods added to this interface should only use the MainThreadOnly or
// BlockedMainThread variables from ThreadProxy.
// See crbug/527200.
class CC_EXPORT ProxyMain {
public:
    // TODO(khushalsagar): Make this ChannelMain*. When ProxyMain and
    // ProxyImpl are split, ProxyImpl will be passed a reference to ChannelImpl
    // at creation. Right now we just set it directly from ThreadedChannel
    // when the impl side is initialized.
    virtual void SetChannel(scoped_ptr<ThreadedChannel> threaded_channel) = 0;

    // Callback for main side commands received from the Channel.
    virtual void DidCompleteSwapBuffers() = 0;

    // TODO(khushalsagar): Rename as GetWeakPtr() once ThreadProxy is split.
    virtual base::WeakPtr<ProxyMain> GetMainWeakPtr() = 0;

protected:
    virtual ~ProxyMain() { }
};

} // namespace cc

#endif // CC_TREES_PROXY_MAIN_H_
