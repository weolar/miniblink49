/*
 * Copyright (c) 2008, 2009, 2012 Google Inc. All rights reserved.
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
#include "core/clipboard/DataObject.h"

#include "core/clipboard/DraggedIsolatedFileSystem.h"
#include "core/clipboard/Pasteboard.h"
#include "platform/clipboard/ClipboardMimeTypes.h"
#include "platform/clipboard/ClipboardUtilities.h"
#include "public/platform/Platform.h"
#include "public/platform/WebClipboard.h"
#include "public/platform/WebDragData.h"

namespace blink {

DataObject* DataObject::createFromPasteboard(PasteMode pasteMode)
{
    DataObject* dataObject = create();
    WebClipboard::Buffer buffer = Pasteboard::generalPasteboard()->buffer();
    uint64_t sequenceNumber = Platform::current()->clipboard()->sequenceNumber(buffer);
    bool ignored;
    WebVector<WebString> webTypes = Platform::current()->clipboard()->readAvailableTypes(buffer, &ignored);
    ListHashSet<String> types;
    for (size_t i = 0; i < webTypes.size(); ++i)
        types.add(webTypes[i]);
    for (const String& type : types) {
        if (pasteMode == PlainTextOnly && type != mimeTypeTextPlain)
            continue;
        dataObject->m_itemList.append(DataObjectItem::createFromPasteboard(type, sequenceNumber));
    }
    return dataObject;
}

DataObject* DataObject::create()
{
    return new DataObject;
}

DataObject::~DataObject()
{
}

size_t DataObject::length() const
{
    return m_itemList.size();
}

DataObjectItem* DataObject::item(unsigned long index)
{
    if (index >= length())
        return nullptr;
    return m_itemList[index];
}

void DataObject::deleteItem(unsigned long index)
{
    if (index >= length())
        return;
    m_itemList.remove(index);
}

void DataObject::clearAll()
{
    m_itemList.clear();
}

DataObjectItem* DataObject::add(const String& data, const String& type)
{
    DataObjectItem* item = DataObjectItem::createFromString(type, data);
    if (!internalAddStringItem(item))
        return nullptr;
    return item;
}

DataObjectItem* DataObject::add(File* file)
{
    if (!file)
        return nullptr;

    DataObjectItem* item = DataObjectItem::createFromFile(file);
    m_itemList.append(item);
    return item;
}

void DataObject::clearData(const String& type)
{
    for (size_t i = 0; i < m_itemList.size(); ++i) {
        if (m_itemList[i]->kind() == DataObjectItem::StringKind && m_itemList[i]->type() == type) {
            // Per the spec, type must be unique among all items of kind 'string'.
            m_itemList.remove(i);
            return;
        }
    }
}

ListHashSet<String> DataObject::types() const
{
    ListHashSet<String> results;
    bool containsFiles = false;
    for (size_t i = 0; i < m_itemList.size(); ++i) {
        switch (m_itemList[i]->kind()) {
        case DataObjectItem::StringKind:
            results.add(m_itemList[i]->type());
            break;
        case DataObjectItem::FileKind:
            containsFiles = true;
            break;
        }
    }
    if (containsFiles)
        results.add(mimeTypeFiles);
    return results;
}

String DataObject::getData(const String& type) const
{
    for (size_t i = 0; i < m_itemList.size(); ++i)  {
        if (m_itemList[i]->kind() == DataObjectItem::StringKind && m_itemList[i]->type() == type)
            return m_itemList[i]->getAsString();
    }
    return String();
}

void DataObject::setData(const String& type, const String& data)
{
    clearData(type);
    if (!add(data, type))
        ASSERT_NOT_REACHED();
}

void DataObject::urlAndTitle(String& url, String* title) const
{
    DataObjectItem* item = findStringItem(mimeTypeTextURIList);
    if (!item)
        return;
    url = convertURIListToURL(item->getAsString());
    if (title)
        *title = item->title();
}

void DataObject::setURLAndTitle(const String& url, const String& title)
{
    clearData(mimeTypeTextURIList);
    internalAddStringItem(DataObjectItem::createFromURL(url, title));
}

void DataObject::htmlAndBaseURL(String& html, KURL& baseURL) const
{
    DataObjectItem* item = findStringItem(mimeTypeTextHTML);
    if (!item)
        return;
    html = item->getAsString();
    baseURL = item->baseURL();
}

void DataObject::setHTMLAndBaseURL(const String& html, const KURL& baseURL)
{
    clearData(mimeTypeTextHTML);
    internalAddStringItem(DataObjectItem::createFromHTML(html, baseURL));
}

bool DataObject::containsFilenames() const
{
    for (size_t i = 0; i < m_itemList.size(); ++i) {
        if (m_itemList[i]->isFilename())
            return true;
    }
    return false;
}

Vector<String> DataObject::filenames() const
{
    Vector<String> results;
    for (size_t i = 0; i < m_itemList.size(); ++i) {
        if (m_itemList[i]->isFilename())
            results.append(toFile(m_itemList[i]->getAsFile())->path());
    }
    return results;
}

void DataObject::addFilename(const String& filename, const String& displayName)
{
    internalAddFileItem(DataObjectItem::createFromFile(File::createForUserProvidedFile(filename, displayName)));
}

void DataObject::addSharedBuffer(const String& name, PassRefPtr<SharedBuffer> buffer)
{
    internalAddFileItem(DataObjectItem::createFromSharedBuffer(name, buffer));
}

DataObject::DataObject()
    : m_modifiers(0)
{
}

DataObjectItem* DataObject::findStringItem(const String& type) const
{
    for (size_t i = 0; i < m_itemList.size(); ++i) {
        if (m_itemList[i]->kind() == DataObjectItem::StringKind && m_itemList[i]->type() == type)
            return m_itemList[i];
    }
    return nullptr;
}

bool DataObject::internalAddStringItem(DataObjectItem* item)
{
    ASSERT(item->kind() == DataObjectItem::StringKind);
    for (size_t i = 0; i < m_itemList.size(); ++i) {
        if (m_itemList[i]->kind() == DataObjectItem::StringKind && m_itemList[i]->type() == item->type())
            return false;
    }

    m_itemList.append(item);
    return true;
}

void DataObject::internalAddFileItem(DataObjectItem* item)
{
    ASSERT(item->kind() == DataObjectItem::FileKind);
    m_itemList.append(item);
}

DEFINE_TRACE(DataObject)
{
    visitor->trace(m_itemList);
    HeapSupplementable<DataObject>::trace(visitor);
}

DataObject* DataObject::create(WebDragData data)
{
    DataObject* dataObject = create();

    WebVector<WebDragData::Item> items = data.items();
    for (unsigned i = 0; i < items.size(); ++i) {
        WebDragData::Item item = items[i];

        switch (item.storageType) {
        case WebDragData::Item::StorageTypeString:
            if (String(item.stringType) == mimeTypeTextURIList)
                dataObject->setURLAndTitle(item.stringData, item.title);
            else if (String(item.stringType) == mimeTypeTextHTML)
                dataObject->setHTMLAndBaseURL(item.stringData, item.baseURL);
            else
                dataObject->setData(item.stringType, item.stringData);
            break;
        case WebDragData::Item::StorageTypeFilename:
            dataObject->addFilename(item.filenameData, item.displayNameData);
            break;
        case WebDragData::Item::StorageTypeBinaryData:
            // This should never happen when dragging in.
            break;
        case WebDragData::Item::StorageTypeFileSystemFile:
            {
                // FIXME: The file system URL may refer a user visible file, see http://crbug.com/429077
                FileMetadata fileMetadata;
                fileMetadata.length = item.fileSystemFileSize;
                dataObject->add(File::createForFileSystemFile(item.fileSystemURL, fileMetadata, File::IsNotUserVisible));
            }
            break;
        }
    }

    if (!data.filesystemId().isNull())
        DraggedIsolatedFileSystem::prepareForDataObject(dataObject, data.filesystemId());
    return dataObject;
}

WebDragData DataObject::toWebDragData()
{
    WebDragData data;
    data.initialize();
    WebVector<WebDragData::Item> itemList(length());

    for (size_t i = 0; i < length(); ++i) {
        DataObjectItem* originalItem = item(i);
        WebDragData::Item item;
        if (originalItem->kind() == DataObjectItem::StringKind) {
            item.storageType = WebDragData::Item::StorageTypeString;
            item.stringType = originalItem->type();
            item.stringData = originalItem->getAsString();
        } else if (originalItem->kind() == DataObjectItem::FileKind) {
            if (originalItem->sharedBuffer()) {
                item.storageType = WebDragData::Item::StorageTypeBinaryData;
                item.binaryData = originalItem->sharedBuffer();
            } else if (originalItem->isFilename()) {
                Blob* blob = originalItem->getAsFile();
                if (blob->isFile()) {
                    File* file = toFile(blob);
                    if (file->hasBackingFile()) {
                        item.storageType = WebDragData::Item::StorageTypeFilename;
                        item.filenameData = file->path();
                        item.displayNameData = file->name();
                    } else if (!file->fileSystemURL().isEmpty()) {
                        item.storageType = WebDragData::Item::StorageTypeFileSystemFile;
                        item.fileSystemURL = file->fileSystemURL();
                        item.fileSystemFileSize = file->size();
                    } else {
                        // FIXME: support dragging constructed Files across renderers, see http://crbug.com/394955
                        item.storageType = WebDragData::Item::StorageTypeString;
                        item.stringType = "text/plain";
                        item.stringData = file->name();
                    }
                } else {
                    ASSERT_NOT_REACHED();
                }
            } else {
                ASSERT_NOT_REACHED();
            }
        } else {
            ASSERT_NOT_REACHED();
        }
        item.title = originalItem->title();
        item.baseURL = originalItem->baseURL();
        itemList[i] = item;
    }
    data.swapItems(itemList);
    return data;
}

} // namespace blink
