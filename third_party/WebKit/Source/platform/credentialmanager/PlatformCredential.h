// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PlatformCredential_h
#define PlatformCredential_h

#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/WTFString.h"

namespace blink {

class PLATFORM_EXPORT PlatformCredential : public GarbageCollectedFinalized<PlatformCredential> {
    WTF_MAKE_NONCOPYABLE(PlatformCredential);
public:
    static PlatformCredential* create(const String& id, const String& name, const KURL& iconURL);
    virtual ~PlatformCredential();

    const String& id() const { return m_id; }
    const String& name() const { return m_name; }
    const KURL& iconURL() const { return m_iconURL; }
    const String& type() const { return m_type; }

    virtual bool isPassword() { return false; }
    virtual bool isFederated() { return false; }

    DEFINE_INLINE_VIRTUAL_TRACE() { }

protected:
    PlatformCredential(const String& id, const String& name, const KURL& iconURL);

    void setType(const String& type) { m_type = type; }

private:
    String m_id;
    String m_name;
    KURL m_iconURL;
    String m_type;
};

} // namespace blink

#endif // PlatformCredential_h
