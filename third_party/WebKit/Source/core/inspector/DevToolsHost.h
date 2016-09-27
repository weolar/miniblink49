/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
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

#ifndef DevToolsHost_h
#define DevToolsHost_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "wtf/RefCounted.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ContextMenuItem;
class Event;
class FrontendMenuProvider;
class InspectorFrontendClient;
class LocalFrame;

class CORE_EXPORT DevToolsHost : public RefCountedWillBeGarbageCollectedFinalized<DevToolsHost>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<DevToolsHost> create(InspectorFrontendClient* client, LocalFrame* frontendFrame)
    {
        return adoptRefWillBeNoop(new DevToolsHost(client, frontendFrame));
    }

    ~DevToolsHost();
    DECLARE_TRACE();
    void disconnectClient();

    float zoomFactor();

    void setInjectedScriptForOrigin(const String& origin, const String& script);

    void copyText(const String& text);

    // Called from [Custom] implementations.
    void showContextMenu(Event*, const Vector<ContextMenuItem>& items);
    void showContextMenu(LocalFrame* targetFrame, float x, float y, const Vector<ContextMenuItem>& items);
    void sendMessageToBackend(const String& message);
    void sendMessageToEmbedder(const String& message);

    String getSelectionBackgroundColor();
    String getSelectionForegroundColor();

    bool isUnderTest();
    bool isHostedMode();

    LocalFrame* frontendFrame() { return m_frontendFrame; }

    void clearMenuProvider() { m_menuProvider = nullptr; }

private:
    DevToolsHost(InspectorFrontendClient*, LocalFrame* frontendFrame);

    InspectorFrontendClient* m_client;
    RawPtrWillBeMember<LocalFrame> m_frontendFrame;
    RawPtrWillBeMember<FrontendMenuProvider> m_menuProvider;
};

} // namespace blink

#endif // DevToolsHost_h
