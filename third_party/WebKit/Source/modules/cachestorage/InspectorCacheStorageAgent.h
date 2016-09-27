// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorCacheStorageAgent_h
#define InspectorCacheStorageAgent_h

#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "modules/ModulesExport.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

typedef String ErrorString;

class MODULES_EXPORT InspectorCacheStorageAgent final : public InspectorBaseAgent<InspectorCacheStorageAgent, InspectorFrontend::CacheStorage>, public InspectorBackendDispatcher::CacheStorageCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorCacheStorageAgent);

public:
    static PassOwnPtrWillBeRawPtr<InspectorCacheStorageAgent> create()
    {
        return adoptPtrWillBeNoop(new InspectorCacheStorageAgent());
    }

    ~InspectorCacheStorageAgent() override;

    DECLARE_VIRTUAL_TRACE();

    void requestCacheNames(ErrorString*, const String& securityOrigin, PassRefPtrWillBeRawPtr<RequestCacheNamesCallback>) override;
    void requestEntries(ErrorString*, const String& cacheId, int skipCount, int pageSize, PassRefPtrWillBeRawPtr<RequestEntriesCallback>) override;
    void deleteCache(ErrorString*, const String& cacheId, PassRefPtrWillBeRawPtr<DeleteCacheCallback>) override;
    void deleteEntry(ErrorString*, const String& cacheId, const String& request, PassRefPtrWillBeRawPtr<DeleteEntryCallback>) override;

private:
    explicit InspectorCacheStorageAgent();
};

} // namespace blink

#endif // InspectorCacheStorageAgent_h
