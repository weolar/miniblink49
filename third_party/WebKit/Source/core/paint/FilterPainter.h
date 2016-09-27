// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FilterPainter_h
#define FilterPainter_h

#include "core/paint/DeprecatedPaintLayerPaintingInfo.h"
#include "wtf/OwnPtr.h"

namespace blink {

class ClipRect;
class GraphicsContext;
class DeprecatedPaintLayer;
class LayerClipRecorder;

class FilterPainter {
public:
    FilterPainter(DeprecatedPaintLayer&, GraphicsContext*, const LayoutPoint& offsetFromRoot, const ClipRect&, DeprecatedPaintLayerPaintingInfo&, PaintLayerFlags paintFlags, LayoutRect& rootRelativeBounds, bool& rootRelativeBoundsComputed);
    ~FilterPainter();

private:
    bool m_filterInProgress;
    GraphicsContext* m_context;
    OwnPtr<LayerClipRecorder> m_clipRecorder;
    LayoutObject* m_layoutObject;
};

} // namespace blink

#endif // FilterPainter_h
