/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "core/dom/LayoutTreeBuilder.h"

#include "core/HTMLNames.h"
#include "core/SVGNames.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/FirstLetterPseudoElement.h"
#include "core/dom/Fullscreen.h"
#include "core/dom/Node.h"
#include "core/dom/PseudoElement.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/layout/LayoutFullScreen.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutText.h"
#include "core/layout/LayoutView.h"
#include "core/svg/SVGElement.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

LayoutTreeBuilderForElement::LayoutTreeBuilderForElement(Element& element, ComputedStyle* style)
    : LayoutTreeBuilder(element, nullptr)
    , m_style(style)
{
    ASSERT(!isActiveInsertionPoint(element));
    if (element.isFirstLetterPseudoElement()) {
        if (LayoutObject* nextLayoutObject = FirstLetterPseudoElement::firstLetterTextLayoutObject(element))
            m_layoutObjectParent = nextLayoutObject->parent();
    } else if (ContainerNode* containerNode = LayoutTreeBuilderTraversal::parent(element)) {
        m_layoutObjectParent = containerNode->layoutObject();
    }
}

LayoutObject* LayoutTreeBuilderForElement::nextLayoutObject() const
{
    ASSERT(m_layoutObjectParent);

    if (m_node->isInTopLayer())
        return LayoutTreeBuilderTraversal::nextInTopLayer(*m_node);

    if (m_node->isFirstLetterPseudoElement())
        return FirstLetterPseudoElement::firstLetterTextLayoutObject(*m_node);

    return LayoutTreeBuilder::nextLayoutObject();
}

LayoutObject* LayoutTreeBuilderForElement::parentLayoutObject() const
{
    if (m_layoutObjectParent) {
        // FIXME: Guarding this by parentLayoutObject isn't quite right as the spec for
        // top layer only talks about display: none ancestors so putting a <dialog> inside an
        // <optgroup> seems like it should still work even though this check will prevent it.
        if (m_node->isInTopLayer())
            return m_node->document().layoutView();
    }

    return m_layoutObjectParent;
}

bool LayoutTreeBuilderForElement::shouldCreateLayoutObject() const
{
    if (!m_layoutObjectParent)
        return false;

    // FIXME: Should the following be in SVGElement::layoutObjectIsNeeded()?
    if (m_node->isSVGElement()) {
        // SVG elements only render when inside <svg>, or if the element is an <svg> itself.
        if (!isSVGSVGElement(*m_node) && (!m_layoutObjectParent->node() || !m_layoutObjectParent->node()->isSVGElement()))
            return false;
        if (!toSVGElement(m_node)->isValid())
            return false;
    }

    LayoutObject* parentLayoutObject = this->parentLayoutObject();
    if (!parentLayoutObject)
        return false;
    if (!parentLayoutObject->canHaveChildren())
        return false;

    return m_node->layoutObjectIsNeeded(style());
}

ComputedStyle& LayoutTreeBuilderForElement::style() const
{
    if (!m_style)
        m_style = m_node->styleForLayoutObject();
    return *m_style;
}

void LayoutTreeBuilderForElement::createLayoutObject()
{
    ComputedStyle& style = this->style();

    LayoutObject* newLayoutObject = m_node->createLayoutObject(style);
    if (!newLayoutObject)
        return;

    LayoutObject* parentLayoutObject = this->parentLayoutObject();

    if (!parentLayoutObject->isChildAllowed(newLayoutObject, style)) {
        newLayoutObject->destroy();
        return;
    }

    // Make sure the LayoutObject already knows it is going to be added to a LayoutFlowThread before we set the style
    // for the first time. Otherwise code using inLayoutFlowThread() in the styleWillChange and styleDidChange will fail.
    newLayoutObject->setIsInsideFlowThread(parentLayoutObject->isInsideFlowThread());

    LayoutObject* nextLayoutObject = this->nextLayoutObject();
    m_node->setLayoutObject(newLayoutObject);
    newLayoutObject->setStyle(&style); // setStyle() can depend on layoutObject() already being set.

    if (Fullscreen::isActiveFullScreenElement(*m_node)) {
        newLayoutObject = LayoutFullScreen::wrapLayoutObject(newLayoutObject, parentLayoutObject, &m_node->document());
        if (!newLayoutObject)
            return;
    }

    // Note: Adding newLayoutObject instead of layoutObject(). layoutObject() may be a child of newLayoutObject.
    parentLayoutObject->addChild(newLayoutObject, nextLayoutObject);
}

void LayoutTreeBuilderForText::createLayoutObject()
{
    ComputedStyle& style = m_layoutObjectParent->mutableStyleRef();

    ASSERT(m_node->textLayoutObjectIsNeeded(style, *m_layoutObjectParent));

    LayoutText* newLayoutObject = m_node->createTextLayoutObject(style);
    if (!m_layoutObjectParent->isChildAllowed(newLayoutObject, style)) {
        newLayoutObject->destroy();
        return;
    }

    // Make sure the LayoutObject already knows it is going to be added to a LayoutFlowThread before we set the style
    // for the first time. Otherwise code using inLayoutFlowThread() in the styleWillChange and styleDidChange will fail.
    newLayoutObject->setIsInsideFlowThread(m_layoutObjectParent->isInsideFlowThread());

    LayoutObject* nextLayoutObject = this->nextLayoutObject();
    m_node->setLayoutObject(newLayoutObject);
    // Parent takes care of the animations, no need to call setAnimatableStyle.
    newLayoutObject->setStyle(&style);
    m_layoutObjectParent->addChild(newLayoutObject, nextLayoutObject);
}

}
