// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebContentSettingCallbacks_h
#define WebContentSettingCallbacks_h

#include "WebPrivatePtr.h"

namespace WTF { template <typename T> class PassOwnPtr; }

namespace blink {

class ContentSettingCallbacks;
class WebContentSettingCallbacksPrivate;

class WebContentSettingCallbacks {
public:
    ~WebContentSettingCallbacks() { reset(); }
    WebContentSettingCallbacks() { }
    WebContentSettingCallbacks(const WebContentSettingCallbacks& c) { assign(c); }
    WebContentSettingCallbacks& operator=(const WebContentSettingCallbacks& c)
    {
        assign(c);
        return *this;
    }

    BLINK_PLATFORM_EXPORT void reset();
    BLINK_PLATFORM_EXPORT void assign(const WebContentSettingCallbacks&);

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebContentSettingCallbacks(const WTF::PassOwnPtr<ContentSettingCallbacks>&);
#endif

    BLINK_PLATFORM_EXPORT void doAllow();
    BLINK_PLATFORM_EXPORT void doDeny();

private:
    WebPrivatePtr<WebContentSettingCallbacksPrivate> m_private;
};

} // namespace blink

#endif
