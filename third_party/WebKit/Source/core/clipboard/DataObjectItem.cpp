/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "core/clipboard/DataObjectItem.h"

#include "core/clipboard/Pasteboard.h"
#include "core/fileapi/Blob.h"
#include "platform/clipboard/ClipboardMimeTypes.h"
#include "public/platform/Platform.h"
#include "public/platform/WebClipboard.h"

namespace blink {

DataObjectItem* DataObjectItem::createFromString(const String& type, const String& data)
{
    DataObjectItem* item = new DataObjectItem(StringKind, type);
    item->m_data = data;
    return item;
}

DataObjectItem* DataObjectItem::createFromFile(File* file)
{
    DataObjectItem* item = new DataObjectItem(FileKind, file->type());
    item->m_file = file;
    return item;
}

DataObjectItem* DataObjectItem::createFromURL(const String& url, const String& title)
{
    DataObjectItem* item = new DataObjectItem(StringKind, mimeTypeTextURIList);
    item->m_data = url;
    item->m_title = title;
    return item;
}

DataObjectItem* DataObjectItem::createFromHTML(const String& html, const KURL& baseURL)
{
    DataObjectItem* item = new DataObjectItem(StringKind, mimeTypeTextHTML);
    item->m_data = html;
    item->m_baseURL = baseURL;
    return item;
}

DataObjectItem* DataObjectItem::createFromSharedBuffer(const String& name, PassRefPtr<SharedBuffer> buffer)
{
    DataObjectItem* item = new DataObjectItem(FileKind, String());
    item->m_sharedBuffer = buffer;
    item->m_title = name;
    return item;
}

DataObjectItem* DataObjectItem::createFromPasteboard(const String& type, uint64_t sequenceNumber)
{
    if (type == mimeTypeImagePng)
        return new DataObjectItem(FileKind, type, sequenceNumber);
    return new DataObjectItem(StringKind, type, sequenceNumber);
}

DataObjectItem::DataObjectItem(Kind kind, const String& type)
    : m_source(InternalSource)
    , m_kind(kind)
    , m_type(type)
    , m_sequenceNumber(0)
{
}

DataObjectItem::DataObjectItem(Kind kind, const String& type, uint64_t sequenceNumber)
    : m_source(PasteboardSource)
    , m_kind(kind)
    , m_type(type)
    , m_sequenceNumber(sequenceNumber)
{
}

Blob* DataObjectItem::getAsFile() const
{
    if (kind() != FileKind)
        return nullptr;

    if (m_source == InternalSource) {
        if (m_file)
            return m_file.get();
        ASSERT(m_sharedBuffer);
        // FIXME: This code is currently impossible--we never populate m_sharedBuffer when dragging
        // in. At some point though, we may need to support correctly converting a shared buffer
        // into a file.
        return nullptr;
    }

    ASSERT(m_source == PasteboardSource);
    if (type() == mimeTypeImagePng) {
        // FIXME: This is pretty inefficient. We copy the data from the browser
        // to the renderer. We then place it in a blob in WebKit, which
        // registers it and copies it *back* to the browser. When a consumer
        // wants to read the data, we then copy the data back into the renderer.
        // https://bugs.webkit.org/show_bug.cgi?id=58107 has been filed to track
        // improvements to this code (in particular, add a registerClipboardBlob
        // method to the blob registry; that way the data is only copied over
        // into the renderer when it's actually read, not when the blob is
        // initially constructed).
        RefPtr<SharedBuffer> data = static_cast<PassRefPtr<SharedBuffer>>(Platform::current()->clipboard()->readImage(WebClipboard::BufferStandard));
        OwnPtr<BlobData> blobData = BlobData::create();
        blobData->appendBytes(data->data(), data->size());
        blobData->setContentType(mimeTypeImagePng);
        return Blob::create(BlobDataHandle::create(blobData.release(), data->size()));
    }

    return nullptr;
}

String DataObjectItem::getAsString() const
{
    ASSERT(m_kind == StringKind);

    if (m_source == InternalSource)
        return m_data;

    ASSERT(m_source == PasteboardSource);

    WebClipboard::Buffer buffer = Pasteboard::generalPasteboard()->buffer();
    String data;
    // This is ugly but there's no real alternative.
    if (m_type == mimeTypeTextPlain) {
        data = Platform::current()->clipboard()->readPlainText(buffer);
    } else if (m_type == mimeTypeTextHTML) {
        WebURL ignoredSourceURL;
        unsigned ignored;
        data = Platform::current()->clipboard()->readHTML(buffer, &ignoredSourceURL, &ignored, &ignored);
    } else {
        data = Platform::current()->clipboard()->readCustomData(buffer, m_type);
    }

    return Platform::current()->clipboard()->sequenceNumber(buffer) == m_sequenceNumber ? data : String();
}

bool DataObjectItem::isFilename() const
{
    // FIXME: https://bugs.webkit.org/show_bug.cgi?id=81261: When we properly support File dragout,
    // we'll need to make sure this works as expected for DragDataChromium.
    return m_kind == FileKind && m_file;
}

DEFINE_TRACE(DataObjectItem)
{
    visitor->trace(m_file);
}

} // namespace blink
