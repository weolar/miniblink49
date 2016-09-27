// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMRectReadOnly_h
#define DOMRectReadOnly_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "platform/heap/Handle.h"

namespace blink {

class CORE_EXPORT DOMRectReadOnly : public GarbageCollected<DOMRectReadOnly>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static DOMRectReadOnly* create(double x, double y, double width, double height);

    double x() const { return m_x; }
    double y() const { return m_y; }
    double width() const { return m_width; }
    double height() const { return m_height; }

    double top() const { return std::min(m_y, m_y + m_height); }
    double right() const { return std::max(m_x, m_x + m_width); }
    double bottom() const { return std::max(m_y, m_y + m_height); }
    double left() const { return std::min(m_x, m_x + m_width); }

    DEFINE_INLINE_TRACE() { }

protected:
    DOMRectReadOnly(double x, double y, double width, double height);

    double m_x;
    double m_y;
    double m_width;
    double m_height;
};

} // namespace blink

#endif
