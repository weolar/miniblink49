/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2007 David Smith (catfish.man@gmail.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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
#include "core/layout/FloatingObjects.h"

#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutView.h"
#include "core/layout/shapes/ShapeOutsideInfo.h"

using namespace WTF;

namespace blink {

struct SameSizeAsFloatingObject {
    void* pointers[2];
    LayoutRect rect;
    int paginationStrut;
    uint32_t bitfields : 8;
};

static_assert(sizeof(FloatingObject) == sizeof(SameSizeAsFloatingObject), "FloatingObject should stay small");

FloatingObject::FloatingObject(LayoutBox* layoutObject)
    : m_layoutObject(layoutObject)
    , m_originatingLine(nullptr)
    , m_paginationStrut(0)
    , m_shouldPaint(true)
    , m_isDescendant(false)
    , m_isPlaced(false)
    , m_isLowestNonOverhangingFloatInChild(false)
#if ENABLE(ASSERT)
    , m_isInPlacedTree(false)
#endif
{
    EFloat type = layoutObject->style()->floating();
    ASSERT(type != NoFloat);
    if (type == LeftFloat)
        m_type = FloatLeft;
    else if (type == RightFloat)
        m_type = FloatRight;
}

FloatingObject::FloatingObject(LayoutBox* layoutObject, Type type, const LayoutRect& frameRect, bool shouldPaint, bool isDescendant, bool isLowestNonOverhangingFloatInChild)
    : m_layoutObject(layoutObject)
    , m_originatingLine(nullptr)
    , m_frameRect(frameRect)
    , m_paginationStrut(0)
    , m_type(type)
    , m_shouldPaint(shouldPaint)
    , m_isDescendant(isDescendant)
    , m_isPlaced(true)
    , m_isLowestNonOverhangingFloatInChild(isLowestNonOverhangingFloatInChild)
#if ENABLE(ASSERT)
    , m_isInPlacedTree(false)
#endif
{
}

PassOwnPtr<FloatingObject> FloatingObject::create(LayoutBox* layoutObject)
{
    OwnPtr<FloatingObject> newObj = adoptPtr(new FloatingObject(layoutObject));
    newObj->setShouldPaint(!layoutObject->hasSelfPaintingLayer()); // If a layer exists, the float will paint itself. Otherwise someone else will.
    newObj->setIsDescendant(true);

    return newObj.release();
}

PassOwnPtr<FloatingObject> FloatingObject::copyToNewContainer(LayoutSize offset, bool shouldPaint, bool isDescendant) const
{
    return adoptPtr(new FloatingObject(layoutObject(), type(), LayoutRect(frameRect().location() - offset, frameRect().size()), shouldPaint, isDescendant, isLowestNonOverhangingFloatInChild()));
}

PassOwnPtr<FloatingObject> FloatingObject::unsafeClone() const
{
    OwnPtr<FloatingObject> cloneObject = adoptPtr(new FloatingObject(layoutObject(), type(), m_frameRect, m_shouldPaint, m_isDescendant, false));
    cloneObject->m_paginationStrut = m_paginationStrut;
    cloneObject->m_isPlaced = m_isPlaced;
    return cloneObject.release();
}

template <FloatingObject::Type FloatTypeValue>
class ComputeFloatOffsetAdapter {
public:
    typedef FloatingObjectInterval IntervalType;

    ComputeFloatOffsetAdapter(const LayoutBlockFlow* layoutObject, int lineTop, int lineBottom, LayoutUnit offset)
        : m_layoutObject(layoutObject)
        , m_lineTop(lineTop)
        , m_lineBottom(lineBottom)
        , m_offset(offset)
        , m_outermostFloat(nullptr)
    {
    }

    virtual ~ComputeFloatOffsetAdapter() { }

    int lowValue() const { return m_lineTop; }
    int highValue() const { return m_lineBottom; }
    void collectIfNeeded(const IntervalType&);

    LayoutUnit offset() const { return m_offset; }

protected:
    virtual bool updateOffsetIfNeeded(const FloatingObject&) = 0;

    const LayoutBlockFlow* m_layoutObject;
    int m_lineTop;
    int m_lineBottom;
    LayoutUnit m_offset;
    const FloatingObject* m_outermostFloat;
};

template <FloatingObject::Type FloatTypeValue>
class ComputeFloatOffsetForFloatLayoutAdapter : public ComputeFloatOffsetAdapter<FloatTypeValue> {
public:
    ComputeFloatOffsetForFloatLayoutAdapter(const LayoutBlockFlow* layoutObject, LayoutUnit lineTop, LayoutUnit lineBottom, LayoutUnit offset)
        : ComputeFloatOffsetAdapter<FloatTypeValue>(layoutObject, lineTop, lineBottom, offset)
    {
    }

    virtual ~ComputeFloatOffsetForFloatLayoutAdapter() { }

    LayoutUnit heightRemaining() const;

protected:
    virtual bool updateOffsetIfNeeded(const FloatingObject&) override final;
};

template <FloatingObject::Type FloatTypeValue>
class ComputeFloatOffsetForLineLayoutAdapter : public ComputeFloatOffsetAdapter<FloatTypeValue> {
public:
    ComputeFloatOffsetForLineLayoutAdapter(const LayoutBlockFlow* layoutObject, LayoutUnit lineTop, LayoutUnit lineBottom, LayoutUnit offset)
        : ComputeFloatOffsetAdapter<FloatTypeValue>(layoutObject, lineTop, lineBottom, offset)
    {
    }

    virtual ~ComputeFloatOffsetForLineLayoutAdapter() { }

protected:
    virtual bool updateOffsetIfNeeded(const FloatingObject&) override final;
};


FloatingObjects::~FloatingObjects()
{
}
void FloatingObjects::clearLineBoxTreePointers()
{
    // Clear references to originating lines, since the lines are being deleted
    FloatingObjectSetIterator end = m_set.end();
    for (FloatingObjectSetIterator it = m_set.begin(); it != end; ++it) {
        ASSERT(!((*it)->originatingLine()) || (*it)->originatingLine()->layoutObject() == m_layoutObject);
        (*it)->setOriginatingLine(nullptr);
    }
}

FloatingObjects::FloatingObjects(const LayoutBlockFlow* layoutObject, bool horizontalWritingMode)
    : m_placedFloatsTree(UninitializedTree)
    , m_leftObjectsCount(0)
    , m_rightObjectsCount(0)
    , m_horizontalWritingMode(horizontalWritingMode)
    , m_layoutObject(layoutObject)
    , m_cachedHorizontalWritingMode(false)
{
}

void FloatingObjects::clear()
{
    m_set.clear();
    m_placedFloatsTree.clear();
    m_leftObjectsCount = 0;
    m_rightObjectsCount = 0;
    markLowestFloatLogicalBottomCacheAsDirty();
}

LayoutUnit FloatingObjects::lowestFloatLogicalBottom(FloatingObject::Type floatType)
{
    bool isInHorizontalWritingMode = m_horizontalWritingMode;
    if (floatType != FloatingObject::FloatLeftRight) {
        if (hasLowestFloatLogicalBottomCached(isInHorizontalWritingMode, floatType))
            return getCachedlowestFloatLogicalBottom(floatType);
    } else {
        if (hasLowestFloatLogicalBottomCached(isInHorizontalWritingMode, FloatingObject::FloatLeft) && hasLowestFloatLogicalBottomCached(isInHorizontalWritingMode, FloatingObject::FloatRight)) {
            return std::max(getCachedlowestFloatLogicalBottom(FloatingObject::FloatLeft),
                getCachedlowestFloatLogicalBottom(FloatingObject::FloatRight));
        }
    }

    LayoutUnit lowestFloatBottom = 0;
    const FloatingObjectSet& floatingObjectSet = set();
    FloatingObjectSetIterator end = floatingObjectSet.end();
    if (floatType == FloatingObject::FloatLeftRight) {
        FloatingObject* lowestFloatingObjectLeft = nullptr;
        FloatingObject* lowestFloatingObjectRight = nullptr;
        LayoutUnit lowestFloatBottomLeft = 0;
        LayoutUnit lowestFloatBottomRight = 0;
        for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
            FloatingObject& floatingObject = *it->get();
            if (floatingObject.isPlaced()) {
                FloatingObject::Type curType = floatingObject.type();
                LayoutUnit curFloatLogicalBottom = m_layoutObject->logicalBottomForFloat(floatingObject);
                if (curType & FloatingObject::FloatLeft && curFloatLogicalBottom > lowestFloatBottomLeft) {
                    lowestFloatBottomLeft = curFloatLogicalBottom;
                    lowestFloatingObjectLeft = &floatingObject;
                }
                if (curType & FloatingObject::FloatRight && curFloatLogicalBottom > lowestFloatBottomRight) {
                    lowestFloatBottomRight = curFloatLogicalBottom;
                    lowestFloatingObjectRight = &floatingObject;
                }
            }
        }
        lowestFloatBottom = std::max(lowestFloatBottomLeft, lowestFloatBottomRight);
        setCachedLowestFloatLogicalBottom(isInHorizontalWritingMode, FloatingObject::FloatLeft, lowestFloatingObjectLeft);
        setCachedLowestFloatLogicalBottom(isInHorizontalWritingMode, FloatingObject::FloatRight, lowestFloatingObjectRight);
    } else {
        FloatingObject* lowestFloatingObject = nullptr;
        for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
            FloatingObject& floatingObject = *it->get();
            if (floatingObject.isPlaced() && floatingObject.type() == floatType) {
                if (m_layoutObject->logicalBottomForFloat(floatingObject) > lowestFloatBottom) {
                    lowestFloatingObject = &floatingObject;
                    lowestFloatBottom = m_layoutObject->logicalBottomForFloat(floatingObject);
                }
            }
        }
        setCachedLowestFloatLogicalBottom(isInHorizontalWritingMode, floatType, lowestFloatingObject);
    }

    return lowestFloatBottom;
}

