/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 *           (C) 2000 Stefan Schimanski (1Stein@gmx.de)
 * Copyright (C) 2004, 2005, 2006, 2008, 2009, 2010 Apple Inc. All rights reserved.
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
#include "core/layout/LayoutEmbeddedObject.h"

#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutView.h"
#include "core/page/Page.h"
#include "core/paint/EmbeddedObjectPainter.h"
#include "core/plugins/PluginView.h"
#include "platform/text/PlatformLocale.h"

namespace blink {

using namespace HTMLNames;

LayoutEmbeddedObject::LayoutEmbeddedObject(Element* element)
    : LayoutPart(element)
    , m_showsUnavailablePluginIndicator(false)
{
    view()->frameView()->setIsVisuallyNonEmpty();
}

LayoutEmbeddedObject::~LayoutEmbeddedObject()
{
}

DeprecatedPaintLayerType LayoutEmbeddedObject::layerTypeRequired() const
{
    // This can't just use LayoutPart::layerTypeRequired, because DeprecatedPaintLayerCompositor
    // doesn't loop through LayoutEmbeddedObjects the way it does frames in order
    // to update the self painting bit on their Layer.
    // Also, unlike iframes, embeds don't used the usesCompositing bit on LayoutView
    // in requiresAcceleratedCompositing.
    if (requiresAcceleratedCompositing())
        return NormalDeprecatedPaintLayer;
    return LayoutPart::layerTypeRequired();
}

static String localizedUnavailablePluginReplacementText(Node* node, LayoutEmbeddedObject::PluginUnavailabilityReason pluginUnavailabilityReason)
{
    Locale& locale = node ? toElement(node)->locale() : Locale::defaultLocale();
    switch (pluginUnavailabilityReason) {
    case LayoutEmbeddedObject::PluginMissing:
        return locale.queryString(WebLocalizedString::MissingPluginText);
    case LayoutEmbeddedObject::PluginBlockedByContentSecurityPolicy:
        return locale.queryString(WebLocalizedString::BlockedPluginText);
    }

    ASSERT_NOT_REACHED();
    return String();
}

void LayoutEmbeddedObject::setPluginUnavailabilityReason(PluginUnavailabilityReason pluginUnavailabilityReason)
{
    ASSERT(!m_showsUnavailablePluginIndicator);
    m_showsUnavailablePluginIndicator = true;
    m_pluginUnavailabilityReason = pluginUnavailabilityReason;

    m_unavailablePluginReplacementText = localizedUnavailablePluginReplacementText(node(), pluginUnavailabilityReason);
}

bool LayoutEmbeddedObject::showsUnavailablePluginIndicator() const
{
    return m_showsUnavailablePluginIndicator;
}

void LayoutEmbeddedObject::paintContents(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    Element* element = toElement(node());
    if (!isHTMLPlugInElement(element))
        return;

    LayoutPart::paintContents(paintInfo, paintOffset);
}

void LayoutEmbeddedObject::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (showsUnavailablePluginIndicator()) {
        LayoutReplaced::paint(paintInfo, paintOffset);
        return;
    }

    LayoutPart::paint(paintInfo, paintOffset);
}

void LayoutEmbeddedObject::paintReplaced(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    EmbeddedObjectPainter(*this).paintReplaced(paintInfo, paintOffset);
}

void LayoutEmbeddedObject::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);

    updateLogicalWidth();
    updateLogicalHeight();

    m_overflow.clear();
    addVisualEffectOverflow();

    updateLayerTransformAfterLayout();

    Widget* widget = this->widget();
    if (widget) {
        if (widget->isPluginView())
            toPluginView(widget)->layoutIfNeeded();
    } else if (frameView()) {
        frameView()->addPartToUpdate(*this);
    }

    clearNeedsLayout();
}

PaintInvalidationReason LayoutEmbeddedObject::invalidatePaintIfNeeded(
    PaintInvalidationState& paintInvalidationState, const LayoutBoxModelObject& newPaintInvalidationContainer)
{
    PaintInvalidationReason reason =
        LayoutPart::invalidatePaintIfNeeded(paintInvalidationState, newPaintInvalidationContainer);

    Widget* widget = this->widget();
    if (widget && widget->isPluginView())
        toPluginView(widget)->invalidatePaintIfNeeded();

    return reason;
}

ScrollResultOneDimensional LayoutEmbeddedObject::scroll(ScrollDirectionPhysical direction, ScrollGranularity granularity, float)
{
    return ScrollResultOneDimensional(false);
}

CompositingReasons LayoutEmbeddedObject::additionalCompositingReasons() const
{
    if (requiresAcceleratedCompositing())
        return CompositingReasonPlugin;
    return CompositingReasonNone;
}

LayoutBox* LayoutEmbeddedObject::embeddedContentBox() const
{
    if (!node() || !widget() || !widget()->isFrameView())
        return nullptr;
    return toFrameView(widget())->embeddedContentBox();
}

} // namespace blink
