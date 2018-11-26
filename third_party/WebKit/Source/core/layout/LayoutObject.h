/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 *           (C) 2004 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef LayoutObject_h
#define LayoutObject_h

#include "core/CoreExport.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentLifecycle.h"
#include "core/dom/Element.h"
#include "core/dom/Position.h"
#include "core/editing/PositionWithAffinity.h"
#include "core/editing/TextAffinity.h"
#include "core/fetch/ImageResourceClient.h"
#include "core/html/HTMLElement.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/layout/HitTestRequest.h"
#include "core/layout/LayoutObjectChildList.h"
#include "core/layout/PaintInvalidationState.h"
#include "core/layout/ScrollAlignment.h"
#include "core/layout/SubtreeLayoutScope.h"
#include "core/layout/compositing/CompositingState.h"
#include "core/layout/compositing/CompositingTriggers.h"
#include "core/style/ComputedStyle.h"
#include "core/style/StyleInheritedData.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/CompositingReasons.h"
#include "platform/graphics/PaintInvalidationReason.h"
#include "platform/graphics/paint/DisplayItemClient.h"
#include "platform/transforms/TransformationMatrix.h"

namespace blink {

class AffineTransform;
class Cursor;
class Document;
class HitTestLocation;
class HitTestResult;
class InlineBox;
class PseudoStyleRequest;
class LayoutBoxModelObject;
class LayoutBlock;
class LayoutFlowThread;
class LayoutGeometryMap;
class DeprecatedPaintLayer;
class LayoutMultiColumnSpannerPlaceholder;
class LayoutView;
class TransformState;

struct PaintInfo;

enum CursorDirective {
    SetCursorBasedOnStyle,
    SetCursor,
    DoNotSetCursor
};

enum HitTestFilter {
    HitTestAll,
    HitTestSelf,
    HitTestDescendants
};

enum HitTestAction {
    HitTestBlockBackground,
    HitTestChildBlockBackground,
    HitTestChildBlockBackgrounds,
    HitTestFloat,
    HitTestForeground
};

enum MarkingBehavior {
    MarkOnlyThis,
    MarkContainerChain,
};

enum MapCoordinatesMode {
    IsFixed = 1 << 0,
    UseTransforms = 1 << 1,
    ApplyContainerFlip = 1 << 2,
    TraverseDocumentBoundaries = 1 << 3,
};
typedef unsigned MapCoordinatesFlags;

const LayoutUnit& caretWidth();

struct AnnotatedRegionValue {
    bool operator==(const AnnotatedRegionValue& o) const
    {
        return draggable == o.draggable && bounds == o.bounds;
    }

    LayoutRect bounds;
    bool draggable;
};

typedef WTF::HashMap<const DeprecatedPaintLayer*, Vector<LayoutRect>> LayerHitTestRects;

#ifndef NDEBUG
const int showTreeCharacterOffset = 39;
#endif

// Base class for all layout tree objects.
class CORE_EXPORT LayoutObject : public ImageResourceClient {
    friend class LayoutBlock;
    friend class LayoutBlockFlow;
    friend class DeprecatedPaintLayerReflectionInfo; // For setParent
    friend class DeprecatedPaintLayerScrollableArea; // For setParent.
    friend class LayoutObjectChildList;
    WTF_MAKE_NONCOPYABLE(LayoutObject);
public:
    // Anonymous objects should pass the document as their node, and they will then automatically be
    // marked as anonymous in the constructor.
    explicit LayoutObject(Node*);
    virtual ~LayoutObject();

    // Returns the name of the layout object.
    virtual const char* name() const = 0;

    // Returns the decorated name used by run-layout-tests. The name contains the name of the object
    // along with extra information about the layout object state (e.g. positioning).
    String decoratedName() const;

    // Returns the decorated name along with the debug information from the associated Node object.
    String debugName() const;

    LayoutObject* parent() const { return m_parent; }
    bool isDescendantOf(const LayoutObject*) const;

    LayoutObject* previousSibling() const { return m_previous; }
    LayoutObject* nextSibling() const { return m_next; }

    LayoutObject* slowFirstChild() const
    {
        if (const LayoutObjectChildList* children = virtualChildren())
            return children->firstChild();
        return nullptr;
    }
    LayoutObject* slowLastChild() const
    {
        if (const LayoutObjectChildList* children = virtualChildren())
            return children->lastChild();
        return nullptr;
    }

    virtual LayoutObjectChildList* virtualChildren() { return nullptr; }
    virtual const LayoutObjectChildList* virtualChildren() const { return nullptr; }

    LayoutObject* nextInPreOrder() const;
    LayoutObject* nextInPreOrder(const LayoutObject* stayWithin) const;
    LayoutObject* nextInPreOrderAfterChildren() const;
    LayoutObject* nextInPreOrderAfterChildren(const LayoutObject* stayWithin) const;
    LayoutObject* previousInPreOrder() const;
    LayoutObject* previousInPreOrder(const LayoutObject* stayWithin) const;
    LayoutObject* childAt(unsigned) const;

    LayoutObject* lastLeafChild() const;

    // The following six functions are used when the layout tree hierarchy changes to make sure layers get
    // properly added and removed.  Since containership can be implemented by any subclass, and since a hierarchy
    // can contain a mixture of boxes and other object types, these functions need to be in the base class.
    DeprecatedPaintLayer* enclosingLayer() const;
    void addLayers(DeprecatedPaintLayer* parentLayer);
    void removeLayers(DeprecatedPaintLayer* parentLayer);
    void moveLayers(DeprecatedPaintLayer* oldParent, DeprecatedPaintLayer* newParent);
    DeprecatedPaintLayer* findNextLayer(DeprecatedPaintLayer* parentLayer, LayoutObject* startPoint, bool checkParent = true);

    // Scrolling is a LayoutBox concept, however some code just cares about recursively scrolling our enclosing ScrollableArea(s).
    bool scrollRectToVisible(const LayoutRect&, const ScrollAlignment& alignX = ScrollAlignment::alignCenterIfNeeded, const ScrollAlignment& alignY = ScrollAlignment::alignCenterIfNeeded);

    // Convenience function for getting to the nearest enclosing box of a LayoutObject.
    LayoutBox* enclosingBox() const;
    LayoutBoxModelObject* enclosingBoxModelObject() const;

    LayoutBox* enclosingScrollableBox() const;

    // Function to return our enclosing flow thread if we are contained inside one. This
    // function follows the containing block chain.
    LayoutFlowThread* flowThreadContainingBlock() const
    {
        if (!isInsideFlowThread())
            return nullptr;
        return locateFlowThreadContainingBlock();
    }

#if ENABLE(ASSERT)
    void setHasAXObject(bool flag) { m_hasAXObject = flag; }
    bool hasAXObject() const { return m_hasAXObject; }

    // Helper class forbidding calls to setNeedsLayout() during its lifetime.
    class SetLayoutNeededForbiddenScope {
    public:
        explicit SetLayoutNeededForbiddenScope(LayoutObject&);
        ~SetLayoutNeededForbiddenScope();
    private:
        LayoutObject& m_layoutObject;
        bool m_preexistingForbidden;
    };

    void assertLaidOut() const;
    void assertSubtreeIsLaidOut() const;
    void assertClearedPaintInvalidationState() const;
    void assertSubtreeClearedPaintInvalidationState() const;
#endif

    // Correct version of !layoutObjectHasNoBoxEffectObsolete().
    bool hasBoxEffect() const
    {
        return hasBoxDecorationBackground() || style()->hasVisualOverflowingEffect();
    }

    // Obtains the nearest enclosing block (including this block) that contributes a first-line style to our inline
    // children.
    virtual LayoutBlock* firstLineBlock() const;

    // LayoutObject tree manipulation
    //////////////////////////////////////////
    virtual bool canHaveChildren() const { return virtualChildren(); }
    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const { return true; }
    virtual void addChild(LayoutObject* newChild, LayoutObject* beforeChild = nullptr);
    virtual void addChildIgnoringContinuation(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) { return addChild(newChild, beforeChild); }
    virtual void removeChild(LayoutObject*);
    virtual bool createsAnonymousWrapper() const { return false; }
    //////////////////////////////////////////

protected:
    //////////////////////////////////////////
    // Helper functions. Dangerous to use!
    void setPreviousSibling(LayoutObject* previous) { m_previous = previous; }
    void setNextSibling(LayoutObject* next) { m_next = next; }
    void setParent(LayoutObject* parent)
    {
        m_parent = parent;

        // Only update if our flow thread state is different from our new parent and if we're not a LayoutFlowThread.
        // A LayoutFlowThread is always considered to be inside itself, so it never has to change its state
        // in response to parent changes.
        bool insideFlowThread = parent && parent->isInsideFlowThread();
        if (insideFlowThread != isInsideFlowThread() && !isLayoutFlowThread())
            setIsInsideFlowThreadIncludingDescendants(insideFlowThread);
    }

    //////////////////////////////////////////
private:
#if ENABLE(ASSERT)
    bool isSetNeedsLayoutForbidden() const { return m_setNeedsLayoutForbidden; }
    void setNeedsLayoutIsForbidden(bool flag) { m_setNeedsLayoutForbidden = flag; }
#endif

    void addAbsoluteRectForLayer(IntRect& result);
    bool requiresAnonymousTableWrappers(const LayoutObject*) const;

    // Gets pseudoStyle from Shadow host(in case of input elements)
    // or from Parent element.
    PassRefPtr<ComputedStyle> getUncachedPseudoStyleFromParentOrShadowHost() const;

    bool skipInvalidationWhenLaidOutChildren() const;

public:
#ifndef NDEBUG
    void showTreeForThis() const;
    void showLayoutTreeForThis() const;
    void showLineTreeForThis() const;

    void showLayoutObject() const;
    // We don't make printedCharacters an optional parameter so that
    // showLayoutObject can be called from gdb easily.
    void showLayoutObject(int printedCharacters) const;
    void showLayoutTreeAndMark(const LayoutObject* markedObject1 = nullptr, const char* markedLabel1 = nullptr, const LayoutObject* markedObject2 = nullptr, const char* markedLabel2 = nullptr, int depth = 0) const;
#endif

    static LayoutObject* createObject(Element*, const ComputedStyle&);

