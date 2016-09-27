// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IDBValue_h
#define IDBValue_h

#include "modules/ModulesExport.h"
#include "modules/indexeddb/IDBKey.h"
#include "modules/indexeddb/IDBKeyPath.h"
#include "platform/SharedBuffer.h"
#include "public/platform/WebVector.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class BlobDataHandle;
class WebBlobInfo;

class MODULES_EXPORT IDBValue final : public RefCounted<IDBValue> {
public:
    static PassRefPtr<IDBValue> create();
    static PassRefPtr<IDBValue> create(PassRefPtr<SharedBuffer>, const WebVector<WebBlobInfo>&);
    static PassRefPtr<IDBValue> create(PassRefPtr<SharedBuffer>, const WebVector<WebBlobInfo>&, IDBKey*, const IDBKeyPath&);
    static PassRefPtr<IDBValue> create(const IDBValue*, IDBKey*, const IDBKeyPath&);

    bool isNull() const;
    Vector<String> getUUIDs() const;
    const SharedBuffer* data() const;
    Vector<WebBlobInfo>* blobInfo() const { return m_blobInfo.get(); }
    const IDBKey* primaryKey() const { return m_primaryKey; }
    const IDBKeyPath& keyPath() const { return m_keyPath; }

private:
    IDBValue();
    IDBValue(PassRefPtr<SharedBuffer>, const WebVector<WebBlobInfo>&);
    IDBValue(PassRefPtr<SharedBuffer>, const WebVector<WebBlobInfo>&, IDBKey*, const IDBKeyPath&);
    IDBValue(const IDBValue*, IDBKey*, const IDBKeyPath&);

    const RefPtr<SharedBuffer> m_data;
    const OwnPtr<Vector<RefPtr<BlobDataHandle>>> m_blobData;
    const OwnPtr<Vector<WebBlobInfo>> m_blobInfo;
    const Persistent<IDBKey> m_primaryKey;
    const IDBKeyPath m_keyPath;
};

} // namespace blink

#endif
