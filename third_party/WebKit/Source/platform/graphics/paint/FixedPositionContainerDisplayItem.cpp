// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/paint/FixedPositionContainerDisplayItem.h"

#include "platform/graphics/GraphicsContext.h"
#include "platform/transforms/AffineTransform.h"
#include "public/platform/WebDisplayItemList.h"

namespace blink {

void BeginFixedPositionContainerDisplayItem::appendToWebDisplayItemList(WebDisplayItemList* list) const
{
    // TODO(trchen): Should implement the compositor counterpart and update transform tree algorithm.
}

void EndFixedPositionContainerDisplayItem::appendToWebDisplayItemList(WebDisplayItemList* list) const
{
    // TODO(trchen): Should implement the compositor counterpart and update transform tree algorithm.
}

} // namespace blink