    // LayoutObjects are allocated out of the rendering partition.
    void* operator new(size_t);
    void operator delete(void*);

public:
    bool isPseudoElement() const { return node() && node()->isPseudoElement(); }

    virtual bool isBoxModelObject() const { return false; }
    bool isBR() const { return isOfType(LayoutObjectBr); }
    bool isCanvas() const { return isOfType(LayoutObjectCanvas); }
    bool isCounter() const { return isOfType(LayoutObjectCounter); }
    bool isDetailsMarker() const { return isOfType(LayoutObjectDetailsMarker); }
    bool isEmbeddedObject() const { return isOfType(LayoutObjectEmbeddedObject); }
    bool isFieldset() const { return isOfType(LayoutObjectFieldset); }
    bool isFileUploadControl() const { return isOfType(LayoutObjectFileUploadControl); }
    bool isFrame() const { return isOfType(LayoutObjectFrame); }
    bool isFrameSet() const { return isOfType(LayoutObjectFrameSet); }
    bool isLayoutTableCol() const { return isOfType(LayoutObjectLayoutTableCol); }
    bool isListBox() const { return isOfType(LayoutObjectListBox); }
    bool isListItem() const { return isOfType(LayoutObjectListItem); }
    bool isListMarker() const { return isOfType(LayoutObjectListMarker); }
    bool isMedia() const { return isOfType(LayoutObjectMedia); }
    bool isMenuList() const { return isOfType(LayoutObjectMenuList); }
    bool isMeter() const { return isOfType(LayoutObjectMeter); }
    bool isProgress() const { return isOfType(LayoutObjectProgress); }
    bool isQuote() const { return isOfType(LayoutObjectQuote); }
    bool isLayoutButton() const { return isOfType(LayoutObjectLayoutButton); }
    bool isLayoutFullScreen() const { return isOfType(LayoutObjectLayoutFullScreen); }
    bool isLayoutFullScreenPlaceholder() const { return isOfType(LayoutObjectLayoutFullScreenPlaceholder); }
    bool isLayoutGrid() const { return isOfType(LayoutObjectLayoutGrid); }
    bool isLayoutIFrame() const { return isOfType(LayoutObjectLayoutIFrame); }
    bool isLayoutImage() const { return isOfType(LayoutObjectLayoutImage); }
    bool isLayoutMultiColumnSet() const { return isOfType(LayoutObjectLayoutMultiColumnSet); }
    bool isLayoutMultiColumnSpannerPlaceholder() const { return isOfType(LayoutObjectLayoutMultiColumnSpannerPlaceholder); }
    bool isLayoutScrollbarPart() const { return isOfType(LayoutObjectLayoutScrollbarPart); }
    bool isLayoutView() const { return isOfType(LayoutObjectLayoutView); }
    bool isReplica() const { return isOfType(LayoutObjectReplica); }
    bool isRuby() const { return isOfType(LayoutObjectRuby); }
    bool isRubyBase() const { return isOfType(LayoutObjectRubyBase); }
    bool isRubyRun() const { return isOfType(LayoutObjectRubyRun); }
    bool isRubyText() const { return isOfType(LayoutObjectRubyText); }
    bool isSlider() const { return isOfType(LayoutObjectSlider); }
    bool isSliderThumb() const { return isOfType(LayoutObjectSliderThumb); }
    bool isTable() const { return isOfType(LayoutObjectTable); }
    bool isTableCaption() const { return isOfType(LayoutObjectTableCaption); }
    bool isTableCell() const { return isOfType(LayoutObjectTableCell); }
    bool isTableRow() const { return isOfType(LayoutObjectTableRow); }
    bool isTableSection() const { return isOfType(LayoutObjectTableSection); }
    bool isTextArea() const { return isOfType(LayoutObjectTextArea); }
    bool isTextControl() const { return isOfType(LayoutObjectTextControl); }
    bool isTextField() const { return isOfType(LayoutObjectTextField); }
    bool isVideo() const { return isOfType(LayoutObjectVideo); }
    bool isWidget() const { return isOfType(LayoutObjectWidget); }

    virtual bool isImage() const { return false; }

    virtual bool isInlineBlockOrInlineTable() const { return false; }
    virtual bool isLayoutBlock() const { return false; }
    virtual bool isLayoutBlockFlow() const { return false; }
    virtual bool isLayoutFlowThread() const { return false; }
    virtual bool isLayoutInline() const { return false; }
    virtual bool isLayoutPart() const { return false; }

    bool isDocumentElement() const { return document().documentElement() == m_node; }
    // isBody is called from LayoutBox::styleWillChange and is thus quite hot.
    bool isBody() const { return node() && node()->hasTagName(HTMLNames::bodyTag); }
    bool isHR() const;
    bool isLegend() const;

    bool isTablePart() const { return isTableCell() || isLayoutTableCol() || isTableCaption() || isTableRow() || isTableSection(); }

    inline bool isBeforeContent() const;
    inline bool isAfterContent() const;
    inline bool isBeforeOrAfterContent() const;
    static inline bool isAfterContent(const LayoutObject* obj) { return obj && obj->isAfterContent(); }

    bool hasCounterNodeMap() const { return m_bitfields.hasCounterNodeMap(); }
    void setHasCounterNodeMap(bool hasCounterNodeMap) { m_bitfields.setHasCounterNodeMap(hasCounterNodeMap); }
    bool everHadLayout() const { return m_bitfields.everHadLayout(); }

    bool childrenInline() const { return m_bitfields.childrenInline(); }
    void setChildrenInline(bool b) { m_bitfields.setChildrenInline(b); }

    bool alwaysCreateLineBoxesForLayoutInline() const
    {
        ASSERT(isLayoutInline());
        return m_bitfields.alwaysCreateLineBoxesForLayoutInline();
    }
    void setAlwaysCreateLineBoxesForLayoutInline(bool alwaysCreateLineBoxes)
    {
        ASSERT(isLayoutInline());
        m_bitfields.setAlwaysCreateLineBoxesForLayoutInline(alwaysCreateLineBoxes);
    }

    bool ancestorLineBoxDirty() const { return m_bitfields.ancestorLineBoxDirty(); }
    void setAncestorLineBoxDirty(bool value = true)
    {
        m_bitfields.setAncestorLineBoxDirty(value);
        if (value)
            setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::LineBoxesChanged);
    }

    void setIsInsideFlowThreadIncludingDescendants(bool);

    bool isInsideFlowThread() const { return m_bitfields.isInsideFlowThread(); }
    void setIsInsideFlowThread(bool insideFlowThread) { m_bitfields.setIsInsideFlowThread(insideFlowThread); }

    // FIXME: Until all SVG layoutObjects can be subclasses of LayoutSVGModelObject we have
    // to add SVG layoutObject methods to LayoutObject with an ASSERT_NOT_REACHED() default implementation.
    bool isSVG() const { return isOfType(LayoutObjectSVG); }
    bool isSVGRoot() const { return isOfType(LayoutObjectSVGRoot); }
    bool isSVGContainer() const { return isOfType(LayoutObjectSVGContainer); }
    bool isSVGTransformableContainer() const { return isOfType(LayoutObjectSVGTransformableContainer); }
    bool isSVGViewportContainer() const { return isOfType(LayoutObjectSVGViewportContainer); }
    bool isSVGGradientStop() const { return isOfType(LayoutObjectSVGGradientStop); }
    bool isSVGHiddenContainer() const { return isOfType(LayoutObjectSVGHiddenContainer); }
    bool isSVGShape() const { return isOfType(LayoutObjectSVGShape); }
    bool isSVGText() const { return isOfType(LayoutObjectSVGText); }
    bool isSVGTextPath() const { return isOfType(LayoutObjectSVGTextPath); }
    bool isSVGInline() const { return isOfType(LayoutObjectSVGInline); }
    bool isSVGInlineText() const { return isOfType(LayoutObjectSVGInlineText); }
    bool isSVGImage() const { return isOfType(LayoutObjectSVGImage); }
    bool isSVGForeignObject() const { return isOfType(LayoutObjectSVGForeignObject); }
    bool isSVGResourceContainer() const { return isOfType(LayoutObjectSVGResourceContainer); }
    bool isSVGResourceFilter() const { return isOfType(LayoutObjectSVGResourceFilter); }
    bool isSVGResourceFilterPrimitive() const { return isOfType(LayoutObjectSVGResourceFilterPrimitive); }

    // FIXME: Those belong into a SVG specific base-class for all layoutObjects (see above)
    // Unfortunately we don't have such a class yet, because it's not possible for all layoutObjects
    // to inherit from LayoutSVGObject -> LayoutObject (some need LayoutBlock inheritance for instance)
    virtual void setNeedsTransformUpdate() { }
    virtual void setNeedsBoundariesUpdate();

    bool isBlendingAllowed() const { return !isSVG() || (isSVGContainer() && !isSVGHiddenContainer()) || isSVGShape() || isSVGImage() || isSVGText(); }
    virtual bool hasNonIsolatedBlendingDescendants() const { return false; }
    enum DescendantIsolationState {
        DescendantIsolationRequired,
        DescendantIsolationNeedsUpdate,
    };
    virtual void descendantIsolationRequirementsChanged(DescendantIsolationState) { }

    // Per SVG 1.1 objectBoundingBox ignores clipping, masking, filter effects, opacity and stroke-width.
    // This is used for all computation of objectBoundingBox relative units and by SVGLocatable::getBBox().
    // NOTE: Markers are not specifically ignored here by SVG 1.1 spec, but we ignore them
    // since stroke-width is ignored (and marker size can depend on stroke-width).
    // objectBoundingBox is returned local coordinates.
    // The name objectBoundingBox is taken from the SVG 1.1 spec.
    virtual FloatRect objectBoundingBox() const;
    virtual FloatRect strokeBoundingBox() const;

    // Returns the smallest rectangle enclosing all of the painted content
    // respecting clipping, masking, filters, opacity, stroke-width and markers
    virtual FloatRect paintInvalidationRectInLocalCoordinates() const;

    // This only returns the transform="" value from the element
    // most callsites want localToParentTransform() instead.
    virtual AffineTransform localTransform() const;