bool FloatingObjects::hasLowestFloatLogicalBottomCached(bool isHorizontal, FloatingObject::Type type) const
{
    int floatIndex = static_cast<int>(type) - 1;
    ASSERT(floatIndex < static_cast<int>(sizeof(m_lowestFloatBottomCache) / sizeof(FloatBottomCachedValue)));
    ASSERT(floatIndex >= 0);
    return (m_cachedHorizontalWritingMode == isHorizontal && !m_lowestFloatBottomCache[floatIndex].dirty);
}

LayoutUnit FloatingObjects::getCachedlowestFloatLogicalBottom(FloatingObject::Type type) const
{
    int floatIndex = static_cast<int>(type) - 1;
    ASSERT(floatIndex < static_cast<int>(sizeof(m_lowestFloatBottomCache) / sizeof(FloatBottomCachedValue)));
    ASSERT(floatIndex >= 0);
    if (!m_lowestFloatBottomCache[floatIndex].floatingObject)
        return LayoutUnit();
    return m_layoutObject->logicalBottomForFloat(*m_lowestFloatBottomCache[floatIndex].floatingObject);
}

void FloatingObjects::setCachedLowestFloatLogicalBottom(bool isHorizontal, FloatingObject::Type type, FloatingObject* floatingObject)
{
    int floatIndex = static_cast<int>(type) - 1;
    ASSERT(floatIndex < static_cast<int>(sizeof(m_lowestFloatBottomCache) / sizeof(FloatBottomCachedValue)));
    ASSERT(floatIndex >= 0);
    m_cachedHorizontalWritingMode = isHorizontal;
    m_lowestFloatBottomCache[floatIndex].floatingObject = floatingObject;
    m_lowestFloatBottomCache[floatIndex].dirty = false;
}

