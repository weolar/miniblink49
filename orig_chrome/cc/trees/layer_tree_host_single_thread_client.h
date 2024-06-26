// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_LAYER_TREE_HOST_SINGLE_THREAD_CLIENT_H_
#define CC_TREES_LAYER_TREE_HOST_SINGLE_THREAD_CLIENT_H_

namespace cc {

class LayerTreeHostSingleThreadClient {
public:
    // Request that the client schedule a composite.
    virtual void ScheduleComposite() { }
    // Request that the client schedule a composite now, and calculate appropriate
    // delay for potential future frame.
    virtual void ScheduleAnimation() { }

    // Called whenever the compositor posts a SwapBuffers (either full or
    // partial). After DidPostSwapBuffers(), exactly one of
    // DidCompleteSwapBuffers() or DidAbortSwapBuffers() will be called, thus
    // these functions can be used to keep track of pending swap buffers calls for
    // rate limiting.
    virtual void DidPostSwapBuffers() = 0;
    virtual void DidCompleteSwapBuffers() = 0;
    virtual void DidAbortSwapBuffers() = 0;

protected:
    virtual ~LayerTreeHostSingleThreadClient() { }
};

} // namespace cc

#endif // CC_TREES_LAYER_TREE_HOST_SINGLE_THREAD_CLIENT_H_
