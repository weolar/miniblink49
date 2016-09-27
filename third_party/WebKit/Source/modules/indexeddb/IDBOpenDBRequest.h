/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IDBOpenDBRequest_h
#define IDBOpenDBRequest_h

#include "modules/ModulesExport.h"
#include "modules/indexeddb/IDBRequest.h"
#include "public/platform/modules/indexeddb/WebIDBDatabase.h"

namespace blink {

class IDBDatabaseCallbacks;

class MODULES_EXPORT IDBOpenDBRequest final : public IDBRequest {
    DEFINE_WRAPPERTYPEINFO();
public:
    static IDBOpenDBRequest* create(ScriptState*, IDBDatabaseCallbacks*, int64_t transactionId, int64_t version);
    ~IDBOpenDBRequest() override;
    DECLARE_VIRTUAL_TRACE();

    using IDBRequest::onSuccess;

    void onBlocked(int64_t existingVersion) override;
    void onUpgradeNeeded(int64_t oldVersion, PassOwnPtr<WebIDBDatabase>, const IDBDatabaseMetadata&, WebIDBDataLoss, String dataLossMessage) override;
    void onSuccess(PassOwnPtr<WebIDBDatabase>, const IDBDatabaseMetadata&) override;
    void onSuccess(int64_t oldVersion) override;

    // EventTarget
    const AtomicString& interfaceName() const override;
    bool dispatchEvent(PassRefPtrWillBeRawPtr<Event>) override;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(blocked);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(upgradeneeded);

protected:
    bool shouldEnqueueEvent() const override;

private:
    IDBOpenDBRequest(ScriptState*, IDBDatabaseCallbacks*, int64_t transactionId, int64_t version);

    Member<IDBDatabaseCallbacks> m_databaseCallbacks;
    const int64_t m_transactionId;
    int64_t m_version;
};

} // namespace blink

#endif // IDBOpenDBRequest_h
