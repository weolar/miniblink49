// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGraphicsContext_h
#define WebGraphicsContext_h

#include "public/platform/WebCanvas.h"
#include "public/platform/WebFloatRect.h"

namespace blink {

// Wraps a blink::GraphicsContext.
// Internal consumers can extract the underlying context (via WebGraphicsContextImpl).
// External consumers can make a single drawing using a WebCanvas.
class WebGraphicsContext {
public:
    virtual WebCanvas* beginDrawing(const WebFloatRect& bounds) = 0;
    virtual void endDrawing() = 0;

protected:
    ~WebGraphicsContext() { }
};

} // namespace blink

#endif
