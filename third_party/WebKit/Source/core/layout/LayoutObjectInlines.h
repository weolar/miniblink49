// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LayoutObjectInlines_h
#define LayoutObjectInlines_h

#include "core/dom/StyleEngine.h"
#include "core/layout/LayoutObject.h"

namespace blink {

// The following methods are inlined for performance but not put in
// LayoutObject.h because that would unnecessarily tie LayoutObject.h
// to StyleEngine.h for all users of LayoutObject.h that don't use
// these methods.

inline const ComputedStyle* LayoutObject::firstLineStyle() const
{
    return document().styleEngine().usesFirstLineRules() ? cachedFirstLineStyle() : style();
}

inline const ComputedStyle& LayoutObject::firstLineStyleRef() const
{
    const ComputedStyle* style =  firstLineStyle();
    ASSERT(style);
    return *style;
}

inline const ComputedStyle* LayoutObject::style(bool firstLine) const
{
    return firstLine ? firstLineStyle() : style();
}

inline const ComputedStyle& LayoutObject::styleRef(bool firstLine) const
{
    const ComputedStyle* style = this->style(firstLine);
    ASSERT(style);
    return *style;
}

}

#endif
