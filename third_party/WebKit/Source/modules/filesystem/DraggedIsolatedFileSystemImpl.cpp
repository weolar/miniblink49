/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "modules/filesystem/DraggedIsolatedFileSystemImpl.h"

#include "core/dom/ExecutionContext.h"
#include "modules/filesystem/DOMFileSystem.h"
#include "platform/Supplementable.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/MainThread.h"

namespace blink {

DOMFileSystem* DraggedIsolatedFileSystemImpl::getDOMFileSystem(DataObject* host, ExecutionContext* executionContext)
{
    DraggedIsolatedFileSystemImpl* draggedIsolatedFileSystem = from(host);
    if (!draggedIsolatedFileSystem)
        return 0;
    if (!draggedIsolatedFileSystem->m_filesystem)
        draggedIsolatedFileSystem->m_filesystem = DOMFileSystem::createIsolatedFileSystem(executionContext, host->filesystemId());
    return draggedIsolatedFileSystem->m_filesystem.get();
}

// static
const char* DraggedIsolatedFileSystemImpl::supplementName()
{
    ASSERT(isMainThread());
    return "DraggedIsolatedFileSystemImpl";
}

DraggedIsolatedFileSystemImpl* DraggedIsolatedFileSystemImpl::from(DataObject* dataObject)
{
    return static_cast<DraggedIsolatedFileSystemImpl*>(HeapSupplement<DataObject>::from(dataObject, supplementName()));
}

DraggedIsolatedFileSystemImpl::DraggedIsolatedFileSystemImpl(DataObject& host, const String& filesystemId)
{
    host.setFilesystemId(filesystemId);
}

DEFINE_TRACE(DraggedIsolatedFileSystemImpl)
{
    visitor->trace(m_filesystem);
    HeapSupplement<DataObject>::trace(visitor);
}

void DraggedIsolatedFileSystemImpl::prepareForDataObject(DataObject* dataObject, const String& filesystemId)
{
    DraggedIsolatedFileSystemImpl* fileSystem = create(*dataObject, filesystemId);
    DraggedIsolatedFileSystemImpl::provideTo(*dataObject, DraggedIsolatedFileSystemImpl::supplementName(), fileSystem);
}

} // namespace blink
