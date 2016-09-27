/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef DOMFileSystem_h
#define DOMFileSystem_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "modules/ModulesExport.h"
#include "modules/filesystem/DOMFileSystemBase.h"
#include "modules/filesystem/EntriesCallback.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebTraceLocation.h"

namespace blink {

class DirectoryEntry;
class FileCallback;
class FileEntry;
class FileWriterCallback;

class MODULES_EXPORT DOMFileSystem final : public DOMFileSystemBase, public ScriptWrappable, public ActiveDOMObject {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DOMFileSystem);
public:
    static DOMFileSystem* create(ExecutionContext*, const String& name, FileSystemType, const KURL& rootURL);

    // Creates a new isolated file system for the given filesystemId.
    static DOMFileSystem* createIsolatedFileSystem(ExecutionContext*, const String& filesystemId);

    DirectoryEntry* root() const;

    // DOMFileSystemBase overrides.
    void addPendingCallbacks() override;
    void removePendingCallbacks() override;
    void reportError(ErrorCallback*, FileError*) override;

    // ActiveDOMObject overrides.
    bool hasPendingActivity() const override;

    void createWriter(const FileEntry*, FileWriterCallback*, ErrorCallback*);
    void createFile(const FileEntry*, FileCallback*, ErrorCallback*);

    // Schedule a callback. This should not cross threads (should be called on the same context thread).
    // FIXME: move this to a more generic place.
    template <typename CB, typename CBArg>
    static void scheduleCallback(ExecutionContext*, CB*, CBArg*);

    template <typename CB, typename CBArg>
    static void scheduleCallback(ExecutionContext*, CB*, const HeapVector<CBArg>&);

    template <typename CB, typename CBArg>
    static void scheduleCallback(ExecutionContext*, CB*, const CBArg&);

    template <typename CB, typename CBArg>
    static void scheduleCallback(ExecutionContext*, CB*, const Member<CBArg>&);

    template <typename CB>
    static void scheduleCallback(ExecutionContext*, CB*);

    template <typename CB, typename CBArg>
    void scheduleCallback(CB* callback, CBArg* callbackArg)
    {
        scheduleCallback(executionContext(), callback, callbackArg);
    }

    template <typename CB, typename CBArg>
    void scheduleCallback(CB* callback, const CBArg& callbackArg)
    {
        scheduleCallback(executionContext(), callback, callbackArg);
    }

    DECLARE_VIRTUAL_TRACE();

private:
    DOMFileSystem(ExecutionContext*, const String& name, FileSystemType, const KURL& rootURL);

    class DispatchCallbackTaskBase : public ExecutionContextTask {
    public:
        String taskNameForInstrumentation() const override
        {
            return "FileSystem";
        }
    };

    template <typename CB, typename CBArg>
    class DispatchCallbackPtrArgTask final : public DispatchCallbackTaskBase {
    public:
        DispatchCallbackPtrArgTask(CB* callback, CBArg* arg)
            : m_callback(callback)
            , m_callbackArg(arg)
        {
        }

        void performTask(ExecutionContext*) override
        {
            m_callback->handleEvent(m_callbackArg.get());
        }

    private:
        Persistent<CB> m_callback;
        Persistent<CBArg> m_callbackArg;
    };

    template <typename CB, typename CBArg>
    class DispatchCallbackNonPtrArgTask final : public DispatchCallbackTaskBase {
    public:
        DispatchCallbackNonPtrArgTask(CB* callback, const CBArg& arg)
            : m_callback(callback)
            , m_callbackArg(arg)
        {
        }

        void performTask(ExecutionContext*) override
        {
            m_callback->handleEvent(m_callbackArg);
        }

    private:
        Persistent<CB> m_callback;
        CBArg m_callbackArg;
    };

    template <typename CB>
    class DispatchCallbackNoArgTask final : public DispatchCallbackTaskBase {
    public:
        DispatchCallbackNoArgTask(CB* callback)
            : m_callback(callback)
        {
        }

        void performTask(ExecutionContext*) override
        {
            m_callback->handleEvent();
        }

    private:
        Persistent<CB> m_callback;
    };

    int m_numberOfPendingCallbacks;
    Member<DirectoryEntry> m_rootEntry;
};

template <typename CB, typename CBArg>
void DOMFileSystem::scheduleCallback(ExecutionContext* executionContext, CB* callback, CBArg* arg)
{
    ASSERT(executionContext->isContextThread());
    if (callback)
        executionContext->postTask(FROM_HERE, adoptPtr(new DispatchCallbackPtrArgTask<CB, CBArg>(callback, arg)));
}

template <typename CB, typename CBArg>
void DOMFileSystem::scheduleCallback(ExecutionContext* executionContext, CB* callback, const HeapVector<CBArg>& arg)
{
    ASSERT(executionContext->isContextThread());
    if (callback)
        executionContext->postTask(FROM_HERE, adoptPtr(new DispatchCallbackNonPtrArgTask<CB, PersistentHeapVector<CBArg>>(callback, arg)));
}

template <typename CB, typename CBArg>
void DOMFileSystem::scheduleCallback(ExecutionContext* executionContext, CB* callback, const CBArg& arg)
{
    ASSERT(executionContext->isContextThread());
    if (callback)
        executionContext->postTask(FROM_HERE, adoptPtr(new DispatchCallbackNonPtrArgTask<CB, CBArg>(callback, arg)));
}

template <typename CB, typename CBArg>
void DOMFileSystem::scheduleCallback(ExecutionContext* executionContext, CB* callback, const Member<CBArg>& arg)
{
    ASSERT(executionContext->isContextThread());
    if (callback)
        executionContext->postTask(FROM_HERE, adoptPtr(new DispatchCallbackNonPtrArgTask<CB, Persistent<CBArg>>(callback, arg)));
}

template <typename CB>
void DOMFileSystem::scheduleCallback(ExecutionContext* executionContext, CB* callback)
{
    ASSERT(executionContext->isContextThread());
    if (callback)
        executionContext->postTask(FROM_HERE, adoptPtr(new DispatchCallbackNoArgTask<CB>(callback)));
}

} // namespace blink

#endif // DOMFileSystem_h