    // Returns the full transform mapping from local coordinates to local coords for the parent SVG layoutObject
    // This includes any viewport transforms and x/y offsets as well as the transform="" value off the element.
    virtual const AffineTransform& localToParentTransform() const;

    // SVG uses FloatPoint precise hit testing, and passes the point in parent
    // coordinates instead of in paint invalidaiton container coordinates. Eventually the
    // rest of the layout tree will move to a similar model.
    virtual bool nodeAtFloatPoint(HitTestResult&, const FloatPoint& pointInParent, HitTestAction);

    bool isAnonymous() const { return m_bitfields.isAnonymous(); }
    bool isAnonymousBlock() const
    {
        // This function is kept in sync with anonymous block creation conditions in
        // LayoutBlock::createAnonymousBlock(). This includes creating an anonymous
        // LayoutBlock having a BLOCK or BOX display. Other classes such as LayoutTextFragment
        // are not LayoutBlocks and will return false. See https://bugs.webkit.org/show_bug.cgi?id=56709.
        return isAnonymous() && (style()->display() == BLOCK || style()->display() == BOX) && style()->styleType() == NOPSEUDO && isLayoutBlock() && !isListMarker() && !isLayoutFlowThread() && !isLayoutMultiColumnSet()
            && !isLayoutFullScreen()
            && !isLayoutFullScreenPlaceholder();
    }
    bool isElementContinuation() const { return node() && node()->layoutObject() != this; }
    bool isInlineElementContinuation() const { return isElementContinuation() && isInline(); }
    virtual LayoutBoxModelObject* virtualContinuation() const { return nullptr; }

    bool isFloating() const { return m_bitfields.floating(); }

    bool isOutOfFlowPositioned() const { return m_bitfields.isOutOfFlowPositioned(); } // absolute or fixed positioning
    bool isRelPositioned() const { return m_bitfields.isRelPositioned(); } // relative positioning
    bool isPositioned() const { return m_bitfields.isPositioned(); }

    bool isText() const  { return m_bitfields.isText(); }
    bool isBox() const { return m_bitfields.isBox(); }
    bool isInline() const { return m_bitfields.isInline(); } // inline object
    bool isDragging() const { return m_bitfields.isDragging(); }
    bool isReplaced() const { return m_bitfields.isReplaced(); } // a "replaced" element (see CSS)
    bool isHorizontalWritingMode() const { return m_bitfields.horizontalWritingMode(); }
    bool hasFlippedBlocksWritingMode() const
    {
        return style()->isFlippedBlocksWritingMode();
    }

    bool hasLayer() const { return m_bitfields.hasLayer(); }

    // "Box decoration background" includes all box decorations and backgrounds
    // that are painted as the background of the object. It includes borders,
    // box-shadows, background-color and background-image, etc.
    enum BoxDecorationBackgroundState {
        NoBoxDecorationBackground,
        HasBoxDecorationBackgroundObscurationStatusInvalid,
        HasBoxDecorationBackgroundKnownToBeObscured,
        HasBoxDecorationBackgroundMayBeVisible,
    };
    bool hasBoxDecorationBackground() const { return m_bitfields.boxDecorationBackgroundState() != NoBoxDecorationBackground; }
    bool boxDecorationBackgroundIsKnownToBeObscured();
    bool mustInvalidateFillLayersPaintOnHeightChange(const FillLayer&) const;
    bool hasBackground() const { return style()->hasBackground(); }

    bool needsLayoutBecauseOfChildren() const { return needsLayout() && !selfNeedsLayout() && !needsPositionedMovementLayout() && !needsSimplifiedNormalFlowLayout(); }

    bool needsLayout() const
    {
        return m_bitfields.selfNeedsLayout() || m_bitfields.normalChildNeedsLayout() || m_bitfields.posChildNeedsLayout()
            || m_bitfields.needsSimplifiedNormalFlowLayout() || m_bitfields.needsPositionedMovementLayout();
    }

    //bool selfNeedsLayout() const { return m_bitfields.selfNeedsLayout(); }
    bool selfNeedsLayout() const;
    bool needsPositionedMovementLayout() const { return m_bitfields.needsPositionedMovementLayout(); }
    bool needsPositionedMovementLayoutOnly() const
    {
        return m_bitfields.needsPositionedMovementLayout() && !m_bitfields.selfNeedsLayout() && !m_bitfields.normalChildNeedsLayout()
            && !m_bitfields.posChildNeedsLayout() && !m_bitfields.needsSimplifiedNormalFlowLayout();
    }

    bool posChildNeedsLayout() const { return m_bitfields.posChildNeedsLayout(); }
    bool needsSimplifiedNormalFlowLayout() const { return m_bitfields.needsSimplifiedNormalFlowLayout(); }
    //bool normalChildNeedsLayout() const { return m_bitfields.normalChildNeedsLayout(); }
    bool normalChildNeedsLayout() const;

    bool preferredLogicalWidthsDirty() const { return m_bitfields.preferredLogicalWidthsDirty(); }

    bool needsOverflowRecalcAfterStyleChange() const { return m_bitfields.selfNeedsOverflowRecalcAfterStyleChange() || m_bitfields.childNeedsOverflowRecalcAfterStyleChange(); }
    bool selfNeedsOverflowRecalcAfterStyleChange() const { return m_bitfields.selfNeedsOverflowRecalcAfterStyleChange(); }
    bool childNeedsOverflowRecalcAfterStyleChange() const { return m_bitfields.childNeedsOverflowRecalcAfterStyleChange(); }

    bool isSelectionBorder() const;

    bool hasClip() const { return isOutOfFlowPositioned() && !style()->hasAutoClip(); }
    bool hasOverflowClip() const { return m_bitfields.hasOverflowClip(); }
    bool hasClipOrOverflowClip() const { return hasClip() || hasOverflowClip(); }

    bool hasTransformRelatedProperty() const { return m_bitfields.hasTransformRelatedProperty(); }
    bool hasMask() const { return style() && style()->hasMask(); }
    bool hasClipPath() const { return style() && style()->clipPath(); }
    bool hasHiddenBackface() const { return style() && style()->backfaceVisibility() == BackfaceVisibilityHidden; }

    bool hasFilter() const { return style() && style()->hasFilter(); }

    bool hasShapeOutside() const { return style() && style()->shapeOutside(); }

    inline bool preservesNewline() const;

    // The pseudo element style can be cached or uncached.  Use the cached method if the pseudo element doesn't respect
    // any pseudo classes (and therefore has no concept of changing state).
    ComputedStyle* getCachedPseudoStyle(PseudoId, const ComputedStyle* parentStyle = nullptr) const;
    PassRefPtr<ComputedStyle> getUncachedPseudoStyle(const PseudoStyleRequest&, const ComputedStyle* parentStyle = nullptr, const ComputedStyle* ownStyle = nullptr) const;

    virtual void updateDragState(bool dragOn);

    LayoutView* view() const { return document().layoutView(); }
    FrameView* frameView() const { return document().view(); }

    bool isRooted() const;

    Node* node() const
    {
        return isAnonymous() ? 0 : m_node;
    }

    Node* nonPseudoNode() const
    {
        return isPseudoElement() ? 0 : node();
    }

    void clearNode() { m_node = nullptr; }

    // Returns the styled node that caused the generation of this layoutObject.
    // This is the same as node() except for layoutObjects of :before, :after and
    // :first-letter pseudo elements for which their parent node is returned.
    Node* generatingNode() const { return isPseudoElement() ? node()->parentOrShadowHostNode() : node(); }

    Document& document() const { return m_node->document(); }
    LocalFrame* frame() const { return document().frame(); }

    virtual LayoutMultiColumnSpannerPlaceholder* spannerPlaceholder() const { return nullptr; }
    bool isColumnSpanAll() const { return style()->columnSpan() == ColumnSpanAll && spannerPlaceholder(); }

    // Returns the object containing this one. Can be different from parent for positioned elements.
    // If paintInvalidationContainer and paintInvalidationContainerSkipped are not null, on return *paintInvalidationContainerSkipped
    // is true if the layoutObject returned is an ancestor of paintInvalidationContainer.
    LayoutObject* container(const LayoutBoxModelObject* paintInvalidationContainer = nullptr, bool* paintInvalidationContainerSkipped = nullptr) const;
    LayoutObject* containerCrossingFrameBoundaries() const;
    LayoutBlock* containerForFixedPosition(const LayoutBoxModelObject* paintInvalidationContainer = nullptr, bool* paintInvalidationContainerSkipped = nullptr) const;
    LayoutBlock* containingBlockForAbsolutePosition() const;

    virtual LayoutObject* hoverAncestor() const { return parent(); }

    Element* offsetParent() const;

