// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/HTMLRTElement.h"

#include "core/HTMLNames.h"
#include "core/layout/LayoutRubyText.h"

namespace blink {

using namespace HTMLNames;

inline HTMLRTElement::HTMLRTElement(Document& document)
    : HTMLElement(rtTag, document)
{
}

DEFINE_NODE_FACTORY(HTMLRTElement)

LayoutObject* HTMLRTElement::createLayoutObject(const ComputedStyle& style)
{
    if (style.display() == BLOCK)
        return new LayoutRubyText(this);
    return LayoutObject::createObject(this, style);
}

}
