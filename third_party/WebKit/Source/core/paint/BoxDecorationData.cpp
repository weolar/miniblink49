// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/BoxDecorationData.h"

#include "core/layout/LayoutBox.h"
#include "core/style/BorderEdge.h"
#include "core/style/ComputedStyle.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"

namespace blink {

BoxDecorationData::BoxDecorationData(const LayoutBox& layoutBox)
{
    backgroundColor = layoutBox.style()->visitedDependentColor(CSSPropertyBackgroundColor);
    hasBackground = backgroundColor.alpha() || layoutBox.style()->hasBackgroundImage();
    ASSERT(hasBackground == layoutBox.style()->hasBackground());
    hasBorderDecoration = layoutBox.style()->hasBorderDecoration();
    hasAppearance = layoutBox.style()->hasAppearance();
    bleedAvoidance = determineBackgroundBleedAvoidance(layoutBox);
}

namespace {

bool borderObscuresBackgroundEdge(const ComputedStyle& style)
{
    BorderEdge edges[4];
    style.getBorderEdgeInfo(edges);

    for (auto& edge : edges) {
        if (!edge.obscuresBackgroundEdge())
            return false;
    }

    return true;
}

} // anonymous namespace

BackgroundBleedAvoidance BoxDecorationData::determineBackgroundBleedAvoidance(const LayoutBox& layoutBox)
{
    if (layoutBox.isDocumentElement())
        return BackgroundBleedNone;

    if (!hasBackground)
        return BackgroundBleedNone;

    if (!hasBorderDecoration || !layoutBox.style()->hasBorderRadius() || layoutBox.canRenderBorderImage()) {
        if (layoutBox.backgroundShouldAlwaysBeClipped())
            return BackgroundBleedClipOnly;
        return BackgroundBleedNone;
    }

    if (borderObscuresBackgroundEdge(*layoutBox.style()))
        return BackgroundBleedShrinkBackground;

    return BackgroundBleedClipLayer;
}

} // namespace blink
