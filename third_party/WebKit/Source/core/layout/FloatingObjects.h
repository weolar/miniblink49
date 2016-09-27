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

#ifndef FloatingObjects_h
#define FloatingObjects_h

#include "core/layout/line/RootInlineBox.h"
#include "platform/PODFreeListArena.h"
#include "platform/PODIntervalTree.h"
#include "wtf/ListHashSet.h"
#include "wtf/OwnPtr.h"

namespace blink {

class LayoutBlockFlow;
class LayoutBox;

// FIXME this should be removed once LayoutBlockFlow::nextFloatLogicalBottomBelow doesn't need it anymore. (Bug 123931)
enum ShapeOutsideFloatOffsetMode { ShapeOutsideFloatShapeOffset, ShapeOutsideFloatMarginBoxOffset };

class FloatingObject {
    WTF_MAKE_NONCOPYABLE(FloatingObject); WTF_MAKE_FAST_ALLOCATED(FloatingObject);
public:
#ifndef NDEBUG
    // Used by the PODIntervalTree for debugging the FloatingObject.
    template <class> friend struct ValueToString;
#endif

    // Note that Type uses bits so you can use FloatLeftRight as a mask to query for both left and right.
    enum Type { FloatLeft = 1, FloatRight = 2, FloatLeftRight = 3 };

    static PassOwnPtr<FloatingObject> create(LayoutBox*);

    PassOwnPtr<FloatingObject> copyToNewContainer(LayoutSize, bool shouldPaint = false, bool isDescendant = false) const;

    PassOwnPtr<FloatingObject> unsafeClone() const;

    Type type() const { return static_cast<Type>(m_type); }
    LayoutBox* layoutObject() const { return m_layoutObject; }

    bool isPlaced() const { return m_isPlaced; }
    void setIsPlaced(bool placed = true) { m_isPlaced = placed; }

    LayoutUnit x() const { ASSERT(isPlaced()); return m_frameRect.x(); }
    LayoutUnit maxX() const { ASSERT(isPlaced()); return m_frameRect.maxX(); }
    LayoutUnit y() const { ASSERT(isPlaced()); return m_frameRect.y(); }
    LayoutUnit maxY() const { ASSERT(isPlaced()); return m_frameRect.maxY(); }
    LayoutUnit width() const { return m_frameRect.width(); }
    LayoutUnit height() const { return m_frameRect.height(); }

    void setX(LayoutUnit x) { ASSERT(!isInPlacedTree()); m_frameRect.setX(x); }
    void setY(LayoutUnit y) { ASSERT(!isInPlacedTree()); m_frameRect.setY(y); }
    void setWidth(LayoutUnit width) { ASSERT(!isInPlacedTree()); m_frameRect.setWidth(width); }
    void setHeight(LayoutUnit height) { ASSERT(!isInPlacedTree()); m_frameRect.setHeight(height); }

    const LayoutRect& frameRect() const { ASSERT(isPlaced()); return m_frameRect; }

    int paginationStrut() const { return m_paginationStrut; }
    void setPaginationStrut(int strut) { m_paginationStrut = strut; }

#if ENABLE(ASSERT)
    bool isInPlacedTree() const { return m_isInPlacedTree; }
    void setIsInPlacedTree(bool value) { m_isInPlacedTree = value; }
#endif

    bool shouldPaint() const { return m_shouldPaint; }
    void setShouldPaint(bool shouldPaint) { m_shouldPaint = shouldPaint; }
    bool isDescendant() const { return m_isDescendant; }
    void setIsDescendant(bool isDescendant) { m_isDescendant = isDescendant; }
    bool isLowestNonOverhangingFloatInChild() const { return m_isLowestNonOverhangingFloatInChild; }
    void setIsLowestNonOverhangingFloatInChild(bool isLowestNonOverhangingFloatInChild) { m_isLowestNonOverhangingFloatInChild = isLowestNonOverhangingFloatInChild; }

    // FIXME: Callers of these methods are dangerous and should be whitelisted explicitly or removed.
    RootInlineBox* originatingLine() const { return m_originatingLine; }
    void setOriginatingLine(RootInlineBox* line) { m_originatingLine = line; }

private:
    explicit FloatingObject(LayoutBox*);
    FloatingObject(LayoutBox*, Type, const LayoutRect&, bool shouldPaint, bool isDescendant, bool isLowestNonOverhangingFloatInChild);

    LayoutBox* m_layoutObject;
    RootInlineBox* m_originatingLine;
    LayoutRect m_frameRect;
    int m_paginationStrut; // FIXME: Is this class size-sensitive? Does this need 32-bits?

