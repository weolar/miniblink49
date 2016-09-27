// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InternalsWebAudio_h
#define InternalsWebAudio_h

namespace blink {

class Internals;

class InternalsWebAudio {
public:
    static unsigned audioHandlerCount(Internals&);
};

} // namespace blink

#endif // InternalsWebAudio_h
