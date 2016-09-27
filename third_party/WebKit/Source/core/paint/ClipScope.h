// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ClipScope_h
#define ClipScope_h

#include "platform/graphics/GraphicsContext.h"

namespace blink {

class ClipScope {
public:
    ClipScope(GraphicsContext* context)
        : m_context(context)
        , m_clipCount(0) { }
    ~ClipScope();

    void clip(const LayoutRect& clipRect, SkRegion::Op operation);

private:
    GraphicsContext* m_context;
    int m_clipCount;
    WTF_MAKE_NONCOPYABLE(ClipScope);
};

} // namespace blink

#endif // ClipScope_h
