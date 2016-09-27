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
#include "modules/filesystem/HTMLInputElementFileSystem.h"

#include "core/fileapi/FileList.h"
#include "core/html/HTMLInputElement.h"
#include "modules/filesystem/DOMFilePath.h"
#include "modules/filesystem/DOMFileSystem.h"
#include "modules/filesystem/DirectoryEntry.h"
#include "modules/filesystem/Entry.h"
#include "modules/filesystem/FileEntry.h"
#include "platform/FileMetadata.h"
#include "platform/heap/Handle.h"

namespace blink {

// static
EntryHeapVector HTMLInputElementFileSystem::webkitEntries(ExecutionContext* executionContext, HTMLInputElement& input)
{
    EntryHeapVector entries;
    FileList* files = input.files();

    if (!files)
        return entries;

    DOMFileSystem* filesystem = DOMFileSystem::createIsolatedFileSystem(executionContext, input.droppedFileSystemId());
    if (!filesystem) {
        // Drag-drop isolated filesystem is not available.
        return entries;
    }

    for (unsigned i = 0; i < files->length(); ++i) {
        File* file = files->item(i);

        // FIXME: This involves synchronous file operation.
        FileMetadata metadata;
        if (!getFileMetadata(file->path(), metadata))
            continue;

        // The dropped entries are mapped as top-level entries in the isolated filesystem.
        String virtualPath = DOMFilePath::append("/", file->name());
        if (metadata.type == FileMetadata::TypeDirectory)
            entries.append(DirectoryEntry::create(filesystem, virtualPath));
        else
            entries.append(FileEntry::create(filesystem, virtualPath));
    }
    return entries;
}

HTMLInputElementFileSystem::HTMLInputElementFileSystem()
{
}

HTMLInputElementFileSystem::~HTMLInputElementFileSystem()
{
}

} // namespace blink
