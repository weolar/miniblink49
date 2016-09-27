/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2008 Rob Buis <buis@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGTextContentElement_h
#define SVGTextContentElement_h

#include "core/svg/SVGAnimatedBoolean.h"
#include "core/svg/SVGAnimatedEnumeration.h"
#include "core/svg/SVGAnimatedLength.h"
#include "core/svg/SVGGraphicsElement.h"
#include "core/svg/SVGPointTearOff.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExceptionState;

enum SVGLengthAdjustType {
    SVGLengthAdjustUnknown,
    SVGLengthAdjustSpacing,
    SVGLengthAdjustSpacingAndGlyphs
};
template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGLengthAdjustType>();

class SVGTextContentElement : public SVGGraphicsElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    // Forward declare enumerations in the W3C naming scheme, for IDL generation.
    enum {
        LENGTHADJUST_UNKNOWN = SVGLengthAdjustUnknown,
        LENGTHADJUST_SPACING = SVGLengthAdjustSpacing,
        LENGTHADJUST_SPACINGANDGLYPHS = SVGLengthAdjustSpacingAndGlyphs
    };

    unsigned getNumberOfChars();
    float getComputedTextLength();
    float getSubStringLength(unsigned charnum, unsigned nchars, ExceptionState&);
    PassRefPtrWillBeRawPtr<SVGPointTearOff> getStartPositionOfChar(unsigned charnum, ExceptionState&);
    PassRefPtrWillBeRawPtr<SVGPointTearOff> getEndPositionOfChar(unsigned charnum, ExceptionState&);
    PassRefPtrWillBeRawPtr<SVGRectTearOff> getExtentOfChar(unsigned charnum, ExceptionState&);
    float getRotationOfChar(unsigned charnum, ExceptionState&);
    int getCharNumAtPosition(PassRefPtrWillBeRawPtr<SVGPointTearOff>, ExceptionState&);
    void selectSubString(unsigned charnum, unsigned nchars, ExceptionState&);

    static SVGTextContentElement* elementFromLayoutObject(LayoutObject*);

    SVGAnimatedLength* textLength() { return m_textLength.get(); }
    bool textLengthIsSpecifiedByUser() { return m_textLengthIsSpecifiedByUser; }
    SVGAnimatedEnumeration<SVGLengthAdjustType>* lengthAdjust() { return m_lengthAdjust.get(); }

    DECLARE_VIRTUAL_TRACE();

protected:
    SVGTextContentElement(const QualifiedName&, Document&);

    bool isPresentationAttribute(const QualifiedName&) const final;
    void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) final;
    void svgAttributeChanged(const QualifiedName&) override;

    bool selfHasRelativeLengths() const override;

private:
    bool isTextContent() const final { return true; }

    RefPtrWillBeMember<SVGAnimatedLength> m_textLength;
    bool m_textLengthIsSpecifiedByUser;
    RefPtrWillBeMember<SVGAnimatedEnumeration<SVGLengthAdjustType>> m_lengthAdjust;
};

inline bool isSVGTextContentElement(const SVGElement& element)
{
    return element.isTextContent();
}

DEFINE_SVGELEMENT_TYPE_CASTS_WITH_FUNCTION(SVGTextContentElement);

} // namespace blink

#endif // SVGTextContentElement_h
