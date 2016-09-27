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

#ifndef StorageErrorCallback_h
#define StorageErrorCallback_h

#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class DOMError;

typedef int ExceptionCode;

class StorageErrorCallback : public GarbageCollectedFinalized<StorageErrorCallback> {
public:
    virtual ~StorageErrorCallback() { }
    DEFINE_INLINE_VIRTUAL_TRACE() { }
    virtual void handleEvent(DOMError*) = 0;

    class MODULES_EXPORT CallbackTask final : public ExecutionContextTask {
    public:
        static PassOwnPtr<CallbackTask> create(StorageErrorCallback* callback, ExceptionCode ec)
        {
            return adoptPtr(new CallbackTask(callback, ec));
        }

        void performTask(ExecutionContext*) override;

    private:
        CallbackTask(StorageErrorCallback*, ExceptionCode);

        Persistent<StorageErrorCallback> m_callback;
        ExceptionCode m_ec;
    };
};

} // namespace blink

#endif // StorageErrorCallback_h
