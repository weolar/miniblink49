// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/paint/ClipScope.h"

#include "platform/geometry/LayoutRect.h"

namespace blink {

ClipScope::~ClipScope()
{
    for (int i = 0; i < m_clipCount; i++) {
        m_context->restore();
    }
}

void ClipScope::clip(const LayoutRect& clipRect, SkRegion::Op operation)
{
    m_context->save();
    m_context->clipRect(pixelSnappedIntRect(clipRect), NotAntiAliased, operation);
    m_clipCount++;
}

} // namespace blink
