/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef InspectorBaseAgent_h
#define InspectorBaseAgent_h

#include "core/CoreExport.h"
#include "core/InspectorBackendDispatcher.h"
#include "core/inspector/InstrumentingAgents.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class InspectorFrontend;
class InspectorCompositeState;
class InspectorState;
class InstrumentingAgents;
class LocalFrame;

class CORE_EXPORT InspectorAgent : public NoBaseWillBeGarbageCollectedFinalized<InspectorAgent> {
public:
    explicit InspectorAgent(const String&);
    virtual ~InspectorAgent();
    DECLARE_VIRTUAL_TRACE();

    virtual void init() { }
    virtual void setFrontend(InspectorFrontend*) = 0;
    virtual void clearFrontend() = 0;
    virtual void disable(ErrorString*) { }
    virtual void restore() { }
    virtual void registerInDispatcher(InspectorBackendDispatcher*) = 0;
    virtual void discardAgent() { }
    virtual void didCommitLoadForLocalFrame(LocalFrame*) { }
    virtual void flushPendingProtocolNotifications() { }

    String name() const { return m_name; }
    void appended(InstrumentingAgents*, InspectorState*);

protected:
    RawPtrWillBeMember<InstrumentingAgents> m_instrumentingAgents;
    RawPtrWillBeMember<InspectorState> m_state;

private:
    String m_name;
};

class CORE_EXPORT InspectorAgentRegistry final {
    DISALLOW_ALLOCATION();
    WTF_MAKE_NONCOPYABLE(InspectorAgentRegistry);
public:
    InspectorAgentRegistry(InstrumentingAgents*, InspectorCompositeState*);
    void append(PassOwnPtrWillBeRawPtr<InspectorAgent>);

    void setFrontend(InspectorFrontend*);
    void clearFrontend();
    void restore();
    void registerInDispatcher(InspectorBackendDispatcher*);
    void discardAgents();
    void flushPendingProtocolNotifications();
    void didCommitLoadForLocalFrame(LocalFrame*);

    DECLARE_TRACE();

private:
    RawPtrWillBeMember<InstrumentingAgents> m_instrumentingAgents;
    RawPtrWillBeMember<InspectorCompositeState> m_inspectorState;
    WillBeHeapVector<OwnPtrWillBeMember<InspectorAgent> > m_agents;
};

template<typename AgentClass, typename FrontendClass>
class InspectorBaseAgent : public InspectorAgent {
public:
    virtual ~InspectorBaseAgent() { }

    void setFrontend(InspectorFrontend* frontend) override final
    {
        ASSERT(!m_frontend);
        m_frontend = FrontendClass::from(frontend);
    }

    void clearFrontend() override final
    {
        ErrorString error;
        disable(&error);
        ASSERT(m_frontend);
        m_frontend = nullptr;
    }

    virtual void registerInDispatcher(InspectorBackendDispatcher* dispatcher) override final
    {
        dispatcher->registerAgent(static_cast<AgentClass*>(this));
    }

protected:
    explicit InspectorBaseAgent(const String& name)
        : InspectorAgent(name)
        , m_frontend(nullptr)
    {
    }

    FrontendClass* frontend() const { return m_frontend; }

private:
    FrontendClass* m_frontend;
};

inline bool asBool(const bool* const b)
{
    return b ? *b : false;
}

} // namespace blink

#endif // !defined(InspectorBaseAgent_h)
