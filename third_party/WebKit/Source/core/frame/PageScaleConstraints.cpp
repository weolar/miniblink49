/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/frame/PageScaleConstraints.h"

#include <algorithm>

namespace blink {

PageScaleConstraints::PageScaleConstraints()
    : initialScale(-1), minimumScale(-1), maximumScale(-1) { }

PageScaleConstraints::PageScaleConstraints(float initial, float minimum, float maximum)
    : initialScale(initial), minimumScale(minimum), maximumScale(maximum) { }

void PageScaleConstraints::overrideWith(const PageScaleConstraints& other)
{
    if (other.initialScale != -1) {
        initialScale = other.initialScale;
        if (minimumScale != -1)
            minimumScale = std::min(minimumScale, other.initialScale);
    }
    if (other.minimumScale != -1)
        minimumScale = other.minimumScale;
    if (other.maximumScale != -1)
        maximumScale = other.maximumScale;
    if (!other.layoutSize.isZero())
        layoutSize = other.layoutSize;
    clampAll();
}

float PageScaleConstraints::clampToConstraints(float pageScaleFactor) const
{
    if (pageScaleFactor == -1)
        return pageScaleFactor;
    if (minimumScale != -1)
        pageScaleFactor = std::max(pageScaleFactor, minimumScale);
    if (maximumScale != -1)
        pageScaleFactor = std::min(pageScaleFactor, maximumScale);
    return pageScaleFactor;
}

void PageScaleConstraints::clampAll()
{
    if (minimumScale != -1 && maximumScale != -1)
        maximumScale = std::max(minimumScale, maximumScale);
    initialScale = clampToConstraints(initialScale);
}

void PageScaleConstraints::fitToContentsWidth(float contentsWidth, int viewWidthNotIncludingScrollbars)
{
    if (!contentsWidth || !viewWidthNotIncludingScrollbars)
        return;

    // Clamp the minimum scale so that the viewport can't exceed the document
    // width.
    minimumScale = std::max(minimumScale, viewWidthNotIncludingScrollbars / contentsWidth);

    clampAll();
}

void PageScaleConstraints::resolveAutoInitialScale()
{
    // If the initial scale wasn't defined, set it to minimum scale now that we
    // know the real value.
    if (initialScale == -1)
        initialScale = minimumScale;

    clampAll();
}

bool PageScaleConstraints::operator==(const PageScaleConstraints& other) const
{
    return layoutSize == other.layoutSize
        && initialScale == other.initialScale
        && minimumScale == other.minimumScale
        && maximumScale == other.maximumScale;
}

} // namespace blink
