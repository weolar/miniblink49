// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/testing/RefCountedScriptWrappable.h"

namespace blink {

RefCountedScriptWrappable::RefCountedScriptWrappable(const String& string)
    : m_string(string)
{
}

RefCountedScriptWrappable::~RefCountedScriptWrappable()
{
}

PassRefPtr<RefCountedScriptWrappable> RefCountedScriptWrappable::create(const String& string)
{
    return adoptRef(new RefCountedScriptWrappable(string));
}

} // namespace blink

