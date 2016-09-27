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

#ifndef WebDragData_h
#define WebDragData_h

#include "WebCommon.h"
#include "WebData.h"
#include "WebString.h"
#include "WebURL.h"
#include "WebVector.h"

namespace blink {

template <typename T> class WebVector;

// Holds data that may be exchanged through a drag-n-drop operation. It is
// inexpensive to copy a WebDragData object.
class WebDragData {
public:
    struct Item {
        enum StorageType {
            // String data with an associated MIME type. Depending on the MIME type, there may be
            // optional metadata attributes as well.
            StorageTypeString,
            // Stores the name of one file being dragged into the renderer.
            StorageTypeFilename,
            // An image being dragged out of the renderer. Contains a buffer holding the image data
            // as well as the suggested name for saving the image to.
            StorageTypeBinaryData,
            // Stores the filesystem URL of one file being dragged into the renderer.
            StorageTypeFileSystemFile,
        };

        StorageType storageType;

        // Only valid when storageType == StorageTypeString.
        WebString stringType;
        WebString stringData;

        // Only valid when storageType == StorageTypeFilename.
        WebString filenameData;
        WebString displayNameData;

        // Only valid when storageType == StorageTypeBinaryData.
        WebData binaryData;

        // Title associated with a link when stringType == "text/uri-list".
        // Filename when storageType == StorageTypeBinaryData.
        WebString title;

        // Only valid when storageType == StorageTypeFileSystemFile.
        WebURL fileSystemURL;
        long long fileSystemFileSize;

        // Only valid when stringType == "text/html".
        WebURL baseURL;
    };

    WebDragData()
        : m_valid(false)
        , m_modifierKeyState(0)
    { }

    WebDragData(const WebDragData& object)
        : m_valid(object.m_valid)
        , m_itemList(object.m_itemList)
        , m_modifierKeyState(object.m_modifierKeyState)
        , m_filesystemId(object.m_filesystemId)
    { }

    WebDragData& operator=(const WebDragData& object)
    {
        m_valid = object.m_valid;
        m_itemList = object.m_itemList;
        m_modifierKeyState = object.m_modifierKeyState;
        m_filesystemId = object.m_filesystemId;
        return *this;
    }

    ~WebDragData() { }

    WebVector<Item> items() const
    {
        return m_itemList;
    }

    BLINK_PLATFORM_EXPORT void setItems(WebVector<Item> itemList);
    // FIXME: setItems is slow because setItems copies WebVector.
    // Instead, use swapItems.
    void swapItems(WebVector<Item>& itemList)
    {
        m_itemList.swap(itemList);
    }

    void initialize() { m_valid = true; }
    bool isNull() const { return !m_valid; }
    void reset() { m_itemList = WebVector<Item>(); m_valid = false; }

    BLINK_PLATFORM_EXPORT void addItem(const Item&);

    WebString filesystemId() const
    {
        return m_filesystemId;
    }

    void setFilesystemId(const WebString& filesystemId)
    {
        // The ID is an opaque string, given by and validated by chromium port.
        m_filesystemId = filesystemId;
    }

private:
    bool m_valid;
    WebVector<Item> m_itemList;
    int m_modifierKeyState; // State of Shift/Ctrl/Alt/Meta keys.
    WebString m_filesystemId;
};

} // namespace blink

#endif
