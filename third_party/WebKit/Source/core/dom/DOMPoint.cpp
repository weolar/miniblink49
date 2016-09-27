// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DOMPoint.h"

#include "core/dom/DOMPointInit.h"

namespace blink {

DOMPoint* DOMPoint::create(const DOMPointInit& point)
{
    return new DOMPoint(point.x(), point.y(), point.z(), point.w());
}

DOMPoint* DOMPoint::create(double x, double y, double z, double w)
{
    return new DOMPoint(x, y, z, w);
}

DOMPoint::DOMPoint(double x, double y, double z, double w)
    : DOMPointReadOnly(x, y, z, w)
{
}

} // namespace blink
