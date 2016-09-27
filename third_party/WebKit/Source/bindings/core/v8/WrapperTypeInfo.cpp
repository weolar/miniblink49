// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/WrapperTypeInfo.h"

#include "core/events/EventTarget.h"

namespace blink {

EventTarget* WrapperTypeInfo::toEventTarget(v8::Local<v8::Object> object) const
{
    if (eventTargetInheritance == NotInheritFromEventTarget)
        return 0;
    return static_cast<EventTarget*>(toScriptWrappable(object));
}

} // namespace blink
