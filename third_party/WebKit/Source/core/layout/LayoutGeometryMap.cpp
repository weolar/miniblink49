/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutGeometryMap.h"

#include "core/frame/LocalFrame.h"
#include "core/layout/LayoutView.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/geometry/TransformState.h"
#include "wtf/TemporaryChange.h"

namespace blink {

LayoutGeometryMap::LayoutGeometryMap(MapCoordinatesFlags flags)
    : m_insertionPosition(kNotFound)
    , m_nonUniformStepsCount(0)
    , m_transformedStepsCount(0)
    , m_fixedStepsCount(0)
    , m_mapCoordinatesFlags(flags)
{
}

LayoutGeometryMap::~LayoutGeometryMap()
{
}

void LayoutGeometryMap::mapToContainer(TransformState& transformState, const LayoutBoxModelObject* container) const
{
    // If the mapping includes something like columns, we have to go via layoutObjects.
    if (hasNonUniformStep()) {
        m_mapping.last().m_layoutObject->mapLocalToContainer(container, transformState, ApplyContainerFlip | m_mapCoordinatesFlags);
        transformState.flatten();
        return;
    }

    bool inFixed = false;
#if ENABLE(ASSERT)
    bool foundContainer = !container || (m_mapping.size() && m_mapping[0].m_layoutObject == container);
#endif

    for (int i = m_mapping.size() - 1; i >= 0; --i) {
        const LayoutGeometryMapStep& currentStep = m_mapping[i];

        // If container is the root LayoutView (step 0) we want to apply its fixed position offset.
        if (i > 0 && currentStep.m_layoutObject == container) {
#if ENABLE(ASSERT)
            foundContainer = true;
#endif
            break;
        }

        // If this box has a transform, it acts as a fixed position container
        // for fixed descendants, which prevents the propagation of 'fixed'
        // unless the layer itself is also fixed position.
        if (i && currentStep.m_hasTransform && !currentStep.m_isFixedPosition)
            inFixed = false;
        else if (currentStep.m_isFixedPosition)
            inFixed = true;

        ASSERT(!i == isTopmostLayoutView(currentStep.m_layoutObject));

        if (!i) {
            // A null container indicates mapping through the root LayoutView, so including its transform (the page scale).
            if (!container && currentStep.m_transform)
                transformState.applyTransform(*currentStep.m_transform.get());
        } else {
            TransformState::TransformAccumulation accumulate = currentStep.m_accumulatingTransform ? TransformState::AccumulateTransform : TransformState::FlattenTransform;
            if (currentStep.m_transform)
                transformState.applyTransform(*currentStep.m_transform.get(), accumulate);
            else
                transformState.move(currentStep.m_offset.width(), currentStep.m_offset.height(), accumulate);
        }

        if (inFixed && !currentStep.m_offsetForFixedPosition.isZero()) {
            ASSERT(currentStep.m_layoutObject->isLayoutView());
            transformState.move(currentStep.m_offsetForFixedPosition);
        }
    }

    ASSERT(foundContainer);
    transformState.flatten();
}

FloatPoint LayoutGeometryMap::mapToContainer(const FloatPoint& p, const LayoutBoxModelObject* container) const
{
    FloatPoint result;

    if (!hasFixedPositionStep() && !hasTransformStep() && !hasNonUniformStep() && (!container || (m_mapping.size() && container == m_mapping[0].m_layoutObject))) {
        result = p + m_accumulatedOffset;
    } else {
        TransformState transformState(TransformState::ApplyTransformDirection, p);
        mapToContainer(transformState, container);
        result = transformState.lastPlanarPoint();
    }

#if ENABLE(ASSERT)
    if (m_mapping.size() > 0) {
        const LayoutObject* lastLayoutObject = m_mapping.last().m_layoutObject;
        const DeprecatedPaintLayer* layer = lastLayoutObject->enclosingLayer();

        // Bounds for invisible layers are intentionally not calculated, and are
        // therefore not necessarily expected to be correct here. This is ok,
        // because they will be recomputed if the layer becomes visible.
        if (!layer || !layer->subtreeIsInvisible()) {
            FloatPoint layoutObjectMappedResult = lastLayoutObject->localToContainerPoint(p, container, m_mapCoordinatesFlags);

            ASSERT(roundedIntPoint(layoutObjectMappedResult) == roundedIntPoint(result));
        }
    }
#endif

    return result;
}

#ifndef NDEBUG
// Handy function to call from gdb while debugging mismatched point/rect errors.
void LayoutGeometryMap::dumpSteps() const
{
    fprintf(stderr, "LayoutGeometryMap::dumpSteps accumulatedOffset=%d,%d\n", m_accumulatedOffset.width().toInt(), m_accumulatedOffset.height().toInt());
    for (int i = m_mapping.size() - 1; i >= 0; --i) {
        fprintf(stderr, " [%d] %s: offset=%d,%d", i,
            m_mapping[i].m_layoutObject->debugName().ascii().data(),
            m_mapping[i].m_offset.width().toInt(),
            m_mapping[i].m_offset.height().toInt());
        if (m_mapping[i].m_hasTransform)
            fprintf(stderr, " hasTransform");
        fprintf(stderr, "\n");
    }
}
#endif

FloatQuad LayoutGeometryMap::mapToContainer(const FloatRect& rect, const LayoutBoxModelObject* container) const
{
    FloatQuad result;

    if (!hasFixedPositionStep() && !hasTransformStep() && !hasNonUniformStep() && (!container || (m_mapping.size() && container == m_mapping[0].m_layoutObject))) {
        result = rect;
        result.move(m_accumulatedOffset);
    } else {
        TransformState transformState(TransformState::ApplyTransformDirection, rect.center(), rect);
        mapToContainer(transformState, container);
        result = transformState.lastPlanarQuad();
    }

#if ENABLE(ASSERT)
    if (m_mapping.size() > 0) {
        const LayoutObject* lastLayoutObject = m_mapping.last().m_layoutObject;
        const DeprecatedPaintLayer* layer = lastLayoutObject->enclosingLayer();

        // Bounds for invisible layers are intentionally not calculated, and are
        // therefore not necessarily expected to be correct here. This is ok,
        // because they will be recomputed if the layer becomes visible.
        if (!layer->subtreeIsInvisible() && lastLayoutObject->style()->visibility() == VISIBLE) {
            FloatRect layoutObjectMappedResult = lastLayoutObject->localToContainerQuad(rect, container, m_mapCoordinatesFlags).boundingBox();

            // Inspector creates layoutObjects with negative width <https://bugs.webkit.org/show_bug.cgi?id=87194>.
            // Taking FloatQuad bounds avoids spurious assertions because of that.
            if (enclosingIntRect(layoutObjectMappedResult) != enclosingIntRect(result.boundingBox())) {
                // weolar
                IntRect a1 = enclosingIntRect(layoutObjectMappedResult);
                IntRect a2 = enclosingIntRect(result.boundingBox());
                //DebugBreak();
            }
            //ASSERT(enclosingIntRect(layoutObjectMappedResult) == enclosingIntRect(result.boundingBox()));
        }
    }
#endif

    return result;
}

void LayoutGeometryMap::pushMappingsToAncestor(const LayoutObject* layoutObject, const LayoutBoxModelObject* ancestorLayoutObject)
{
    // We need to push mappings in reverse order here, so do insertions rather than appends.
    TemporaryChange<size_t> positionChange(m_insertionPosition, m_mapping.size());
    do {
        layoutObject = layoutObject->pushMappingToContainer(ancestorLayoutObject, *this);
    } while (layoutObject && layoutObject != ancestorLayoutObject);

    ASSERT(m_mapping.isEmpty() || isTopmostLayoutView(m_mapping[0].m_layoutObject));
}

static bool canMapBetweenLayoutObjects(const LayoutObject* layoutObject, const LayoutObject* ancestor)
{
    for (const LayoutObject* current = layoutObject; ; current = current->parent()) {
        const ComputedStyle& style = current->styleRef();
        if (style.position() == FixedPosition || style.isFlippedBlocksWritingMode())
            return false;

        if (current->hasTransformRelatedProperty() || current->isLayoutFlowThread() || current->isSVGRoot())
            return false;

        if (current == ancestor)
            break;
    }

    return true;
}

void LayoutGeometryMap::pushMappingsToAncestor(const DeprecatedPaintLayer* layer, const DeprecatedPaintLayer* ancestorLayer)
{
    const LayoutObject* layoutObject = layer->layoutObject();

    bool crossDocument = ancestorLayer && layer->layoutObject()->frame() != ancestorLayer->layoutObject()->frame();
    ASSERT(!crossDocument || m_mapCoordinatesFlags & TraverseDocumentBoundaries);

    // We have to visit all the layoutObjects to detect flipped blocks. This might defeat the gains
    // from mapping via layers.
    bool canConvertInLayerTree = (ancestorLayer && !crossDocument) ? canMapBetweenLayoutObjects(layer->layoutObject(), ancestorLayer->layoutObject()) : false;

//    fprintf(stderr, "LayoutGeometryMap::pushMappingsToAncestor from layer %p to layer %p, canConvertInLayerTree=%d\n", layer, ancestorLayer, canConvertInLayerTree);

    if (canConvertInLayerTree) {
        LayoutPoint layerOffset;
        layer->convertToLayerCoords(ancestorLayer, layerOffset);

        // The LayoutView must be pushed first.
        if (!m_mapping.size()) {
            ASSERT(ancestorLayer->layoutObject()->isLayoutView());
            pushMappingsToAncestor(ancestorLayer->layoutObject(), 0);
        }

        TemporaryChange<size_t> positionChange(m_insertionPosition, m_mapping.size());
        bool accumulatingTransform = layer->layoutObject()->style()->preserves3D() || ancestorLayer->layoutObject()->style()->preserves3D();
        push(layoutObject, toLayoutSize(layerOffset), accumulatingTransform, /*isNonUniform*/ false, /*isFixedPosition*/ false, /*hasTransform*/ false);
        return;
    }
    const LayoutBoxModelObject* ancestorLayoutObject = ancestorLayer ? ancestorLayer->layoutObject() : 0;
    pushMappingsToAncestor(layoutObject, ancestorLayoutObject);
}

void LayoutGeometryMap::push(const LayoutObject* layoutObject, const LayoutSize& offsetFromContainer, bool accumulatingTransform, bool isNonUniform, bool isFixedPosition, bool hasTransform, LayoutSize offsetForFixedPosition)
{
//    fprintf(stderr, "LayoutGeometryMap::push %p %d,%d isNonUniform=%d\n", layoutObject, offsetFromContainer.width().toInt(), offsetFromContainer.height().toInt(), isNonUniform);

    ASSERT(m_insertionPosition != kNotFound);
    ASSERT(!layoutObject->isLayoutView() || !m_insertionPosition || m_mapCoordinatesFlags & TraverseDocumentBoundaries);
    ASSERT(offsetForFixedPosition.isZero() || layoutObject->isLayoutView());

    m_mapping.insert(m_insertionPosition, LayoutGeometryMapStep(layoutObject, accumulatingTransform, isNonUniform, isFixedPosition, hasTransform));

    LayoutGeometryMapStep& step = m_mapping[m_insertionPosition];
    step.m_offset = offsetFromContainer;
    step.m_offsetForFixedPosition = offsetForFixedPosition;

    stepInserted(step);
}

void LayoutGeometryMap::push(const LayoutObject* layoutObject, const TransformationMatrix& t, bool accumulatingTransform, bool isNonUniform, bool isFixedPosition, bool hasTransform, LayoutSize offsetForFixedPosition)
{
    ASSERT(m_insertionPosition != kNotFound);
    ASSERT(!layoutObject->isLayoutView() || !m_insertionPosition || m_mapCoordinatesFlags & TraverseDocumentBoundaries);
    ASSERT(offsetForFixedPosition.isZero() || layoutObject->isLayoutView());

    m_mapping.insert(m_insertionPosition, LayoutGeometryMapStep(layoutObject, accumulatingTransform, isNonUniform, isFixedPosition, hasTransform));

    LayoutGeometryMapStep& step = m_mapping[m_insertionPosition];
    step.m_offsetForFixedPosition = offsetForFixedPosition;

    if (!t.isIntegerTranslation())
        step.m_transform = adoptPtr(new TransformationMatrix(t));
    else
        step.m_offset = LayoutSize(t.e(), t.f());

    stepInserted(step);
}

void LayoutGeometryMap::popMappingsToAncestor(const LayoutBoxModelObject* ancestorLayoutObject)
{
    ASSERT(m_mapping.size());

    while (m_mapping.size() && m_mapping.last().m_layoutObject != ancestorLayoutObject) {
        stepRemoved(m_mapping.last());
        m_mapping.removeLast();
    }
}

void LayoutGeometryMap::popMappingsToAncestor(const DeprecatedPaintLayer* ancestorLayer)
{
    const LayoutBoxModelObject* ancestorLayoutObject = ancestorLayer ? ancestorLayer->layoutObject() : 0;
    popMappingsToAncestor(ancestorLayoutObject);
}

void LayoutGeometryMap::stepInserted(const LayoutGeometryMapStep& step)
{
    m_accumulatedOffset += step.m_offset;

    if (step.m_isNonUniform)
        ++m_nonUniformStepsCount;

    if (step.m_transform)
        ++m_transformedStepsCount;

    if (step.m_isFixedPosition)
        ++m_fixedStepsCount;
}

void LayoutGeometryMap::stepRemoved(const LayoutGeometryMapStep& step)
{
    m_accumulatedOffset -= step.m_offset;

    if (step.m_isNonUniform) {
        ASSERT(m_nonUniformStepsCount);
        --m_nonUniformStepsCount;
    }

    if (step.m_transform) {
        ASSERT(m_transformedStepsCount);
        --m_transformedStepsCount;
    }

    if (step.m_isFixedPosition) {
        ASSERT(m_fixedStepsCount);
        --m_fixedStepsCount;
    }
}

#if ENABLE(ASSERT)
bool LayoutGeometryMap::isTopmostLayoutView(const LayoutObject* layoutObject) const
{
    if (!layoutObject->isLayoutView())
        return false;

    // If we're not working with multiple LayoutViews, then any view is considered
    // "topmost" (to preserve original behavior).
    if (!(m_mapCoordinatesFlags & TraverseDocumentBoundaries))
        return true;

    return layoutObject->frame()->isMainFrame();
}
#endif

} // namespace blink
