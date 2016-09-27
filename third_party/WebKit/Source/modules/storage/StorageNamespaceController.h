// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StorageNamespaceController_h
#define StorageNamespaceController_h

#include "core/page/Page.h"
#include "modules/ModulesExport.h"
#include "platform/Supplementable.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class InspectorDOMStorageAgent;
class StorageClient;
class StorageNamespace;

class MODULES_EXPORT StorageNamespaceController final : public NoBaseWillBeGarbageCollectedFinalized<StorageNamespaceController>, public WillBeHeapSupplement<Page> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(StorageNamespaceController);
public:
    StorageNamespace* sessionStorage(bool optionalCreate = true);
    StorageClient* storageClient() { return m_client; }
    ~StorageNamespaceController();

    static void provideStorageNamespaceTo(Page&, StorageClient*);
    static StorageNamespaceController* from(Page* page) { return static_cast<StorageNamespaceController*>(WillBeHeapSupplement<Page>::from(page, supplementName())); }

    DECLARE_TRACE();

    InspectorDOMStorageAgent* inspectorAgent() { return m_inspectorAgent; }
    void setInspectorAgent(InspectorDOMStorageAgent* agent) { m_inspectorAgent = agent; }
private:
    explicit StorageNamespaceController(StorageClient*);
    static const char* supplementName();
    OwnPtr<StorageNamespace> m_sessionStorage;
    StorageClient* m_client;
    RawPtrWillBeMember<InspectorDOMStorageAgent> m_inspectorAgent;
};

} // namespace blink

#endif // StorageNamespaceController_h
