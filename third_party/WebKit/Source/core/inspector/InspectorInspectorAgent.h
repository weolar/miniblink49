/*
 * Copyright (C) 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef InspectorInspectorAgent_h
#define InspectorInspectorAgent_h

#include "core/CoreExport.h"
#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class LocalFrame;
class InjectedScriptManager;
class JSONObject;

typedef String ErrorString;

class CORE_EXPORT InspectorInspectorAgent final : public InspectorBaseAgent<InspectorInspectorAgent, InspectorFrontend::Inspector>, public InspectorBackendDispatcher::InspectorCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorInspectorAgent);
public:
    static PassOwnPtrWillBeRawPtr<InspectorInspectorAgent> create(InjectedScriptManager* injectedScriptManager)
    {
        return adoptPtrWillBeNoop(new InspectorInspectorAgent(injectedScriptManager));
    }

    virtual ~InspectorInspectorAgent();
    DECLARE_VIRTUAL_TRACE();

    // Inspector front-end API.
    void enable(ErrorString*) override;

    // InspectorAgent overrides.
    void disable(ErrorString*) override;
    void didCommitLoadForLocalFrame(LocalFrame*) override;
    void restore() override;

    // Generic code called from custom implementations.
    void evaluateForTestInFrontend(long testCallId, const String& script);

    void inspect(PassRefPtr<TypeBuilder::Runtime::RemoteObject> objectToInspect, PassRefPtr<JSONObject> hints);

private:
    explicit InspectorInspectorAgent(InjectedScriptManager*);

    RawPtrWillBeMember<InjectedScriptManager> m_injectedScriptManager;

    Vector<pair<long, String> > m_pendingEvaluateTestCommands;
};

} // namespace blink

#endif // !defined(InspectorInspectorAgent_h)
