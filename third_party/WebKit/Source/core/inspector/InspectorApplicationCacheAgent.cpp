/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#include "config.h"
#include "core/inspector/InspectorApplicationCacheAgent.h"

#include "core/frame/LocalFrame.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoader.h"
#include "core/page/NetworkStateNotifier.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

namespace ApplicationCacheAgentState {
static const char applicationCacheAgentEnabled[] = "applicationCacheAgentEnabled";
}

InspectorApplicationCacheAgent::InspectorApplicationCacheAgent(InspectorPageAgent* pageAgent)
    : InspectorBaseAgent<InspectorApplicationCacheAgent, InspectorFrontend::ApplicationCache>("ApplicationCache")
    , m_pageAgent(pageAgent)
{
}

void InspectorApplicationCacheAgent::restore()
{
    if (m_state->getBoolean(ApplicationCacheAgentState::applicationCacheAgentEnabled)) {
        ErrorString error;
        enable(&error);
    }
}

void InspectorApplicationCacheAgent::enable(ErrorString*)
{
    m_state->setBoolean(ApplicationCacheAgentState::applicationCacheAgentEnabled, true);
    m_instrumentingAgents->setInspectorApplicationCacheAgent(this);
    frontend()->networkStateUpdated(networkStateNotifier().onLine());
}

void InspectorApplicationCacheAgent::disable(ErrorString*)
{
    m_state->setBoolean(ApplicationCacheAgentState::applicationCacheAgentEnabled, false);
    m_instrumentingAgents->setInspectorApplicationCacheAgent(nullptr);
}

void InspectorApplicationCacheAgent::updateApplicationCacheStatus(LocalFrame* frame)
{
    DocumentLoader* documentLoader = frame->loader().documentLoader();
    if (!documentLoader)
        return;

    ApplicationCacheHost* host = documentLoader->applicationCacheHost();
    ApplicationCacheHost::Status status = host->status();
    ApplicationCacheHost::CacheInfo info = host->applicationCacheInfo();

    String manifestURL = info.m_manifest.string();
    String frameId = IdentifiersFactory::frameId(frame);
    frontend()->applicationCacheStatusUpdated(frameId, manifestURL, static_cast<int>(status));
}

void InspectorApplicationCacheAgent::networkStateChanged(LocalFrame* frame, bool online)
{
    if (frame == m_pageAgent->inspectedFrame())
        frontend()->networkStateUpdated(online);
}

void InspectorApplicationCacheAgent::getFramesWithManifests(ErrorString*, RefPtr<TypeBuilder::Array<TypeBuilder::ApplicationCache::FrameWithManifest> >& result)
{
    result = TypeBuilder::Array<TypeBuilder::ApplicationCache::FrameWithManifest>::create();

    LocalFrame* inspectedFrame = m_pageAgent->inspectedFrame();
    for (Frame* frame = inspectedFrame; frame; frame = frame->tree().traverseNext(inspectedFrame)) {
        if (!frame->isLocalFrame())
            continue;
        DocumentLoader* documentLoader = toLocalFrame(frame)->loader().documentLoader();
        if (!documentLoader)
            continue;

        ApplicationCacheHost* host = documentLoader->applicationCacheHost();
        ApplicationCacheHost::CacheInfo info = host->applicationCacheInfo();
        String manifestURL = info.m_manifest.string();
        if (!manifestURL.isEmpty()) {
            RefPtr<TypeBuilder::ApplicationCache::FrameWithManifest> value = TypeBuilder::ApplicationCache::FrameWithManifest::create()
                .setFrameId(IdentifiersFactory::frameId(toLocalFrame(frame)))
                .setManifestURL(manifestURL)
                .setStatus(static_cast<int>(host->status()));
            result->addItem(value);
        }
    }
}

DocumentLoader* InspectorApplicationCacheAgent::assertFrameWithDocumentLoader(ErrorString* errorString, String frameId)
{
    LocalFrame* frame = m_pageAgent->assertFrame(errorString, frameId);
    if (!frame)
        return nullptr;

    return InspectorPageAgent::assertDocumentLoader(errorString, frame);
}

