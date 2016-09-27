// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGLContextAttributeHelpers_h
#define WebGLContextAttributeHelpers_h

#include "core/html/canvas/CanvasContextCreationAttributes.h"
#include "modules/webgl/WebGLContextAttributes.h"
#include "public/platform/WebGraphicsContext3D.h"

namespace blink {

class Settings;

WebGLContextAttributes toWebGLContextAttributes(const CanvasContextCreationAttributes&);

// Set up the attributes that can be used to initialize a WebGraphicsContext3D.
// It's mostly based on WebGLContextAttributes, but may be adjusted based
// on settings.
WebGraphicsContext3D::Attributes toWebGraphicsContext3DAttributes(const WebGLContextAttributes&, const WebString&, Settings*, unsigned webGLVersion);

} // namespace blink

#endif // WebGLContextAttributeHelpers_h
