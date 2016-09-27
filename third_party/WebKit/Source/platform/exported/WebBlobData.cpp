/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
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
#include "public/platform/WebBlobData.h"

#include "platform/blob/BlobData.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

WebBlobData::WebBlobData()
{
}

WebBlobData::~WebBlobData()
{
    m_private.reset(0);
}

size_t WebBlobData::itemCount() const
{
    ASSERT(!isNull());
    return m_private->items().size();
}

bool WebBlobData::itemAt(size_t index, Item& result) const
{
    ASSERT(!isNull());

    if (index >= m_private->items().size())
        return false;

    const BlobDataItem& item = m_private->items()[index];
    result.data.reset();
    result.filePath.reset();
    result.blobUUID.reset();
    result.offset = item.offset;
    result.length = item.length;
    result.expectedModificationTime = item.expectedModificationTime;

    switch (item.type) {
    case BlobDataItem::Data:
        result.type = Item::TypeData;
        result.data = item.data;
        return true;
    case BlobDataItem::File:
        result.type = Item::TypeFile;
        result.filePath = item.path;
        return true;
    case BlobDataItem::Blob:
        result.type = Item::TypeBlob;
        result.blobUUID = item.blobDataHandle->uuid();
        return true;
    case BlobDataItem::FileSystemURL:
        result.type = Item::TypeFileSystemURL;
        result.fileSystemURL = item.fileSystemURL;
        return true;
    }
    ASSERT_NOT_REACHED();
    return false;
}

WebString WebBlobData::contentType() const
{
    ASSERT(!isNull());
    return m_private->contentType();
}

WebBlobData::WebBlobData(const PassOwnPtr<BlobData>& data)
    : m_private(data)
{
}

WebBlobData& WebBlobData::operator=(const PassOwnPtr<BlobData>& data)
{
    m_private.reset(data);
    return *this;
}

WebBlobData::operator PassOwnPtr<BlobData>()
{
    return m_private.release();
}

} // namespace blink
