// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScrollRecorder_h
#define ScrollRecorder_h

#include "core/CoreExport.h"
#include "core/paint/PaintPhase.h"
#include "platform/geometry/IntSize.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "wtf/FastAllocBase.h"

namespace blink {

class GraphicsContext;

// Emits display items which represent a region which is scrollable, so that it
// can be translated by the scroll offset.
class CORE_EXPORT ScrollRecorder {
    WTF_MAKE_FAST_ALLOCATED(ScrollRecorder);
public:
    ScrollRecorder(GraphicsContext&, const DisplayItemClientWrapper&, PaintPhase, const IntSize& currentOffset);
    ~ScrollRecorder();
private:
    DisplayItemClientWrapper m_client;
    DisplayItem::Type m_beginItemType;
    GraphicsContext& m_context;
};

} // namespace blink

#endif // ScrollRecorder_h
