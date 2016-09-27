// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/PropertyHandle.h"

namespace blink {

bool PropertyHandle::operator==(const PropertyHandle& other) const
{
    if (handleType != other.handleType)
        return false;

    switch (handleType) {
    case HandleCSSProperty:
        return property == other.property;
    case HandleSVGAttribute:
        return attribute == other.attribute;
    default:
        return true;
    }
}

unsigned PropertyHandle::hash() const
{
    switch (handleType) {
    case HandleCSSProperty:
        return property;
    case HandleSVGAttribute:
        return QualifiedNameHash::hash(*attribute);
    default:
        ASSERT_NOT_REACHED();
        return 0;
    }
}

}
