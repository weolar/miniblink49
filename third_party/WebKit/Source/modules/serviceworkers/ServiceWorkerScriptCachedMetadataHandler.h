// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerScriptCachedMetadataHandler_h
#define ServiceWorkerScriptCachedMetadataHandler_h

#include "core/fetch/CachedMetadataHandler.h"
#include "platform/weborigin/KURL.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class WorkerGlobalScope;
class CachedMetadata;

class ServiceWorkerScriptCachedMetadataHandler : public CachedMetadataHandler {
public:
    static PassOwnPtr<ServiceWorkerScriptCachedMetadataHandler> create(WorkerGlobalScope* workerGlobalScope, const KURL& scriptURL, const Vector<char>* metaData)
    {
        return adoptPtr(new ServiceWorkerScriptCachedMetadataHandler(workerGlobalScope, scriptURL, metaData));
    }
    ~ServiceWorkerScriptCachedMetadataHandler() override;
    void setCachedMetadata(unsigned dataTypeID, const char*, size_t, CacheType) override;
    void clearCachedMetadata(CacheType) override;
    CachedMetadata* cachedMetadata(unsigned dataTypeID) const override;
    String encoding() const override;

private:
    ServiceWorkerScriptCachedMetadataHandler(WorkerGlobalScope*, const KURL& scriptURL, const Vector<char>* metaData);

    WorkerGlobalScope* m_workerGlobalScope;
    KURL m_scriptURL;
    RefPtr<CachedMetadata> m_cachedMetadata;
};

} // namespace blink

#endif // ServiceWorkerScriptCachedMetadataHandler_h
