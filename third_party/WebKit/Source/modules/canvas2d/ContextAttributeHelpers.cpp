// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/canvas2d/ContextAttributeHelpers.h"

namespace blink {

Canvas2DContextAttributes to2DContextAttributes(const CanvasContextCreationAttributes& attrs)
{
    Canvas2DContextAttributes result;
    result.setAlpha(attrs.alpha());
    return result;
}

} // namespace blink