FloatingObject* FloatingObjects::lowestFloatingObject() const
{
    bool isInHorizontalWritingMode = m_horizontalWritingMode;
    if (!hasLowestFloatLogicalBottomCached(isInHorizontalWritingMode, FloatingObject::FloatLeft) && !hasLowestFloatLogicalBottomCached(isInHorizontalWritingMode, FloatingObject::FloatRight))
        return nullptr;
    FloatingObject* lowestLeftObject = m_lowestFloatBottomCache[0].floatingObject;
    FloatingObject* lowestRightObject = m_lowestFloatBottomCache[1].floatingObject;
    LayoutUnit lowestFloatBottomLeft = lowestLeftObject ? m_layoutObject->logicalBottomForFloat(*lowestLeftObject) : LayoutUnit();
    LayoutUnit lowestFloatBottomRight = lowestRightObject ? m_layoutObject->logicalBottomForFloat(*lowestRightObject) : LayoutUnit();

    if (lowestFloatBottomLeft > lowestFloatBottomRight)
        return lowestLeftObject;
    return lowestRightObject;
}

void FloatingObjects::markLowestFloatLogicalBottomCacheAsDirty()
{
    for (size_t i = 0; i < sizeof(m_lowestFloatBottomCache) / sizeof(FloatBottomCachedValue); ++i)
        m_lowestFloatBottomCache[i].dirty = true;
}

