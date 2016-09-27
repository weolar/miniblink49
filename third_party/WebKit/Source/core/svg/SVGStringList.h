/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SVGStringList_h
#define SVGStringList_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/svg/SVGString.h"
#include "core/svg/properties/SVGPropertyHelper.h"

namespace blink {

class SVGStringListTearOff;

// Implementation of SVGStringList spec:
// http://www.w3.org/TR/SVG/single-page.html#types-InterfaceSVGStringList
// See SVGStringListTearOff for actual Javascript interface.
// Unlike other SVG*List implementations, SVGStringList is NOT tied to SVGString.
// SVGStringList operates directly on DOMString.
//
// In short:
//   SVGStringList has_a Vector<String>.
//   SVGStringList items are exposed to Javascript as DOMString (not SVGString) as in the spec.
//   SVGString is used only for boxing values for non-list string property SVGAnimatedString,
//   and not used for SVGStringList.
class SVGStringList final : public SVGPropertyHelper<SVGStringList> {
public:
    typedef SVGStringListTearOff TearOffType;

    static PassRefPtrWillBeRawPtr<SVGStringList> create()
    {
        return adoptRefWillBeNoop(new SVGStringList());
    }

    ~SVGStringList() override;

    const Vector<String>& values() const { return m_values; }

    // SVGStringList DOM Spec implementation. These are only to be called from SVGStringListTearOff:
    unsigned long length() { return m_values.size(); }
    void clear() { m_values.clear(); }
    void initialize(const String&);
    String getItem(size_t, ExceptionState&);
    void insertItemBefore(const String&, size_t);
    String removeItem(size_t, ExceptionState&);
    void appendItem(const String&);
    void replaceItem(const String&, size_t, ExceptionState&);

    // SVGPropertyBase:
    void setValueAsString(const String&, ExceptionState&);
    String valueAsString() const override;

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> fromValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toValue, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement*) override;

    static AnimatedPropertyType classType() { return AnimatedStringList; }

private:
    SVGStringList();

    template <typename CharType>
    void parseInternal(const CharType*& ptr, const CharType* end);
    bool checkIndexBound(size_t, ExceptionState&);

    Vector<String> m_values;
};

} // namespace blink

#endif // SVGStringList_h
