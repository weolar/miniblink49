// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScreenScreenOrientation_h
#define ScreenScreenOrientation_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class ScreenOrientation;
class Screen;
class ScriptState;

class ScreenScreenOrientation final : public GarbageCollected<ScreenScreenOrientation>, public HeapSupplement<Screen> {
    USING_GARBAGE_COLLECTED_MIXIN(ScreenScreenOrientation);
public:
    static ScreenScreenOrientation& from(Screen&);

    static ScreenOrientation* orientation(ScriptState*, Screen&);

    DECLARE_VIRTUAL_TRACE();

private:
    static const char* supplementName();

    Member<ScreenOrientation> m_orientation;
};

} // namespace blink

#endif // ScreenScreenOrientation_h
