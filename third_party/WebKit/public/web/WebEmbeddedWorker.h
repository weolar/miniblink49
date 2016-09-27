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

#ifndef WebEmbeddedWorker_h
#define WebEmbeddedWorker_h

#include "public/platform/WebCommon.h"

namespace blink {

class WebServiceWorkerContextClient;
class WebString;
class WebWorkerContentSettingsClientProxy;
struct WebEmbeddedWorkerStartData;

// An interface to start and terminate an embedded worker.
// All methods of this class must be called on the main thread.
class WebEmbeddedWorker {
public:
    // Invoked on the main thread to instantiate a WebEmbeddedWorker.
    // The given WebWorkerContextClient and WebWorkerContentSettingsClientProxy
    // are going to be passed on to the worker thread and is held by a newly
    // created WorkerGlobalScope.
    BLINK_EXPORT static WebEmbeddedWorker* create(
        WebServiceWorkerContextClient*,
        WebWorkerContentSettingsClientProxy*);

    virtual ~WebEmbeddedWorker() { }

    // Starts and terminates WorkerThread and WorkerGlobalScope.
    virtual void startWorkerContext(const WebEmbeddedWorkerStartData&) = 0;
    virtual void terminateWorkerContext() = 0;

    // Resumes starting a worker startup that was paused via
    // WebEmbeddedWorkerStartData.pauseAfterDownloadMode.
    virtual void resumeAfterDownload() = 0;

    // Inspector related methods.
    virtual void attachDevTools(const WebString& hostId) = 0;
    virtual void reattachDevTools(const WebString& hostId, const WebString& savedState) = 0;
    virtual void detachDevTools() = 0;
    virtual void dispatchDevToolsMessage(const WebString&) = 0;
};

} // namespace blink

#endif // WebEmbeddedWorker_h
