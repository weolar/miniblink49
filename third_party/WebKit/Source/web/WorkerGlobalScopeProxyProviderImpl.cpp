/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "web/WorkerGlobalScopeProxyProviderImpl.h"

#include "core/dom/Document.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/workers/DedicatedWorkerMessagingProxy.h"
#include "core/workers/Worker.h"
#include "core/workers/WorkerClients.h"
#include "public/platform/WebString.h"
#include "public/web/WebContentSettingsClient.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebWorkerContentSettingsClientProxy.h"
#include "web/LocalFileSystemClient.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"
#include "web/WorkerContentSettingsClient.h"

namespace blink {

WorkerGlobalScopeProxy* WorkerGlobalScopeProxyProviderImpl::createWorkerGlobalScopeProxy(Worker* worker)
{
    if (worker->executionContext()->isDocument()) {
        Document* document = toDocument(worker->executionContext());
        WebLocalFrameImpl* webFrame = WebLocalFrameImpl::fromFrame(document->frame());
        OwnPtrWillBeRawPtr<WorkerClients> workerClients = WorkerClients::create();
#ifdef MINIBLINK_NOT_IMPLEMENTED
        provideLocalFileSystemToWorker(workerClients.get(), LocalFileSystemClient::create());
#endif
        provideContentSettingsClientToWorker(workerClients.get(), adoptPtr(webFrame->client()->createWorkerContentSettingsClientProxy(webFrame)));
        // FIXME: call provideServiceWorkerContainerClientToWorker here when we
        // support ServiceWorker in dedicated workers (http://crbug.com/371690)
        return new DedicatedWorkerMessagingProxy(worker, workerClients.release());
    }
    ASSERT_NOT_REACHED();
    return 0;
}

} // namespace blink
