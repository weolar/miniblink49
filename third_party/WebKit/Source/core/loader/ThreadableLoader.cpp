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
#include "core/loader/ThreadableLoader.h"

#include "core/dom/Document.h"
#include "core/dom/ExecutionContext.h"
#include "core/loader/DocumentThreadableLoader.h"
#include "core/loader/ThreadableLoaderClientWrapper.h"
#include "core/loader/WorkerLoaderClientBridge.h"
#include "core/loader/WorkerThreadableLoader.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerThread.h"

namespace blink {

PassRefPtr<ThreadableLoader> ThreadableLoader::create(ExecutionContext& context, ThreadableLoaderClient* client, const ResourceRequest& request, const ThreadableLoaderOptions& options, const ResourceLoaderOptions& resourceLoaderOptions)
{
    ASSERT(client);

    if (context.isWorkerGlobalScope()) {
        WorkerGlobalScope& workerGlobalScope = toWorkerGlobalScope(context);
        RefPtr<ThreadableLoaderClientWrapper> clientWrapper(ThreadableLoaderClientWrapper::create(client));
        OwnPtr<ThreadableLoaderClient> clientBridge(WorkerLoaderClientBridge::create(clientWrapper, workerGlobalScope.thread()->workerLoaderProxy()));
        return WorkerThreadableLoader::create(workerGlobalScope, clientWrapper, clientBridge.release(), request, options, resourceLoaderOptions);
    }

    return DocumentThreadableLoader::create(toDocument(context), client, request, options, resourceLoaderOptions);
}

void ThreadableLoader::loadResourceSynchronously(ExecutionContext& context, const ResourceRequest& request, ThreadableLoaderClient& client, const ThreadableLoaderOptions& options, const ResourceLoaderOptions& resourceLoaderOptions)
{
    if (context.isWorkerGlobalScope()) {
        WorkerThreadableLoader::loadResourceSynchronously(toWorkerGlobalScope(context), request, client, options, resourceLoaderOptions);
        return;
    }

    DocumentThreadableLoader::loadResourceSynchronously(toDocument(context), request, client, options, resourceLoaderOptions);
}

} // namespace blink
