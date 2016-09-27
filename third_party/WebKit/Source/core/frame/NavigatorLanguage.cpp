// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/NavigatorLanguage.h"

#include "platform/Language.h"

namespace blink {

NavigatorLanguage::NavigatorLanguage()
    : m_languagesChanged(true)
{
}

AtomicString NavigatorLanguage::language()
{
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
