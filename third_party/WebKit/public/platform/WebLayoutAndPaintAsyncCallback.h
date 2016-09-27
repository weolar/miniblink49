// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebLayoutAndPaintAsyncCallback_h
#define WebLayoutAndPaintAsyncCallback_h

namespace blink {

class WebLayoutAndPaintAsyncCallback {
public:
    virtual void didLayoutAndPaint() = 0;
};

} // namespace blink

#endif // WebLayoutAndPaintAsyncCallback_h
