// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/compositing/CompositingReasonFinder.h"

#include "core/CSSPropertyNames.h"
#include "core/dom/Document.h"
#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/layout/LayoutView.h"
#include "core/page/Page.h"
#include "core/paint/DeprecatedPaintLayer.h"

namespace blink {

CompositingReasonFinder::CompositingReasonFinder(LayoutView& layoutView)
    : m_layoutView(layoutView)
    , m_compositingTriggers(static_cast<CompositingTriggerFlags>(AllCompositingTriggers))
{
    updateTriggers();
}

void CompositingReasonFinder::updateTriggers()
{
    m_compositingTriggers = 0;

    Settings& settings = m_layoutView.document().page()->settings();
    if (settings.preferCompositingToLCDTextEnabled()) {
        m_compositingTriggers |= ScrollableInnerFrameTrigger;
        m_compositingTriggers |= OverflowScrollTrigger;
        m_compositingTriggers |= ViewportConstrainedPositionedTrigger;
    }
}

bool CompositingReasonFinder::hasOverflowScrollTrigger() const
{
    return m_compositingTriggers & OverflowScrollTrigger;
}

bool CompositingReasonFinder::isMainFrame() const
{
    // FIXME: LocalFrame::isMainFrame() is probably better.
    return !m_layoutView.document().ownerElement();
}

CompositingReasons CompositingReasonFinder::directReasons(const DeprecatedPaintLayer* layer) const
{
    if (RuntimeEnabledFeatures::slimmingPaintCompositorLayerizationEnabled())
        return CompositingReasonNone;

    ASSERT(potentialCompositingReasonsFromStyle(layer->layoutObject()) == layer->potentialCompositingReasonsFromStyle());
    CompositingReasons styleDeterminedDirectCompositingReasons = layer->potentialCompositingReasonsFromStyle() & CompositingReasonComboAllDirectStyleDeterminedReasons;

    // Apply optimizations for scroll-blocks-on which require comparing style between objects.
    if ((styleDeterminedDirectCompositingReasons & CompositingReasonScrollBlocksOn) && !requiresCompositingForScrollBlocksOn(layer->layoutObject()))
        styleDeterminedDirectCompositingReasons &= ~CompositingReasonScrollBlocksOn;

    return styleDeterminedDirectCompositingReasons | nonStyleDeterminedDirectReasons(layer);
}

// This information doesn't appear to be incorporated into CompositingReasons.
bool CompositingReasonFinder::requiresCompositingForScrollableFrame() const
{
    // Need this done first to determine overflow.
    ASSERT(!m_layoutView.needsLayout());
    if (isMainFrame())
        return false;

    if (!(m_compositingTriggers & ScrollableInnerFrameTrigger))
        return false;

    return m_layoutView.frameView()->isScrollable();
}

CompositingReasons CompositingReasonFinder::potentialCompositingReasonsFromStyle(LayoutObject* layoutObject) const
{
    if (RuntimeEnabledFeatures::slimmingPaintCompositorLayerizationEnabled())
        return CompositingReasonNone;

    CompositingReasons reasons = CompositingReasonNone;

    const ComputedStyle& style = layoutObject->styleRef();

    if (requiresCompositingForTransform(layoutObject))
        reasons |= CompositingReason3DTransform;

    if (style.backfaceVisibility() == BackfaceVisibilityHidden)
        reasons |= CompositingReasonBackfaceVisibilityHidden;

    if (requiresCompositingForAnimation(style))
        reasons |= CompositingReasonActiveAnimation;

    if (style.hasWillChangeCompositingHint() && !style.subtreeWillChangeContents())
        reasons |= CompositingReasonWillChangeCompositingHint;

    if (style.hasInlineTransform())
        reasons |= CompositingReasonInlineTransform;

    if (style.transformStyle3D() == TransformStyle3DPreserve3D)
        reasons |= CompositingReasonPreserve3DWith3DDescendants;

    if (style.hasPerspective())
        reasons |= CompositingReasonPerspectiveWith3DDescendants;

    // Ignore scroll-blocks-on on the document element, because it will get propagated to
    // the LayoutView (by Document::inheritHtmlAndBodyElementStyles) and we don't want to
    // create two composited layers.
    if (style.hasScrollBlocksOn() && !layoutObject->isDocumentElement())
        reasons |= CompositingReasonScrollBlocksOn;

    if (style.hasCompositorProxy())
        reasons |= CompositingReasonCompositorProxy;

    // If the implementation of createsGroup changes, we need to be aware of that in this part of code.
    ASSERT((layoutObject->isTransparent() || layoutObject->hasMask() || layoutObject->hasFilter() || style.hasBlendMode()) == layoutObject->createsGroup());

    if (style.hasMask())
        reasons |= CompositingReasonMaskWithCompositedDescendants;

    if (style.hasFilter())
        reasons |= CompositingReasonFilterWithCompositedDescendants;

    // See Layer::updateTransform for an explanation of why we check both.
    if (layoutObject->hasTransformRelatedProperty() && style.hasTransform())
        reasons |= CompositingReasonTransformWithCompositedDescendants;

    if (layoutObject->isTransparent())
        reasons |= CompositingReasonOpacityWithCompositedDescendants;

    if (style.hasBlendMode())
        reasons |= CompositingReasonBlendingWithCompositedDescendants;

    if (layoutObject->hasReflection())
        reasons |= CompositingReasonReflectionWithCompositedDescendants;

    ASSERT(!(reasons & ~CompositingReasonComboAllStyleDeterminedReasons));
    return reasons;
}

bool CompositingReasonFinder::requiresCompositingForTransform(LayoutObject* layoutObject) const
{
    // Note that we ask the layoutObject if it has a transform, because the style may have transforms,
    // but the layoutObject may be an inline that doesn't support them.
    return layoutObject->hasTransformRelatedProperty() && layoutObject->style()->has3DTransform();
}

CompositingReasons CompositingReasonFinder::nonStyleDeterminedDirectReasons(const DeprecatedPaintLayer* layer) const
{
    CompositingReasons directReasons = CompositingReasonNone;
    LayoutObject* layoutObject = layer->layoutObject();

    if (hasOverflowScrollTrigger()) {
        if (layer->clipParent())
            directReasons |= CompositingReasonOutOfFlowClipping;

        if (layer->needsCompositedScrolling())
            directReasons |= CompositingReasonOverflowScrollingTouch;
    }

    // Composite |layer| if it is inside of an ancestor scrolling layer, but that
    // scrolling layer is not not on the stacking context ancestor chain of |layer|.
    // See the definition of the scrollParent property in Layer for more detail.
    if (const DeprecatedPaintLayer* scrollingAncestor = layer->ancestorScrollingLayer()) {
        if (scrollingAncestor->needsCompositedScrolling() && layer->scrollParent())
            directReasons |= CompositingReasonOverflowScrollingParent;
    }

    if (requiresCompositingForPositionFixed(layer))
        directReasons |= CompositingReasonPositionFixed;

    directReasons |= layoutObject->additionalCompositingReasons();

    ASSERT(!(directReasons & CompositingReasonComboAllStyleDeterminedReasons));
    return directReasons;
}

bool CompositingReasonFinder::requiresCompositingForAnimation(const ComputedStyle& style) const
{
    if (style.subtreeWillChangeContents())
        return style.isRunningAnimationOnCompositor();

    return style.shouldCompositeForCurrentAnimations();
}

bool CompositingReasonFinder::requiresCompositingForPositionFixed(const DeprecatedPaintLayer* layer) const
{
    if (!(m_compositingTriggers & ViewportConstrainedPositionedTrigger))
        return false;
    // Don't promote fixed position elements that are descendants of a non-view container, e.g. transformed elements.
    // They will stay fixed wrt the container rather than the enclosing frame.
    return layer->scrollsWithViewport() && m_layoutView.frameView()->isScrollable();
}

bool CompositingReasonFinder::requiresCompositingForScrollBlocksOn(const LayoutObject* layoutObject) const
{
    // Note that the other requires* functions run at LayoutObject::styleDidChange time and so can rely
    // only on the style of their object.  This function runs at CompositingRequirementsUpdater::update
    // time, and so can consider the style of other objects.
    const ComputedStyle& style = layoutObject->styleRef();

    // We should only get here by CompositingReasonScrollBlocksOn being a potential compositing reason.
    ASSERT(style.hasScrollBlocksOn() && !layoutObject->isDocumentElement());

    // scroll-blocks-on style is propagated from the document element to the document.
    ASSERT(!layoutObject->isLayoutView()
        || !layoutObject->document().documentElement()
        || !layoutObject->document().documentElement()->layoutObject()
        || layoutObject->document().documentElement()->layoutObject()->style()->scrollBlocksOn() == style.scrollBlocksOn());

    // When a scroll occurs, it's the union of all bits set on the target element's containing block
    // chain that determines the behavior.  Thus we really only need a new layer if this object contains
    // additional bits from those set by all objects in it's containing block chain.  But determining
    // this fully is probably more expensive than it's worth.  Instead we just have fast-paths here for
    // the most common cases of unnecessary layer creation.
    // Optimizing this fully would avoid layer explosion in pathological cases like '*' rules.
    // We could consider tracking the current state in CompositingRequirementsUpdater::update.

    // Ensure iframes don't get composited when they require no more blocking than their parent.
    if (layoutObject->isLayoutView()) {
        if (const FrameView* parentFrame = toLayoutView(layoutObject)->frameView()->parentFrameView()) {
            if (const LayoutView* parentLayoutObject = parentFrame->layoutView()) {
                // Does this frame contain only blocks-on bits already present in the parent frame?
                if (!(style.scrollBlocksOn() & ~parentLayoutObject->style()->scrollBlocksOn()))
                    return false;
            }
        } else {
            // The root frame will either always already be composited, or compositing will be disabled.
            // Either way, we don't need to require compositing for scroll blocks on.  This avoids
            // enabling compositing by default, and avoids cluttering the root layers compositing reasons.
            return false;
        }
    }

    return true;
}

}
