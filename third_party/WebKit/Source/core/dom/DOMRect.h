// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMRect_h
#define DOMRect_h

#include "bindings/core/v8/Dictionary.h"
#include "core/CoreExport.h"
#include "core/dom/DOMRectReadOnly.h"

namespace blink {

class CORE_EXPORT DOMRect final : public DOMRectReadOnly {
    DEFINE_WRAPPERTYPEINFO();
public:
    static DOMRect* create(double x = 0, double y = 0, double width = 0, double height = 0);

    void setX(double x) { m_x = x; }
    void setY(double y) { m_y = y; }
    void setWidth(double width) { m_width = width; }
    void setHeight(double height) { m_height = height; }

protected:
    DOMRect(double x, double y, double z, double w);
};

} // namespace blink

#endif
