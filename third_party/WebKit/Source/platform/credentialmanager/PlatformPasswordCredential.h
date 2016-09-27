// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PlatformPasswordCredential_h
#define PlatformPasswordCredential_h

#include "platform/credentialmanager/PlatformCredential.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class PLATFORM_EXPORT PlatformPasswordCredential final : public PlatformCredential {
    WTF_MAKE_NONCOPYABLE(PlatformPasswordCredential);
public:
    static PlatformPasswordCredential* create(const String& id, const String& password, const String& name, const KURL& iconURL);
    ~PlatformPasswordCredential() override;

    const String& password() const { return m_password; }

    bool isPassword() override { return true; }

private:
    PlatformPasswordCredential(const String& id, const String& password, const String& name, const KURL& iconURL);

    String m_password;
};

} // namespace blink

#endif // PlatformPasswordCredential_h