    void markContainerChainForLayout(bool scheduleRelayout = true, SubtreeLayoutScope* = nullptr);
    void setNeedsLayout(LayoutInvalidationReasonForTracing, MarkingBehavior = MarkContainerChain, SubtreeLayoutScope* = nullptr);
    void setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReasonForTracing, MarkingBehavior = MarkContainerChain, SubtreeLayoutScope* = nullptr);
    void clearNeedsLayout();
    void setChildNeedsLayout(MarkingBehavior = MarkContainerChain, SubtreeLayoutScope* = nullptr);
    void setNeedsPositionedMovementLayout();
    void setPreferredLogicalWidthsDirty(MarkingBehavior = MarkContainerChain);
    void clearPreferredLogicalWidthsDirty();

    void setNeedsLayoutAndPrefWidthsRecalc(LayoutInvalidationReasonForTracing reason)
    {
        setNeedsLayout(reason);
        setPreferredLogicalWidthsDirty();
    }
    void setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReasonForTracing reason)
    {
        setNeedsLayoutAndFullPaintInvalidation(reason);
        setPreferredLogicalWidthsDirty();
    }

    void setPositionState(EPosition position)
    {
        ASSERT((position != AbsolutePosition && position != FixedPosition) || isBox());
        m_bitfields.setPositionedState(position);
    }
    void clearPositionedState() { m_bitfields.clearPositionedState(); }

    void setFloating(bool isFloating) { m_bitfields.setFloating(isFloating); }
    void setInline(bool isInline) { m_bitfields.setIsInline(isInline); }

    void setHasBoxDecorationBackground(bool);
    void invalidateBackgroundObscurationStatus();
    virtual bool computeBackgroundIsKnownToBeObscured() { return false; }

    void setIsText() { m_bitfields.setIsText(true); }
    void setIsBox() { m_bitfields.setIsBox(true); }
    void setReplaced(bool isReplaced) { m_bitfields.setIsReplaced(isReplaced); }
    void setHorizontalWritingMode(bool hasHorizontalWritingMode) { m_bitfields.setHorizontalWritingMode(hasHorizontalWritingMode); }
    void setHasOverflowClip(bool hasOverflowClip) { m_bitfields.setHasOverflowClip(hasOverflowClip); }
    void setHasLayer(bool hasLayer) { m_bitfields.setHasLayer(hasLayer); }
    void setHasTransformRelatedProperty(bool hasTransform) { m_bitfields.setHasTransformRelatedProperty(hasTransform); }
    void setHasReflection(bool hasReflection) { m_bitfields.setHasReflection(hasReflection); }

    // paintOffset is the offset from the origin of the GraphicsContext at which to paint the current object.
    virtual void paint(const PaintInfo&, const LayoutPoint& paintOffset);

    // Subclasses must reimplement this method to compute the size and position
    // of this object and all its descendants.
    virtual void layout() = 0;
    virtual bool updateImageLoadingPriorities() { return false; }
    void setHasPendingResourceUpdate(bool hasPendingResourceUpdate) { m_bitfields.setHasPendingResourceUpdate(hasPendingResourceUpdate); }
    bool hasPendingResourceUpdate() const { return m_bitfields.hasPendingResourceUpdate(); }

    void handleSubtreeModifications();
    virtual void subtreeDidChange() { }

    // Flags used to mark if an object consumes subtree change notifications.
    bool consumesSubtreeChangeNotification() const { return m_bitfields.consumesSubtreeChangeNotification(); }
    void setConsumesSubtreeChangeNotification() { m_bitfields.setConsumesSubtreeChangeNotification(true); }

    // Flags used to mark if a descendant subtree of this object has changed.
    void notifyOfSubtreeChange();
    void notifyAncestorsOfSubtreeChange();
    bool wasNotifiedOfSubtreeChange() const { return m_bitfields.notifiedOfSubtreeChange(); }

    // Flags used to signify that a layoutObject needs to be notified by its descendants that they have
    // had their child subtree changed.
    void registerSubtreeChangeListenerOnDescendants(bool);
    bool hasSubtreeChangeListenerRegistered() const { return m_bitfields.subtreeChangeListenerRegistered(); }

    /* This function performs a layout only if one is needed. */
    void layoutIfNeeded()
    {
        if (needsLayout())
            layout();
    }

    void forceLayout();
    void forceChildLayout();

    // Used for element state updates that cannot be fixed with a
    // paint invalidation and do not need a relayout.
    virtual void updateFromElement() { }

    virtual void addAnnotatedRegions(Vector<AnnotatedRegionValue>&);

    CompositingState compositingState() const;
    virtual CompositingReasons additionalCompositingReasons() const;

    bool hitTest(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestFilter = HitTestAll);
    virtual void updateHitTestResult(HitTestResult&, const LayoutPoint&);
    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction);

    virtual PositionWithAffinity positionForPoint(const LayoutPoint&);
    PositionWithAffinity createPositionWithAffinity(int offset, EAffinity);
    PositionWithAffinity createPositionWithAffinity(const Position&);

    virtual void dirtyLinesFromChangedChild(LayoutObject*);

    // Set the style of the object and update the state of the object accordingly.
    void setStyle(PassRefPtr<ComputedStyle>);

    // Set the style of the object if it's generated content.
    void setPseudoStyle(PassRefPtr<ComputedStyle>);

    // Updates only the local style ptr of the object.  Does not update the state of the object,
    // and so only should be called when the style is known not to have changed (or from setStyle).
    void setStyleInternal(PassRefPtr<ComputedStyle> style) { m_style = style; }

    void firstLineStyleDidChange(const ComputedStyle& oldStyle, const ComputedStyle& newStyle);

    // returns the containing block level element for this element.
    LayoutBlock* containingBlock() const;

    bool canContainFixedPositionObjects() const
    {
        return isLayoutView() || (hasTransformRelatedProperty() && isLayoutBlock()) || isSVGForeignObject();
    }

    // Convert the given local point to absolute coordinates
    // FIXME: Temporary. If UseTransforms is true, take transforms into account. Eventually localToAbsolute() will always be transform-aware.
    FloatPoint localToAbsolute(const FloatPoint& localPoint = FloatPoint(), MapCoordinatesFlags = 0) const;
    FloatPoint absoluteToLocal(const FloatPoint&, MapCoordinatesFlags = 0) const;

    // Convert a local quad to absolute coordinates, taking transforms into account.
    FloatQuad localToAbsoluteQuad(const FloatQuad& quad, MapCoordinatesFlags mode = 0, bool* wasFixed = nullptr) const
    {
        return localToContainerQuad(quad, 0, mode, wasFixed);
    }
    // Convert an absolute quad to local coordinates.
    FloatQuad absoluteToLocalQuad(const FloatQuad&, MapCoordinatesFlags mode = 0) const;

    // Convert a local quad into the coordinate system of container, taking transforms into account.
    FloatQuad localToContainerQuad(const FloatQuad&, const LayoutBoxModelObject* paintInvalidationContainer, MapCoordinatesFlags = 0, bool* wasFixed = nullptr) const;
    FloatPoint localToContainerPoint(const FloatPoint&, const LayoutBoxModelObject* paintInvalidationContainer, MapCoordinatesFlags = 0, bool* wasFixed = nullptr, const PaintInvalidationState* = nullptr) const;

    // Convert a local point into the coordinate system of backing coordinates. Also returns the backing layer if needed.
    FloatPoint localToInvalidationBackingPoint(const LayoutPoint&, DeprecatedPaintLayer** backingLayer = nullptr);

    // Return the offset from the container() layoutObject (excluding transforms). In multi-column layout,
    // different offsets apply at different points, so return the offset that applies to the given point.
    virtual LayoutSize offsetFromContainer(const LayoutObject*, const LayoutPoint&, bool* offsetDependsOnPoint = nullptr) const;
    // Return the offset from an object up the container() chain. Asserts that none of the intermediate objects have transforms.
    LayoutSize offsetFromAncestorContainer(const LayoutObject*) const;

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint&) const { }

    FloatRect absoluteBoundingBoxFloatRect() const;
    // This returns an IntRect enclosing this object. If this object has an
    // integral size and the position has fractional values, the resultant
    // IntRect can be larger than the integral size.
    IntRect absoluteBoundingBoxRect() const;
    // FIXME: This function should go away eventually
    IntRect absoluteBoundingBoxRectIgnoringTransforms() const;

    // Build an array of quads in absolute coords for line boxes
    virtual void absoluteQuads(Vector<FloatQuad>&, bool* /* wasFixed */ = nullptr) const { }

    virtual IntRect absoluteFocusRingBoundingBoxRect() const;

    static FloatRect absoluteBoundingBoxRectForRange(const Range*);

    // the rect that will be painted if this object is passed as the paintingRoot
    IntRect paintingRootRect(IntRect& topLevelRect);

    virtual LayoutUnit minPreferredLogicalWidth() const { return 0; }
    virtual LayoutUnit maxPreferredLogicalWidth() const { return 0; }

    const ComputedStyle* style() const { return m_style.get(); }
    ComputedStyle* mutableStyle() const { return m_style.get(); }

    // m_style can only be nullptr before the first style is set, thus most
    // callers will never see a nullptr style and should use styleRef().
    // FIXME: It would be better if style() returned a const reference.
    const ComputedStyle& styleRef() const { return mutableStyleRef(); }
    ComputedStyle& mutableStyleRef() const { ASSERT(m_style); return *m_style; }

    /* The following methods are inlined in LayoutObjectInlines.h */
    const ComputedStyle* firstLineStyle() const;
    const ComputedStyle& firstLineStyleRef() const;
    const ComputedStyle* style(bool firstLine) const;
    const ComputedStyle& styleRef(bool firstLine) const;

    static inline Color resolveColor(const ComputedStyle& styleToUse, int colorProperty)
    {
        return styleToUse.visitedDependentColor(colorProperty);
    }

    inline Color resolveColor(int colorProperty) const
    {
        return style()->visitedDependentColor(colorProperty);
    }

    // Used only by Element::pseudoStyleCacheIsInvalid to get a first line style based off of a
    // given new style, without accessing the cache.
    PassRefPtr<ComputedStyle> uncachedFirstLineStyle(ComputedStyle*) const;

    virtual CursorDirective getCursor(const LayoutPoint&, Cursor&) const;

    struct AppliedTextDecoration {
        Color color;
        TextDecorationStyle style;
        AppliedTextDecoration() : color(Color::transparent), style(TextDecorationStyleSolid) { }
    };

    void getTextDecorations(unsigned decorations, AppliedTextDecoration& underline, AppliedTextDecoration& overline, AppliedTextDecoration& linethrough, bool quirksMode = false, bool firstlineStyle = false);

    // Return the LayoutBoxModelObject in the container chain which is responsible for painting this object, or layout view
    // if painting is root-relative. This is the container that should be passed to the 'forPaintInvalidation'
    // methods.
    const LayoutBoxModelObject& containerForPaintInvalidationOnRootedTree() const;

    // This method will be deprecated in a long term, replaced by containerForPaintInvalidationOnRootedTree.
    const LayoutBoxModelObject* containerForPaintInvalidation() const;

    const LayoutBoxModelObject* adjustCompositedContainerForSpecialAncestors(const LayoutBoxModelObject* paintInvalidationContainer) const;
    bool isPaintInvalidationContainer() const;

    LayoutRect computePaintInvalidationRect()
    {
        return computePaintInvalidationRect(containerForPaintInvalidation());
    }

    // Returns the paint invalidation rect for this LayoutObject in the coordinate space of the paint backing (typically a GraphicsLayer) for |paintInvalidationContainer|.
    LayoutRect computePaintInvalidationRect(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const;

    // Returns the rect bounds needed to invalidate the paint of this object, in the coordinate space of the layoutObject backing of |paintInvalidationContainer|
    LayoutRect boundsRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const;

    // Actually do the paint invalidate of rect r for this object which has been computed in the coordinate space
    // of the GraphicsLayer backing of |paintInvalidationContainer|. Note that this coordinaten space is not the same
    // as the local coordinate space of |paintInvalidationContainer| in the presence of layer squashing.
    void invalidatePaintUsingContainer(const LayoutBoxModelObject& paintInvalidationContainer, const LayoutRect&, PaintInvalidationReason) const;

    // Invalidate the paint of a specific subrectangle within a given object. The rect |r| is in the object's coordinate space.
    void invalidatePaintRectangle(const LayoutRect&) const;
    void invalidatePaintRectangleNotInvalidatingDisplayItemClients(const LayoutRect& r) const { invalidatePaintRectangleInternal(r); }

    // Walk the tree after layout issuing paint invalidations for layoutObjects that have changed or moved, updating bounds that have changed, and clearing paint invalidation state.
    virtual void invalidateTreeIfNeeded(PaintInvalidationState&);

    virtual void invalidatePaintForOverflow();
    void invalidatePaintForOverflowIfNeeded();

    void invalidatePaintIncludingNonCompositingDescendants();
    void setShouldDoFullPaintInvalidationIncludingNonCompositingDescendants();

    // Returns the rect that should have paint invalidated whenever this object changes. The rect is in the view's
    // coordinate space. This method deals with outlines and overflow.
    virtual LayoutRect absoluteClippedOverflowRect() const;
    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const;
    virtual LayoutRect rectWithOutlineForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, LayoutUnit outlineWidth, const PaintInvalidationState* = nullptr) const;

    // Given a rect in the object's coordinate space, compute a rect suitable for invalidating paints of
    // that rect in the coordinate space of paintInvalidationContainer.
    virtual void mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&, const PaintInvalidationState*) const;

    // Return the offset to the column in which the specified point (in flow-thread coordinates)
    // lives. This is used to convert a flow-thread point to a visual point.
    virtual LayoutSize columnOffset(const LayoutPoint&) const { return LayoutSize(); }

    virtual unsigned length() const { return 1; }

    bool isFloatingOrOutOfFlowPositioned() const { return (isFloating() || isOutOfFlowPositioned()); }

    bool isTransparent() const { return style()->hasOpacity(); }
    float opacity() const { return style()->opacity(); }

    bool hasReflection() const { return m_bitfields.hasReflection(); }

    enum SelectionState {
        SelectionNone, // The object is not selected.
        SelectionStart, // The object either contains the start of a selection run or is the start of a run
        SelectionInside, // The object is fully encompassed by a selection run
        SelectionEnd, // The object either contains the end of a selection run or is the end of a run
        SelectionBoth // The object contains an entire run or is the sole selected object in that run
    };

    // The current selection state for an object.  For blocks, the state refers to the state of the leaf
    // descendants (as described above in the SelectionState enum declaration).
    SelectionState selectionState() const { return m_bitfields.selectionState(); }
    virtual void setSelectionState(SelectionState state) { m_bitfields.setSelectionState(state); }
    inline void setSelectionStateIfNeeded(SelectionState);
    bool canUpdateSelectionOnRootLineBoxes() const;

    // A single rectangle that encompasses all of the selected objects within this object.  Used to determine the tightest
    // possible bounding box for the selection. The rect returned is in the coordinate space of the paint invalidation container's backing.
    virtual LayoutRect selectionRectForPaintInvalidation(const LayoutBoxModelObject* /*paintInvalidationContainer*/) const { return LayoutRect(); }

    // View coordinates means the coordinate space of |view()|.
    LayoutRect selectionRectInViewCoordinates() const;

    virtual bool canBeSelectionLeaf() const { return false; }
    bool hasSelectedChildren() const { return selectionState() != SelectionNone; }

    bool isSelectable() const;
    // Obtains the selection colors that should be used when painting a selection.
    Color selectionBackgroundColor() const;
    Color selectionForegroundColor() const;
    Color selectionEmphasisMarkColor() const;

    // Whether or not a given block needs to paint selection gaps.
    virtual bool shouldPaintSelectionGaps() const { return false; }

    /**
     * Returns the local coordinates of the caret within this layout object.
     * @param caretOffset zero-based offset determining position within the layout object.
     * @param extraWidthToEndOfLine optional out arg to give extra width to end of line -
     * useful for character range rect computations
     */
    virtual LayoutRect localCaretRect(InlineBox*, int caretOffset, LayoutUnit* extraWidthToEndOfLine = nullptr);

    // When performing a global document tear-down, the layoutObject of the document is cleared. We use this
    // as a hook to detect the case of document destruction and don't waste time doing unnecessary work.
    bool documentBeingDestroyed() const;

    void destroyAndCleanupAnonymousWrappers();

    // While the destroy() method is virtual, this should only be overriden in very rare circumstances.
    // You want to override willBeDestroyed() instead unless you explicitly need to stop this object
    // from being destroyed (for example, LayoutPart overrides destroy() for this purpose).
    virtual void destroy();

    // Virtual function helpers for the deprecated Flexible Box Layout (display: -webkit-box).
    virtual bool isDeprecatedFlexibleBox() const { return false; }

    // Virtual function helper for the new FlexibleBox Layout (display: -webkit-flex).
    virtual bool isFlexibleBox() const { return false; }

    bool isFlexibleBoxIncludingDeprecated() const
    {
        return isFlexibleBox() || isDeprecatedFlexibleBox();
    }

    virtual bool isCombineText() const { return false; }

    virtual int caretMinOffset() const;
    virtual int caretMaxOffset() const;

    virtual int previousOffset(int current) const;
    virtual int previousOffsetForBackwardDeletion(int current) const;
    virtual int nextOffset(int current) const;

    virtual void imageChanged(ImageResource*, const IntRect* = nullptr) override final;
    virtual void imageChanged(WrappedImagePtr, const IntRect* = nullptr) { }
    virtual bool willRenderImage(ImageResource*) override final;
    virtual bool getImageAnimationPolicy(ImageResource*, ImageAnimationPolicy&) override final;

    void selectionStartEnd(int& spos, int& epos) const;

    void remove()
    {
        if (parent())
            parent()->removeChild(this);
    }

    bool visibleToHitTestRequest(const HitTestRequest& request) const { return style()->visibility() == VISIBLE && (request.ignorePointerEventsNone() || style()->pointerEvents() != PE_NONE) && !isInert(); }

    bool visibleToHitTesting() const { return style()->visibility() == VISIBLE && style()->pointerEvents() != PE_NONE && !isInert(); }

    // Map points and quads through elements, potentially via 3d transforms. You should never need to call these directly; use
    // localToAbsolute/absoluteToLocal methods instead.
    virtual void mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState&, MapCoordinatesFlags = ApplyContainerFlip, bool* wasFixed = nullptr, const PaintInvalidationState* = nullptr) const;
    virtual void mapAbsoluteToLocalPoint(MapCoordinatesFlags, TransformState&) const;

    // Pushes state onto LayoutGeometryMap about how to map coordinates from this layoutObject to its container, or ancestorToStopAt (whichever is encountered first).
    // Returns the layoutObject which was mapped to (container or ancestorToStopAt).
    virtual const LayoutObject* pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap&) const;

    bool shouldUseTransformFromContainer(const LayoutObject* container) const;
    void getTransformFromContainer(const LayoutObject* container, const LayoutSize& offsetInContainer, TransformationMatrix&) const;

    bool createsGroup() const { return isTransparent() || hasMask() || hasFilter() || style()->hasBlendMode(); }

    virtual void addFocusRingRects(Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const { }

    // Compute a list of hit-test rectangles per layer rooted at this layoutObject.
    virtual void computeLayerHitTestRects(LayerHitTestRects&) const;

    RespectImageOrientationEnum shouldRespectImageOrientation() const;

    bool isRelayoutBoundaryForInspector() const;

    // The previous paint invalidation rect in the object's previous paint backing.
    const LayoutRect& previousPaintInvalidationRect() const { return m_previousPaintInvalidationRect; }
    void setPreviousPaintInvalidationRect(const LayoutRect& rect) { m_previousPaintInvalidationRect = rect; }

    // The previous position of the top-left corner of the object in its previous paint backing.
    const LayoutPoint& previousPositionFromPaintInvalidationBacking() const { return m_previousPositionFromPaintInvalidationBacking; }
    void setPreviousPositionFromPaintInvalidationBacking(const LayoutPoint& positionFromPaintInvalidationBacking) { m_previousPositionFromPaintInvalidationBacking = positionFromPaintInvalidationBacking; }

    PaintInvalidationReason fullPaintInvalidationReason() const { return m_bitfields.fullPaintInvalidationReason(); }
    bool shouldDoFullPaintInvalidation() const { return m_bitfields.fullPaintInvalidationReason() != PaintInvalidationNone; }
    void setShouldDoFullPaintInvalidation(PaintInvalidationReason = PaintInvalidationFull);
    void clearShouldDoFullPaintInvalidation() { m_bitfields.setFullPaintInvalidationReason(PaintInvalidationNone); }

    bool shouldInvalidateOverflowForPaint() const { return m_bitfields.shouldInvalidateOverflowForPaint(); }

    virtual void clearPaintInvalidationState(const PaintInvalidationState&);

    // Indicates whether this layout object was re-laid-out since the last frame.
    // The flag will be cleared during invalidateTreeIfNeeded.
    bool layoutDidGetCalledSinceLastFrame() const { return m_bitfields.layoutDidGetCalledSinceLastFrame(); }

    bool mayNeedPaintInvalidation() const { return m_bitfields.mayNeedPaintInvalidation(); }
    void setMayNeedPaintInvalidation();

    bool shouldInvalidateSelection() const { return m_bitfields.shouldInvalidateSelection(); }
    void setShouldInvalidateSelection();
    void clearShouldInvalidateSelection() { m_bitfields.setShouldInvalidateSelection(false); }

    bool neededLayoutBecauseOfChildren() const { return m_bitfields.neededLayoutBecauseOfChildren(); }
    void setNeededLayoutBecauseOfChildren(bool b) { m_bitfields.setNeededLayoutBecauseOfChildren(b); }

    bool shouldCheckForPaintInvalidation(const PaintInvalidationState& paintInvalidationState) const
    {
        // Should check for paint invalidation if some ancestor changed location, because this object
        // may also change paint offset or location in paint invalidation container, even if there is
        // no paint invalidation flag set.
        return paintInvalidationState.ancestorHadPaintInvalidationForLocationChange() || shouldCheckForPaintInvalidationRegardlessOfPaintInvalidationState();
    }

    bool shouldCheckForPaintInvalidationRegardlessOfPaintInvalidationState() const
    {
        return layoutDidGetCalledSinceLastFrame() || mayNeedPaintInvalidation() || shouldDoFullPaintInvalidation() || shouldInvalidateSelection();
    }

    virtual bool supportsPaintInvalidationStateCachedOffsets() const { return !hasTransformRelatedProperty() && !hasReflection() && !style()->isFlippedBlocksWritingMode(); }

    virtual LayoutRect viewRect() const;

    DisplayItemClient displayItemClient() const { return toDisplayItemClient(this); }

    void invalidateDisplayItemClient(const DisplayItemClientWrapper&) const;
    void invalidateDisplayItemClientForNonCompositingDescendants() const { invalidateDisplayItemClientForNonCompositingDescendantsOf(*this); }
    // A normal object should use invalidateDisplayItemClientForNonCompositingDescendants()
    // to invalidate its descendants which are painted on the same backing. However, for
    // an object (e.g. LayoutScrollbarPart, custom scroll corner, custom resizer) which is
    // not hooked up in the layout tree and not able to find its paint backing, it should
    // let its owning layout object call the following function.
    // FIXME: should we hook up scrollbar parts in the layout tree? crbug.com/484263.
    void invalidateDisplayItemClientForNonCompositingDescendantsOf(const LayoutObject&) const;

protected:
    enum LayoutObjectType {
        LayoutObjectBr,
        LayoutObjectCanvas,
        LayoutObjectFieldset,
        LayoutObjectCounter,
        LayoutObjectDetailsMarker,
        LayoutObjectEmbeddedObject,
        LayoutObjectFileUploadControl,
        LayoutObjectFrame,
        LayoutObjectFrameSet,
        LayoutObjectLayoutTableCol,
        LayoutObjectListBox,
        LayoutObjectListItem,
        LayoutObjectListMarker,
        LayoutObjectMedia,
        LayoutObjectMenuList,
        LayoutObjectMeter,
        LayoutObjectProgress,
        LayoutObjectQuote,
        LayoutObjectLayoutButton,
        LayoutObjectLayoutFlowThread,
        LayoutObjectLayoutFullScreen,
        LayoutObjectLayoutFullScreenPlaceholder,
        LayoutObjectLayoutGrid,
        LayoutObjectLayoutIFrame,
        LayoutObjectLayoutImage,
        LayoutObjectLayoutInline,
        LayoutObjectLayoutMultiColumnSet,
        LayoutObjectLayoutMultiColumnSpannerPlaceholder,
        LayoutObjectLayoutPart,
        LayoutObjectLayoutScrollbarPart,
        LayoutObjectLayoutView,
        LayoutObjectReplica,
        LayoutObjectRuby,
        LayoutObjectRubyBase,
        LayoutObjectRubyRun,
        LayoutObjectRubyText,
        LayoutObjectSlider,
        LayoutObjectSliderThumb,
        LayoutObjectTable,
        LayoutObjectTableCaption,
        LayoutObjectTableCell,
        LayoutObjectTableRow,
        LayoutObjectTableSection,
        LayoutObjectTextArea,
        LayoutObjectTextControl,
        LayoutObjectTextField,
        LayoutObjectVideo,
        LayoutObjectWidget,

        LayoutObjectSVG, /* Keep by itself? */
        LayoutObjectSVGRoot,
        LayoutObjectSVGContainer,
        LayoutObjectSVGTransformableContainer,
        LayoutObjectSVGViewportContainer,
        LayoutObjectSVGHiddenContainer,
        LayoutObjectSVGGradientStop,
        LayoutObjectSVGShape,
        LayoutObjectSVGText,
        LayoutObjectSVGTextPath,
        LayoutObjectSVGInline,
        LayoutObjectSVGInlineText,
        LayoutObjectSVGImage,
        LayoutObjectSVGForeignObject,
        LayoutObjectSVGResourceContainer,
        LayoutObjectSVGResourceFilter,
        LayoutObjectSVGResourceFilterPrimitive,
    };
    virtual bool isOfType(LayoutObjectType type) const { return false; }

    inline bool layerCreationAllowedForSubtree() const;

    // Overrides should call the superclass at the end. m_style will be 0 the first time
    // this function will be called.
    virtual void styleWillChange(StyleDifference, const ComputedStyle& newStyle);
    // Overrides should call the superclass at the start. |oldStyle| will be 0 the first
    // time this function is called.
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle);
    void propagateStyleToAnonymousChildren(bool blockChildrenOnly = false);
    virtual void updateAnonymousChildStyle(const LayoutObject& child, ComputedStyle& style) const { }

