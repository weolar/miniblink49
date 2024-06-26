// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_MIDI_RESULT_H_
#define MEDIA_MIDI_RESULT_H_

namespace media {
namespace midi {

    // Result codes for MIDI.
    enum class Result {
        NOT_INITIALIZED = 0,
        OK,
        NOT_SUPPORTED,
        INITIALIZATION_ERROR,
        // New code should be inserted here so that existing members keep the same
        // assigned value. tools/metrics/histograms/histograms.xml contains Result
        // enum entry, and it should be consistent with enum class Result.

        // |MAX| is used in content/common/media/midi_messages.h with
        // IPC_ENUM_TRAITS_MAX_VALUE macro. Keep the value up to date. Otherwise
        // a new value can not be passed to the renderer.
        MAX = INITIALIZATION_ERROR,
    };

} // namespace midi
} // namespace media

#endif // MEDIA_MIDI_RESULT_H_
