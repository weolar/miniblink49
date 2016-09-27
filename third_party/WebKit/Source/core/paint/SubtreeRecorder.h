// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SubtreeRecorder_h
#define SubtreeRecorder_h

#include "core/paint/PaintPhase.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class DisplayItem;
class DisplayItemList;
class GraphicsContext;
class LayoutObject;

// Records subtree information during painting. The recorder's life span covers
// all painting operations executed during the root of the subtree's paint method
// for the paint phase.
class SubtreeRecorder {
public:
    SubtreeRecorder(GraphicsContext&, const LayoutObject& root, PaintPhase);

    ~SubtreeRecorder();

    // This method should be called before we actually paint the subtree, then we'll
    // issue a pair of BeginSubtree/EndSubtree display items to enclose all display
    // items when we paint the subtree.
    // Omit this call if we skip painting of the whole subtree (when e.g. the subtree
    // is out of the viewport, or the whole subtree is not invalidated), then we'll
    // issue a SubtreeCached display item.
    void begin();

private:
    DisplayItemList* m_displayItemList;
    const LayoutObject& m_subtreeRoot;
    const PaintPhase m_paintPhase;
    bool m_begun;
};

} // namespace blink

#endif // SubtreeRecorder_h
