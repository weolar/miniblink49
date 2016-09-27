/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "platform/graphics/GraphicsLayerDebugInfo.h"

#include "public/platform/WebGraphicsLayerDebugInfo.h"
#include "wtf/text/CString.h"

namespace blink {

GraphicsLayerDebugInfo::GraphicsLayerDebugInfo()
    : m_compositingReasons(CompositingReasonNone)
    , m_ownerNodeId(0)
{
}

GraphicsLayerDebugInfo::~GraphicsLayerDebugInfo() { }

void GraphicsLayerDebugInfo::appendAsTraceFormat(WebString* out) const
{
    RefPtr<JSONObject> jsonObject = JSONObject::create();
    appendAnnotatedInvalidateRects(jsonObject.get());
    appendCompositingReasons(jsonObject.get());
    appendDebugName(jsonObject.get());
    appendOwnerNodeId(jsonObject.get());
    *out = jsonObject->toJSONString();
}

GraphicsLayerDebugInfo* GraphicsLayerDebugInfo::clone() const
{
    GraphicsLayerDebugInfo* toReturn = new GraphicsLayerDebugInfo();
    toReturn->setCompositingReasons(m_compositingReasons);
    toReturn->setOwnerNodeId(m_ownerNodeId);
    toReturn->m_invalidations = m_invalidations;
    toReturn->m_previousInvalidations = m_previousInvalidations;
    return toReturn;
}

void GraphicsLayerDebugInfo::appendAnnotatedInvalidateRects(JSONObject* jsonObject) const
{
    RefPtr<JSONArray> jsonArray = JSONArray::create();
    for (const auto& annotatedRect : m_previousInvalidations) {
        RefPtr<JSONObject> rectContainer = JSONObject::create();
        RefPtr<JSONArray> rectArray = JSONArray::create();
        const FloatRect& rect = annotatedRect.rect;
        rectArray->pushNumber(rect.x());
        rectArray->pushNumber(rect.y());
        rectArray->pushNumber(rect.width());
        rectArray->pushNumber(rect.height());
        rectContainer->setArray("geometry_rect", rectArray);
        rectContainer->setString("reason", paintInvalidationReasonToString(annotatedRect.reason));
        jsonArray->pushObject(rectContainer);
    }
    jsonObject->setArray("annotated_invalidation_rects", jsonArray);
}

void GraphicsLayerDebugInfo::appendCompositingReasons(JSONObject* jsonObject) const
{
    RefPtr<JSONArray> jsonArray = JSONArray::create();
    for (size_t i = 0; i < kNumberOfCompositingReasons; ++i) {
        if (!(m_compositingReasons & kCompositingReasonStringMap[i].reason))
            continue;
        jsonArray->pushString(kCompositingReasonStringMap[i].description);
    }
    jsonObject->setArray("compositing_reasons", jsonArray);
}

void GraphicsLayerDebugInfo::appendDebugName(JSONObject* jsonObject) const
{
    if (m_debugName.isEmpty())
        return;

    jsonObject->setString("layer_name", m_debugName);
}

void GraphicsLayerDebugInfo::appendOwnerNodeId(JSONObject* jsonObject) const
{
    if (!m_ownerNodeId)
        return;

    jsonObject->setNumber("owner_node", m_ownerNodeId);
}

void GraphicsLayerDebugInfo::appendAnnotatedInvalidateRect(const FloatRect& rect, PaintInvalidationReason invalidationReason)
{
    AnnotatedInvalidationRect annotatedRect = {
        rect,
        invalidationReason
    };
    m_invalidations.append(annotatedRect);
}

void GraphicsLayerDebugInfo::clearAnnotatedInvalidateRects()
{
    m_previousInvalidations.clear();
    m_previousInvalidations.swap(m_invalidations);
}

} // namespace blink
