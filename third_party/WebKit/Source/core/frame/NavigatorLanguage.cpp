// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/NavigatorLanguage.h"
#include "core/frame/Settings.h"
#include "core/page/Page.h"

#include "platform/Language.h"

namespace blink {

NavigatorLanguage::NavigatorLanguage(LocalFrame* frame)
    : m_languagesChanged(true)
    , m_frame_(frame)
{
}

AtomicString NavigatorLanguage::language()
{
    Page *page = m_frame_->page();
    if (page) {
        Settings &seting = page->settings();
        return AtomicString(seting.language());
    } else
        return defaultLanguage();
}

bool NavigatorLanguage::hasLanguagesChanged()
{
    if (!m_languagesChanged)
        return false;

    m_languagesChanged = false;
    return true;
}

void NavigatorLanguage::setLanguagesChanged()
{
    m_languagesChanged = true;
}

} // namespace blink
