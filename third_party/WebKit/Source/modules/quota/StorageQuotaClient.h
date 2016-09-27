/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#ifndef StorageQuotaClient_h
#define StorageQuotaClient_h

#include "core/page/Page.h"
#include "modules/ModulesExport.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebStorageQuotaType.h"
#include "wtf/Forward.h"

namespace blink {

class ExecutionContext;
class Page;
class ScriptPromise;
class ScriptState;
class StorageErrorCallback;
class StorageQuotaCallback;

class StorageQuotaClient : public WillBeHeapSupplement<Page> {
    WTF_MAKE_NONCOPYABLE(StorageQuotaClient);
public:
    StorageQuotaClient() { }
    virtual ~StorageQuotaClient() { }

    virtual void requestQuota(ExecutionContext*, WebStorageQuotaType, unsigned long long newQuotaInBytes, StorageQuotaCallback*, StorageErrorCallback*) = 0;
    virtual ScriptPromise requestPersistentQuota(ScriptState*, unsigned long long newQuotaInBytes) = 0;

    static const char* supplementName();
    static StorageQuotaClient* from(ExecutionContext*);
};

MODULES_EXPORT void provideStorageQuotaClientTo(Page&, PassOwnPtrWillBeRawPtr<StorageQuotaClient>);

} // namespace blink

#endif // StorageQuotaClient_h
