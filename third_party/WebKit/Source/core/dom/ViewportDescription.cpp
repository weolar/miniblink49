/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2012-2013 Intel Corporation. All rights reserved.
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

#include "config.h"
#include "core/dom/ViewportDescription.h"

#include "core/dom/Document.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/Platform.h"

namespace blink {

static const float& blinkMax(const float& left, const float& right)
{
    return ((right > left) ? right : left);
}

static const float& blinkMin(const float& left, const float& right)
{
    return ((right < left) ? right : left);
}

static const float& compareIgnoringAuto(const float& value1, const float& value2, const float& (*compare) (const float&, const float&))
{
    if (value1 == ViewportDescription::ValueAuto)
        return value2;

    if (value2 == ViewportDescription::ValueAuto)
        return value1;

    return compare(value1, value2);
}

float ViewportDescription::resolveViewportLength(const Length& length, const FloatSize& initialViewportSize, Direction direction)
{
    if (length.isAuto())
        return ViewportDescription::ValueAuto;

    if (length.isFixed())
        return length.getFloatValue();

    if (length.type() == ExtendToZoom)
        return ViewportDescription::ValueExtendToZoom;

    if (length.type() == Percent && direction == Horizontal)
        return initialViewportSize.width() * length.getFloatValue() / 100.0f;

    if (length.type() == Percent && direction == Vertical)
        return initialViewportSize.height() * length.getFloatValue() / 100.0f;

    if (length.type() == DeviceWidth)
        return initialViewportSize.width();

    if (length.type() == DeviceHeight)
        return initialViewportSize.height();

    ASSERT_NOT_REACHED();
    return ViewportDescription::ValueAuto;
}

PageScaleConstraints ViewportDescription::resolve(const FloatSize& initialViewportSize, Length legacyFallbackWidth) const
{
    float resultWidth = ValueAuto;

    Length copyMaxWidth = maxWidth;
    Length copyMinWidth = minWidth;
    // In case the width (used for min- and max-width) is undefined.
    if (isLegacyViewportType() && maxWidth.isAuto()) {
        // The width viewport META property is translated into 'width' descriptors, setting
        // the 'min' value to 'extend-to-zoom' and the 'max' value to the intended length.
        // In case the UA-defines a min-width, use that as length.
        if (zoom == ViewportDescription::ValueAuto) {
            copyMinWidth = Length(ExtendToZoom);
            copyMaxWidth = legacyFallbackWidth;
        } else if (maxHeight.isAuto()) {
            copyMinWidth = Length(ExtendToZoom);
            copyMaxWidth = Length(ExtendToZoom);
        }
    }

    float resultMaxWidth = resolveViewportLength(copyMaxWidth, initialViewportSize, Horizontal);
    float resultMinWidth = resolveViewportLength(copyMinWidth, initialViewportSize, Horizontal);

    float resultHeight = ValueAuto;
    float resultMaxHeight = resolveViewportLength(maxHeight, initialViewportSize, Vertical);
    float resultMinHeight = resolveViewportLength(minHeight, initialViewportSize, Vertical);

    float resultZoom = zoom;
    float resultMinZoom = minZoom;
    float resultMaxZoom = maxZoom;
    bool resultUserZoom = userZoom;

    // 1. Resolve min-zoom and max-zoom values.
    if (resultMinZoom != ViewportDescription::ValueAuto && resultMaxZoom != ViewportDescription::ValueAuto)
        resultMaxZoom = std::max(resultMinZoom, resultMaxZoom);

    // 2. Constrain zoom value to the [min-zoom, max-zoom] range.
    if (resultZoom != ViewportDescription::ValueAuto)
        resultZoom = compareIgnoringAuto(resultMinZoom, compareIgnoringAuto(resultMaxZoom, resultZoom, blinkMin), blinkMax);

    float extendZoom = compareIgnoringAuto(resultZoom, resultMaxZoom, blinkMin);

    // 3. Resolve non-"auto" lengths to pixel lengths.
    if (extendZoom == ViewportDescription::ValueAuto) {
        if (resultMaxWidth == ViewportDescription::ValueExtendToZoom)
            resultMaxWidth = ViewportDescription::ValueAuto;

        if (resultMaxHeight == ViewportDescription::ValueExtendToZoom)
            resultMaxHeight = ViewportDescription::ValueAuto;

        if (resultMinWidth == ViewportDescription::ValueExtendToZoom)
            resultMinWidth = resultMaxWidth;

        if (resultMinHeight == ViewportDescription::ValueExtendToZoom)
            resultMinHeight = resultMaxHeight;
    } else {
        float extendWidth = initialViewportSize.width() / extendZoom;
        float extendHeight = initialViewportSize.height() / extendZoom;

        if (resultMaxWidth == ViewportDescription::ValueExtendToZoom)
            resultMaxWidth = extendWidth;

        if (resultMaxHeight == ViewportDescription::ValueExtendToZoom)
            resultMaxHeight = extendHeight;

        if (resultMinWidth == ViewportDescription::ValueExtendToZoom)
            resultMinWidth = compareIgnoringAuto(extendWidth, resultMaxWidth, blinkMax);

        if (resultMinHeight == ViewportDescription::ValueExtendToZoom)
            resultMinHeight = compareIgnoringAuto(extendHeight, resultMaxHeight, blinkMax);
    }

    // 4. Resolve initial width from min/max descriptors.
    if (resultMinWidth != ViewportDescription::ValueAuto || resultMaxWidth != ViewportDescription::ValueAuto)
        resultWidth = compareIgnoringAuto(resultMinWidth, compareIgnoringAuto(resultMaxWidth, initialViewportSize.width(), blinkMin), blinkMax);

    // 5. Resolve initial height from min/max descriptors.
    if (resultMinHeight != ViewportDescription::ValueAuto || resultMaxHeight != ViewportDescription::ValueAuto)
        resultHeight = compareIgnoringAuto(resultMinHeight, compareIgnoringAuto(resultMaxHeight, initialViewportSize.height(), blinkMin), blinkMax);

    // 6-7. Resolve width value.
    if (resultWidth == ViewportDescription::ValueAuto) {
        if (resultHeight == ViewportDescription::ValueAuto || !initialViewportSize.height())
            resultWidth = initialViewportSize.width();
        else
            resultWidth = resultHeight * (initialViewportSize.width() / initialViewportSize.height());
    }

    // 8. Resolve height value.
    if (resultHeight == ViewportDescription::ValueAuto) {
        if (!initialViewportSize.width())
            resultHeight = initialViewportSize.height();
        else
            resultHeight = resultWidth * initialViewportSize.height() / initialViewportSize.width();
    }

    // Resolve initial-scale value.
    if (resultZoom == ViewportDescription::ValueAuto) {
        if (resultWidth != ViewportDescription::ValueAuto && resultWidth > 0)
            resultZoom = initialViewportSize.width() / resultWidth;
        if (resultHeight != ViewportDescription::ValueAuto && resultHeight > 0) {
            // if 'auto', the initial-scale will be negative here and thus ignored.
            resultZoom = std::max<float>(resultZoom, initialViewportSize.height() / resultHeight);
        }
    }

    // If user-scalable = no, lock the min/max scale to the computed initial
    // scale.
    if (!resultUserZoom)
        resultMinZoom = resultMaxZoom = resultZoom;

    // Only set initialScale to a value if it was explicitly set.
    if (zoom == ViewportDescription::ValueAuto)
        resultZoom = ViewportDescription::ValueAuto;

    PageScaleConstraints result;
    result.minimumScale = resultMinZoom;
    result.maximumScale = resultMaxZoom;
    result.initialScale = resultZoom;
    result.layoutSize.setWidth(resultWidth);
    result.layoutSize.setHeight(resultHeight);
    return result;
}

void ViewportDescription::reportMobilePageStats(const LocalFrame* mainFrame) const
{
#if OS(ANDROID)
    enum ViewportUMAType {
        NoViewportTag,
        DeviceWidth,
        ConstantWidth,
        MetaWidthOther,
        MetaHandheldFriendly,
        MetaMobileOptimized,
        XhtmlMobileProfile,
        TypeCount
    };

    if (!mainFrame || !mainFrame->host() || !mainFrame->view() || !mainFrame->document())
        return;

    // Avoid chrome:// pages like the new-tab page (on Android new tab is non-http).
    if (!mainFrame->document()->url().protocolIsInHTTPFamily())
        return;

    if (!isSpecifiedByAuthor()) {
        if (mainFrame->document()->isMobileDocument())
            Platform::current()->histogramEnumeration("Viewport.MetaTagType", XhtmlMobileProfile, TypeCount);
        else
            Platform::current()->histogramEnumeration("Viewport.MetaTagType", NoViewportTag, TypeCount);

        return;
    }

    if (isMetaViewportType()) {
        if (maxWidth.type() == blink::Fixed) {
            Platform::current()->histogramEnumeration("Viewport.MetaTagType", ConstantWidth, TypeCount);

            if (mainFrame->view()) {
                // To get an idea of how "far" the viewport is from the device's ideal width, we
                // report the zoom level that we'd need to be at for the entire page to be visible.
                int viewportWidth = maxWidth.intValue();
                int windowWidth = mainFrame->host()->pinchViewport().size().width();
                int overviewZoomPercent = 100 * windowWidth / static_cast<float>(viewportWidth);
                Platform::current()->histogramSparse("Viewport.OverviewZoom", overviewZoomPercent);
            }

        } else if (maxWidth.type() == blink::DeviceWidth || maxWidth.type() == blink::ExtendToZoom) {
            Platform::current()->histogramEnumeration("Viewport.MetaTagType", DeviceWidth, TypeCount);
        } else {
            // Overflow bucket for cases we may be unaware of.
            Platform::current()->histogramEnumeration("Viewport.MetaTagType", MetaWidthOther, TypeCount);
        }
    } else if (type == ViewportDescription::HandheldFriendlyMeta) {
        Platform::current()->histogramEnumeration("Viewport.MetaTagType", MetaHandheldFriendly, TypeCount);
    } else if (type == ViewportDescription::MobileOptimizedMeta) {
        Platform::current()->histogramEnumeration("Viewport.MetaTagType", MobileOptimizedMeta, TypeCount);
    }
#endif
}

bool ViewportDescription::matchesHeuristicsForGpuRasterization() const
{
    return maxWidth == Length(DeviceWidth)
        && minZoom == 1.0
        && minZoomIsExplicit;
}

} // namespace blink
