// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCredential_h
#define WebCredential_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebPrivatePtr.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"

namespace blink {

class PlatformCredential;

class WebCredential {
public:
    BLINK_PLATFORM_EXPORT WebCredential(const WebString& id, const WebString& name, const WebURL& iconURL);
    BLINK_PLATFORM_EXPORT WebCredential(const WebCredential&);
    virtual ~WebCredential() { reset(); }

    BLINK_PLATFORM_EXPORT void assign(const WebCredential&);
    BLINK_PLATFORM_EXPORT void reset();

    BLINK_PLATFORM_EXPORT WebString id() const;
    BLINK_PLATFORM_EXPORT WebString name() const;
    BLINK_PLATFORM_EXPORT WebURL iconURL() const;
    BLINK_PLATFORM_EXPORT WebString type() const;

    BLINK_PLATFORM_EXPORT bool isPasswordCredential() const;
    BLINK_PLATFORM_EXPORT bool isFederatedCredential() const;

    // TODO(mkwst): Drop this once Chromium is updated. https://crbug.com/494880
    BLINK_PLATFORM_EXPORT bool isLocalCredential() const { return isPasswordCredential(); }

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT static WebCredential create(PlatformCredential*);
    BLINK_PLATFORM_EXPORT WebCredential& operator=(PlatformCredential*);
    BLINK_PLATFORM_EXPORT PlatformCredential* platformCredential() const { return m_platformCredential.get(); }
#endif

protected:
#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebCredential(PlatformCredential*);
#endif

    WebPrivatePtr<PlatformCredential> m_platformCredential;
};

} // namespace blink

#endif // WebCredential_h