void FloatingObjects::moveAllToFloatInfoMap(LayoutBoxToFloatInfoMap& map)
{
    while (!m_set.isEmpty()) {
        OwnPtr<FloatingObject> floatingObject = m_set.takeFirst();
        LayoutBox* layoutObject = floatingObject->layoutObject();
        map.add(layoutObject, floatingObject.release());
    }
    clear();
}

inline void FloatingObjects::increaseObjectsCount(FloatingObject::Type type)
{
    if (type == FloatingObject::FloatLeft)
        m_leftObjectsCount++;
    else
        m_rightObjectsCount++;
}

inline void FloatingObjects::decreaseObjectsCount(FloatingObject::Type type)
{
    if (type == FloatingObject::FloatLeft)
        m_leftObjectsCount--;
    else
        m_rightObjectsCount--;
}

inline FloatingObjectInterval FloatingObjects::intervalForFloatingObject(FloatingObject& floatingObject)
{
    if (m_horizontalWritingMode)
        return FloatingObjectInterval(floatingObject.frameRect().pixelSnappedY(), floatingObject.frameRect().pixelSnappedMaxY(), &floatingObject);
    return FloatingObjectInterval(floatingObject.frameRect().pixelSnappedX(), floatingObject.frameRect().pixelSnappedMaxX(), &floatingObject);
}

void FloatingObjects::addPlacedObject(FloatingObject& floatingObject)
{
    ASSERT(!floatingObject.isInPlacedTree());

    floatingObject.setIsPlaced(true);
    if (m_placedFloatsTree.isInitialized())
        m_placedFloatsTree.add(intervalForFloatingObject(floatingObject));

#if ENABLE(ASSERT)
    floatingObject.setIsInPlacedTree(true);
#endif
    markLowestFloatLogicalBottomCacheAsDirty();
}

void FloatingObjects::removePlacedObject(FloatingObject& floatingObject)
{
    ASSERT(floatingObject.isPlaced() && floatingObject.isInPlacedTree());

    if (m_placedFloatsTree.isInitialized()) {
        bool removed = m_placedFloatsTree.remove(intervalForFloatingObject(floatingObject));
        ASSERT_UNUSED(removed, removed);
    }

    floatingObject.setIsPlaced(false);
#if ENABLE(ASSERT)
    floatingObject.setIsInPlacedTree(false);
#endif
    markLowestFloatLogicalBottomCacheAsDirty();
}

FloatingObject* FloatingObjects::add(PassOwnPtr<FloatingObject> floatingObject)
{
    FloatingObject* newObject = floatingObject.leakPtr();
    increaseObjectsCount(newObject->type());
    m_set.add(adoptPtr(newObject));
    if (newObject->isPlaced())
        addPlacedObject(*newObject);
    markLowestFloatLogicalBottomCacheAsDirty();
    return newObject;
}

void FloatingObjects::remove(FloatingObject* toBeRemoved)
{
    decreaseObjectsCount(toBeRemoved->type());
    OwnPtr<FloatingObject> floatingObject = m_set.take(toBeRemoved);
    ASSERT(floatingObject->isPlaced() || !floatingObject->isInPlacedTree());
    if (floatingObject->isPlaced())
        removePlacedObject(*floatingObject);
    markLowestFloatLogicalBottomCacheAsDirty();
    ASSERT(!floatingObject->originatingLine());
}

