/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Matt Lilek <webkit@mattlilek.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/inspector/DevToolsHost.h"

#include "bindings/core/v8/ScriptFunctionCall.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/clipboard/Pasteboard.h"
#include "core/dom/ExecutionContext.h"
#include "core/events/Event.h"
#include "core/events/EventTarget.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/inspector/InspectorFrontendClient.h"
#include "core/layout/LayoutTheme.h"
#include "core/loader/FrameLoader.h"
#include "core/page/ContextMenuController.h"
#include "core/page/ContextMenuProvider.h"
#include "core/page/Page.h"
#include "platform/ContextMenu.h"
#include "platform/ContextMenuItem.h"
#include "platform/SharedBuffer.h"
#include "platform/UserGestureIndicator.h"
#include "platform/network/ResourceError.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"

namespace blink {

class FrontendMenuProvider final : public ContextMenuProvider {
public:
    static PassRefPtrWillBeRawPtr<FrontendMenuProvider> create(DevToolsHost* devtoolsHost, ScriptValue devtoolsApiObject, const Vector<ContextMenuItem>& items)
    {
        return adoptRefWillBeNoop(new FrontendMenuProvider(devtoolsHost, devtoolsApiObject, items));
    }

    virtual ~FrontendMenuProvider()
    {
        // Verify that this menu provider has been detached.
        ASSERT(!m_devtoolsHost);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_devtoolsHost);
        ContextMenuProvider::trace(visitor);
    }

    void disconnect()
    {
        m_devtoolsApiObject = ScriptValue();
        m_devtoolsHost = nullptr;
    }

    virtual void contextMenuCleared() override
    {
        if (m_devtoolsHost) {
            ScriptFunctionCall function(m_devtoolsApiObject, "contextMenuCleared");
            function.call();

            m_devtoolsHost->clearMenuProvider();
            m_devtoolsHost = nullptr;
        }
        m_items.clear();
    }

    virtual void populateContextMenu(ContextMenu* menu) override
    {
        for (size_t i = 0; i < m_items.size(); ++i)
            menu->appendItem(m_items[i]);
    }

    virtual void contextMenuItemSelected(const ContextMenuItem* item) override
    {
        if (!m_devtoolsHost)
            return;

        UserGestureIndicator gestureIndicator(DefinitelyProcessingNewUserGesture);
        int itemNumber = item->action() - ContextMenuItemBaseCustomTag;

        ScriptFunctionCall function(m_devtoolsApiObject, "contextMenuItemSelected");
        function.appendArgument(itemNumber);
        function.call();
    }

private:
    FrontendMenuProvider(DevToolsHost* devtoolsHost, ScriptValue devtoolsApiObject, const Vector<ContextMenuItem>& items)
        : m_devtoolsHost(devtoolsHost)
        , m_devtoolsApiObject(devtoolsApiObject)
        , m_items(items)
    {
    }

    RawPtrWillBeMember<DevToolsHost> m_devtoolsHost;
    ScriptValue m_devtoolsApiObject;

    Vector<ContextMenuItem> m_items;
};

DevToolsHost::DevToolsHost(InspectorFrontendClient* client, LocalFrame* frontendFrame)
    : m_client(client)
    , m_frontendFrame(frontendFrame)
    , m_menuProvider(nullptr)
{
}

DevToolsHost::~DevToolsHost()
{
    ASSERT(!m_client);
}

DEFINE_TRACE(DevToolsHost)
{
    visitor->trace(m_frontendFrame);
    visitor->trace(m_menuProvider);
}

void DevToolsHost::disconnectClient()
{
    m_client = 0;
    if (m_menuProvider) {
        m_menuProvider->disconnect();
        m_menuProvider = nullptr;
    }
    m_frontendFrame = nullptr;
}

float DevToolsHost::zoomFactor()
{
    return m_frontendFrame ? m_frontendFrame->pageZoomFactor() : 1;
}

void DevToolsHost::setInjectedScriptForOrigin(const String& origin, const String& script)
{
    if (m_client)
        m_client->setInjectedScriptForOrigin(origin, script);
}

void DevToolsHost::copyText(const String& text)
{
    Pasteboard::generalPasteboard()->writePlainText(text, Pasteboard::CannotSmartReplace);
}

static String escapeUnicodeNonCharacters(const String& str)
{
    const UChar nonChar = 0xD800;

    unsigned i = 0;
    while (i < str.length() && str[i] < nonChar)
        ++i;
    if (i == str.length())
        return str;

    StringBuilder dst;
    dst.append(str, 0, i);
    for (; i < str.length(); ++i) {
        UChar c = str[i];
        if (c >= nonChar) {
            unsigned symbol = static_cast<unsigned>(c);
            String symbolCode = String::format("\\u%04X", symbol);
            dst.append(symbolCode);
        } else {
            dst.append(c);
        }
    }
    return dst.toString();
}

void DevToolsHost::sendMessageToBackend(const String& message)
{
    if (m_client)
        m_client->sendMessageToBackend(escapeUnicodeNonCharacters(message));
}

void DevToolsHost::sendMessageToEmbedder(const String& message)
{
    if (m_client)
        m_client->sendMessageToEmbedder(escapeUnicodeNonCharacters(message));
}

void DevToolsHost::showContextMenu(LocalFrame* targetFrame, float x, float y, const Vector<ContextMenuItem>& items)
{
    ASSERT(m_frontendFrame);
    ScriptState* frontendScriptState = ScriptState::forMainWorld(m_frontendFrame);
    ScriptValue devtoolsApiObject = frontendScriptState->getFromGlobalObject("DevToolsAPI");
    ASSERT(devtoolsApiObject.isObject());

    RefPtrWillBeRawPtr<FrontendMenuProvider> menuProvider = FrontendMenuProvider::create(this, devtoolsApiObject, items);
    m_menuProvider = menuProvider.get();
    float zoom = targetFrame->pageZoomFactor();
    if (m_client)
        m_client->showContextMenu(targetFrame, x * zoom, y * zoom, menuProvider);
}

void DevToolsHost::showContextMenu(Event* event, const Vector<ContextMenuItem>& items)
{
    if (!event)
        return;

    ASSERT(m_frontendFrame);
    ScriptState* frontendScriptState = ScriptState::forMainWorld(m_frontendFrame);
    ScriptValue devtoolsApiObject = frontendScriptState->getFromGlobalObject("DevToolsAPI");
    ASSERT(devtoolsApiObject.isObject());

    Page* targetPage = m_frontendFrame->page();
    if (event->target() && event->target()->executionContext() && event->target()->executionContext()->executingWindow()) {
        LocalDOMWindow* window = event->target()->executionContext()->executingWindow();
        if (window->document() && window->document()->page())
            targetPage = window->document()->page();
    }

    RefPtrWillBeRawPtr<FrontendMenuProvider> menuProvider = FrontendMenuProvider::create(this, devtoolsApiObject, items);
    targetPage->contextMenuController().showContextMenu(event, menuProvider);
    m_menuProvider = menuProvider.get();
}

String DevToolsHost::getSelectionBackgroundColor()
{
    return LayoutTheme::theme().activeSelectionBackgroundColor().serialized();
}

String DevToolsHost::getSelectionForegroundColor()
{
    return LayoutTheme::theme().activeSelectionForegroundColor().serialized();
}

bool DevToolsHost::isUnderTest()
{
    return m_client && m_client->isUnderTest();
}

bool DevToolsHost::isHostedMode()
{
    return false;
}

} // namespace blink
