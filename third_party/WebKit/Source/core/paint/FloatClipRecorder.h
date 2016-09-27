// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FloatClipRecorder_h
#define FloatClipRecorder_h

#include "core/paint/PaintPhase.h"
#include "platform/geometry/FloatRect.h"
#include "platform/graphics/paint/DisplayItem.h"

namespace blink {

class FloatClipRecorder {
public:
    FloatClipRecorder(GraphicsContext&, const DisplayItemClientWrapper&, PaintPhase, const FloatRect&);

    ~FloatClipRecorder();

private:
    GraphicsContext& m_context;
    DisplayItemClientWrapper m_client;
    DisplayItem::Type m_clipType;
};

} // namespace blink

#endif // FloatClipRecorder_h