void InspectorApplicationCacheAgent::getManifestForFrame(ErrorString* errorString, const String& frameId, String* manifestURL)
{
    DocumentLoader* documentLoader = assertFrameWithDocumentLoader(errorString, frameId);
    if (!documentLoader)
        return;

    ApplicationCacheHost::CacheInfo info = documentLoader->applicationCacheHost()->applicationCacheInfo();
    *manifestURL = info.m_manifest.string();
}

void InspectorApplicationCacheAgent::getApplicationCacheForFrame(ErrorString* errorString, const String& frameId, RefPtr<TypeBuilder::ApplicationCache::ApplicationCache>& applicationCache)
{
    DocumentLoader* documentLoader = assertFrameWithDocumentLoader(errorString, frameId);
    if (!documentLoader)
        return;

    ApplicationCacheHost* host = documentLoader->applicationCacheHost();
    ApplicationCacheHost::CacheInfo info = host->applicationCacheInfo();

    ApplicationCacheHost::ResourceInfoList resources;
    host->fillResourceList(&resources);

    applicationCache = buildObjectForApplicationCache(resources, info);
}

PassRefPtr<TypeBuilder::ApplicationCache::ApplicationCache> InspectorApplicationCacheAgent::buildObjectForApplicationCache(const ApplicationCacheHost::ResourceInfoList& applicationCacheResources, const ApplicationCacheHost::CacheInfo& applicationCacheInfo)
{
    return TypeBuilder::ApplicationCache::ApplicationCache::create()
        .setManifestURL(applicationCacheInfo.m_manifest.string())
        .setSize(applicationCacheInfo.m_size)
        .setCreationTime(applicationCacheInfo.m_creationTime)
        .setUpdateTime(applicationCacheInfo.m_updateTime)
        .setResources(buildArrayForApplicationCacheResources(applicationCacheResources))
        .release();
}

PassRefPtr<TypeBuilder::Array<TypeBuilder::ApplicationCache::ApplicationCacheResource> > InspectorApplicationCacheAgent::buildArrayForApplicationCacheResources(const ApplicationCacheHost::ResourceInfoList& applicationCacheResources)
{
    RefPtr<TypeBuilder::Array<TypeBuilder::ApplicationCache::ApplicationCacheResource> > resources = TypeBuilder::Array<TypeBuilder::ApplicationCache::ApplicationCacheResource>::create();

    ApplicationCacheHost::ResourceInfoList::const_iterator end = applicationCacheResources.end();
    ApplicationCacheHost::ResourceInfoList::const_iterator it = applicationCacheResources.begin();
    for (int i = 0; it != end; ++it, i++)
        resources->addItem(buildObjectForApplicationCacheResource(*it));

    return resources;
}

PassRefPtr<TypeBuilder::ApplicationCache::ApplicationCacheResource> InspectorApplicationCacheAgent::buildObjectForApplicationCacheResource(const ApplicationCacheHost::ResourceInfo& resourceInfo)
{
    StringBuilder builder;
    if (resourceInfo.m_isMaster)
        builder.appendLiteral("Master ");

    if (resourceInfo.m_isManifest)
        builder.appendLiteral("Manifest ");

    if (resourceInfo.m_isFallback)
        builder.appendLiteral("Fallback ");

    if (resourceInfo.m_isForeign)
        builder.appendLiteral("Foreign ");

    if (resourceInfo.m_isExplicit)
        builder.appendLiteral("Explicit ");

    RefPtr<TypeBuilder::ApplicationCache::ApplicationCacheResource> value = TypeBuilder::ApplicationCache::ApplicationCacheResource::create()
        .setUrl(resourceInfo.m_resource.string())
        .setSize(static_cast<int>(resourceInfo.m_size))
        .setType(builder.toString());
    return value;
}

DEFINE_TRACE(InspectorApplicationCacheAgent)
{
    visitor->trace(m_pageAgent);
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink

