// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DOMPointReadOnly.h"

namespace blink {

DOMPointReadOnly* DOMPointReadOnly::create(double x, double y, double z, double w)
{
    return new DOMPointReadOnly(x, y, z, w);
}

DOMPointReadOnly::DOMPointReadOnly(double x, double y, double z, double w)
    : m_x(x)
    , m_y(y)
    , m_z(z)
    , m_w(w)
{
}

} // namespace blink
