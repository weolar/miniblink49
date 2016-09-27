// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CompositingInputsUpdater_h
#define CompositingInputsUpdater_h

#include "core/layout/LayoutGeometryMap.h"

namespace blink {

class DeprecatedPaintLayer;

class CompositingInputsUpdater {
public:
    explicit CompositingInputsUpdater(DeprecatedPaintLayer* rootLayer);
    ~CompositingInputsUpdater();

    void update();

#if ENABLE(ASSERT)
    static void assertNeedsCompositingInputsUpdateBitsCleared(DeprecatedPaintLayer*);
#endif

private:
    enum UpdateType {
        DoNotForceUpdate,
        ForceUpdate,
    };

    struct AncestorInfo {
        AncestorInfo()
            : ancestorStackingContext(nullptr)
            , enclosingCompositedLayer(nullptr)
            , lastScrollingAncestor(nullptr)
            , hasAncestorWithClipOrOverflowClip(false)
            , hasAncestorWithClipPath(false)
        {
        }

        DeprecatedPaintLayer* ancestorStackingContext;
        DeprecatedPaintLayer* enclosingCompositedLayer;
        // Notice that lastScrollingAncestor isn't the same thing as
        // ancestorScrollingLayer. The former is just the nearest scrolling
        // along the DeprecatedPaintLayer::parent() chain. The latter is the layer that
        // actually controls the scrolling of this layer, which we find on the
        // containing block chain.
        DeprecatedPaintLayer* lastScrollingAncestor;
        bool hasAncestorWithClipOrOverflowClip;
        bool hasAncestorWithClipPath;
    };

    void updateRecursive(DeprecatedPaintLayer*, UpdateType, AncestorInfo);

    LayoutGeometryMap m_geometryMap;
    DeprecatedPaintLayer* m_rootLayer;
};

} // namespace blink

#endif // CompositingInputsUpdater_h
