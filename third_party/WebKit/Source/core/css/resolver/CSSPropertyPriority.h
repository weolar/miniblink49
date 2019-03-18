// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSPropertyPriority_h
#define CSSPropertyPriority_h

#include "core/CSSPropertyNames.h"

namespace blink {

// The values of high priority properties affect the values of low priority
// properties. For example, the value of the high priority property 'font-size'
// decides the pixel value of low priority properties with 'em' units.

enum CSSPropertyPriority {
    ResolveVariables,
    HighPropertyPriority,
    LowPropertyPriority
};

template <CSSPropertyPriority priority>
class CSSPropertyPriorityData {
public:
    static inline CSSPropertyID first();
    static inline CSSPropertyID last();
    static inline bool propertyHasPriority(CSSPropertyID prop)
    {
        return first() <= prop && prop <= last();
    }
};

template<>
inline CSSPropertyID CSSPropertyPriorityData<ResolveVariables>::first()
{
    static_assert(CSSPropertyVariable == firstCSSProperty - 1, "CSSPropertyVariable should be directly before the first CSS property.");
    return CSSPropertyVariable;
}

template<>
inline CSSPropertyID CSSPropertyPriorityData<ResolveVariables>::last()
{
    return CSSPropertyVariable;
}

template<>
inline CSSPropertyID CSSPropertyPriorityData<HighPropertyPriority>::first()
{
    static_assert(CSSPropertyColor == firstCSSProperty, "CSSPropertyColor should be the first high priority property");
    return CSSPropertyColor;
}

template<>
inline CSSPropertyID CSSPropertyPriorityData<HighPropertyPriority>::last()
{
    static_assert(CSSPropertyZoom == CSSPropertyColor + 19, "CSSPropertyZoom should be the end of the high priority property range");
    static_assert(CSSPropertyTextRendering == CSSPropertyZoom - 1, "CSSPropertyTextRendering should be immediately before CSSPropertyZoom");
    return CSSPropertyZoom;
}

template<>
inline CSSPropertyID CSSPropertyPriorityData<LowPropertyPriority>::first()
{
    static_assert(CSSPropertyAlignContent == CSSPropertyZoom + 1, "CSSPropertyAlignContent should be the first low priority property");
    return CSSPropertyAlignContent;
}

template<>
inline CSSPropertyID CSSPropertyPriorityData<LowPropertyPriority>::last()
{
    return static_cast<CSSPropertyID>(lastCSSProperty);
}

} // namespace blink

#endif // CSSPropertyPriority_h
