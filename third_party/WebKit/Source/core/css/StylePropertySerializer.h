/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Research In Motion Limited. All rights reserved.
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

#ifndef StylePropertySerializer_h
#define StylePropertySerializer_h

#include "core/css/CSSValueList.h"
#include "core/css/StylePropertySet.h"

#include "wtf/BitArray.h"

namespace blink {

class StylePropertySet;
class StylePropertyShorthand;

class StylePropertySerializer {
public:
    explicit StylePropertySerializer(const StylePropertySet&);

    String asText() const;
    String getPropertyValue(CSSPropertyID) const;
private:
    String getCommonValue(const StylePropertyShorthand&) const;
    enum CommonValueMode { OmitUncommonValues, ReturnNullOnUncommonValues };
    String borderPropertyValue(CommonValueMode) const;
    String getLayeredShorthandValue(const StylePropertyShorthand&) const;
    String get4Values(const StylePropertyShorthand&) const;
    String borderSpacingValue(const StylePropertyShorthand&) const;
    String getShorthandValue(const StylePropertyShorthand&, String separator = " ") const;
    String fontValue() const;
    void appendFontLonghandValueIfNotNormal(CSSPropertyID, StringBuilder& result, String& value) const;
    String backgroundRepeatPropertyValue() const;
    String getPropertyText(CSSPropertyID, const String& value, bool isImportant, bool isNotFirstDecl) const;
    bool isPropertyShorthandAvailable(const StylePropertyShorthand&) const;
    bool shorthandHasOnlyInitialOrInheritedValue(const StylePropertyShorthand&) const;
    void appendBackgroundPropertyAsText(StringBuilder& result, unsigned& numDecls) const;

    // Only StylePropertySerializer uses the following two classes.
    class PropertyValueForSerializer {
    public:
        explicit PropertyValueForSerializer(StylePropertySet::PropertyReference property)
            : m_value(property.value())
            , m_id(property.id())
            , m_isImportant(property.isImportant())
            , m_isImplicit(property.isImplicit())
            , m_isInherited(property.isInherited()) { }

        PropertyValueForSerializer(CSSPropertyID id, const CSSValue* value, bool isImportant)
            : m_value(value)
            , m_id(id)
            , m_isImportant(isImportant)
            , m_isImplicit(value->isImplicitInitialValue())
            , m_isInherited(value->isInheritedValue()) { }

        CSSPropertyID id() const { return m_id; }
        const CSSValue* value() const { return m_value; }
        bool isImportant() const { return m_isImportant; }
        bool isImplicit() const { return m_isImplicit; }
        bool isInherited() const { return m_isInherited; }
        bool isValid() const { return m_value; }

    private:
        const CSSValue* m_value;
        CSSPropertyID m_id;
        bool m_isImportant;
        bool m_isImplicit;
        bool m_isInherited;
    };

    class StylePropertySetForSerializer {
    public:
        explicit StylePropertySetForSerializer(const StylePropertySet&);

        unsigned propertyCount() const;
        PropertyValueForSerializer propertyAt(unsigned index) const;
        bool shouldProcessPropertyAt(unsigned index) const;
        int findPropertyIndex(CSSPropertyID) const;
        const CSSValue* getPropertyCSSValue(CSSPropertyID) const;
        String getPropertyValue(CSSPropertyID) const;
        bool isPropertyImplicit(CSSPropertyID) const;
        bool propertyIsImportant(CSSPropertyID) const;

    private:
        bool hasExpandedAllProperty() const { return hasAllProperty() && m_needToExpandAll; }
        bool hasAllProperty() const { return m_allIndex != -1; }

        const StylePropertySet& m_propertySet;
        int m_allIndex;
        BitArray<numCSSProperties> m_longhandPropertyUsed;
        bool m_needToExpandAll;
    };

    const StylePropertySetForSerializer m_propertySet;
};

} // namespace blink

#endif
