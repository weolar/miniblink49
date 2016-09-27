// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/canvas/CanvasRenderingContextFactory.h"

#include "core/html/HTMLCanvasElement.h"
//#include "core/html/canvas/CanvasRenderingContext2D.h"
#include "modules/canvas2d/CanvasRenderingContext2D.h"
#include "wtf/OwnPtr.h"

namespace blink {

void CanvasRenderingContextFactory::init()
{
    OwnPtr<CanvasRenderingContextFactory> canvas2dFactory = adoptPtr(new CanvasRenderingContext2D::Factory());
    HTMLCanvasElement::registerRenderingContextFactory(canvas2dFactory.release());
}

} // namespace blink
