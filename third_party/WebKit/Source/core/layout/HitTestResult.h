/*
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
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
 *
*/

#ifndef HitTestResult_h
#define HitTestResult_h

#include "core/CoreExport.h"
#include "core/editing/PositionWithAffinity.h"
#include "core/layout/HitTestLocation.h"
#include "core/layout/HitTestRequest.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/heap/Handle.h"
#include "platform/text/TextDirection.h"
#include "wtf/Forward.h"
#include "wtf/ListHashSet.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/VectorTraits.h"

namespace blink {

class Element;
class LocalFrame;
class HTMLAreaElement;
class HTMLMediaElement;
class Image;
class KURL;
class Node;
class LayoutObject;
class Scrollbar;

class CORE_EXPORT HitTestResult {
    ALLOW_ONLY_INLINE_ALLOCATION();

public:
    typedef WillBeHeapListHashSet<RefPtrWillBeMember<Node>> NodeSet;

    HitTestResult();
    HitTestResult(const HitTestRequest&, const LayoutPoint&);
    // Pass positive padding values to perform a rect-based hit test.
    HitTestResult(const HitTestRequest&, const LayoutPoint& centerPoint, unsigned topPadding, unsigned rightPadding, unsigned bottomPadding, unsigned leftPadding);
    HitTestResult(const HitTestRequest&, const HitTestLocation&);
    HitTestResult(const HitTestResult&);
    ~HitTestResult();
    HitTestResult& operator=(const HitTestResult&);
    DECLARE_TRACE();

    bool equalForCacheability(const HitTestResult&) const;
    unsigned equalityScore(const HitTestResult&) const;
    void cacheValues(const HitTestResult&);

    // Populate this object based on another HitTestResult; similar to assignment operator
    // but don't assign any of the request parameters. ie. Thie method avoids setting
    // |m_hitTestLocation|, |m_hitTestRequest|.
    void populateFromCachedResult(const HitTestResult&);

    // For point-based hit tests, these accessors provide information about the node
    // under the point. For rect-based hit tests they are meaningless (reflect the
    // last candidate node observed in the rect).
    // FIXME: Make these less error-prone for rect-based hit tests (center point or fail).
    Node* innerNode() const { return m_innerNode.get(); }
    Node* innerPossiblyPseudoNode() const { return m_innerPossiblyPseudoNode.get(); }
    Element* innerElement() const;

    // If innerNode is an image map or image map area, return the associated image node.
    Node* innerNodeOrImageMapImage() const;

    Element* URLElement() const { return m_innerURLElement.get(); }
    Scrollbar* scrollbar() const { return m_scrollbar.get(); }
    bool isOverWidget() const { return m_isOverWidget; }

    // Forwarded from HitTestLocation
    bool isRectBasedTest() const { return m_hitTestLocation.isRectBasedTest(); }

    // The hit-tested point in the coordinates of the main frame.
    const LayoutPoint& pointInMainFrame() const { return m_hitTestLocation.point(); }
    IntPoint roundedPointInMainFrame() const { return roundedIntPoint(pointInMainFrame()); }

    // The hit-tested point in the coordinates of the innerNode frame, the frame containing innerNode.
    const LayoutPoint& pointInInnerNodeFrame() const { return m_pointInInnerNodeFrame; }
    IntPoint roundedPointInInnerNodeFrame() const { return roundedIntPoint(pointInInnerNodeFrame()); }
    LocalFrame* innerNodeFrame() const;

    // The hit-tested point in the coordinates of the inner node.
    const LayoutPoint& localPoint() const { return m_localPoint; }
    void setNodeAndPosition(Node* node, const LayoutPoint& p) { m_localPoint = p; setInnerNode(node); }

    PositionWithAffinity position() const;
    LayoutObject* layoutObject() const;

    void setToShadowHostIfInUserAgentShadowRoot();

    const HitTestLocation& hitTestLocation() const { return m_hitTestLocation; }
    const HitTestRequest& hitTestRequest() const { return m_hitTestRequest; }

    void setInnerNode(Node*);
    HTMLAreaElement* imageAreaForImage() const;
    void setURLElement(Element*);
    void setScrollbar(Scrollbar*);
    void setIsOverWidget(bool b) { m_isOverWidget = b; }

    bool isSelected() const;
    String spellingToolTip(TextDirection&) const;
    String title(TextDirection&) const;
    const AtomicString& altDisplayString() const;
    Image* image() const;
    IntRect imageRect() const;
    KURL absoluteImageURL() const;
    KURL absoluteMediaURL() const;
    KURL absoluteLinkURL() const;
    String textContent() const;
    bool isLiveLink() const;
    bool isMisspelled() const;
    bool isContentEditable() const;

    bool isOverLink() const;

    bool isCacheable() const { return m_cacheable; }
    void setCacheable(bool cacheable) { m_cacheable = cacheable; }

    // Return true if the test is a list-based test and we should continue testing.
    bool addNodeToListBasedTestResult(Node*, const HitTestLocation& pointInContainer, const LayoutRect& = LayoutRect());
    bool addNodeToListBasedTestResult(Node*, const HitTestLocation& pointInContainer, const FloatRect&);
    void append(const HitTestResult&);

    // If m_listBasedTestResult is 0 then set it to a new NodeSet. Return *m_listBasedTestResult. Lazy allocation makes
    // sense because the NodeSet is seldom necessary, and it's somewhat expensive to allocate and initialize. This method does
    // the same thing as mutableListBasedTestResult(), but here the return value is const.
    const NodeSet& listBasedTestResult() const;

    // Collapse the rect-based test result into a single target at the specified location.
    void resolveRectBasedTest(Node* resolvedInnerNode, const LayoutPoint& resolvedPointInMainFrame);

private:
    NodeSet& mutableListBasedTestResult(); // See above.
    HTMLMediaElement* mediaElement() const;

    HitTestLocation m_hitTestLocation;
    HitTestRequest m_hitTestRequest;
    bool m_cacheable;

    RefPtrWillBeMember<Node> m_innerNode;
    RefPtrWillBeMember<Node> m_innerPossiblyPseudoNode;
    // FIXME: Nothing changes this to a value different from m_hitTestLocation!
    LayoutPoint m_pointInInnerNodeFrame; // The hit-tested point in innerNode frame coordinates.
    LayoutPoint m_localPoint; // A point in the local coordinate space of m_innerNode's layoutObject. Allows us to efficiently
        // determine where inside the layoutObject we hit on subsequent operations.
    RefPtrWillBeMember<Element> m_innerURLElement;
    RefPtrWillBeMember<Scrollbar> m_scrollbar;
    bool m_isOverWidget; // Returns true if we are over a widget (and not in the border/padding area of a LayoutPart for example).

    mutable OwnPtrWillBeMember<NodeSet> m_listBasedTestResult;
};

} // namespace blink

WTF_ALLOW_CLEAR_UNUSED_SLOTS_WITH_MEM_FUNCTIONS(blink::HitTestResult);

#endif // HitTestResult_h
