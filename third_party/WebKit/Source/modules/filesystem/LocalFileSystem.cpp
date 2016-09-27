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
#include "modules/filesystem/LocalFileSystem.h"

#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "core/fileapi/FileError.h"
#include "core/frame/LocalFrame.h"
#include "core/workers/WorkerGlobalScope.h"
#include "modules/filesystem/FileSystemClient.h"
#include "platform/AsyncFileSystemCallbacks.h"
#include "platform/ContentSettingCallbacks.h"
#include "public/platform/Platform.h"
#include "public/platform/WebFileSystem.h"
#include "wtf/Functional.h"
#include "wtf/RefCounted.h"

namespace blink {

namespace {

void reportFailure(PassOwnPtr<AsyncFileSystemCallbacks> callbacks, FileError::ErrorCode error)
{
    callbacks->didFail(error);
}

} // namespace

class CallbackWrapper final : public GarbageCollectedFinalized<CallbackWrapper> {
public:
    CallbackWrapper(PassOwnPtr<AsyncFileSystemCallbacks> c)
        : m_callbacks(c)
    {
    }
    virtual ~CallbackWrapper() { }
    PassOwnPtr<AsyncFileSystemCallbacks> release()
    {
        return m_callbacks.release();
    }

    DEFINE_INLINE_TRACE() { }

private:
    OwnPtr<AsyncFileSystemCallbacks> m_callbacks;
};

PassOwnPtrWillBeRawPtr<LocalFileSystem> LocalFileSystem::create(PassOwnPtr<FileSystemClient> client)
{
    return adoptPtrWillBeNoop(new LocalFileSystem(client));
}

LocalFileSystem::~LocalFileSystem()
{
}

void LocalFileSystem::resolveURL(ExecutionContext* context, const KURL& fileSystemURL, PassOwnPtr<AsyncFileSystemCallbacks> callbacks)
{
    RefPtrWillBeRawPtr<ExecutionContext> contextPtr(context);
    CallbackWrapper* wrapper = new CallbackWrapper(callbacks);
    requestFileSystemAccessInternal(context,
        bind(&LocalFileSystem::resolveURLInternal, this, contextPtr, fileSystemURL, wrapper),
        bind(&LocalFileSystem::fileSystemNotAllowedInternal, this, contextPtr, wrapper));
}

void LocalFileSystem::requestFileSystem(ExecutionContext* context, FileSystemType type, long long size, PassOwnPtr<AsyncFileSystemCallbacks> callbacks)
{
    RefPtrWillBeRawPtr<ExecutionContext> contextPtr(context);
    CallbackWrapper* wrapper = new CallbackWrapper(callbacks);
    requestFileSystemAccessInternal(context,
        bind(&LocalFileSystem::fileSystemAllowedInternal, this, contextPtr, type, wrapper),
        bind(&LocalFileSystem::fileSystemNotAllowedInternal, this, contextPtr, wrapper));
}

void LocalFileSystem::deleteFileSystem(ExecutionContext* context, FileSystemType type, PassOwnPtr<AsyncFileSystemCallbacks> callbacks)
{
    RefPtrWillBeRawPtr<ExecutionContext> contextPtr(context);
    ASSERT(context);
    ASSERT_WITH_SECURITY_IMPLICATION(context->isDocument());

    CallbackWrapper* wrapper = new CallbackWrapper(callbacks);
    requestFileSystemAccessInternal(context,
        bind(&LocalFileSystem::deleteFileSystemInternal, this, contextPtr, type, wrapper),
        bind(&LocalFileSystem::fileSystemNotAllowedInternal, this, contextPtr, wrapper));
}

WebFileSystem* LocalFileSystem::fileSystem() const
{
    Platform* platform = Platform::current();
    if (!platform)
        return nullptr;
    return Platform::current()->fileSystem();
}

void LocalFileSystem::requestFileSystemAccessInternal(ExecutionContext* context, PassOwnPtr<Closure> allowed, PassOwnPtr<Closure> denied)
{
    if (!client()) {
        (*denied)();
        return;
    }
    if (!context->isDocument()) {
        if (!client()->requestFileSystemAccessSync(context)) {
            (*denied)();
            return;
        }
        (*allowed)();
        return;
    }
    client()->requestFileSystemAccessAsync(context, ContentSettingCallbacks::create(allowed, denied));
}

void LocalFileSystem::fileSystemNotAvailable(
    PassRefPtrWillBeRawPtr<ExecutionContext> context,
    CallbackWrapper* callbacks)
{
    context->postTask(FROM_HERE, createSameThreadTask(&reportFailure, callbacks->release(), FileError::ABORT_ERR));
}

void LocalFileSystem::fileSystemNotAllowedInternal(
    PassRefPtrWillBeRawPtr<ExecutionContext> context,
    CallbackWrapper* callbacks)
{
    context->postTask(FROM_HERE, createSameThreadTask(&reportFailure, callbacks->release(), FileError::ABORT_ERR));
}

void LocalFileSystem::fileSystemAllowedInternal(
    PassRefPtrWillBeRawPtr<ExecutionContext> context,
    FileSystemType type,
    CallbackWrapper* callbacks)
{
    if (!fileSystem()) {
        fileSystemNotAvailable(context, callbacks);
        return;
    }

    KURL storagePartition = KURL(KURL(), context->securityOrigin()->toString());
    fileSystem()->openFileSystem(storagePartition, static_cast<WebFileSystemType>(type), callbacks->release());
}

void LocalFileSystem::resolveURLInternal(
    PassRefPtrWillBeRawPtr<ExecutionContext> context,
    const KURL& fileSystemURL,
    CallbackWrapper* callbacks)
{
    if (!fileSystem()) {
        fileSystemNotAvailable(context, callbacks);
        return;
    }
    fileSystem()->resolveURL(fileSystemURL, callbacks->release());
}

void LocalFileSystem::deleteFileSystemInternal(
    PassRefPtrWillBeRawPtr<ExecutionContext> context,
    FileSystemType type,
    CallbackWrapper* callbacks)
{
    if (!fileSystem()) {
        fileSystemNotAvailable(context, callbacks);
        return;
    }
    KURL storagePartition = KURL(KURL(), context->securityOrigin()->toString());
    fileSystem()->deleteFileSystem(storagePartition, static_cast<WebFileSystemType>(type), callbacks->release());
}

LocalFileSystem::LocalFileSystem(PassOwnPtr<FileSystemClient> client)
    : m_client(client)
{
}

const char* LocalFileSystem::supplementName()
{
    return "LocalFileSystem";
}

LocalFileSystem* LocalFileSystem::from(ExecutionContext& context)
{
    if (context.isDocument())
        return static_cast<LocalFileSystem*>(WillBeHeapSupplement<LocalFrame>::from(toDocument(context).frame(), supplementName()));

    WorkerClients* clients = toWorkerGlobalScope(context).clients();
    ASSERT(clients);
    return static_cast<LocalFileSystem*>(WillBeHeapSupplement<WorkerClients>::from(clients, supplementName()));
}

void provideLocalFileSystemTo(LocalFrame& frame, PassOwnPtr<FileSystemClient> client)
{
    frame.provideSupplement(LocalFileSystem::supplementName(), LocalFileSystem::create(client));
}

void provideLocalFileSystemToWorker(WorkerClients* clients, PassOwnPtr<FileSystemClient> client)
{
    clients->provideSupplement(LocalFileSystem::supplementName(), LocalFileSystem::create(client));
}

} // namespace blink
