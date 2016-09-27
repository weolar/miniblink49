// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/ServiceWorkerScriptCachedMetadataHandler.h"

#include "core/fetch/CachedMetadata.h"
#include "core/workers/WorkerGlobalScope.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"

namespace blink {

ServiceWorkerScriptCachedMetadataHandler::ServiceWorkerScriptCachedMetadataHandler(WorkerGlobalScope* workerGlobalScope, const KURL& scriptURL, const Vector<char>* metaData)
    : m_workerGlobalScope(workerGlobalScope)
    , m_scriptURL(scriptURL)
{
    if (metaData)
        m_cachedMetadata = CachedMetadata::deserialize(metaData->data(), metaData->size());
}

ServiceWorkerScriptCachedMetadataHandler::~ServiceWorkerScriptCachedMetadataHandler()
{
}

void ServiceWorkerScriptCachedMetadataHandler::setCachedMetadata(unsigned dataTypeID, const char* data, size_t size, CacheType type)
{
    if (type != SendToPlatform)
        return;
    m_cachedMetadata = CachedMetadata::create(dataTypeID, data, size);
    const Vector<char>& serializedData = m_cachedMetadata->serialize();
    ServiceWorkerGlobalScopeClient::from(m_workerGlobalScope)->setCachedMetadata(m_scriptURL, serializedData.data(), serializedData.size());
}

void ServiceWorkerScriptCachedMetadataHandler::clearCachedMetadata(CacheType type)
{
    if (type != SendToPlatform)
        return;
    m_cachedMetadata = nullptr;
    ServiceWorkerGlobalScopeClient::from(m_workerGlobalScope)->clearCachedMetadata(m_scriptURL);
}

CachedMetadata* ServiceWorkerScriptCachedMetadataHandler::cachedMetadata(unsigned dataTypeID) const
{
    if (!m_cachedMetadata || m_cachedMetadata->dataTypeID() != dataTypeID)
        return nullptr;
    return m_cachedMetadata.get();
}

String ServiceWorkerScriptCachedMetadataHandler::encoding() const
{
    return emptyString();
}

} // namespace blink