void FloatingObjects::computePlacedFloatsTree()
{
    ASSERT(!m_placedFloatsTree.isInitialized());
    if (m_set.isEmpty())
        return;
    m_placedFloatsTree.initIfNeeded(m_layoutObject->view()->intervalArena());
    FloatingObjectSetIterator it = m_set.begin();
    FloatingObjectSetIterator end = m_set.end();
    for (; it != end; ++it) {
        FloatingObject& floatingObject = *it->get();
        if (floatingObject.isPlaced())
            m_placedFloatsTree.add(intervalForFloatingObject(floatingObject));
    }
}

LayoutUnit FloatingObjects::logicalLeftOffsetForPositioningFloat(LayoutUnit fixedOffset, LayoutUnit logicalTop, LayoutUnit *heightRemaining)
{
    int logicalTopAsInt = roundToInt(logicalTop);
    ComputeFloatOffsetForFloatLayoutAdapter<FloatingObject::FloatLeft> adapter(m_layoutObject, logicalTopAsInt, logicalTopAsInt, fixedOffset);
    placedFloatsTree().allOverlapsWithAdapter(adapter);

    if (heightRemaining)
        *heightRemaining = adapter.heightRemaining();

    return adapter.offset();
}

LayoutUnit FloatingObjects::logicalRightOffsetForPositioningFloat(LayoutUnit fixedOffset, LayoutUnit logicalTop, LayoutUnit *heightRemaining)
{
    int logicalTopAsInt = roundToInt(logicalTop);
    ComputeFloatOffsetForFloatLayoutAdapter<FloatingObject::FloatRight> adapter(m_layoutObject, logicalTopAsInt, logicalTopAsInt, fixedOffset);
    placedFloatsTree().allOverlapsWithAdapter(adapter);

    if (heightRemaining)
        *heightRemaining = adapter.heightRemaining();

    return std::min(fixedOffset, adapter.offset());
}

LayoutUnit FloatingObjects::logicalLeftOffset(LayoutUnit fixedOffset, LayoutUnit logicalTop, LayoutUnit logicalHeight)
{
    ComputeFloatOffsetForLineLayoutAdapter<FloatingObject::FloatLeft> adapter(m_layoutObject, roundToInt(logicalTop), roundToInt(logicalTop + logicalHeight), fixedOffset);
    placedFloatsTree().allOverlapsWithAdapter(adapter);

    return adapter.offset();
}

LayoutUnit FloatingObjects::logicalRightOffset(LayoutUnit fixedOffset, LayoutUnit logicalTop, LayoutUnit logicalHeight)
{
    ComputeFloatOffsetForLineLayoutAdapter<FloatingObject::FloatRight> adapter(m_layoutObject, roundToInt(logicalTop), roundToInt(logicalTop + logicalHeight), fixedOffset);
    placedFloatsTree().allOverlapsWithAdapter(adapter);

    return std::min(fixedOffset, adapter.offset());
}

FloatingObjects::FloatBottomCachedValue::FloatBottomCachedValue()
    : floatingObject(nullptr)
    , dirty(true)
{
}

inline static bool rangesIntersect(int floatTop, int floatBottom, int objectTop, int objectBottom)
{
    if (objectTop >= floatBottom || objectBottom < floatTop)
        return false;

    // The top of the object overlaps the float
    if (objectTop >= floatTop)
        return true;

    // The object encloses the float
    if (objectTop < floatTop && objectBottom > floatBottom)
        return true;

    // The bottom of the object overlaps the float
    if (objectBottom > objectTop && objectBottom > floatTop && objectBottom <= floatBottom)
        return true;

    return false;
}

template<>
inline bool ComputeFloatOffsetForFloatLayoutAdapter<FloatingObject::FloatLeft>::updateOffsetIfNeeded(const FloatingObject& floatingObject)
{
    LayoutUnit logicalRight = m_layoutObject->logicalRightForFloat(floatingObject);
    if (logicalRight > m_offset) {
        m_offset = logicalRight;
        return true;
    }
    return false;
}