protected:
    void setSelfMayNeedPaintInvalidation();

    virtual void willBeDestroyed();

    virtual void insertedIntoTree();
    virtual void willBeRemovedFromTree();

    void setDocumentForAnonymous(Document* document) { ASSERT(isAnonymous()); m_node = document; }

    // Add hit-test rects for the layout tree rooted at this node to the provided collection on a
    // per-Layer basis.
    // currentLayer must be the enclosing layer, and layerOffset is the current offset within
    // this layer. Subclass implementations will add any offset for this layoutObject within it's
    // container, so callers should provide only the offset of the container within it's layer.
    // containerRect is a rect that has already been added for the currentLayer which is likely to
    // be a container for child elements. Any rect wholly contained by containerRect can be
    // skipped.
    virtual void addLayerHitTestRects(LayerHitTestRects&, const DeprecatedPaintLayer* currentLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const;

    // Add hit-test rects for this layoutObject only to the provided list. layerOffset is the offset
    // of this layoutObject within the current layer that should be used for each result.
    virtual void computeSelfHitTestRects(Vector<LayoutRect>&, const LayoutPoint& layerOffset) const { }

    virtual PaintInvalidationReason paintInvalidationReason(const LayoutBoxModelObject& paintInvalidationContainer,
        const LayoutRect& oldPaintInvalidationRect, const LayoutPoint& oldPositionFromPaintInvalidationBacking,
        const LayoutRect& newPaintInvalidationRect, const LayoutPoint& newPositionFromPaintInvalidationBacking) const;
    virtual void incrementallyInvalidatePaint(const LayoutBoxModelObject& paintInvalidationContainer, const LayoutRect& oldBounds, const LayoutRect& newBounds, const LayoutPoint& positionFromPaintInvalidationBacking);
    void fullyInvalidatePaint(const LayoutBoxModelObject& paintInvalidationContainer, PaintInvalidationReason, const LayoutRect& oldBounds, const LayoutRect& newBounds);

    virtual bool hasNonCompositedScrollbars() const { return false; }

#if ENABLE(ASSERT)
    virtual bool paintInvalidationStateIsDirty() const
    {
        return neededLayoutBecauseOfChildren() || shouldCheckForPaintInvalidationRegardlessOfPaintInvalidationState();
    }
#endif

    virtual void invalidatePaintOfSubtreesIfNeeded(PaintInvalidationState& childPaintInvalidationState);
    virtual PaintInvalidationReason invalidatePaintIfNeeded(PaintInvalidationState&, const LayoutBoxModelObject& paintInvalidationContainer);

    // When this object is invalidated for paint, this method is called to invalidate any DisplayItemClients
    // owned by this object, including the object itself, LayoutText/LayoutInline line boxes, etc.,
    // not including children which will be invalidated normally during invalidateTreeIfNeeded() and
    // parts which are invalidated separately (e.g. scrollbars).
    virtual void invalidateDisplayItemClients(const LayoutBoxModelObject& paintInvalidationContainer) const;

    void setIsSlowRepaintObject(bool);

private:
    void clearLayoutRootIfNeeded() const;

    bool isInert() const;

    void updateImage(StyleImage*, StyleImage*);

    void scheduleRelayout();

    void updateShapeImage(const ShapeValue*, const ShapeValue*);
    void updateFillImages(const FillLayer* oldLayers, const FillLayer& newLayers);

    void setNeedsOverflowRecalcAfterStyleChange();

    // FIXME: This should be 'markContaingBoxChainForOverflowRecalc when we make LayoutBox
    // recomputeOverflow-capable. crbug.com/437012 and crbug.com/434700.
    inline void markContainingBlocksForOverflowRecalc();

    inline void markContainerChainForPaintInvalidation();

    inline void invalidateSelectionIfNeeded(const LayoutBoxModelObject&, PaintInvalidationReason);

    inline void invalidateContainerPreferredLogicalWidths();

    void clearMayNeedPaintInvalidation();

    void setLayoutDidGetCalledSinceLastFrame();
    void clearLayoutDidGetCalledSinceLastFrame() { m_bitfields.setLayoutDidGetCalledSinceLastFrame(false); }

    void invalidatePaintIncludingNonCompositingDescendantsInternal(const LayoutBoxModelObject& repaintContainer);

    LayoutRect previousSelectionRectForPaintInvalidation() const;
    void setPreviousSelectionRectForPaintInvalidation(const LayoutRect&);

    LayoutObject* containerForAbsolutePosition(const LayoutBoxModelObject* paintInvalidationContainer = nullptr, bool* paintInvalidationContainerSkipped = nullptr) const;

    const LayoutBoxModelObject* enclosingCompositedContainer() const;

    LayoutFlowThread* locateFlowThreadContainingBlock() const;
    void removeFromLayoutFlowThread();
    void removeFromLayoutFlowThreadRecursive(LayoutFlowThread*);

    ComputedStyle* cachedFirstLineStyle() const;
    StyleDifference adjustStyleDifference(StyleDifference) const;

    Color selectionColor(int colorProperty) const;

    void removeShapeImageClient(ShapeValue*);

#if ENABLE(ASSERT)
    void checkBlockPositionedObjectsNeedLayout();
#endif

    bool isTextOrSVGChild() const { return isText() || (isSVG() && !isSVGRoot()); }

    static bool isAllowedToModifyLayoutTreeStructure(Document&);

    const LayoutBoxModelObject* invalidatePaintRectangleInternal(const LayoutRect&) const;

    RefPtr<ComputedStyle> m_style;

    // Oilpan: raw pointer back to the owning Node is considered safe.
    Node* m_node;

    LayoutObject* m_parent;
    LayoutObject* m_previous;
    LayoutObject* m_next;

#if ENABLE(ASSERT)
    unsigned m_hasAXObject             : 1;
    unsigned m_setNeedsLayoutForbidden : 1;
#endif

#define ADD_BOOLEAN_BITFIELD(name, Name) \
    private:\
        unsigned m_##name : 1;\
    public:\
        bool name() const { return m_##name; }\
        void set##Name(bool name) { m_##name = name; }\

    class LayoutObjectBitfields {
        enum PositionedState {
            IsStaticallyPositioned = 0,
            IsRelativelyPositioned = 1,
            IsOutOfFlowPositioned = 2,
        };

    public:
        LayoutObjectBitfields(Node* node)
            : m_selfNeedsLayout(false)
            , m_shouldInvalidateOverflowForPaint(false)
            // FIXME: We should remove mayNeedPaintInvalidation once we are able to
            // use the other layout flags to detect the same cases. crbug.com/370118
            , m_mayNeedPaintInvalidation(false)
            , m_shouldInvalidateSelection(false)
            , m_neededLayoutBecauseOfChildren(false)
            , m_needsPositionedMovementLayout(false)
            , m_normalChildNeedsLayout(false)
            , m_posChildNeedsLayout(false)
            , m_needsSimplifiedNormalFlowLayout(false)
            , m_preferredLogicalWidthsDirty(false)
            , m_floating(false)
            , m_selfNeedsOverflowRecalcAfterStyleChange(false)
            , m_childNeedsOverflowRecalcAfterStyleChange(false)
            , m_isAnonymous(!node)
            , m_isText(false)
            , m_isBox(false)
            , m_isInline(true)
            , m_isReplaced(false)
            , m_horizontalWritingMode(true)
            , m_isDragging(false)
            , m_hasLayer(false)
            , m_hasOverflowClip(false)
            , m_hasTransformRelatedProperty(false)
            , m_hasReflection(false)
            , m_hasCounterNodeMap(false)
            , m_everHadLayout(false)
            , m_ancestorLineBoxDirty(false)
            , m_layoutDidGetCalledSinceLastFrame(false)
            , m_hasPendingResourceUpdate(false)
            , m_isInsideFlowThread(false)
            , m_subtreeChangeListenerRegistered(false)
            , m_notifiedOfSubtreeChange(false)
            , m_consumesSubtreeChangeNotification(false)
            , m_childrenInline(false)
            , m_alwaysCreateLineBoxesForLayoutInline(false)
            , m_lastBoxDecorationBackgroundObscured(false)
            , m_isSlowRepaintObject(false)
            , m_positionedState(IsStaticallyPositioned)
            , m_selectionState(SelectionNone)
            , m_boxDecorationBackgroundState(NoBoxDecorationBackground)
            , m_fullPaintInvalidationReason(PaintInvalidationNone)
        {
        }

        // 32 bits have been used in the first word, and 16 in the second.
        ADD_BOOLEAN_BITFIELD(selfNeedsLayout, SelfNeedsLayout);
        ADD_BOOLEAN_BITFIELD(shouldInvalidateOverflowForPaint, ShouldInvalidateOverflowForPaint);
        ADD_BOOLEAN_BITFIELD(mayNeedPaintInvalidation, MayNeedPaintInvalidation);
        ADD_BOOLEAN_BITFIELD(shouldInvalidateSelection, ShouldInvalidateSelection);
        ADD_BOOLEAN_BITFIELD(neededLayoutBecauseOfChildren, NeededLayoutBecauseOfChildren);
        ADD_BOOLEAN_BITFIELD(needsPositionedMovementLayout, NeedsPositionedMovementLayout);
        ADD_BOOLEAN_BITFIELD(normalChildNeedsLayout, NormalChildNeedsLayout);
        ADD_BOOLEAN_BITFIELD(posChildNeedsLayout, PosChildNeedsLayout);
        ADD_BOOLEAN_BITFIELD(needsSimplifiedNormalFlowLayout, NeedsSimplifiedNormalFlowLayout);
        ADD_BOOLEAN_BITFIELD(preferredLogicalWidthsDirty, PreferredLogicalWidthsDirty);
        ADD_BOOLEAN_BITFIELD(floating, Floating);
        ADD_BOOLEAN_BITFIELD(selfNeedsOverflowRecalcAfterStyleChange, SelfNeedsOverflowRecalcAfterStyleChange);
        ADD_BOOLEAN_BITFIELD(childNeedsOverflowRecalcAfterStyleChange, ChildNeedsOverflowRecalcAfterStyleChange);

        ADD_BOOLEAN_BITFIELD(isAnonymous, IsAnonymous);
        ADD_BOOLEAN_BITFIELD(isText, IsText);
        ADD_BOOLEAN_BITFIELD(isBox, IsBox);
        ADD_BOOLEAN_BITFIELD(isInline, IsInline);
        ADD_BOOLEAN_BITFIELD(isReplaced, IsReplaced);
        ADD_BOOLEAN_BITFIELD(horizontalWritingMode, HorizontalWritingMode);
        ADD_BOOLEAN_BITFIELD(isDragging, IsDragging);

        ADD_BOOLEAN_BITFIELD(hasLayer, HasLayer);
        ADD_BOOLEAN_BITFIELD(hasOverflowClip, HasOverflowClip); // Set in the case of overflow:auto/scroll/hidden
        ADD_BOOLEAN_BITFIELD(hasTransformRelatedProperty, HasTransformRelatedProperty);
        ADD_BOOLEAN_BITFIELD(hasReflection, HasReflection);

        ADD_BOOLEAN_BITFIELD(hasCounterNodeMap, HasCounterNodeMap);
        ADD_BOOLEAN_BITFIELD(everHadLayout, EverHadLayout);
        ADD_BOOLEAN_BITFIELD(ancestorLineBoxDirty, AncestorLineBoxDirty);

        ADD_BOOLEAN_BITFIELD(layoutDidGetCalledSinceLastFrame, LayoutDidGetCalledSinceLastFrame);

        ADD_BOOLEAN_BITFIELD(hasPendingResourceUpdate, HasPendingResourceUpdate);

        ADD_BOOLEAN_BITFIELD(isInsideFlowThread, IsInsideFlowThread);

        ADD_BOOLEAN_BITFIELD(subtreeChangeListenerRegistered, SubtreeChangeListenerRegistered);
        ADD_BOOLEAN_BITFIELD(notifiedOfSubtreeChange, NotifiedOfSubtreeChange);
        ADD_BOOLEAN_BITFIELD(consumesSubtreeChangeNotification, ConsumesSubtreeChangeNotification);

        // from LayoutBlock
        ADD_BOOLEAN_BITFIELD(childrenInline, ChildrenInline);

        // from LayoutInline
        ADD_BOOLEAN_BITFIELD(alwaysCreateLineBoxesForLayoutInline, AlwaysCreateLineBoxesForLayoutInline);

        // For slimming-paint.
        ADD_BOOLEAN_BITFIELD(lastBoxDecorationBackgroundObscured, LastBoxDecorationBackgroundObscured);

        ADD_BOOLEAN_BITFIELD(isSlowRepaintObject, IsSlowRepaintObject);

    private:
        unsigned m_positionedState : 2; // PositionedState
        unsigned m_selectionState : 3; // SelectionState
        unsigned m_boxDecorationBackgroundState : 2; // BoxDecorationBackgroundState
        unsigned m_fullPaintInvalidationReason : 5; // PaintInvalidationReason

    public:
        bool isOutOfFlowPositioned() const { return m_positionedState == IsOutOfFlowPositioned; }
        bool isRelPositioned() const { return m_positionedState == IsRelativelyPositioned; }
        bool isPositioned() const { return m_positionedState != IsStaticallyPositioned; }

        void setPositionedState(int positionState)
        {
            // This mask maps FixedPosition and AbsolutePosition to IsOutOfFlowPositioned, saving one bit.
            m_positionedState = static_cast<PositionedState>(positionState & 0x3);
        }
        void clearPositionedState() { m_positionedState = StaticPosition; }

        ALWAYS_INLINE SelectionState selectionState() const { return static_cast<SelectionState>(m_selectionState); }
        ALWAYS_INLINE void setSelectionState(SelectionState selectionState) { m_selectionState = selectionState; }

        ALWAYS_INLINE BoxDecorationBackgroundState boxDecorationBackgroundState() const { return static_cast<BoxDecorationBackgroundState>(m_boxDecorationBackgroundState); }
        ALWAYS_INLINE void setBoxDecorationBackgroundState(BoxDecorationBackgroundState s) { m_boxDecorationBackgroundState = s; }

        PaintInvalidationReason fullPaintInvalidationReason() const { return static_cast<PaintInvalidationReason>(m_fullPaintInvalidationReason); }
        void setFullPaintInvalidationReason(PaintInvalidationReason reason) { m_fullPaintInvalidationReason = reason; }
    };

#undef ADD_BOOLEAN_BITFIELD

    LayoutObjectBitfields m_bitfields;

//     void setSelfNeedsLayout(bool b) { m_bitfields.setSelfNeedsLayout(b); }
//     void setNeedsPositionedMovementLayout(bool b) { m_bitfields.setNeedsPositionedMovementLayout(b); }
//     void setNormalChildNeedsLayout(bool b) { m_bitfields.setNormalChildNeedsLayout(b); }
//     void setPosChildNeedsLayout(bool b) { m_bitfields.setPosChildNeedsLayout(b); }
//     void setNeedsSimplifiedNormalFlowLayout(bool b) { m_bitfields.setNeedsSimplifiedNormalFlowLayout(b); }
//     void setIsDragging(bool b) { m_bitfields.setIsDragging(b); }
//     void setEverHadLayout(bool b) { m_bitfields.setEverHadLayout(b); }
//     void setShouldInvalidateOverflowForPaint(bool b) { m_bitfields.setShouldInvalidateOverflowForPaint(b); }
//     void setSelfNeedsOverflowRecalcAfterStyleChange(bool b) { m_bitfields.setSelfNeedsOverflowRecalcAfterStyleChange(b); }
//     void setChildNeedsOverflowRecalcAfterStyleChange(bool b) { m_bitfields.setChildNeedsOverflowRecalcAfterStyleChange(b); }

    void setSelfNeedsLayout(bool b);
    void setNeedsPositionedMovementLayout(bool b);
    void setNormalChildNeedsLayout(bool b);
    void setPosChildNeedsLayout(bool b);
    void setNeedsSimplifiedNormalFlowLayout(bool b);
    void setIsDragging(bool b);
    void setEverHadLayout(bool b);
    void setShouldInvalidateOverflowForPaint(bool b);
    void setSelfNeedsOverflowRecalcAfterStyleChange(bool b);
    void setChildNeedsOverflowRecalcAfterStyleChange(bool b);
        
private:
    // Store state between styleWillChange and styleDidChange
    static bool s_affectsParentBlock;

    // This stores the paint invalidation rect from the previous frame.
    LayoutRect m_previousPaintInvalidationRect;

    // This stores the position in the paint invalidation backing's coordinate.
    // It is used to detect layoutObject shifts that forces a full invalidation.
    LayoutPoint m_previousPositionFromPaintInvalidationBacking;
};

// FIXME: remove this once the layout object lifecycle ASSERTS are no longer hit.
class DeprecatedDisableModifyLayoutTreeStructureAsserts {
    WTF_MAKE_NONCOPYABLE(DeprecatedDisableModifyLayoutTreeStructureAsserts);
public:
    DeprecatedDisableModifyLayoutTreeStructureAsserts();

    static bool canModifyLayoutTreeStateInAnyState();

private:
    TemporaryChange<bool> m_disabler;
};

// FIXME: We should not allow paint invalidation out of paint invalidation state. crbug.com/457415
// Remove this once we fix the bug.
class DisablePaintInvalidationStateAsserts {
    WTF_MAKE_NONCOPYABLE(DisablePaintInvalidationStateAsserts);
public:
    DisablePaintInvalidationStateAsserts();
private:
    TemporaryChange<bool> m_disabler;
};

// Allow equality comparisons of LayoutObjects by reference or pointer, interchangeably.
DEFINE_COMPARISON_OPERATORS_WITH_REFERENCES(LayoutObject)

inline bool LayoutObject::documentBeingDestroyed() const
{
    return document().lifecycle().state() >= DocumentLifecycle::Stopping;
}

inline bool LayoutObject::isBeforeContent() const
{
    if (style()->styleType() != BEFORE)
        return false;
    // Text nodes don't have their own styles, so ignore the style on a text node.
    if (isText() && !isBR())
        return false;
    return true;
}

inline bool LayoutObject::isAfterContent() const
{
    if (style()->styleType() != AFTER)
        return false;
    // Text nodes don't have their own styles, so ignore the style on a text node.
    if (isText() && !isBR())
        return false;
    return true;
}

inline bool LayoutObject::isBeforeOrAfterContent() const
{
    return isBeforeContent() || isAfterContent();
}

// setNeedsLayout() won't cause full paint invalidations as
// setNeedsLayoutAndFullPaintInvalidation() does. Otherwise the two methods are identical.
// inline void LayoutObject::setNeedsLayout(LayoutInvalidationReasonForTracing reason, MarkingBehavior markParents, SubtreeLayoutScope* layouter)
// {
//     ASSERT(!isSetNeedsLayoutForbidden());
//     bool alreadyNeededLayout = m_bitfields.selfNeedsLayout();
//     setSelfNeedsLayout(true);
//     if (!alreadyNeededLayout) {
//         TRACE_EVENT_INSTANT1(
//             TRACE_DISABLED_BY_DEFAULT("devtools.timeline.invalidationTracking"),
//             "LayoutInvalidationTracking",
//             TRACE_EVENT_SCOPE_THREAD,
//             "data",
//             InspectorLayoutInvalidationTrackingEvent::data(this, reason));
//         if (markParents == MarkContainerChain && (!layouter || layouter->root() != this))
//             markContainerChainForLayout(true, layouter);
//     }
// }
// 
// inline void LayoutObject::setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReasonForTracing reason, MarkingBehavior markParents, SubtreeLayoutScope* layouter)
// {
//     setNeedsLayout(reason, markParents, layouter);
//     setShouldDoFullPaintInvalidation();
// }

// inline void LayoutObject::clearNeedsLayout()
// {
//     setNeededLayoutBecauseOfChildren(needsLayoutBecauseOfChildren());
//     setLayoutDidGetCalledSinceLastFrame();
//     setSelfNeedsLayout(false);
//     setEverHadLayout(true);
//     setPosChildNeedsLayout(false);
//     setNeedsSimplifiedNormalFlowLayout(false);
//     setNormalChildNeedsLayout(false);
//     setNeedsPositionedMovementLayout(false);
//     setAncestorLineBoxDirty(false);
// #if ENABLE(ASSERT)
//     checkBlockPositionedObjectsNeedLayout();
// #endif
// }

// inline void LayoutObject::setChildNeedsLayout(MarkingBehavior markParents, SubtreeLayoutScope* layouter)
// {
//     ASSERT(!isSetNeedsLayoutForbidden());
//     bool alreadyNeededLayout = normalChildNeedsLayout();
//     setNormalChildNeedsLayout(true);
//     // FIXME: Replace MarkOnlyThis with the SubtreeLayoutScope code path and remove the MarkingBehavior argument entirely.
//     if (!alreadyNeededLayout && markParents == MarkContainerChain && (!layouter || layouter->root() != this))
//         markContainerChainForLayout(true, layouter);
// }

// void LayoutObject::setNeedsPositionedMovementLayout()
// {
//     bool alreadyNeededLayout = needsPositionedMovementLayout();
//     setNeedsPositionedMovementLayout(true);
//     ASSERT(!isSetNeedsLayoutForbidden());
//     if (!alreadyNeededLayout)
//         markContainerChainForLayout();
// }

inline bool LayoutObject::preservesNewline() const
{
    if (isSVGInlineText())
        return false;

    return style()->preserveNewline();
}

inline bool LayoutObject::layerCreationAllowedForSubtree() const
{
    LayoutObject* parentLayoutObject = parent();
    while (parentLayoutObject) {
        if (parentLayoutObject->isSVGHiddenContainer())
            return false;
        parentLayoutObject = parentLayoutObject->parent();
    }

    return true;
}

inline void LayoutObject::setSelectionStateIfNeeded(SelectionState state)
{
    if (selectionState() == state)
        return;

    setSelectionState(state);
}

inline void LayoutObject::setHasBoxDecorationBackground(bool b)
{
    if (!b) {
        m_bitfields.setBoxDecorationBackgroundState(NoBoxDecorationBackground);
        return;
    }
    if (hasBoxDecorationBackground())
        return;
    m_bitfields.setBoxDecorationBackgroundState(HasBoxDecorationBackgroundObscurationStatusInvalid);
}

inline void LayoutObject::invalidateBackgroundObscurationStatus()
{
    if (!hasBoxDecorationBackground())
        return;
    m_bitfields.setBoxDecorationBackgroundState(HasBoxDecorationBackgroundObscurationStatusInvalid);
}

inline bool LayoutObject::boxDecorationBackgroundIsKnownToBeObscured()
{
    if (m_bitfields.boxDecorationBackgroundState() == HasBoxDecorationBackgroundObscurationStatusInvalid) {
        BoxDecorationBackgroundState state = computeBackgroundIsKnownToBeObscured() ? HasBoxDecorationBackgroundKnownToBeObscured : HasBoxDecorationBackgroundMayBeVisible;
        m_bitfields.setBoxDecorationBackgroundState(state);
    }
    return m_bitfields.boxDecorationBackgroundState() == HasBoxDecorationBackgroundKnownToBeObscured;
}

inline void makeMatrixRenderable(TransformationMatrix& matrix, bool has3DRendering)
{
    if (!has3DRendering)
        matrix.makeAffine();
}

inline int adjustForAbsoluteZoom(int value, LayoutObject* layoutObject)
{
    return adjustForAbsoluteZoom(value, layoutObject->style());
}

inline double adjustDoubleForAbsoluteZoom(double value, LayoutObject& layoutObject)
{
    ASSERT(layoutObject.style());
    return adjustDoubleForAbsoluteZoom(value, *layoutObject.style());
}

inline LayoutUnit adjustLayoutUnitForAbsoluteZoom(LayoutUnit value, LayoutObject& layoutObject)
{
    ASSERT(layoutObject.style());
    return adjustLayoutUnitForAbsoluteZoom(value, *layoutObject.style());
}

inline void adjustFloatQuadForAbsoluteZoom(FloatQuad& quad, LayoutObject& layoutObject)
{
    float zoom = layoutObject.style()->effectiveZoom();
    if (zoom != 1)
        quad.scale(1 / zoom, 1 / zoom);
}

inline void adjustFloatRectForAbsoluteZoom(FloatRect& rect, LayoutObject& layoutObject)
{
    float zoom = layoutObject.style()->effectiveZoom();
    if (zoom != 1)
        rect.scale(1 / zoom, 1 / zoom);
}

inline double adjustScrollForAbsoluteZoom(double value, LayoutObject& layoutObject)
{
    ASSERT(layoutObject.style());
    return adjustScrollForAbsoluteZoom(value, *layoutObject.style());
}

#define DEFINE_LAYOUT_OBJECT_TYPE_CASTS(thisType, predicate) \
    DEFINE_TYPE_CASTS(thisType, LayoutObject, object, object->predicate, object.predicate)

} // namespace blink

#ifndef NDEBUG
// Outside the WebCore namespace for ease of invocation from gdb.
void showTree(const blink::LayoutObject*);
void showLineTree(const blink::LayoutObject*);
void showLayoutTree(const blink::LayoutObject* object1);
// We don't make object2 an optional parameter so that showLayoutTree
// can be called from gdb easily.
void showLayoutTree(const blink::LayoutObject* object1, const blink::LayoutObject* object2);

#endif

#endif // LayoutObject_h
