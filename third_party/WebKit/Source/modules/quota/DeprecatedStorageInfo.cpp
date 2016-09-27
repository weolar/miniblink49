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

#include "config.h"

#include "modules/quota/DeprecatedStorageInfo.h"

#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/quota/DeprecatedStorageQuota.h"
#include "modules/quota/StorageErrorCallback.h"
#include "modules/quota/StorageQuotaCallback.h"
#include "modules/quota/StorageUsageCallback.h"
#include "public/platform/WebTraceLocation.h"

namespace blink {

DeprecatedStorageInfo::DeprecatedStorageInfo()
{
}

void DeprecatedStorageInfo::queryUsageAndQuota(ExecutionContext* executionContext, int storageType, StorageUsageCallback* successCallback, StorageErrorCallback* errorCallback)
{
    // Dispatching the request to DeprecatedStorageQuota, as this interface is deprecated in favor of DeprecatedStorageQuota.
    DeprecatedStorageQuota* storageQuota = getStorageQuota(storageType);
    if (!storageQuota) {
        // Unknown storage type is requested.
        executionContext->postTask(FROM_HERE, StorageErrorCallback::CallbackTask::create(errorCallback, NotSupportedError));
        return;
    }
    storageQuota->queryUsageAndQuota(executionContext, successCallback, errorCallback);
}

void DeprecatedStorageInfo::requestQuota(ExecutionContext* executionContext, int storageType, unsigned long long newQuotaInBytes, StorageQuotaCallback* successCallback, StorageErrorCallback* errorCallback)
{
    // Dispatching the request to DeprecatedStorageQuota, as this interface is deprecated in favor of DeprecatedStorageQuota.
    DeprecatedStorageQuota* storageQuota = getStorageQuota(storageType);
    if (!storageQuota) {
        // Unknown storage type is requested.
        executionContext->postTask(FROM_HERE, StorageErrorCallback::CallbackTask::create(errorCallback, NotSupportedError));
        return;
    }
    storageQuota->requestQuota(executionContext, newQuotaInBytes, successCallback, errorCallback);
}

DeprecatedStorageQuota* DeprecatedStorageInfo::getStorageQuota(int storageType)
{
    switch (storageType) {
    case TEMPORARY:
        if (!m_temporaryStorage)
            m_temporaryStorage = DeprecatedStorageQuota::create(DeprecatedStorageQuota::Temporary);
        return m_temporaryStorage.get();
    case PERSISTENT:
        if (!m_persistentStorage)
            m_persistentStorage = DeprecatedStorageQuota::create(DeprecatedStorageQuota::Persistent);
        return m_persistentStorage.get();
    }
    return 0;
}

DEFINE_TRACE(DeprecatedStorageInfo)
{
    visitor->trace(m_temporaryStorage);
    visitor->trace(m_persistentStorage);
}

} // namespace blink