template<>
inline bool ComputeFloatOffsetForFloatLayoutAdapter<FloatingObject::FloatRight>::updateOffsetIfNeeded(const FloatingObject& floatingObject)
{
    LayoutUnit logicalLeft = m_layoutObject->logicalLeftForFloat(floatingObject);
    if (logicalLeft < m_offset) {
        m_offset = logicalLeft;
        return true;
    }
    return false;
}

template <FloatingObject::Type FloatTypeValue>
LayoutUnit ComputeFloatOffsetForFloatLayoutAdapter<FloatTypeValue>::heightRemaining() const
{
    return this->m_outermostFloat ? this->m_layoutObject->logicalBottomForFloat(*this->m_outermostFloat) - this->m_lineTop : LayoutUnit(1);
}

template <FloatingObject::Type FloatTypeValue>
inline void ComputeFloatOffsetAdapter<FloatTypeValue>::collectIfNeeded(const IntervalType& interval)
{
    const FloatingObject& floatingObject = *(interval.data());
    if (floatingObject.type() != FloatTypeValue || !rangesIntersect(interval.low(), interval.high(), m_lineTop, m_lineBottom))
        return;

    // Make sure the float hasn't changed since it was added to the placed floats tree.
    ASSERT(floatingObject.isPlaced());
    ASSERT(interval.low() == m_layoutObject->pixelSnappedLogicalTopForFloat(floatingObject));
    ASSERT(interval.high() == m_layoutObject->pixelSnappedLogicalBottomForFloat(floatingObject));

    bool floatIsNewExtreme = updateOffsetIfNeeded(floatingObject);
    if (floatIsNewExtreme)
        m_outermostFloat = &floatingObject;
}

template<>
inline bool ComputeFloatOffsetForLineLayoutAdapter<FloatingObject::FloatLeft>::updateOffsetIfNeeded(const FloatingObject& floatingObject)
{
    LayoutUnit logicalRight = m_layoutObject->logicalRightForFloat(floatingObject);
    if (ShapeOutsideInfo* shapeOutside = floatingObject.layoutObject()->shapeOutsideInfo()) {
        ShapeOutsideDeltas shapeDeltas = shapeOutside->computeDeltasForContainingBlockLine(LineLayoutBlockFlow(const_cast<LayoutBlockFlow*>(m_layoutObject)), floatingObject, m_lineTop, m_lineBottom - m_lineTop);
        if (!shapeDeltas.lineOverlapsShape())
            return false;

        logicalRight += shapeDeltas.rightMarginBoxDelta();
    }
    if (logicalRight > m_offset) {
        m_offset = logicalRight;
        return true;
    }

    return false;
}

template<>
inline bool ComputeFloatOffsetForLineLayoutAdapter<FloatingObject::FloatRight>::updateOffsetIfNeeded(const FloatingObject& floatingObject)
{
    LayoutUnit logicalLeft = m_layoutObject->logicalLeftForFloat(floatingObject);
    if (ShapeOutsideInfo* shapeOutside = floatingObject.layoutObject()->shapeOutsideInfo()) {
        ShapeOutsideDeltas shapeDeltas = shapeOutside->computeDeltasForContainingBlockLine(LineLayoutBlockFlow(const_cast<LayoutBlockFlow*>(m_layoutObject)), floatingObject, m_lineTop, m_lineBottom - m_lineTop);
        if (!shapeDeltas.lineOverlapsShape())
            return false;

        logicalLeft += shapeDeltas.leftMarginBoxDelta();
    }
    if (logicalLeft < m_offset) {
        m_offset = logicalLeft;
        return true;
    }

    return false;
}

#ifndef NDEBUG
// These helpers are only used by the PODIntervalTree for debugging purposes.
String ValueToString<int>::string(const int value)
{
    return String::number(value);
}

String ValueToString<FloatingObject*>::string(const FloatingObject* floatingObject)
{
    return String::format("%p (%dx%d %dx%d)", floatingObject, floatingObject->frameRect().pixelSnappedX(), floatingObject->frameRect().pixelSnappedY(), floatingObject->frameRect().pixelSnappedMaxX(), floatingObject->frameRect().pixelSnappedMaxY());
}
#endif


} // namespace blink
