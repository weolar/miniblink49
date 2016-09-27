// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/Counter.h"

namespace blink {

#if ENABLE(OILPAN)
DEFINE_TRACE(Counter)
{
    visitor->trace(m_identifier);
    visitor->trace(m_listStyle);
    visitor->trace(m_separator);
}
#endif

}
