// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCompositorAnimationTimeline_h
#define WebCompositorAnimationTimeline_h

namespace blink {

class WebCompositorAnimationPlayerClient;

// A compositor representation for timeline.
class WebCompositorAnimationTimeline {
public:
    virtual ~WebCompositorAnimationTimeline() { }

    virtual void playerAttached(const WebCompositorAnimationPlayerClient&) { }
    virtual void playerDestroyed(const WebCompositorAnimationPlayerClient&) { }
};

} // namespace blink

#endif // WebCompositorAnimationTimeline_h
