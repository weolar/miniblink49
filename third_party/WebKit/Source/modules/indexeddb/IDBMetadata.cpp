// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/indexeddb/IDBMetadata.h"

#include "public/platform/modules/indexeddb/WebIDBMetadata.h"

namespace blink {

IDBDatabaseMetadata::IDBDatabaseMetadata(const WebIDBMetadata& webMetadata)
    : name(webMetadata.name)
    , id(webMetadata.id)
    , version(webMetadata.version)
    , intVersion(webMetadata.intVersion)
    , maxObjectStoreId(webMetadata.maxObjectStoreId)
{
    for (size_t i = 0; i < webMetadata.objectStores.size(); ++i) {
        const WebIDBMetadata::ObjectStore webObjectStore = webMetadata.objectStores[i];
        IDBObjectStoreMetadata objectStore(webObjectStore.name, webObjectStore.id, IDBKeyPath(webObjectStore.keyPath), webObjectStore.autoIncrement, webObjectStore.maxIndexId);

        for (size_t j = 0; j < webObjectStore.indexes.size(); ++j) {
            const WebIDBMetadata::Index webIndex = webObjectStore.indexes[j];
            IDBIndexMetadata index(webIndex.name, webIndex.id, IDBKeyPath(webIndex.keyPath), webIndex.unique, webIndex.multiEntry);
            objectStore.indexes.set(index.id, index);
        }
        objectStores.set(objectStore.id, objectStore);
    }
}

} // namespace blink
