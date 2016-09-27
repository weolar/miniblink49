// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SelectionStrategy_h
#define SelectionStrategy_h

namespace blink {

enum class SelectionStrategy {
    // Always using CharacterGranularity
    Character,
    // Switches between WordGranularity and CharacterGranularity
    // Depending on whether the selection or growing or shrinking
    Direction,
};

} // namespace blink

#endif // SelectionStrategy_h
