// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TextTrackKindUserPreference_h
#define TextTrackKindUserPreference_h

namespace blink {

// Defines user preference for text track kind.
enum class TextTrackKindUserPreference {
    // Display only tracks marked as default.
    Default,
    // If available, display captions track in preferred language, else display subtitles.
    Captions,
    // If available, display subtitles track in preferred language, else display captions.
    Subtitles
};

} // namespace blink

#endif // TextTrackKindUserPreference_h
