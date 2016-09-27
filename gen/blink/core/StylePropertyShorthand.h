/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
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

#ifndef StylePropertyShorthand_h
#define StylePropertyShorthand_h

#include "CSSPropertyNames.h"
#include "wtf/Vector.h"

namespace blink {

class StylePropertyShorthand {
    WTF_MAKE_FAST_ALLOCATED(StylePropertyShorthand);
public:
    StylePropertyShorthand()
        : m_properties(0)
        , m_propertiesForInitialization(0)
        , m_length(0)
        , m_shorthandID(CSSPropertyInvalid)
    {
    }

    StylePropertyShorthand(CSSPropertyID id, const CSSPropertyID* properties, unsigned numProperties)
        : m_properties(properties)
        , m_propertiesForInitialization(0)
        , m_length(numProperties)
        , m_shorthandID(id)
    {
    }

    StylePropertyShorthand(CSSPropertyID id, const CSSPropertyID* properties, const StylePropertyShorthand** propertiesForInitialization, unsigned numProperties)
        : m_properties(properties)
        , m_propertiesForInitialization(propertiesForInitialization)
        , m_length(numProperties)
        , m_shorthandID(id)
    {
    }

    const CSSPropertyID* properties() const { return m_properties; }
    const StylePropertyShorthand** propertiesForInitialization() const { return m_propertiesForInitialization; }
    unsigned length() const { return m_length; }
    CSSPropertyID id() const { return m_shorthandID; }

private:
    const CSSPropertyID* m_properties;
    const StylePropertyShorthand** m_propertiesForInitialization;
    unsigned m_length;
    CSSPropertyID m_shorthandID;
};

const StylePropertyShorthand& backgroundRepeatShorthand();
const StylePropertyShorthand& flexShorthand();
const StylePropertyShorthand& markerShorthand();
const StylePropertyShorthand& flexFlowShorthand();
const StylePropertyShorthand& outlineShorthand();
const StylePropertyShorthand& webkitColumnsShorthand();
const StylePropertyShorthand& overflowShorthand();
const StylePropertyShorthand& webkitTextStrokeShorthand();
const StylePropertyShorthand& listStyleShorthand();
const StylePropertyShorthand& borderStyleShorthand();
const StylePropertyShorthand& webkitMarginCollapseShorthand();
const StylePropertyShorthand& marginShorthand();
const StylePropertyShorthand& borderSpacingShorthand();
const StylePropertyShorthand& textDecorationShorthand();
const StylePropertyShorthand& borderRightShorthand();
const StylePropertyShorthand& backgroundShorthand();
const StylePropertyShorthand& webkitMaskRepeatShorthand();
const StylePropertyShorthand& gridShorthand();
const StylePropertyShorthand& webkitBorderEndShorthand();
const StylePropertyShorthand& borderTopShorthand();
const StylePropertyShorthand& webkitBorderAfterShorthand();
const StylePropertyShorthand& fontShorthand();
const StylePropertyShorthand& transitionShorthand();
const StylePropertyShorthand& animationShorthand();
const StylePropertyShorthand& webkitBorderBeforeShorthand();
const StylePropertyShorthand& paddingShorthand();
const StylePropertyShorthand& webkitMaskPositionShorthand();
const StylePropertyShorthand& borderLeftShorthand();
const StylePropertyShorthand& webkitMaskShorthand();
const StylePropertyShorthand& gridColumnShorthand();
const StylePropertyShorthand& webkitBorderStartShorthand();
const StylePropertyShorthand& gridAreaShorthand();
const StylePropertyShorthand& borderColorShorthand();
const StylePropertyShorthand& webkitColumnRuleShorthand();
const StylePropertyShorthand& borderWidthShorthand();
const StylePropertyShorthand& gridTemplateShorthand();
const StylePropertyShorthand& motionShorthand();
const StylePropertyShorthand& backgroundPositionShorthand();
const StylePropertyShorthand& borderShorthand();
const StylePropertyShorthand& borderBottomShorthand();
const StylePropertyShorthand& gridRowShorthand();
const StylePropertyShorthand& borderRadiusShorthand();
const StylePropertyShorthand& webkitTextEmphasisShorthand();
const StylePropertyShorthand& webkitMaskBoxImageShorthand();
const StylePropertyShorthand& borderImageShorthand();

const StylePropertyShorthand& borderShorthandForParsing();
const StylePropertyShorthand& animationShorthandForParsing();
const StylePropertyShorthand& transitionShorthandForParsing();

// Returns an empty list if the property is not a shorthand.
const StylePropertyShorthand& shorthandForProperty(CSSPropertyID);

// Return the list of shorthands for a given longhand.
// The client must pass in an empty result vector.
void getMatchingShorthandsForLonghand(CSSPropertyID, Vector<StylePropertyShorthand, 4>* result);

unsigned indexOfShorthandForLonghand(CSSPropertyID, const Vector<StylePropertyShorthand, 4>&);

bool isShorthandProperty(CSSPropertyID);

} // namespace blink

#endif // StylePropertyShorthand_h
