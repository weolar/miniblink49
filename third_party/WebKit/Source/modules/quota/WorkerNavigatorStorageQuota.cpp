/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "modules/quota/WorkerNavigatorStorageQuota.h"

#include "modules/quota/DeprecatedStorageQuota.h"

namespace blink {

WorkerNavigatorStorageQuota::WorkerNavigatorStorageQuota()
{
}

const char* WorkerNavigatorStorageQuota::supplementName()
{
    return "WorkerNavigatorStorageQuota";
}

WorkerNavigatorStorageQuota& WorkerNavigatorStorageQuota::from(WorkerNavigator& navigator)
{
    WorkerNavigatorStorageQuota* supplement = static_cast<WorkerNavigatorStorageQuota*>(HeapSupplement<WorkerNavigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new WorkerNavigatorStorageQuota();
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

DeprecatedStorageQuota* WorkerNavigatorStorageQuota::webkitTemporaryStorage(WorkerNavigator& navigator)
{
    return WorkerNavigatorStorageQuota::from(navigator).webkitTemporaryStorage();
}

DeprecatedStorageQuota* WorkerNavigatorStorageQuota::webkitPersistentStorage(WorkerNavigator& navigator)
{
    return WorkerNavigatorStorageQuota::from(navigator).webkitPersistentStorage();
}

DeprecatedStorageQuota* WorkerNavigatorStorageQuota::webkitTemporaryStorage() const
{
    if (!m_temporaryStorage)
        m_temporaryStorage = DeprecatedStorageQuota::create(DeprecatedStorageQuota::Temporary);
    return m_temporaryStorage.get();
}

DeprecatedStorageQuota* WorkerNavigatorStorageQuota::webkitPersistentStorage() const
{
    if (!m_persistentStorage)
        m_persistentStorage = DeprecatedStorageQuota::create(DeprecatedStorageQuota::Persistent);
    return m_persistentStorage.get();
}

DEFINE_TRACE(WorkerNavigatorStorageQuota)
{
    visitor->trace(m_temporaryStorage);
    visitor->trace(m_persistentStorage);
    HeapSupplement<WorkerNavigator>::trace(visitor);
}

} // namespace blink
