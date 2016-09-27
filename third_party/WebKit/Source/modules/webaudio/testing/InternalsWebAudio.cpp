// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webaudio/testing/InternalsWebAudio.h"

#include "core/inspector/InstanceCounters.h"
#include "modules/webaudio/AudioNode.h"

namespace blink {

unsigned InternalsWebAudio::audioHandlerCount(Internals& internals)
{
    return InstanceCounters::counterValue(InstanceCounters::AudioHandlerCounter);
}

} // namespace blink
