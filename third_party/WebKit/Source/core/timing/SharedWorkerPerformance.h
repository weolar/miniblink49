/*
 * Copyright (c) 2014, Opera Software ASA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Opera Software ASA nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SharedWorkerPerformance_h
#define SharedWorkerPerformance_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExecutionContext;
class SharedWorker;

class SharedWorkerPerformance final : public NoBaseWillBeGarbageCollected<SharedWorkerPerformance>, public WillBeHeapSupplement<SharedWorker> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SharedWorkerPerformance);
public:
    static SharedWorkerPerformance& from(SharedWorker&);

    static double workerStart(ExecutionContext*, SharedWorker&);
    double getWorkerStart(ExecutionContext*, SharedWorker&) const;

    DEFINE_INLINE_VIRTUAL_TRACE() { WillBeHeapSupplement<SharedWorker>::trace(visitor); }

private:
    SharedWorkerPerformance();
    static const char* supplementName();

    double m_timeOrigin;
};

} // namespace blink

#endif // SharedWorkerPerformance_h
