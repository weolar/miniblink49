// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebEncryptedMediaKeyInformation_h
#define WebEncryptedMediaKeyInformation_h

#include "WebCommon.h"
#include "public/platform/WebData.h"

namespace blink {

class BLINK_PLATFORM_EXPORT WebEncryptedMediaKeyInformation {
public:
    enum class KeyStatus {
        Usable,
        Expired,
        Released,
        OutputRestricted,
        OutputDownscaled,
        StatusPending,
        InternalError
    };

    WebEncryptedMediaKeyInformation();
    ~WebEncryptedMediaKeyInformation();

    WebData id() const;
    void setId(const WebData&);

    KeyStatus status() const;
    void setStatus(KeyStatus);

    uint32_t systemCode() const;
    void setSystemCode(uint32_t);

private:
    WebData m_id;
    KeyStatus m_status;
    uint32_t m_systemCode;
};

} // namespace blink

#endif // WebEncryptedMediaKeyInformation_h
