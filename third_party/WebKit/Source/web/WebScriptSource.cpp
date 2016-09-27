// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/WebScriptSource.h"

#include "bindings/core/v8/ScriptSourceCode.h"
#include "wtf/text/TextPosition.h"

namespace blink {

WebScriptSource::operator ScriptSourceCode() const
{
    TextPosition position(OrdinalNumber::fromOneBasedInt(startLine), OrdinalNumber::first());
    return ScriptSourceCode(code, url, position);
}

} // namespace blink
