/*
 * Copyright (C) 2006, 2007, 2009 Apple Inc. All rights reserved.
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
#include "core/html/HTMLFrameOwnerElement.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/ExceptionCode.h"
#include "core/events/Event.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/layout/LayoutPart.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/plugins/PluginView.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

typedef WillBeHeapHashMap<RefPtrWillBeMember<Widget>, RawPtrWillBeMember<FrameView>> WidgetToParentMap;
static WidgetToParentMap& widgetNewParentMap()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<WidgetToParentMap>, map, (adoptPtrWillBeNoop(new WidgetToParentMap())));
    return *map;
}

WillBeHeapHashCountedSet<RawPtrWillBeMember<Node>>& SubframeLoadingDisabler::disabledSubtreeRoots()
{
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<WillBeHeapHashCountedSet<RawPtrWillBeMember<Node>>>, nodes, (adoptPtrWillBeNoop(new WillBeHeapHashCountedSet<RawPtrWillBeMember<Node>>())));
    return *nodes;
}

static unsigned s_updateSuspendCount = 0;

HTMLFrameOwnerElement::UpdateSuspendScope::UpdateSuspendScope()
{
    ++s_updateSuspendCount;
}

void HTMLFrameOwnerElement::UpdateSuspendScope::performDeferredWidgetTreeOperations()
{
    WidgetToParentMap map;
    widgetNewParentMap().swap(map);
    for (const auto& widget : map) {
        Widget* child = widget.key.get();
        FrameView* currentParent = toFrameView(child->parent());
        FrameView* newParent = widget.value;
        if (newParent != currentParent) {
            if (currentParent)
                currentParent->removeChild(child);
            if (newParent)
                newParent->addChild(child);
#if ENABLE(OILPAN)
            if (currentParent && !newParent)
                child->dispose();
#endif
        }
    }
}

HTMLFrameOwnerElement::UpdateSuspendScope::~UpdateSuspendScope()
{
    ASSERT(s_updateSuspendCount > 0);
    if (s_updateSuspendCount == 1)
        performDeferredWidgetTreeOperations();
    --s_updateSuspendCount;
}

static void moveWidgetToParentSoon(Widget* child, FrameView* parent)
{
    if (!s_updateSuspendCount) {
        if (parent) {
            parent->addChild(child);
        } else if (toFrameView(child->parent())) {
            toFrameView(child->parent())->removeChild(child);
#if ENABLE(OILPAN)
            child->dispose();
#endif
        }
        return;
    }
    widgetNewParentMap().set(child, parent);
}

HTMLFrameOwnerElement::HTMLFrameOwnerElement(const QualifiedName& tagName, Document& document)
    : HTMLElement(tagName, document)
    , m_contentFrame(nullptr)
    , m_widget(nullptr)
    , m_sandboxFlags(SandboxNone)
{
}

LayoutPart* HTMLFrameOwnerElement::layoutPart() const
{
    // HTMLObjectElement and HTMLEmbedElement may return arbitrary layoutObjects
    // when using fallback content.
    if (!layoutObject() || !layoutObject()->isLayoutPart())
        return nullptr;
    return toLayoutPart(layoutObject());
}

void HTMLFrameOwnerElement::setContentFrame(Frame& frame)
{
    // Make sure we will not end up with two frames referencing the same owner element.
    ASSERT(!m_contentFrame || m_contentFrame->owner() != this);
    // Disconnected frames should not be allowed to load.
    ASSERT(inDocument());
    m_contentFrame = &frame;

    for (ContainerNode* node = this; node; node = node->parentOrShadowHostNode())
        node->incrementConnectedSubframeCount();
}

void HTMLFrameOwnerElement::clearContentFrame()
{
    if (!m_contentFrame)
        return;

    m_contentFrame = nullptr;

    for (ContainerNode* node = this; node; node = node->parentOrShadowHostNode())
        node->decrementConnectedSubframeCount();
}

void HTMLFrameOwnerElement::disconnectContentFrame()
{
    // FIXME: Currently we don't do this in removedFrom because this causes an
    // unload event in the subframe which could execute script that could then
    // reach up into this document and then attempt to look back down. We should
    // see if this behavior is really needed as Gecko does not allow this.
    if (RefPtrWillBeRawPtr<Frame> frame = contentFrame()) {
        frame->detach(FrameDetachType::Remove);
    }
#if ENABLE(OILPAN)
    // Oilpan: a plugin container must be explicitly disposed before it
    // is swept and finalized. This is because the underlying plugin needs
    // to be able to access a fully-functioning frame (and all it refers
    // to) while it destructs and cleans out its resources.
    if (m_widget) {
        m_widget->dispose();
        m_widget = nullptr;
    }
#endif
}

HTMLFrameOwnerElement::~HTMLFrameOwnerElement()
{
    // An owner must by now have been informed of detachment
    // when the frame was closed.
    ASSERT(!m_contentFrame);
}

Document* HTMLFrameOwnerElement::contentDocument() const
{
    return (m_contentFrame && m_contentFrame->isLocalFrame()) ? toLocalFrame(m_contentFrame)->document() : 0;
}

DOMWindow* HTMLFrameOwnerElement::contentWindow() const
{
    return m_contentFrame ? m_contentFrame->domWindow() : 0;
}

void HTMLFrameOwnerElement::setSandboxFlags(SandboxFlags flags)
{
    m_sandboxFlags = flags;
    // Don't notify about updates if contentFrame() is null, for example when
    // the subframe hasn't been created yet.
    if (contentFrame())
        document().frame()->loader().client()->didChangeSandboxFlags(contentFrame(), flags);
}

bool HTMLFrameOwnerElement::isKeyboardFocusable() const
{
    return m_contentFrame && HTMLElement::isKeyboardFocusable();
}

void HTMLFrameOwnerElement::dispatchLoad()
{
    dispatchEvent(Event::create(EventTypeNames::load));
}

Document* HTMLFrameOwnerElement::getSVGDocument(ExceptionState& exceptionState) const
{
    Document* doc = contentDocument();
    if (doc && doc->isSVGDocument())
        return doc;
    return nullptr;
}

void HTMLFrameOwnerElement::setWidget(PassRefPtrWillBeRawPtr<Widget> widget)
{
    if (widget == m_widget)
        return;

    if (m_widget) {
        if (m_widget->parent())
            moveWidgetToParentSoon(m_widget.get(), 0);
        m_widget = nullptr;
    }

    m_widget = widget;

    LayoutPart* layoutPart = toLayoutPart(layoutObject());
    if (!layoutPart)
        return;

    if (m_widget) {
        layoutPart->updateOnWidgetChange();

        ASSERT(document().view() == layoutPart->frameView());
        ASSERT(layoutPart->frameView());
        moveWidgetToParentSoon(m_widget.get(), layoutPart->frameView());
    }

    if (AXObjectCache* cache = document().existingAXObjectCache())
        cache->childrenChanged(layoutPart);
}

Widget* HTMLFrameOwnerElement::ownedWidget() const
{
    return m_widget.get();
}

bool HTMLFrameOwnerElement::loadOrRedirectSubframe(const KURL& url, const AtomicString& frameName, bool lockBackForwardList)
{
    RefPtrWillBeRawPtr<LocalFrame> parentFrame = document().frame();
    if (contentFrame()) {
        contentFrame()->navigate(document(), url, lockBackForwardList, UserGestureStatus::None);
        return true;
    }

    if (!document().securityOrigin()->canDisplay(url)) {
        FrameLoader::reportLocalLoadFailed(parentFrame.get(), url.string());
        return false;
    }

    if (!SubframeLoadingDisabler::canLoadFrame(*this))
        return false;

    if (document().frame()->host()->subframeCount() >= FrameHost::maxNumberOfFrames)
        return false;

    return parentFrame->loader().client()->createFrame(FrameLoadRequest(&document(), url, "_self", CheckContentSecurityPolicy), frameName, this);
}

DEFINE_TRACE(HTMLFrameOwnerElement)
{
    visitor->trace(m_contentFrame);
    visitor->trace(m_widget);
    HTMLElement::trace(visitor);
    FrameOwner::trace(visitor);
}


} // namespace blink
