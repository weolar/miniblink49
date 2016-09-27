// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/paint/DisplayItemTransformTreeBuilder.h"

#include "platform/graphics/paint/DisplayItem.h"
#include "platform/graphics/paint/DisplayItemTransformTree.h"
#include "platform/graphics/paint/Transform3DDisplayItem.h"

namespace blink {

DisplayItemTransformTreeBuilder::DisplayItemTransformTreeBuilder()
    : m_transformTree(adoptPtr(new DisplayItemTransformTree))
    , m_rangeBeginIndex(0)
    , m_currentIndex(0)
{
    pushCurrentTransformNode(0 /* root node */, FloatSize());
}

DisplayItemTransformTreeBuilder::~DisplayItemTransformTreeBuilder()
{
}

PassOwnPtr<DisplayItemTransformTree> DisplayItemTransformTreeBuilder::releaseTransformTree()
{
    ASSERT(m_currentTransformNodeStack.size() == 1);
    ASSERT(currentTransformNodeData().ignoredBeginCount == 0);

    finishRange();
    return m_transformTree.release();
}

namespace {

enum BeginDisplayItemClassification { NotATransform = 0, Only2DTranslation, RequiresTransformNode };

// Classifies a display item based on whether it is a transform, and if so,
// whether it should be get a transform node.
// If it is a transform (including a translation), the TransformationMatrix
// will be copied to output parameter.
static BeginDisplayItemClassification classifyBeginItem(const DisplayItem& beginDisplayItem, TransformationMatrix* transform)
{
    ASSERT(beginDisplayItem.isBegin());

    if (DisplayItem::isTransform3DType(beginDisplayItem.type())) {
        const auto& begin3D = static_cast<const BeginTransform3DDisplayItem&>(beginDisplayItem);
        *transform = begin3D.transform();
        if (transform->isIdentityOr2DTranslation())
            return Only2DTranslation;
        return RequiresTransformNode;
    }
    return NotATransform;
}

} // namespace

void DisplayItemTransformTreeBuilder::processDisplayItem(const DisplayItem& displayItem)
{
    if (displayItem.isBegin()) {
        TransformationMatrix matrix;
        switch (classifyBeginItem(displayItem, &matrix)) {
        case NotATransform:
            // Remember to ignore this begin later on.
            currentTransformNodeData().ignoredBeginCount++;
            break;
        case Only2DTranslation:
            // Adjust the offset associated with the current transform node.
            finishRange();
            pushCurrentTransformNode(
                currentTransformNodeData().transformNode,
                currentTransformNodeData().offset + matrix.to2DTranslation());
            break;
        case RequiresTransformNode:
            // Emit a transform node.
            finishRange();
            size_t newNode = m_transformTree->createNewNode(
                currentTransformNodeData().transformNode,
                matrix);
            pushCurrentTransformNode(newNode, FloatSize());
            break;
        }
    } else if (displayItem.isEnd()) {
        if (currentTransformNodeData().ignoredBeginCount) {
            // Ignored this end display item.
            currentTransformNodeData().ignoredBeginCount--;
        } else {
            // We've closed the scope of a transform.
            finishRange();
            popCurrentTransformNode();
            ASSERT(!m_currentTransformNodeStack.isEmpty());
        }
    }
    m_currentIndex++;
}

void DisplayItemTransformTreeBuilder::finishRange()
{
    // Don't emit an empty range record.
    if (m_rangeBeginIndex != m_currentIndex) {
        const auto& current = currentTransformNodeData();
        m_transformTree->appendRangeRecord(m_rangeBeginIndex, m_currentIndex, current.transformNode, current.offset);
    }

    // The current display item is a boundary.
    // The earliest the next range could begin is the next one.
    m_rangeBeginIndex = m_currentIndex + 1;
}

} // namespace blink
