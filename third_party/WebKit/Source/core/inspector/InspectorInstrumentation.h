/*
* Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef InspectorInstrumentation_h
#define InspectorInstrumentation_h

#include "bindings/core/v8/ScriptString.h"
#include "core/CoreExport.h"
#include "core/css/CSSSelector.h"
#include "core/css/CSSStyleDeclaration.h"
#include "core/css/CSSStyleSheet.h"
#include "core/dom/CharacterData.h"
#include "core/dom/Element.h"
#include "core/dom/ExecutionContext.h"
#include "core/events/NodeEventContext.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/ConsoleAPITypes.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutImage.h"
#include "core/page/ChromeClient.h"
#include "platform/network/FormData.h"
#include "platform/network/WebSocketHandshakeRequest.h"
#include "platform/network/WebSocketHandshakeResponse.h"
#include "wtf/RefPtr.h"

namespace blink {

class Document;
class EventTarget;
class ExecutionContext;
class InstrumentingAgents;
class ThreadableLoaderClient;
class WorkerGlobalScope;
class WorkerInspectorProxy;

#define FAST_RETURN_IF_NO_FRONTENDS(value) if (!hasFrontends()) return value;

class CORE_EXPORT InspectorInstrumentationCookie {
    STACK_ALLOCATED();
public:
    InspectorInstrumentationCookie();
    explicit InspectorInstrumentationCookie(InstrumentingAgents*);
    InspectorInstrumentationCookie(const InspectorInstrumentationCookie&);
    InspectorInstrumentationCookie& operator=(const InspectorInstrumentationCookie&);
    ~InspectorInstrumentationCookie();

    InstrumentingAgents* instrumentingAgents() const { return m_instrumentingAgents.get(); }
    bool isValid() const { return !!m_instrumentingAgents; }

private:
    RefPtrWillBeMember<InstrumentingAgents> m_instrumentingAgents;
};

namespace InspectorInstrumentation {

class CORE_EXPORT FrontendCounter {
private:
    friend void frontendCreated();
    friend void frontendDeleted();
    friend bool hasFrontends();
    static int s_frontendCounter;
};

inline void frontendCreated() { atomicIncrement(&FrontendCounter::s_frontendCounter); }
inline void frontendDeleted() { atomicDecrement(&FrontendCounter::s_frontendCounter); }
inline bool hasFrontends() { return FrontendCounter::s_frontendCounter; }

CORE_EXPORT void registerInstrumentingAgents(InstrumentingAgents*);
CORE_EXPORT void unregisterInstrumentingAgents(InstrumentingAgents*);

// Called from generated instrumentation code.
CORE_EXPORT InstrumentingAgents* instrumentingAgentsFor(LocalFrame*);
InstrumentingAgents* instrumentingAgentsFor(EventTarget*);
InstrumentingAgents* instrumentingAgentsFor(ExecutionContext*);
InstrumentingAgents* instrumentingAgentsFor(Document&);
InstrumentingAgents* instrumentingAgentsFor(Document*);
InstrumentingAgents* instrumentingAgentsFor(LayoutObject*);
InstrumentingAgents* instrumentingAgentsFor(Node*);
InstrumentingAgents* instrumentingAgentsFor(WorkerGlobalScope*);

// Helper for the one above.
CORE_EXPORT InstrumentingAgents* instrumentingAgentsForNonDocumentContext(ExecutionContext*);

}  // namespace InspectorInstrumentation

namespace InstrumentationEvents {
extern const char PaintSetup[];
extern const char Paint[];
extern const char Layer[];
extern const char RequestMainThreadFrame[];
extern const char BeginFrame[];
extern const char DrawFrame[];
extern const char ActivateLayerTree[];
extern const char EmbedderCallback[];
};

namespace InstrumentationEventArguments {
extern const char FrameId[];
extern const char LayerId[];
extern const char LayerTreeId[];
extern const char PageId[];
extern const char CallbackName[];
};

namespace InspectorInstrumentation {

inline InstrumentingAgents* instrumentingAgentsFor(ExecutionContext* context)
{
    if (!context)
        return 0;
    return context->isDocument() ? instrumentingAgentsFor(*toDocument(context)) : instrumentingAgentsForNonDocumentContext(context);
}

inline InstrumentingAgents* instrumentingAgentsFor(Document& document)
{
    LocalFrame* frame = document.frame();
    if (!frame && document.templateDocumentHost())
        frame = document.templateDocumentHost()->frame();
    return instrumentingAgentsFor(frame);
}

inline InstrumentingAgents* instrumentingAgentsFor(Document* document)
{
    return document ? instrumentingAgentsFor(*document) : 0;
}

inline InstrumentingAgents* instrumentingAgentsFor(CSSStyleSheet* styleSheet)
{
    return styleSheet ? instrumentingAgentsFor(styleSheet->ownerDocument()) : 0;
}

inline InstrumentingAgents* instrumentingAgentsFor(Node* node)
{
    return node ? instrumentingAgentsFor(node->document()) : 0;
}

inline InstrumentingAgents* instrumentingAgentsFor(CSSStyleDeclaration* declaration)
{
    return declaration ? instrumentingAgentsFor(declaration->parentStyleSheet()) : 0;
}

} // namespace InspectorInstrumentation

InstrumentingAgents* instrumentationForWorkerGlobalScope(WorkerGlobalScope*);

} // namespace blink

#include "core/InspectorInstrumentationInl.h"

#include "core/inspector/InspectorInstrumentationCustomInl.h"

#include "core/InspectorOverridesInl.h"

#endif // !defined(InspectorInstrumentation_h)
