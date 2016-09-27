// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebSerializedOrigin.h"

#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

WebSerializedOrigin::WebSerializedOrigin(const SecurityOrigin& origin)
    : m_string(origin.toString())
{
}

} // namespace blink
