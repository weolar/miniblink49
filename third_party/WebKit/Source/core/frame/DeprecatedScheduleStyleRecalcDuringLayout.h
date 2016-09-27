// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DeprecatedScheduleStyleRecalcDuringLayout_h
#define DeprecatedScheduleStyleRecalcDuringLayout_h

#include "core/dom/DocumentLifecycle.h"

namespace blink {

class DeprecatedScheduleStyleRecalcDuringLayout {
    WTF_MAKE_NONCOPYABLE(DeprecatedScheduleStyleRecalcDuringLayout);
public:
    explicit DeprecatedScheduleStyleRecalcDuringLayout(DocumentLifecycle&);
    ~DeprecatedScheduleStyleRecalcDuringLayout();

private:
    DocumentLifecycle& m_lifecycle;
    DocumentLifecycle::DeprecatedTransition m_deprecatedTransition;
    bool m_wasInPerformLayout;
};

}

#endif
