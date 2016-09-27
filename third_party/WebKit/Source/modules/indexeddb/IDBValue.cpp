// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/indexeddb/IDBValue.h"

#include "platform/blob/BlobData.h"
#include "public/platform/WebBlobInfo.h"

namespace blink {

IDBValue::IDBValue() = default;

IDBValue::IDBValue(PassRefPtr<SharedBuffer> data, const WebVector<WebBlobInfo>& webBlobInfo, IDBKey* primaryKey, const IDBKeyPath& keyPath)
    : m_data(data)
    , m_blobData(adoptPtr(new Vector<RefPtr<BlobDataHandle>>()))
    , m_blobInfo(adoptPtr(new Vector<WebBlobInfo>(webBlobInfo.size())))
    , m_primaryKey(primaryKey && primaryKey->isValid() ? primaryKey : nullptr)
    , m_keyPath(keyPath)
{
    for (size_t i = 0; i < webBlobInfo.size(); ++i) {
        const WebBlobInfo& info = (*m_blobInfo)[i] = webBlobInfo[i];
        m_blobData->append(BlobDataHandle::create(info.uuid(), info.type(), info.size()));
    }
}

IDBValue::IDBValue(PassRefPtr<SharedBuffer> data, const WebVector<WebBlobInfo>& webBlobInfo)
    : IDBValue(data, webBlobInfo, nullptr, IDBKeyPath())
{
}

IDBValue::IDBValue(const IDBValue* value, IDBKey* primaryKey, const IDBKeyPath& keyPath)
    : m_data(value->m_data)
    , m_blobData(adoptPtr(new Vector<RefPtr<BlobDataHandle>>()))
    , m_blobInfo(adoptPtr(new Vector<WebBlobInfo>(value->m_blobInfo->size())))
    , m_primaryKey(primaryKey)
    , m_keyPath(keyPath)
{
    for (size_t i = 0; i < value->m_blobInfo->size(); ++i) {
        const WebBlobInfo& info = (*m_blobInfo)[i] = value->m_blobInfo->at(i);
        m_blobData->append(BlobDataHandle::create(info.uuid(), info.type(), info.size()));
    }
}

PassRefPtr<IDBValue> IDBValue::create()
{
    return adoptRef(new IDBValue());
}

PassRefPtr<IDBValue> IDBValue::create(PassRefPtr<SharedBuffer>data, const WebVector<WebBlobInfo>& webBlobInfo)
{
    return adoptRef(new IDBValue(data, webBlobInfo));
}

PassRefPtr<IDBValue> IDBValue::create(PassRefPtr<SharedBuffer>data, const WebVector<WebBlobInfo>& webBlobInfo, IDBKey* primaryKey, const IDBKeyPath& keyPath)
{
    return adoptRef(new IDBValue(data, webBlobInfo, primaryKey, keyPath));
}

PassRefPtr<IDBValue> IDBValue::create(const IDBValue* value, IDBKey* primaryKey, const IDBKeyPath& keyPath)
{
    return adoptRef(new IDBValue(value, primaryKey, keyPath));
}

Vector<String> IDBValue::getUUIDs() const
{
    Vector<String> uuids;
    uuids.reserveCapacity(m_blobInfo->size());
    for (const auto& info : *m_blobInfo)
        uuids.append(info.uuid());
    return uuids;
}

const SharedBuffer* IDBValue::data() const
{
    return m_data.get();
}

bool IDBValue::isNull() const
{
    return !m_data.get();
}

} // namespace blink
