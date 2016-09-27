// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/ScopeRecorder.h"

#include "core/layout/LayoutObject.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"

namespace blink {

ScopeRecorder::ScopeRecorder(GraphicsContext& context, const DisplayItemClientWrapper& object)
    : m_displayItemList(context.displayItemList())
    , m_object(object)
{
    if (!RuntimeEnabledFeatures::slimmingPaintEnabled())
        return;

    ASSERT(m_displayItemList);
    m_displayItemList->beginScope(object.displayItemClient());
}

ScopeRecorder::~ScopeRecorder()
{
    if (!RuntimeEnabledFeatures::slimmingPaintEnabled())
        return;

    m_displayItemList->endScope(m_object.displayItemClient());
}

} // namespace blink
