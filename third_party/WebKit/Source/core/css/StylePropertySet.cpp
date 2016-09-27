/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012, 2013 Apple Inc. All rights reserved.
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

#include "config.h"
#include "core/css/StylePropertySet.h"

#include "core/StylePropertyShorthand.h"
#include "core/css/CSSPropertyMetadata.h"
#include "core/css/CSSValuePool.h"
#include "core/css/StylePropertySerializer.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/parser/CSSParser.h"
#include "core/frame/UseCounter.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/text/StringBuilder.h"

#ifndef NDEBUG
#include "wtf/text/CString.h"
#include <stdio.h>
#endif

namespace blink {

static size_t sizeForImmutableStylePropertySetWithPropertyCount(unsigned count)
{
    return sizeof(ImmutableStylePropertySet) - sizeof(void*) + sizeof(CSSValue*) * count + sizeof(StylePropertyMetadata) * count;
}

PassRefPtrWillBeRawPtr<ImmutableStylePropertySet> ImmutableStylePropertySet::create(const CSSProperty* properties, unsigned count, CSSParserMode cssParserMode)
{
    ASSERT(count <= MaxArraySize);
#if ENABLE(OILPAN)
    void* slot = Heap::allocate<StylePropertySet>(sizeForImmutableStylePropertySetWithPropertyCount(count));
#else
    void* slot = WTF::fastMalloc(sizeForImmutableStylePropertySetWithPropertyCount(count));
#endif // ENABLE(OILPAN)
    return adoptRefWillBeNoop(new (slot) ImmutableStylePropertySet(properties, count, cssParserMode));
}

PassRefPtrWillBeRawPtr<ImmutableStylePropertySet> StylePropertySet::immutableCopyIfNeeded() const
{
    if (!isMutable())
        return toImmutableStylePropertySet(const_cast<StylePropertySet*>(this));
    const MutableStylePropertySet* mutableThis = toMutableStylePropertySet(this);
    return ImmutableStylePropertySet::create(mutableThis->m_propertyVector.data(), mutableThis->m_propertyVector.size(), cssParserMode());
}

MutableStylePropertySet::MutableStylePropertySet(CSSParserMode cssParserMode)
    : StylePropertySet(cssParserMode)
{
}

MutableStylePropertySet::MutableStylePropertySet(const CSSProperty* properties, unsigned length)
    : StylePropertySet(HTMLStandardMode)
{
    m_propertyVector.reserveInitialCapacity(length);
    for (unsigned i = 0; i < length; ++i)
        m_propertyVector.uncheckedAppend(properties[i]);
}

ImmutableStylePropertySet::ImmutableStylePropertySet(const CSSProperty* properties, unsigned length, CSSParserMode cssParserMode)
    : StylePropertySet(cssParserMode, length)
{
    StylePropertyMetadata* metadataArray = const_cast<StylePropertyMetadata*>(this->metadataArray());
    RawPtrWillBeMember<CSSValue>* valueArray = const_cast<RawPtrWillBeMember<CSSValue>*>(this->valueArray());
    for (unsigned i = 0; i < m_arraySize; ++i) {
        metadataArray[i] = properties[i].metadata();
        valueArray[i] = properties[i].value();
#if !ENABLE(OILPAN)
        valueArray[i]->ref();
#endif
    }
}

ImmutableStylePropertySet::~ImmutableStylePropertySet()
{
#if !ENABLE(OILPAN)
    RawPtrWillBeMember<CSSValue>* valueArray = const_cast<RawPtrWillBeMember<CSSValue>*>(this->valueArray());
    for (unsigned i = 0; i < m_arraySize; ++i) {
        // Checking for nullptr here is a workaround to prevent crashing.  http://crbug.com/449032
        if (valueArray[i])
            valueArray[i]->deref();
    }
#endif
}

int ImmutableStylePropertySet::findPropertyIndex(CSSPropertyID propertyID) const
{
    // Convert here propertyID into an uint16_t to compare it with the metadata's m_propertyID to avoid
    // the compiler converting it to an int multiple times in the loop.
    uint16_t id = static_cast<uint16_t>(propertyID);
    for (int n = m_arraySize - 1 ; n >= 0; --n) {
        if (metadataArray()[n].m_propertyID == id) {
            // Only enabled properties should be part of the style.
            ASSERT(CSSPropertyMetadata::isEnabledProperty(propertyID));
            return n;
        }
    }

    return -1;
}

DEFINE_TRACE_AFTER_DISPATCH(ImmutableStylePropertySet)
{
    const RawPtrWillBeMember<CSSValue>* values = valueArray();
    for (unsigned i = 0; i < m_arraySize; i++)
        visitor->trace(values[i]);
    StylePropertySet::traceAfterDispatch(visitor);
}

MutableStylePropertySet::MutableStylePropertySet(const StylePropertySet& other)
    : StylePropertySet(other.cssParserMode())
{
    if (other.isMutable()) {
        m_propertyVector = toMutableStylePropertySet(other).m_propertyVector;
    } else {
        m_propertyVector.reserveInitialCapacity(other.propertyCount());
        for (unsigned i = 0; i < other.propertyCount(); ++i)
            m_propertyVector.uncheckedAppend(other.propertyAt(i).toCSSProperty());
    }
}

String StylePropertySet::getPropertyValue(CSSPropertyID propertyID) const
{
    RefPtrWillBeRawPtr<CSSValue> value = getPropertyCSSValue(propertyID);
    if (value)
        return value->cssText();

    return StylePropertySerializer(*this).getPropertyValue(propertyID);
}

PassRefPtrWillBeRawPtr<CSSValue> StylePropertySet::getPropertyCSSValue(CSSPropertyID propertyID) const
{
    int foundPropertyIndex = findPropertyIndex(propertyID);
    if (foundPropertyIndex == -1)
        return nullptr;
    return propertyAt(foundPropertyIndex).value();
}

DEFINE_TRACE(StylePropertySet)
{
    if (m_isMutable)
        toMutableStylePropertySet(this)->traceAfterDispatch(visitor);
    else
        toImmutableStylePropertySet(this)->traceAfterDispatch(visitor);
}

#if ENABLE(OILPAN)
void StylePropertySet::finalizeGarbageCollectedObject()
{
    if (m_isMutable)
        toMutableStylePropertySet(this)->~MutableStylePropertySet();
    else
        toImmutableStylePropertySet(this)->~ImmutableStylePropertySet();
}
#endif

bool MutableStylePropertySet::removeShorthandProperty(CSSPropertyID propertyID)
{
    StylePropertyShorthand shorthand = shorthandForProperty(propertyID);
    if (!shorthand.length())
        return false;

    return removePropertiesInSet(shorthand.properties(), shorthand.length());
}

bool MutableStylePropertySet::removeProperty(CSSPropertyID propertyID, String* returnText)
{
    if (removeShorthandProperty(propertyID)) {
        // FIXME: Return an equivalent shorthand when possible.
        if (returnText)
            *returnText = "";
        return true;
    }

    int foundPropertyIndex = findPropertyIndex(propertyID);
    if (foundPropertyIndex == -1) {
        if (returnText)
            *returnText = "";
        return false;
    }

    if (returnText)
        *returnText = propertyAt(foundPropertyIndex).value()->cssText();

    // A more efficient removal strategy would involve marking entries as empty
    // and sweeping them when the vector grows too big.
    m_propertyVector.remove(foundPropertyIndex);

    return true;
}

bool StylePropertySet::propertyIsImportant(CSSPropertyID propertyID) const
{
    int foundPropertyIndex = findPropertyIndex(propertyID);
    if (foundPropertyIndex != -1)
        return propertyAt(foundPropertyIndex).isImportant();

    StylePropertyShorthand shorthand = shorthandForProperty(propertyID);
    if (!shorthand.length())
        return false;

    for (unsigned i = 0; i < shorthand.length(); ++i) {
        if (!propertyIsImportant(shorthand.properties()[i]))
            return false;
    }
    return true;
}

CSSPropertyID StylePropertySet::getPropertyShorthand(CSSPropertyID propertyID) const
{
    int foundPropertyIndex = findPropertyIndex(propertyID);
    if (foundPropertyIndex == -1)
        return CSSPropertyInvalid;
    return propertyAt(foundPropertyIndex).shorthandID();
}

bool StylePropertySet::isPropertyImplicit(CSSPropertyID propertyID) const
{
    int foundPropertyIndex = findPropertyIndex(propertyID);
    if (foundPropertyIndex == -1)
        return false;
    return propertyAt(foundPropertyIndex).isImplicit();
}

bool MutableStylePropertySet::setProperty(CSSPropertyID unresolvedProperty, const String& value, bool important, StyleSheetContents* contextStyleSheet)
{
    // Setting the value to an empty string just removes the property in both IE and Gecko.
    // Setting it to null seems to produce less consistent results, but we treat it just the same.
    if (value.isEmpty())
        return removeProperty(resolveCSSPropertyID(unresolvedProperty));

    // When replacing an existing property value, this moves the property to the end of the list.
    // Firefox preserves the position, and MSIE moves the property to the beginning.
    return CSSParser::parseValue(this, unresolvedProperty, value, important, cssParserMode(), contextStyleSheet);
}

void MutableStylePropertySet::setProperty(CSSPropertyID propertyID, PassRefPtrWillBeRawPtr<CSSValue> prpValue, bool important)
{
    StylePropertyShorthand shorthand = shorthandForProperty(propertyID);
    if (!shorthand.length()) {
        setProperty(CSSProperty(propertyID, prpValue, important));
        return;
    }

    removePropertiesInSet(shorthand.properties(), shorthand.length());

    RefPtrWillBeRawPtr<CSSValue> value = prpValue;
    for (unsigned i = 0; i < shorthand.length(); ++i)
        m_propertyVector.append(CSSProperty(shorthand.properties()[i], value, important));
}

bool MutableStylePropertySet::setProperty(const CSSProperty& property, CSSProperty* slot)
{
    if (!removeShorthandProperty(property.id())) {
        CSSProperty* toReplace = slot ? slot : findCSSPropertyWithID(property.id());
        if (toReplace && *toReplace == property)
            return false;
        if (toReplace) {
            *toReplace = property;
            return true;
        }
    }
    m_propertyVector.append(property);
    return true;
}

bool MutableStylePropertySet::setProperty(CSSPropertyID propertyID, CSSValueID identifier, bool important)
{
    setProperty(CSSProperty(propertyID, cssValuePool().createIdentifierValue(identifier), important));
    return true;
}

void MutableStylePropertySet::parseDeclarationList(const String& styleDeclaration, StyleSheetContents* contextStyleSheet)
{
    m_propertyVector.clear();

    CSSParserContext context(cssParserMode(), UseCounter::getFrom(contextStyleSheet));
    if (contextStyleSheet) {
        context = contextStyleSheet->parserContext();
        context.setMode(cssParserMode());
    }

    CSSParser::parseDeclarationList(context, this, styleDeclaration);
}

bool MutableStylePropertySet::addParsedProperties(const WillBeHeapVector<CSSProperty, 256>& properties)
{
    bool changed = false;
    m_propertyVector.reserveCapacity(m_propertyVector.size() + properties.size());
    for (unsigned i = 0; i < properties.size(); ++i)
        changed |= setProperty(properties[i]);
    return changed;
}

bool MutableStylePropertySet::addRespectingCascade(const CSSProperty& property)
{
    // Only add properties that have no !important counterpart present
    if (!propertyIsImportant(property.id()) || property.isImportant())
        return setProperty(property);
    return false;
}

String StylePropertySet::asText() const
{
    return StylePropertySerializer(*this).asText();
}

void MutableStylePropertySet::mergeAndOverrideOnConflict(const StylePropertySet* other)
{
    unsigned size = other->propertyCount();
    for (unsigned n = 0; n < size; ++n) {
        PropertyReference toMerge = other->propertyAt(n);
        CSSProperty* old = findCSSPropertyWithID(toMerge.id());
        if (old)
            setProperty(toMerge.toCSSProperty(), old);
        else
            m_propertyVector.append(toMerge.toCSSProperty());
    }
}

bool StylePropertySet::hasFailedOrCanceledSubresources() const
{
    unsigned size = propertyCount();
    for (unsigned i = 0; i < size; ++i) {
        if (propertyAt(i).value()->hasFailedOrCanceledSubresources())
            return true;
    }
    return false;
}

void MutableStylePropertySet::clear()
{
    m_propertyVector.clear();
}

inline bool containsId(const CSSPropertyID* set, unsigned length, CSSPropertyID id)
{
    for (unsigned i = 0; i < length; ++i) {
        if (set[i] == id)
            return true;
    }
    return false;
}

bool MutableStylePropertySet::removePropertiesInSet(const CSSPropertyID* set, unsigned length)
{
    if (m_propertyVector.isEmpty())
        return false;

    CSSProperty* properties = m_propertyVector.data();
    unsigned oldSize = m_propertyVector.size();
    unsigned newIndex = 0;
    for (unsigned oldIndex = 0; oldIndex < oldSize; ++oldIndex) {
        const CSSProperty& property = properties[oldIndex];
        if (containsId(set, length, property.id()))
            continue;
        // Modify m_propertyVector in-place since this method is performance-sensitive.
        properties[newIndex++] = properties[oldIndex];
    }
    if (newIndex != oldSize) {
        m_propertyVector.shrink(newIndex);
        return true;
    }
    return false;
}

CSSProperty* MutableStylePropertySet::findCSSPropertyWithID(CSSPropertyID propertyID)
{
    int foundPropertyIndex = findPropertyIndex(propertyID);
    if (foundPropertyIndex == -1)
        return 0;
    return &m_propertyVector.at(foundPropertyIndex);
}

bool StylePropertySet::propertyMatches(CSSPropertyID propertyID, const CSSValue* propertyValue) const
{
    int foundPropertyIndex = findPropertyIndex(propertyID);
    if (foundPropertyIndex == -1)
        return false;
    return propertyAt(foundPropertyIndex).value()->equals(*propertyValue);
}

void MutableStylePropertySet::removeEquivalentProperties(const StylePropertySet* style)
{
    Vector<CSSPropertyID> propertiesToRemove;
    unsigned size = m_propertyVector.size();
    for (unsigned i = 0; i < size; ++i) {
        PropertyReference property = propertyAt(i);
        if (style->propertyMatches(property.id(), property.value()))
            propertiesToRemove.append(property.id());
    }
    // FIXME: This should use mass removal.
    for (unsigned i = 0; i < propertiesToRemove.size(); ++i)
        removeProperty(propertiesToRemove[i]);
}

void MutableStylePropertySet::removeEquivalentProperties(const CSSStyleDeclaration* style)
{
    Vector<CSSPropertyID> propertiesToRemove;
    unsigned size = m_propertyVector.size();
    for (unsigned i = 0; i < size; ++i) {
        PropertyReference property = propertyAt(i);
        if (style->cssPropertyMatches(property.id(), property.value()))
            propertiesToRemove.append(property.id());
    }
    // FIXME: This should use mass removal.
    for (unsigned i = 0; i < propertiesToRemove.size(); ++i)
        removeProperty(propertiesToRemove[i]);
}

PassRefPtrWillBeRawPtr<MutableStylePropertySet> StylePropertySet::mutableCopy() const
{
    return adoptRefWillBeNoop(new MutableStylePropertySet(*this));
}

PassRefPtrWillBeRawPtr<MutableStylePropertySet> StylePropertySet::copyPropertiesInSet(const Vector<CSSPropertyID>& properties) const
{
    WillBeHeapVector<CSSProperty, 256> list;
    list.reserveInitialCapacity(properties.size());
    for (unsigned i = 0; i < properties.size(); ++i) {
        RefPtrWillBeRawPtr<CSSValue> value = getPropertyCSSValue(properties[i]);
        if (value)
            list.append(CSSProperty(properties[i], value.release(), false));
    }
    return MutableStylePropertySet::create(list.data(), list.size());
}

CSSStyleDeclaration* MutableStylePropertySet::ensureCSSStyleDeclaration()
{
    // FIXME: get rid of this weirdness of a CSSStyleDeclaration inside of a
    // style property set.
    if (m_cssomWrapper) {
        ASSERT(!static_cast<CSSStyleDeclaration*>(m_cssomWrapper.get())->parentRule());
        ASSERT(!m_cssomWrapper->parentElement());
        return m_cssomWrapper.get();
    }
    m_cssomWrapper = adoptPtrWillBeNoop(new PropertySetCSSStyleDeclaration(*this));
    return m_cssomWrapper.get();
}

int MutableStylePropertySet::findPropertyIndex(CSSPropertyID propertyID) const
{
    const CSSProperty* begin = m_propertyVector.data();
    const CSSProperty* end = begin + m_propertyVector.size();
    // Convert here propertyID into an uint16_t to compare it with the metadata's m_propertyID to avoid
    // the compiler converting it to an int multiple times in the loop.
    uint16_t id = static_cast<uint16_t>(propertyID);

    auto compare = [propertyID, id](const CSSProperty& property) -> bool {
        if (property.metadata().m_propertyID == id) {
            // Only enabled properties should be part of the style.
            ASSERT(CSSPropertyMetadata::isEnabledProperty(propertyID));
            return true;
        }
        return false;
    };

    const CSSProperty* it = std::find_if(begin, end, compare);

    return (it == end) ? -1 : it - begin;
}

DEFINE_TRACE_AFTER_DISPATCH(MutableStylePropertySet)
{
#if ENABLE(OILPAN)
    visitor->trace(m_cssomWrapper);
    visitor->trace(m_propertyVector);
#endif
    StylePropertySet::traceAfterDispatch(visitor);
}

unsigned StylePropertySet::averageSizeInBytes()
{
    // Please update this if the storage scheme changes so that this longer reflects the actual size.
    return sizeForImmutableStylePropertySetWithPropertyCount(4);
}

// See the function above if you need to update this.
struct SameSizeAsStylePropertySet : public RefCountedWillBeGarbageCollectedFinalized<SameSizeAsStylePropertySet> {
    unsigned bitfield;
};
static_assert(sizeof(StylePropertySet) == sizeof(SameSizeAsStylePropertySet), "StylePropertySet should stay small");

#ifndef NDEBUG
void StylePropertySet::showStyle()
{
    fprintf(stderr, "%s\n", asText().ascii().data());
}
#endif

PassRefPtrWillBeRawPtr<MutableStylePropertySet> MutableStylePropertySet::create(CSSParserMode cssParserMode)
{
    return adoptRefWillBeNoop(new MutableStylePropertySet(cssParserMode));
}

PassRefPtrWillBeRawPtr<MutableStylePropertySet> MutableStylePropertySet::create(const CSSProperty* properties, unsigned count)
{
    return adoptRefWillBeNoop(new MutableStylePropertySet(properties, count));
}

} // namespace blink
