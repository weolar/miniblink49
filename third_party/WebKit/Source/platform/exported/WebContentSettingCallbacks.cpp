// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "public/platform/WebContentSettingCallbacks.h"

#include "platform/ContentSettingCallbacks.h"

namespace blink {

class WebContentSettingCallbacksPrivate : public RefCounted<WebContentSettingCallbacksPrivate> {
public:
    static PassRefPtr<WebContentSettingCallbacksPrivate> create(const PassOwnPtr<ContentSettingCallbacks>& callbacks)
    {
        return adoptRef(new WebContentSettingCallbacksPrivate(callbacks));
    }

    ContentSettingCallbacks* callbacks() { return m_callbacks.get(); }

private:
    WebContentSettingCallbacksPrivate(const PassOwnPtr<ContentSettingCallbacks>& callbacks) : m_callbacks(callbacks) { }
    OwnPtr<ContentSettingCallbacks> m_callbacks;
};

WebContentSettingCallbacks::WebContentSettingCallbacks(const PassOwnPtr<ContentSettingCallbacks>& callbacks)
{
    m_private = WebContentSettingCallbacksPrivate::create(callbacks);
}

void WebContentSettingCallbacks::reset()
{
    m_private.reset();
}

void WebContentSettingCallbacks::assign(const WebContentSettingCallbacks& other)
{
    m_private = other.m_private;
}

void WebContentSettingCallbacks::doAllow()
{
    ASSERT(!m_private.isNull());
    m_private->callbacks()->onAllowed();
    m_private.reset();
}

void WebContentSettingCallbacks::doDeny()
{
    ASSERT(!m_private.isNull());
    m_private->callbacks()->onDenied();
    m_private.reset();
}

} // namespace blink
