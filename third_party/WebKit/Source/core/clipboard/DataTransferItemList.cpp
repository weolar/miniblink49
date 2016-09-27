/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008, 2009 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/clipboard/DataTransferItemList.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/clipboard/DataObject.h"
#include "core/clipboard/DataTransfer.h"
#include "core/clipboard/DataTransferItem.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

DataTransferItemList* DataTransferItemList::create(DataTransfer* dataTransfer, DataObject* list)
{
    return new DataTransferItemList(dataTransfer, list);
}

size_t DataTransferItemList::length() const
{
    if (!m_dataTransfer->canReadTypes())
        return 0;
    return m_dataObject->length();
}

DataTransferItem* DataTransferItemList::item(unsigned long index)
{
    if (!m_dataTransfer->canReadTypes())
        return nullptr;
    DataObjectItem* item = m_dataObject->item(index);
    if (!item)
        return nullptr;

    return DataTransferItem::create(m_dataTransfer, item);
}

void DataTransferItemList::deleteItem(unsigned long index, ExceptionState& exceptionState)
{
    if (!m_dataTransfer->canWriteData()) {
        exceptionState.throwDOMException(InvalidStateError, "The list is not writable.");
        return;
    }
    m_dataObject->deleteItem(index);
}

void DataTransferItemList::clear()
{
    if (!m_dataTransfer->canWriteData())
        return;
    m_dataObject->clearAll();
}

DataTransferItem* DataTransferItemList::add(const String& data, const String& type, ExceptionState& exceptionState)
{
    if (!m_dataTransfer->canWriteData())
        return nullptr;
    DataObjectItem* item = m_dataObject->add(data, type);
    if (!item) {
        exceptionState.throwDOMException(NotSupportedError, "An item already exists for type '" + type + "'.");
        return nullptr;
    }
    return DataTransferItem::create(m_dataTransfer, item);
}

DataTransferItem* DataTransferItemList::add(File* file)
{
    if (!m_dataTransfer->canWriteData())
        return nullptr;
    DataObjectItem* item = m_dataObject->add(file);
    if (!item)
        return nullptr;
    return DataTransferItem::create(m_dataTransfer, item);
}

DataTransferItemList::DataTransferItemList(DataTransfer* dataTransfer, DataObject* dataObject)
    : m_dataTransfer(dataTransfer)
    , m_dataObject(dataObject)
{
}

DEFINE_TRACE(DataTransferItemList)
{
    visitor->trace(m_dataTransfer);
    visitor->trace(m_dataObject);
}

} // namespace blink
