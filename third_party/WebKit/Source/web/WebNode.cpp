/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "public/web/WebNode.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/Node.h"
#include "core/dom/NodeList.h"
#include "core/dom/TagCollection.h"
#include "core/editing/markup.h"
#include "core/events/Event.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLElement.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutPart.h"
#include "modules/accessibility/AXObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "platform/Task.h"
#include "platform/Widget.h"
#include "public/platform/WebString.h"
#include "public/platform/WebSuspendableTask.h"
#include "public/web/WebAXObject.h"
#include "public/web/WebDOMEvent.h"
#include "public/web/WebDocument.h"
#include "public/web/WebElement.h"
#include "public/web/WebElementCollection.h"
#include "public/web/WebNodeList.h"
#include "public/web/WebPluginContainer.h"
#include "web/FrameLoaderClientImpl.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebPluginContainerImpl.h"

namespace blink {

namespace {

class NodeDispatchEventTask: public SuspendableTask {
    WTF_MAKE_NONCOPYABLE(NodeDispatchEventTask);
public:
    NodeDispatchEventTask(const WebPrivatePtr<Node>& node, WebDOMEvent event)
        : m_event(event)
    {
        m_node = node;
    }

    ~NodeDispatchEventTask()
    {
        m_node.reset();
    }

    void run() override
    {
        m_node->dispatchEvent(m_event);
    }
private:
    WebPrivatePtr<Node> m_node;
    WebDOMEvent m_event;
};

class NodeDispatchSimulatedClickTask: public SuspendableTask {
    WTF_MAKE_NONCOPYABLE(NodeDispatchSimulatedClickTask);
public:
    NodeDispatchSimulatedClickTask(const WebPrivatePtr<Node>& node)
    {
        m_node = node;
    }

    ~NodeDispatchSimulatedClickTask()
    {
        m_node.reset();
    }

    void run() override
    {
        m_node->dispatchSimulatedClick(nullptr);
    }
private:
    WebPrivatePtr<Node> m_node;
};

} // namespace

void WebNode::reset()
{
    m_private.reset();
}

void WebNode::assign(const WebNode& other)
{
    m_private = other.m_private;
}

bool WebNode::equals(const WebNode& n) const
{
    return m_private.get() == n.m_private.get();
}

bool WebNode::lessThan(const WebNode& n) const
{
    return m_private.get() < n.m_private.get();
}

WebNode::NodeType WebNode::nodeType() const
{
    return static_cast<NodeType>(m_private->nodeType());
}

WebNode WebNode::parentNode() const
{
    return WebNode(const_cast<ContainerNode*>(m_private->parentNode()));
}

WebString WebNode::nodeName() const
{
    return m_private->nodeName();
}

WebString WebNode::nodeValue() const
{
    return m_private->nodeValue();
}

WebDocument WebNode::document() const
{
    return WebDocument(&m_private->document());
}

WebNode WebNode::firstChild() const
{
    return WebNode(m_private->firstChild());
}

WebNode WebNode::lastChild() const
{
    return WebNode(m_private->lastChild());
}

WebNode WebNode::previousSibling() const
{
    return WebNode(m_private->previousSibling());
}

WebNode WebNode::nextSibling() const
{
    return WebNode(m_private->nextSibling());
}

bool WebNode::hasChildNodes() const
{
    return m_private->hasChildren();
}

WebNodeList WebNode::childNodes()
{
    return WebNodeList(m_private->childNodes());
}

WebString WebNode::createMarkup() const
{
    return blink::createMarkup(m_private.get());
}

bool WebNode::isLink() const
{
    return m_private->isLink();
}

bool WebNode::isTextNode() const
{
    return m_private->isTextNode();
}

bool WebNode::isDraggable() const
{
    Node* n = m_private.get();

    LayoutObject* obj = m_private->layoutObject();
    if (!obj)
        return false;

    const ComputedStyle* style = obj->style();
    if (!style)
        return false;

    return DraggableRegionDrag == style->getDraggableRegionMode();
}

bool WebNode::isFocusable() const
{
    if (!m_private->isElementNode())
        return false;
    m_private->document().updateLayoutIgnorePendingStylesheets();
    return toElement(m_private.get())->isFocusable();
}

bool WebNode::isContentEditable() const
{
    return m_private->isContentEditable();
}

bool WebNode::isInsideFocusableElementOrARIAWidget() const
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    return AXObject::isInsideFocusableElementOrARIAWidget(*this->constUnwrap<Node>());
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return false;
}

bool WebNode::isElementNode() const
{
    return m_private->isElementNode();
}

void WebNode::dispatchEvent(const WebDOMEvent& event)
{
    if (!event.isNull())
        m_private->executionContext()->postSuspendableTask(adoptPtr(new NodeDispatchEventTask(m_private, event)));
}

void WebNode::simulateClick()
{
    m_private->executionContext()->postSuspendableTask(adoptPtr(new NodeDispatchSimulatedClickTask(m_private)));
}

WebElementCollection WebNode::getElementsByHTMLTagName(const WebString& tag) const
{
    if (m_private->isContainerNode())
        return WebElementCollection(toContainerNode(m_private.get())->getElementsByTagNameNS(HTMLNames::xhtmlNamespaceURI, tag));
    return WebElementCollection();
}

WebElement WebNode::querySelector(const WebString& tag, WebExceptionCode& ec) const
{
    TrackExceptionState exceptionState;
    WebElement element;
    if (m_private->isContainerNode())
        element = toContainerNode(m_private.get())->querySelector(tag, exceptionState);
    ec = exceptionState.code();
    return element;
}

bool WebNode::focused() const
{
    return m_private->focused();
}

bool WebNode::remove()
{
    TrackExceptionState exceptionState;
    m_private->remove(exceptionState);
    return !exceptionState.hadException();
}

bool WebNode::hasNonEmptyBoundingBox() const
{
    m_private->document().updateLayoutIgnorePendingStylesheets();
    return m_private->hasNonEmptyBoundingBox();
}

bool WebNode::containsIncludingShadowDOM(const WebNode& other) const
{
    return m_private->containsIncludingShadowDOM(other.m_private.get());
}

WebPluginContainer* WebNode::pluginContainer() const
{
    if (isNull())
        return 0;
    const Node& coreNode = *constUnwrap<Node>();
    if (isHTMLObjectElement(coreNode) || isHTMLEmbedElement(coreNode)) {
        LayoutObject* object = coreNode.layoutObject();
        if (object && object->isLayoutPart()) {
            Widget* widget = toLayoutPart(object)->widget();
            if (widget && widget->isPluginContainer())
                return toWebPluginContainerImpl(widget);
        }
    }
    return 0;
}

#ifdef MINIBLINK_NOT_IMPLEMENTED
WebAXObject WebNode::accessibilityObject()
{
    WebDocument webDocument = document();
    const Document* doc = document().constUnwrap<Document>();
    AXObjectCacheImpl* cache = toAXObjectCacheImpl(doc->existingAXObjectCache());
    Node* node = unwrap<Node>();
    return cache ? WebAXObject(cache->get(node)) : WebAXObject();
}
#endif // MINIBLINK_NOT_IMPLEMENTED

WebNode::WebNode(const PassRefPtrWillBeRawPtr<Node>& node)
    : m_private(node)
{
}

WebNode& WebNode::operator=(const PassRefPtrWillBeRawPtr<Node>& node)
{
    m_private = node;
    return *this;
}

WebNode::operator PassRefPtrWillBeRawPtr<Node>() const
{
    return m_private.get();
}

} // namespace blink
