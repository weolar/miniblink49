/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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

#ifndef SharedWorker_h
#define SharedWorker_h

#include "core/CoreExport.h"
#include "core/workers/AbstractWorker.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExceptionState;

class CORE_EXPORT SharedWorker final : public AbstractWorker, public WillBeHeapSupplementable<SharedWorker> {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SharedWorker);
public:
    static PassRefPtrWillBeRawPtr<SharedWorker> create(ExecutionContext*, const String& url, const String& name, ExceptionState&);
    virtual ~SharedWorker();

    MessagePort* port() const { return m_port.get(); }

    virtual const AtomicString& interfaceName() const override;

    void setIsBeingConnected(bool b) { m_isBeingConnected = b; }

    virtual bool hasPendingActivity() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SharedWorker(ExecutionContext*);

    PersistentWillBeMember<MessagePort> m_port;
    bool m_isBeingConnected;
};

} // namespace blink

#endif // SharedWorker_h
