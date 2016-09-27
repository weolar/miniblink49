// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TransformRecorder_h
#define TransformRecorder_h

#include "core/CoreExport.h"
#include "platform/graphics/paint/DisplayItem.h"

namespace blink {

class GraphicsContext;
class AffineTransform;

class CORE_EXPORT TransformRecorder {
public:
    TransformRecorder(GraphicsContext&, const DisplayItemClientWrapper&, const AffineTransform&);
    ~TransformRecorder();

private:
    GraphicsContext& m_context;
    DisplayItemClientWrapper m_client;
    bool m_skipRecordingForIdentityTransform;
};

} // namespace blink

#endif // TransformRecorder_h
