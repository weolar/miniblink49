// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webmidi/MIDIInputMap.h"

#include "bindings/modules/v8/V8MIDIInput.h"

namespace blink {

MIDIInputMap::MIDIInputMap(const HeapVector<Member<MIDIInput>>& entries)
    : MIDIPortMap<MIDIInput>(entries)
{
}

} // namespace blink

