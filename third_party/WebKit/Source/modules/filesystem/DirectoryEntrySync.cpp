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

#include "config.h"
#include "modules/filesystem/DirectoryEntrySync.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "modules/filesystem/DirectoryReaderSync.h"
#include "modules/filesystem/FileEntrySync.h"
#include "modules/filesystem/FileSystemFlags.h"
#include "modules/filesystem/SyncCallbackHelper.h"

namespace blink {

DirectoryEntrySync::DirectoryEntrySync(DOMFileSystemBase* fileSystem, const String& fullPath)
    : EntrySync(fileSystem, fullPath)
{
}

DirectoryReaderSync* DirectoryEntrySync::createReader()
{
    return DirectoryReaderSync::create(m_fileSystem, m_fullPath);
}

FileEntrySync* DirectoryEntrySync::getFile(const String& path, const FileSystemFlags& options, ExceptionState& exceptionState)
{
    EntrySyncCallbackHelper* helper = EntrySyncCallbackHelper::create();
    m_fileSystem->getFile(this, path, options, helper->successCallback(), helper->errorCallback(), DOMFileSystemBase::Synchronous);
    return static_cast<FileEntrySync*>(helper->getResult(exceptionState));
}

DirectoryEntrySync* DirectoryEntrySync::getDirectory(const String& path, const FileSystemFlags& options, ExceptionState& exceptionState)
{
    EntrySyncCallbackHelper* helper = EntrySyncCallbackHelper::create();
    m_fileSystem->getDirectory(this, path, options, helper->successCallback(), helper->errorCallback(), DOMFileSystemBase::Synchronous);
    return static_cast<DirectoryEntrySync*>(helper->getResult(exceptionState));
}

void DirectoryEntrySync::removeRecursively(ExceptionState& exceptionState)
{
    VoidSyncCallbackHelper* helper = VoidSyncCallbackHelper::create();
    m_fileSystem->removeRecursively(this, helper->successCallback(), helper->errorCallback(), DOMFileSystemBase::Synchronous);
    helper->getResult(exceptionState);
}

DEFINE_TRACE(DirectoryEntrySync)
{
    EntrySync::trace(visitor);
}

}
