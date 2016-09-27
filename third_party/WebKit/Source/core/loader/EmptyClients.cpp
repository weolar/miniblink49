/*
 * Copyright (C) 2006 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008, 2009, 2012 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/loader/EmptyClients.h"

#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/forms/ColorChooser.h"
#include "core/html/forms/DateTimeChooser.h"
#include "core/loader/DocumentLoader.h"
#include "core/plugins/PluginPlaceholder.h"
#include "platform/FileChooser.h"
#include "platform/Widget.h"
#include "public/platform/WebApplicationCacheHost.h"
#include "public/platform/WebServiceWorkerProvider.h"
#include "public/platform/WebServiceWorkerProviderClient.h"

namespace blink {

void fillWithEmptyClients(Page::PageClients& pageClients)
{
    static ChromeClient* dummyChromeClient = adoptPtr(new EmptyChromeClient).leakPtr();
    pageClients.chromeClient = dummyChromeClient;

    static ContextMenuClient* dummyContextMenuClient = adoptPtr(new EmptyContextMenuClient).leakPtr();
    pageClients.contextMenuClient = dummyContextMenuClient;

    static DragClient* dummyDragClient = adoptPtr(new EmptyDragClient).leakPtr();
    pageClients.dragClient = dummyDragClient;

    static EditorClient* dummyEditorClient = adoptPtr(new EmptyEditorClient).leakPtr();
    pageClients.editorClient = dummyEditorClient;

    static SpellCheckerClient* dummySpellCheckerClient = adoptPtr(new EmptySpellCheckerClient).leakPtr();
    pageClients.spellCheckerClient = dummySpellCheckerClient;
}

class EmptyPopupMenu : public PopupMenu {
public:
    void show(const FloatQuad&, const IntSize&, int) override { }
    void hide() override { }
    void updateFromElement() override { }
    void disconnectClient() override { }
};

PassRefPtrWillBeRawPtr<PopupMenu> EmptyChromeClient::openPopupMenu(LocalFrame&, PopupMenuClient*)
{
    return adoptRefWillBeNoop(new EmptyPopupMenu());
}

PassOwnPtrWillBeRawPtr<ColorChooser> EmptyChromeClient::openColorChooser(LocalFrame*, ColorChooserClient*, const Color&)
{
    return nullptr;
}

PassRefPtr<DateTimeChooser> EmptyChromeClient::openDateTimeChooser(DateTimeChooserClient*, const DateTimeChooserParameters&)
{
    return PassRefPtr<DateTimeChooser>();
}

void EmptyChromeClient::openTextDataListChooser(HTMLInputElement&)
{
}

void EmptyChromeClient::openFileChooser(LocalFrame*, PassRefPtr<FileChooser>)
{
}

String EmptyChromeClient::acceptLanguages()
{
    return String();
}

NavigationPolicy EmptyFrameLoaderClient::decidePolicyForNavigation(const ResourceRequest&, DocumentLoader*, NavigationPolicy)
{
    return NavigationPolicyIgnore;
}

void EmptyFrameLoaderClient::dispatchWillSendSubmitEvent(HTMLFormElement*)
{
}

void EmptyFrameLoaderClient::dispatchWillSubmitForm(HTMLFormElement*)
{
}

PassRefPtrWillBeRawPtr<DocumentLoader> EmptyFrameLoaderClient::createDocumentLoader(LocalFrame* frame, const ResourceRequest& request, const SubstituteData& substituteData)
{
    return DocumentLoader::create(frame, request, substituteData);
}

PassRefPtrWillBeRawPtr<LocalFrame> EmptyFrameLoaderClient::createFrame(const FrameLoadRequest&, const AtomicString&, HTMLFrameOwnerElement*)
{
    return nullptr;
}

PassOwnPtrWillBeRawPtr<PluginPlaceholder> EmptyFrameLoaderClient::createPluginPlaceholder(Document&, const KURL&, const Vector<String>& paramNames, const Vector<String>& paramValues, const String& mimeType, bool loadManually)
{
    return nullptr;
}

PassRefPtrWillBeRawPtr<Widget> EmptyFrameLoaderClient::createPlugin(HTMLPlugInElement*, const KURL&, const Vector<String>&, const Vector<String>&, const String&, bool, DetachedPluginPolicy)
{
    return nullptr;
}

PassRefPtrWillBeRawPtr<Widget> EmptyFrameLoaderClient::createJavaAppletWidget(HTMLAppletElement*, const KURL&, const Vector<String>&, const Vector<String>&)
{
    return nullptr;
}

void EmptyTextCheckerClient::requestCheckingOfString(PassRefPtrWillBeRawPtr<TextCheckingRequest>)
{
}

void EmptyFrameLoaderClient::didRequestAutocomplete(HTMLFormElement*)
{
}

v8::Local<v8::Value> EmptyFrameLoaderClient::createTestInterface(const AtomicString& name)
{
    return v8::Local<v8::Value>();
}

PassOwnPtr<WebServiceWorkerProvider> EmptyFrameLoaderClient::createServiceWorkerProvider()
{
    return nullptr;
}

PassOwnPtr<WebApplicationCacheHost> EmptyFrameLoaderClient::createApplicationCacheHost(WebApplicationCacheHostClient*)
{
    return nullptr;
}

} // namespace blink
