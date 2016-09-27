// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebIDBValue_h
#define WebIDBValue_h

#include "public/platform/WebBlobInfo.h"
#include "public/platform/WebData.h"
#include "public/platform/WebVector.h"
#include "public/platform/modules/indexeddb/WebIDBKey.h"
#include "public/platform/modules/indexeddb/WebIDBKeyPath.h"

namespace blink {

struct WebIDBValue {
    WebIDBValue() { }
    explicit WebIDBValue(const WebData& data) : data(data) { }
    WebIDBValue(const WebData& data, const WebVector<WebBlobInfo>& blobInfo) : data(data), webBlobInfo(blobInfo) { }
    WebIDBValue(const WebData& data, const WebVector<WebBlobInfo>& blobInfo, const WebIDBKey& primaryKey, const WebIDBKeyPath& keyPath) : data(data), webBlobInfo(blobInfo), primaryKey(primaryKey), keyPath(keyPath) { }

    // The serialized JavaScript bits (ignoring blob data) for this IDB Value. Required value.
    WebData data;
    // Collection of blob info referenced by [[data]]. Optional and empty for values without blobs.
    WebVector<WebBlobInfo> webBlobInfo;
    // The auto-generated primary key and key path. Both are set when IDB is generating keys (and not JavaScript).
    // Optional; If set then a property named [[keyPath]] will be set to [[primaryKey]] on the deserialized
    // [[data]] object before calling the event handler.
    WebIDBKey primaryKey;
    WebIDBKeyPath keyPath;
};

} // namespace blink

#endif
