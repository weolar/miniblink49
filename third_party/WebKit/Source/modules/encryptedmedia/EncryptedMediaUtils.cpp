// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/encryptedmedia/EncryptedMediaUtils.h"

namespace blink {

namespace {

const char kTemporary[] = "temporary";
const char kPersistentLicense[] = "persistent-license";

} // namespace

WebEncryptedMediaInitDataType EncryptedMediaUtils::convertToInitDataType(const String& initDataType)
{
    if (initDataType == "cenc")
        return WebEncryptedMediaInitDataType::Cenc;
    if (initDataType == "keyids")
        return WebEncryptedMediaInitDataType::Keyids;
    if (initDataType == "webm")
        return WebEncryptedMediaInitDataType::Webm;

    // |initDataType| is not restricted in the idl, so anything is possible.
    return WebEncryptedMediaInitDataType::Unknown;
}

String EncryptedMediaUtils::convertFromInitDataType(WebEncryptedMediaInitDataType initDataType)
{
    switch (initDataType) {
    case WebEncryptedMediaInitDataType::Cenc:
        return "cenc";
    case WebEncryptedMediaInitDataType::Keyids:
        return "keyids";
    case WebEncryptedMediaInitDataType::Webm:
        return "webm";
    case WebEncryptedMediaInitDataType::Unknown:
        // Chromium should not use Unknown, but we use it in Blink when the
        // actual value has been blocked for non-same-origin or mixed content.
        return String();
    }

    ASSERT_NOT_REACHED();
    return String();
}

WebEncryptedMediaSessionType EncryptedMediaUtils::convertToSessionType(const String& sessionType)
{
    if (sessionType == kTemporary)
        return WebEncryptedMediaSessionType::Temporary;
    if (sessionType == kPersistentLicense)
        return WebEncryptedMediaSessionType::PersistentLicense;

    // |sessionType| is not restricted in the idl, so anything is possible.
    return WebEncryptedMediaSessionType::Unknown;
}

String EncryptedMediaUtils::convertFromSessionType(WebEncryptedMediaSessionType sessionType)
{
    switch (sessionType) {
    case WebEncryptedMediaSessionType::Temporary:
        return kTemporary;
    case WebEncryptedMediaSessionType::PersistentLicense:
        return kPersistentLicense;
    // FIXME: Remove once removed from Chromium (crbug.com/448888).
    case WebEncryptedMediaSessionType::PersistentReleaseMessage:
    case WebEncryptedMediaSessionType::Unknown:
        // Chromium should not use Unknown.
        ASSERT_NOT_REACHED();
        return String();
    }

    ASSERT_NOT_REACHED();
    return String();
}

} // namespace blink
