/*
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef SVGTextPathElement_h
#define SVGTextPathElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGTextContentElement.h"
#include "core/svg/SVGURIReference.h"
#include "platform/heap/Handle.h"

namespace blink {

enum SVGTextPathMethodType {
    SVGTextPathMethodUnknown = 0,
    SVGTextPathMethodAlign,
    SVGTextPathMethodStretch
};

enum SVGTextPathSpacingType {
    SVGTextPathSpacingUnknown = 0,
    SVGTextPathSpacingAuto,
    SVGTextPathSpacingExact
};

template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGTextPathMethodType>();
template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGTextPathSpacingType>();

class SVGTextPathElement final : public SVGTextContentElement,
                                 public SVGURIReference {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGTextPathElement);
public:
    // Forward declare enumerations in the W3C naming scheme, for IDL generation.
    enum {
        TEXTPATH_METHODTYPE_UNKNOWN = SVGTextPathMethodUnknown,
        TEXTPATH_METHODTYPE_ALIGN = SVGTextPathMethodAlign,
        TEXTPATH_METHODTYPE_STRETCH = SVGTextPathMethodStretch,
        TEXTPATH_SPACINGTYPE_UNKNOWN = SVGTextPathSpacingUnknown,
        TEXTPATH_SPACINGTYPE_AUTO = SVGTextPathSpacingAuto,
        TEXTPATH_SPACINGTYPE_EXACT = SVGTextPathSpacingExact
    };

    DECLARE_NODE_FACTORY(SVGTextPathElement);

    SVGAnimatedLength* startOffset() const { return m_startOffset.get(); }
    SVGAnimatedEnumeration<SVGTextPathMethodType>* method() { return m_method.get(); }
    SVGAnimatedEnumeration<SVGTextPathSpacingType>* spacing() { return m_spacing.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGTextPathElement(Document&);

    ~SVGTextPathElement() override;

    void clearResourceReferences();

    void buildPendingResource() override;
    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) override;

    void svgAttributeChanged(const QualifiedName&) override;

    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    bool layoutObjectIsNeeded(const ComputedStyle&) override;

    bool selfHasRelativeLengths() const override;

    RefPtrWillBeMember<SVGAnimatedLength> m_startOffset;
    RefPtrWillBeMember<SVGAnimatedEnumeration<SVGTextPathMethodType>> m_method;
    RefPtrWillBeMember<SVGAnimatedEnumeration<SVGTextPathSpacingType>> m_spacing;
};

} // namespace blink

#endif // SVGTextPathElement_h