    unsigned m_type : 2; // Type (left or right aligned)
    unsigned m_shouldPaint : 1;
    unsigned m_isDescendant : 1;
    unsigned m_isPlaced : 1;
    unsigned m_isLowestNonOverhangingFloatInChild : 1;
#if ENABLE(ASSERT)
    unsigned m_isInPlacedTree : 1;
#endif
};

struct FloatingObjectHashFunctions {
    static unsigned hash(FloatingObject* key) { return DefaultHash<LayoutBox*>::Hash::hash(key->layoutObject()); }
    static unsigned hash(const OwnPtr<FloatingObject>& key) { return hash(key.get()); }
    static unsigned hash(const PassOwnPtr<FloatingObject>& key) { return hash(key.get()); }
    static bool equal(OwnPtr<FloatingObject>& a, FloatingObject* b) { return a->layoutObject() == b->layoutObject(); }
    static bool equal(OwnPtr<FloatingObject>& a, const OwnPtr<FloatingObject>& b) { return equal(a, b.get()); }
    static bool equal(OwnPtr<FloatingObject>& a, const PassOwnPtr<FloatingObject>& b) { return equal(a, b.get()); }

    static const bool safeToCompareToEmptyOrDeleted = true;
};
struct FloatingObjectHashTranslator {
    static unsigned hash(LayoutBox* key) { return DefaultHash<LayoutBox*>::Hash::hash(key); }
    static bool equal(FloatingObject* a, LayoutBox* b) { return a->layoutObject() == b; }
    static bool equal(const OwnPtr<FloatingObject>& a, LayoutBox* b) { return a->layoutObject() == b; }
};
typedef ListHashSet<OwnPtr<FloatingObject>, 4, FloatingObjectHashFunctions> FloatingObjectSet;
typedef FloatingObjectSet::const_iterator FloatingObjectSetIterator;
typedef PODInterval<int, FloatingObject*> FloatingObjectInterval;
typedef PODIntervalTree<int, FloatingObject*> FloatingObjectTree;
typedef PODFreeListArena<PODRedBlackTree<FloatingObjectInterval>::Node> IntervalArena;
typedef HashMap<LayoutBox*, OwnPtr<FloatingObject>> LayoutBoxToFloatInfoMap;

class FloatingObjects {
    WTF_MAKE_NONCOPYABLE(FloatingObjects); WTF_MAKE_FAST_ALLOCATED(FloatingObjects);
public:
    FloatingObjects(const LayoutBlockFlow*, bool horizontalWritingMode);
    ~FloatingObjects();

    void clear();
    void moveAllToFloatInfoMap(LayoutBoxToFloatInfoMap&);
    FloatingObject* add(PassOwnPtr<FloatingObject>);
    void remove(FloatingObject*);
    void addPlacedObject(FloatingObject&);
    void removePlacedObject(FloatingObject&);
    void setHorizontalWritingMode(bool b = true) { m_horizontalWritingMode = b; }

    bool hasLeftObjects() const { return m_leftObjectsCount > 0; }
    bool hasRightObjects() const { return m_rightObjectsCount > 0; }
    const FloatingObjectSet& set() const { return m_set; }
    void clearLineBoxTreePointers();

    LayoutUnit logicalLeftOffset(LayoutUnit fixedOffset, LayoutUnit logicalTop, LayoutUnit logicalHeight);
    LayoutUnit logicalRightOffset(LayoutUnit fixedOffset, LayoutUnit logicalTop, LayoutUnit logicalHeight);

    LayoutUnit logicalLeftOffsetForPositioningFloat(LayoutUnit fixedOffset, LayoutUnit logicalTop, LayoutUnit* heightRemaining);
    LayoutUnit logicalRightOffsetForPositioningFloat(LayoutUnit fixedOffset, LayoutUnit logicalTop, LayoutUnit* heightRemaining);

    LayoutUnit lowestFloatLogicalBottom(FloatingObject::Type);
    FloatingObject* lowestFloatingObject() const;

private:
    bool hasLowestFloatLogicalBottomCached(bool isHorizontal, FloatingObject::Type floatType) const;
    LayoutUnit getCachedlowestFloatLogicalBottom(FloatingObject::Type floatType) const;
    void setCachedLowestFloatLogicalBottom(bool isHorizontal, FloatingObject::Type floatType, FloatingObject*);
    void markLowestFloatLogicalBottomCacheAsDirty();

    void computePlacedFloatsTree();
    const FloatingObjectTree& placedFloatsTree()
    {
        if (!m_placedFloatsTree.isInitialized())
            computePlacedFloatsTree();
        return m_placedFloatsTree;
    }
    void increaseObjectsCount(FloatingObject::Type);
    void decreaseObjectsCount(FloatingObject::Type);
    FloatingObjectInterval intervalForFloatingObject(FloatingObject&);

    FloatingObjectSet m_set;
    FloatingObjectTree m_placedFloatsTree;
    unsigned m_leftObjectsCount;
    unsigned m_rightObjectsCount;
    bool m_horizontalWritingMode;
    const LayoutBlockFlow* m_layoutObject;

    struct FloatBottomCachedValue {
        FloatBottomCachedValue();
        FloatingObject* floatingObject;
        bool dirty;
    };
    FloatBottomCachedValue m_lowestFloatBottomCache[2];
    bool m_cachedHorizontalWritingMode;
};

#ifndef NDEBUG
// These structures are used by PODIntervalTree for debugging purposes.
template <> struct ValueToString<int> {
    static String string(const int value);
};
template<> struct ValueToString<FloatingObject*> {
    static String string(const FloatingObject*);
};
#endif

} // namespace blink

#endif // FloatingObjects_h
