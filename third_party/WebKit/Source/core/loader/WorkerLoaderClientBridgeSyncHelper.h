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

#ifndef WorkerLoaderClientBridgeSyncHelper_h
#define WorkerLoaderClientBridgeSyncHelper_h

#include "core/loader/ThreadableLoaderClientWrapper.h"
#include "wtf/Forward.h"
#include "wtf/Functional.h"
#include "wtf/ThreadingPrimitives.h"
#include "wtf/Vector.h"

namespace blink {
class WebWaitableEvent;
}

namespace blink {

// This bridge is created and destroyed on the worker thread, but is
// passed to and used on the main thread. Each did* method records the given
// data so that they can be run on the worker thread later (by run()).
class WorkerLoaderClientBridgeSyncHelper : public ThreadableLoaderClient {
    WTF_MAKE_FAST_ALLOCATED(WorkerLoaderClientBridgeSyncHelper);
public:
    static PassOwnPtr<WorkerLoaderClientBridgeSyncHelper> create(ThreadableLoaderClientWrapper*, PassOwnPtr<WebWaitableEvent>);
    ~WorkerLoaderClientBridgeSyncHelper() override;

    // Called on the worker context thread.
    void run();

    // Called on the main thread.
    void didSendData(unsigned long long bytesSent, unsigned long long totalBytesToBeSent) override;
    void didReceiveResponse(unsigned long identifier, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>) override;
    void didReceiveData(const char*, unsigned dataLength) override;
    void didDownloadData(int dataLength) override;
    void didReceiveCachedMetadata(const char*, int dataLength) override;
    void didFinishLoading(unsigned long identifier, double finishTime) override;
    void didFail(const ResourceError&) override;
    void didFailAccessControlCheck(const ResourceError&) override;
    void didFailRedirectCheck() override;
    void didReceiveResourceTiming(const ResourceTimingInfo&) override;

private:
    WorkerLoaderClientBridgeSyncHelper(ThreadableLoaderClientWrapper*, PassOwnPtr<WebWaitableEvent>);

    bool m_done;
    RefPtr<ThreadableLoaderClientWrapper> m_client;
    OwnPtr<WebWaitableEvent> m_event;
    Vector<Vector<char>*> m_receivedData;
    Vector<OwnPtr<Closure>> m_clientTasks;
    Mutex m_lock;
};

} // namespace blink

#endif // WorkerLoaderClientBridgeSyncHelper_h
