// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CompositingRecorder_h
#define CompositingRecorder_h

#include "platform/geometry/FloatRect.h"
#include "platform/graphics/GraphicsTypes.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "public/platform/WebBlendMode.h"

namespace blink {

class GraphicsContext;
class LayoutObject;

class CompositingRecorder {
public:
    CompositingRecorder(GraphicsContext&, const DisplayItemClientWrapper&, const SkXfermode::Mode, const float opacity, const FloatRect* bounds = 0, ColorFilter = ColorFilterNone);

    ~CompositingRecorder();

    // FIXME: These helpers only exist to ease the transition to slimming paint
    //        and should be removed once slimming paint is enabled by default.
    static void beginCompositing(GraphicsContext&, const DisplayItemClientWrapper&, const SkXfermode::Mode, const float opacity, const FloatRect* bounds = 0, ColorFilter = ColorFilterNone);
    static void endCompositing(GraphicsContext&, const DisplayItemClientWrapper&);

private:
    DisplayItemClientWrapper m_client;
    GraphicsContext& m_graphicsContext;
};

} // namespace blink

#endif // CompositingRecorder_h
