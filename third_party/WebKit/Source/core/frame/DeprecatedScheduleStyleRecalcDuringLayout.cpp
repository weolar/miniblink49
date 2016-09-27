// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/DeprecatedScheduleStyleRecalcDuringLayout.h"

#include "wtf/Assertions.h"

namespace blink {

DeprecatedScheduleStyleRecalcDuringLayout::DeprecatedScheduleStyleRecalcDuringLayout(DocumentLifecycle& lifecycle)
    : m_lifecycle(lifecycle)
    , m_deprecatedTransition(DocumentLifecycle::InPerformLayout, DocumentLifecycle::VisualUpdatePending)
    , m_wasInPerformLayout(lifecycle.state() == DocumentLifecycle::InPerformLayout)
{
}

DeprecatedScheduleStyleRecalcDuringLayout::~DeprecatedScheduleStyleRecalcDuringLayout()
{
    // This block of code is intended to restore the state machine to the
    // proper state. The style recalc will still have been schedule, however.
    if (m_wasInPerformLayout && m_lifecycle.state() != DocumentLifecycle::InPerformLayout) {
        ASSERT(m_lifecycle.state() == DocumentLifecycle::VisualUpdatePending);
        m_lifecycle.advanceTo(DocumentLifecycle::InPerformLayout);
    }
}

}
