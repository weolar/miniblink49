/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InspectorApplicationCacheAgent_h
#define InspectorApplicationCacheAgent_h

#include "core/CoreExport.h"
#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "core/loader/appcache/ApplicationCacheHost.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class LocalFrame;
class InspectorFrontend;
class InspectorPageAgent;

typedef String ErrorString;

class CORE_EXPORT InspectorApplicationCacheAgent final : public InspectorBaseAgent<InspectorApplicationCacheAgent, InspectorFrontend::ApplicationCache>, public InspectorBackendDispatcher::ApplicationCacheCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorApplicationCacheAgent);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InspectorApplicationCacheAgent);
public:
    static PassOwnPtrWillBeRawPtr<InspectorApplicationCacheAgent> create(InspectorPageAgent* pageAgent)
    {
        return adoptPtrWillBeNoop(new InspectorApplicationCacheAgent(pageAgent));
    }
    virtual ~InspectorApplicationCacheAgent() { }
    DECLARE_VIRTUAL_TRACE();

    // InspectorBaseAgent
    void restore() override;
    void disable(ErrorString*) override;

    // InspectorInstrumentation API
    void updateApplicationCacheStatus(LocalFrame*);
    void networkStateChanged(LocalFrame*, bool online);

    // ApplicationCache API for InspectorFrontend
    virtual void enable(ErrorString*) override;
    virtual void getFramesWithManifests(ErrorString*, RefPtr<TypeBuilder::Array<TypeBuilder::ApplicationCache::FrameWithManifest> >& result) override;
    virtual void getManifestForFrame(ErrorString*, const String& frameId, String* manifestURL) override;
    virtual void getApplicationCacheForFrame(ErrorString*, const String& frameId, RefPtr<TypeBuilder::ApplicationCache::ApplicationCache>&) override;

private:
    InspectorApplicationCacheAgent(InspectorPageAgent*);
    PassRefPtr<TypeBuilder::ApplicationCache::ApplicationCache> buildObjectForApplicationCache(const ApplicationCacheHost::ResourceInfoList&, const ApplicationCacheHost::CacheInfo&);
    PassRefPtr<TypeBuilder::Array<TypeBuilder::ApplicationCache::ApplicationCacheResource> > buildArrayForApplicationCacheResources(const ApplicationCacheHost::ResourceInfoList&);
    PassRefPtr<TypeBuilder::ApplicationCache::ApplicationCacheResource> buildObjectForApplicationCacheResource(const ApplicationCacheHost::ResourceInfo&);

    DocumentLoader* assertFrameWithDocumentLoader(ErrorString*, String frameId);

    RawPtrWillBeMember<InspectorPageAgent> m_pageAgent;
};

} // namespace blink

#endif // InspectorApplicationCacheAgent_h
