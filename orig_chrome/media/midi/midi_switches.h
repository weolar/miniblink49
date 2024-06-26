// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Defines all the "midi" command-line switches.

#ifndef MEDIA_MIDI_MIDI_SWITCHES_H_
#define MEDIA_MIDI_MIDI_SWITCHES_H_

#include "build/build_config.h"
#include "media/midi/midi_export.h"

namespace switches {

#if defined(OS_ANDROID)
MIDI_EXPORT extern const char kUseAndroidMidiApi[];
#endif

} // namespace switches

#endif // MEDIA_MIDI_MIDI_SWITCHES_H_
