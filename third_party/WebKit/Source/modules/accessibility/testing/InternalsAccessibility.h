// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InternalsAccessibility_h
#define InternalsAccessibility_h

namespace blink {

class Internals;

class InternalsAccessibility {
public:
    static unsigned numberOfLiveAXObjects(Internals&);
};

} // namespace blink

#endif // InternalsAccessibility_h
