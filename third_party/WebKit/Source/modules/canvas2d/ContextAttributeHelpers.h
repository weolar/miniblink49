// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ContextAttributeHelpers_h
#define ContextAttributeHelpers_h

#include "core/html/canvas/CanvasContextCreationAttributes.h"
#include "modules/canvas2d/Canvas2DContextAttributes.h"

namespace blink {

Canvas2DContextAttributes to2DContextAttributes(const CanvasContextCreationAttributes&);

} // namespace blink

#endif // ContextAttributeHelpers_h
