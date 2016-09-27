/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
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
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/PluginDocument.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/dom/RawDataDocumentParser.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLEmbedElement.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/layout/LayoutEmbeddedObject.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/plugins/PluginView.h"

namespace blink {

using namespace HTMLNames;

// FIXME: Share more code with MediaDocumentParser.
class PluginDocumentParser : public RawDataDocumentParser {
public:
    static PassRefPtrWillBeRawPtr<PluginDocumentParser> create(PluginDocument* document)
    {
        return adoptRefWillBeNoop(new PluginDocumentParser(document));
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_embedElement);
        RawDataDocumentParser::trace(visitor);
    }

private:
    PluginDocumentParser(Document* document)
        : RawDataDocumentParser(document)
        , m_embedElement(nullptr)
    {
    }

    void appendBytes(const char*, size_t) override;

    void finish() override;

    void createDocumentStructure();

    PluginView* pluginView() const;

    RefPtrWillBeMember<HTMLEmbedElement> m_embedElement;
};

void PluginDocumentParser::createDocumentStructure()
{
    // FIXME: Assert we have a loader to figure out why the original null checks
    // and assert were added for the security bug in http://trac.webkit.org/changeset/87566
    ASSERT(document());
    RELEASE_ASSERT(document()->loader());

    LocalFrame* frame = document()->frame();
    if (!frame)
        return;

    // FIXME: Why does this check settings?
    if (!frame->settings() || !frame->loader().allowPlugins(NotAboutToInstantiatePlugin))
        return;

    RefPtrWillBeRawPtr<HTMLHtmlElement> rootElement = HTMLHtmlElement::create(*document());
    rootElement->insertedByParser();
    document()->appendChild(rootElement);
    frame->loader().dispatchDocumentElementAvailable();

    RefPtrWillBeRawPtr<HTMLBodyElement> body = HTMLBodyElement::create(*document());
    body->setAttribute(styleAttr, "background-color: rgb(38,38,38); height: 100%; width: 100%; overflow: hidden; margin: 0");
    rootElement->appendChild(body);

    m_embedElement = HTMLEmbedElement::create(*document());
    m_embedElement->setAttribute(widthAttr, "100%");
    m_embedElement->setAttribute(heightAttr, "100%");
    m_embedElement->setAttribute(nameAttr, "plugin");
    m_embedElement->setAttribute(srcAttr, AtomicString(document()->url().string()));
    m_embedElement->setAttribute(typeAttr, document()->loader()->mimeType());
    body->appendChild(m_embedElement);

    toPluginDocument(document())->setPluginNode(m_embedElement.get());

    document()->updateLayout();

    // We need the plugin to load synchronously so we can get the PluginView
    // below so flush the layout tasks now instead of waiting on the timer.
    frame->view()->flushAnyPendingPostLayoutTasks();

    if (PluginView* view = pluginView())
        view->didReceiveResponse(document()->loader()->response());
}

void PluginDocumentParser::appendBytes(const char* data, size_t length)
{
    if (!m_embedElement)
        createDocumentStructure();

    if (!length)
        return;
    if (PluginView* view = pluginView())
        view->didReceiveData(data, length);
}

void PluginDocumentParser::finish()
{
    if (PluginView* view = pluginView()) {
        const ResourceError& error = document()->loader()->mainDocumentError();
        if (error.isNull())
            view->didFinishLoading();
        else
            view->didFailLoading(error);
        m_embedElement = nullptr;
    }
    RawDataDocumentParser::finish();
}

PluginView* PluginDocumentParser::pluginView() const
{
    if (Widget* widget = toPluginDocument(document())->pluginWidget()) {
        ASSERT_WITH_SECURITY_IMPLICATION(widget->isPluginContainer());
        return toPluginView(widget);
    }
    return 0;
}

PluginDocument::PluginDocument(const DocumentInit& initializer)
    : HTMLDocument(initializer, PluginDocumentClass)
{
    setCompatibilityMode(QuirksMode);
    lockCompatibilityMode();
}

PassRefPtrWillBeRawPtr<DocumentParser> PluginDocument::createParser()
{
    return PluginDocumentParser::create(this);
}

Widget* PluginDocument::pluginWidget()
{
    if (m_pluginNode && m_pluginNode->layoutObject()) {
        ASSERT(m_pluginNode->layoutObject()->isEmbeddedObject());
        return toLayoutEmbeddedObject(m_pluginNode->layoutObject())->widget();
    }
    return 0;
}

Node* PluginDocument::pluginNode()
{
    return m_pluginNode.get();
}

void PluginDocument::detach(const AttachContext& context)
{
    // Release the plugin node so that we don't have a circular reference.
    m_pluginNode = nullptr;
    HTMLDocument::detach(context);
}

DEFINE_TRACE(PluginDocument)
{
    visitor->trace(m_pluginNode);
    HTMLDocument::trace(visitor);
}

}
