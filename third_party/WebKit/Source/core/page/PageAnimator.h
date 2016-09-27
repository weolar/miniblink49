// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PageAnimator_h
#define PageAnimator_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"

namespace blink {

class LocalFrame;
class Page;

class CORE_EXPORT PageAnimator final : public RefCountedWillBeGarbageCollected<PageAnimator> {
public:
    static PassRefPtrWillBeRawPtr<PageAnimator> create(Page&);
    DECLARE_TRACE();
    void scheduleVisualUpdate(LocalFrame* = 0);
    void serviceScriptedAnimations(double monotonicAnimationStartTime);

    bool isServicingAnimations() const { return m_servicingAnimations; }
    void updateLayoutAndStyleForPainting(LocalFrame* rootFrame);

private:
    explicit PageAnimator(Page&);

    RawPtrWillBeMember<Page> m_page;
    bool m_servicingAnimations;
    bool m_updatingLayoutAndStyleForPainting;
};

}

#endif // PageAnimator_h
