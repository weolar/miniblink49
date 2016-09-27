// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StyleMotionPath_h
#define StyleMotionPath_h

#include "wtf/RefCounted.h"

namespace blink {

class StyleMotionPath : public RefCounted<StyleMotionPath> {
public:
    virtual ~StyleMotionPath() { }
    virtual bool isPathStyleMotionPath() const { return false; }

protected:
    StyleMotionPath() { }
};

} // namespace blink

#endif // StyleMotionPath_h
