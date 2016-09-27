// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/websockets/CloseEvent.h"

namespace blink {

CloseEvent::CloseEvent(const AtomicString& type, const CloseEventInit& initializer)
    : Event(type, initializer)
    , m_wasClean(false)
    , m_code(0)
{
    if (initializer.hasWasClean())
        m_wasClean = initializer.wasClean();
    if (initializer.hasCode())
        m_code = initializer.code();
    if (initializer.hasReason())
        m_reason = initializer.reason();
}

} // namespace blink
