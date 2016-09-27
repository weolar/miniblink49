// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ComputedStyleCSSValueMapping_h
#define ComputedStyleCSSValueMapping_h

#include "core/CSSPropertyNames.h"
#include "core/css/CSSValue.h"

namespace blink {

class CSSPrimitiveValue;
class LayoutObject;
class ComputedStyle;
class ShadowData;
class ShadowList;
class StyleColor;
class Node;

class ComputedStyleCSSValueMapping {
public:
    // FIXME: Resolve computed auto alignment in applyProperty/ComputedStyle and remove this non-const styledNode parameter.
    static PassRefPtrWillBeRawPtr<CSSValue> get(CSSPropertyID, const ComputedStyle&, const LayoutObject* = nullptr, Node* styledNode = nullptr, bool allowVisitedStyle = false);
private:
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> currentColorOrValidColor(const ComputedStyle&, const StyleColor&);
    static PassRefPtrWillBeRawPtr<CSSValue> valueForShadowData(const ShadowData&, const ComputedStyle&, bool useSpread);
    static PassRefPtrWillBeRawPtr<CSSValue> valueForShadowList(const ShadowList*, const ComputedStyle&, bool useSpread);
    static PassRefPtrWillBeRawPtr<CSSValue> valueForFilter(const ComputedStyle&);
    static PassRefPtrWillBeRawPtr<CSSValue> valueForFont(const ComputedStyle&);
};

} // namespace blink

#endif // ComputedStyleCSSValueMapping_h
