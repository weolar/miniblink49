// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webmidi/MIDIOutputMap.h"

#include "bindings/modules/v8/V8MIDIOutput.h"

namespace blink {

MIDIOutputMap::MIDIOutputMap(HeapVector<Member<MIDIOutput>>& entries)
    : MIDIPortMap<MIDIOutput>(entries)
{
}

} // namespace blink
