/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
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
#include "core/css/resolver/StyleResolverState.h"

#include "core/animation/css/CSSAnimations.h"
#include "core/dom/Node.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/frame/FrameHost.h"

namespace blink {

StyleResolverState::StyleResolverState(Document& document, const ElementResolveContext& elementContext, const ComputedStyle* parentStyle)
    : m_elementContext(elementContext)
    , m_document(document)
    , m_style(nullptr)
    // TODO(jchaffraix): We should make m_parentStyle const (https://crbug.com/468152)
    , m_parentStyle(const_cast<ComputedStyle*>(parentStyle))
    , m_applyPropertyToRegularStyle(true)
    , m_applyPropertyToVisitedLinkStyle(false)
    , m_hasDirAutoAttribute(false)
    , m_fontBuilder(document)
{
    if (!m_parentStyle) {
        // TODO(jchaffraix): We should make m_parentStyle const (https://crbug.com/468152)
        m_parentStyle = const_cast<ComputedStyle*>(m_elementContext.parentStyle());
    }

    ASSERT(document.isActive());
    m_elementStyleResources.setDeviceScaleFactor(document.frameHost()->deviceScaleFactor());
}

StyleResolverState::StyleResolverState(Document& document, Element* element, const ComputedStyle* parentStyle)
    : StyleResolverState(document, element ? ElementResolveContext(*element) : ElementResolveContext(document), parentStyle)
{
}

StyleResolverState::~StyleResolverState()
{
}

void StyleResolverState::setAnimationUpdate(PassOwnPtrWillBeRawPtr<CSSAnimationUpdate> update)
{
    m_animationUpdate = update;
}

PassOwnPtrWillBeRawPtr<CSSAnimationUpdate> StyleResolverState::takeAnimationUpdate()
{
    return m_animationUpdate.release();
}

} // namespace blink
