// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/midi/midi_switches.h"

namespace switches {

#if defined(OS_ANDROID)
// Use Android Midi API for WebMIDI
const char kUseAndroidMidiApi[] = "use-android-midi-api";
#endif

} // namespace switches
