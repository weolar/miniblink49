// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/HTMLRubyElement.h"

#include "core/HTMLNames.h"
#include "core/layout/LayoutRuby.h"

namespace blink {

using namespace HTMLNames;

inline HTMLRubyElement::HTMLRubyElement(Document& document)
    : HTMLElement(rubyTag, document)
{
}

DEFINE_NODE_FACTORY(HTMLRubyElement)

LayoutObject* HTMLRubyElement::createLayoutObject(const ComputedStyle& style)
{
    if (style.display() == INLINE)
        return new LayoutRubyAsInline(this);
    if (style.display() == BLOCK)
        return new LayoutRubyAsBlock(this);
    return LayoutObject::createObject(this, style);
}

}
