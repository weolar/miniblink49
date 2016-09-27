// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DOMRectReadOnly.h"

namespace blink {

DOMRectReadOnly* DOMRectReadOnly::create(double x, double y, double width, double height)
{
    return new DOMRectReadOnly(x, y, width, height);
}

DOMRectReadOnly::DOMRectReadOnly(double x, double y, double width, double height)
    : m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
{
}

} // namespace blink
