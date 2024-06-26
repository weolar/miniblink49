// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_SWAP_PROMISE_H_
#define CC_OUTPUT_SWAP_PROMISE_H_

#include "cc/output/compositor_frame_metadata.h"

namespace cc {

// When a change to the compositor's state/invalidation/whatever happens, a
// Swap Promise can be inserted into LayerTreeHost/LayerTreeImpl, to track
// whether the compositor's reply to the new state/invaliadtion/whatever is
// completed in the compositor, i.e. the compositor knows it has been sent
// to its output or not.
//
// If the commit results in a successful activation of the pending layer tree,
// SwapPromise::DidActivate() will be called.
//
// If the new compositor state is sent to the output, SwapPromise::DidSwap()
// will be called.
//
// If the scheduler fails to activate the pending tree, or the compositor
// fails to send its new state to the output, SwapPromise::DidNotSwap() will
// be called. Note that it is possible to activate, and subsequently not swap.
//
// Promises complete afer either DidSwap() or DidNotSwap() is called, thus
// there are three possible call sequences:
//   DidNotSwap()
//   DidActivate() ; DidSwap()
//   DidActivate() ; DidNotSwap()
//
// Clients that wish to use SwapPromise should have a subclass that defines
// the behavior of DidActivate(), DidSwap() and DidNotSwap(). Notice that the
// promise can be broken at either main or impl thread, e.g. commit fails on
// main thread, new frame data has no actual damage so
// LayerTreeHostImpl::SwapBuffers() bails out early on impl thread, so don't
// assume that Did*() methods are called at a particular thread. It is better
// to let the subclass carry thread-safe member data and operate on that
// member data in Did*().
class CC_EXPORT SwapPromise {
public:
    enum DidNotSwapReason {
        SWAP_FAILS,
        COMMIT_FAILS,
        COMMIT_NO_UPDATE,
        ACTIVATION_FAILS,
    };

    SwapPromise() { }
    virtual ~SwapPromise() { }

    virtual void DidActivate() = 0;
    virtual void DidSwap(CompositorFrameMetadata* metadata) = 0;
    virtual void DidNotSwap(DidNotSwapReason reason) = 0;
    // This is called when the main thread starts a (blocking) commit
    virtual void OnCommit() { }

    // A non-zero trace id identifies a trace flow object that is embedded in the
    // swap promise. This can be used for registering additional flow steps to
    // visualize the object's path through the system.
    virtual int64 TraceId() const = 0;
};

} // namespace cc

#endif // CC_OUTPUT_SWAP_PROMISE_H_
