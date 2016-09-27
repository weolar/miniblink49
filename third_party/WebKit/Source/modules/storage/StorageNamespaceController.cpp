// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/storage/StorageNamespaceController.h"

#include "modules/storage/InspectorDOMStorageAgent.h"
#include "modules/storage/StorageClient.h"
#include "modules/storage/StorageNamespace.h"

namespace blink {

const char* StorageNamespaceController::supplementName()
{
    return "StorageNamespaceController";
}

StorageNamespaceController::StorageNamespaceController(StorageClient* client)
    : m_client(client)
    , m_inspectorAgent(nullptr)
{
}

StorageNamespaceController::~StorageNamespaceController()
{
}

DEFINE_TRACE(StorageNamespaceController)
{
    WillBeHeapSupplement<Page>::trace(visitor);
    visitor->trace(m_inspectorAgent);
}

StorageNamespace* StorageNamespaceController::sessionStorage(bool optionalCreate)
{
    if (!m_sessionStorage && optionalCreate)
        m_sessionStorage = m_client->createSessionStorageNamespace();
    return m_sessionStorage.get();
}

void StorageNamespaceController::provideStorageNamespaceTo(Page& page, StorageClient* client)
{
    StorageNamespaceController::provideTo(page, supplementName(), adoptPtrWillBeNoop(new StorageNamespaceController(client)));
}

} // namespace blink
