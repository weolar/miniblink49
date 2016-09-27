// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/TracedLayoutObject.h"

#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutText.h"
#include "core/layout/LayoutView.h"
#include "platform/JSONValues.h"

namespace blink {

PassRefPtr<TraceEvent::ConvertableToTraceFormat> TracedLayoutObject::create(const LayoutView& view)
{
    return adoptRef(new TracedLayoutObject(view));
}

String TracedLayoutObject::asTraceFormat() const
{
    StringBuilder builder;
    RefPtr<JSONObject> json(toJSON());
    json->writeJSON(&builder);
    return builder.toString();
}

TracedLayoutObject::TracedLayoutObject(const LayoutObject& object)
    : m_address((unsigned long) &object)
    , m_isAnonymous(object.isAnonymous())
    , m_isPositioned(object.isOutOfFlowPositioned())
    , m_isRelPositioned(object.isRelPositioned())
    , m_isFloating(object.isFloating())
    , m_selfNeeds(object.selfNeedsLayout())
    , m_positionedMovement(object.needsPositionedMovementLayout())
    , m_childNeeds(object.normalChildNeedsLayout())
    , m_posChildNeeds(object.posChildNeedsLayout())
    , m_isTableCell(object.isTableCell())
    , m_name(String(object.name()).isolatedCopy())
    , m_absRect(object.absoluteBoundingBoxRect())
{
    if (Node* node = object.node()) {
        m_tag = String(node->nodeName()).isolatedCopy();
        if (node->isElementNode()) {
            Element& element = toElement(*node);
            if (element.hasID())
                m_id = String(element.getIdAttribute()).isolatedCopy();
            if (element.hasClass()) {
                for (size_t i = 0; i < element.classNames().size(); ++i) {
                    m_classNames.append(
                        String(element.classNames()[i]).isolatedCopy());
                }
            }
        }
    }

    // FIXME: When the fixmes in LayoutTreeAsText::writeLayoutObject() are
    // fixed, deduplicate it with this.
    if (object.isText()) {
        m_rect = LayoutRect(toLayoutText(object).linesBoundingBox());
    } else if (object.isLayoutInline()) {
        m_rect = LayoutRect(toLayoutInline(object).linesBoundingBox());
    } else if (object.isBox()) {
        m_rect = toLayoutBox(&object)->frameRect();
    }

    if (m_isTableCell) {
        const LayoutTableCell& c = toLayoutTableCell(object);
        if (c.row() && c.row()->rowIndexWasSet() && c.hasCol()
            && (!c.row()->section() || !c.row()->section()->needsCellRecalc())) {
            m_row = c.rowIndex();
            m_col = c.col();
            m_rowSpan = c.rowSpan();
            m_colSpan = c.colSpan();
        }
    }

    for (LayoutObject* child = object.slowFirstChild(); child; child = child->nextSibling()) {
        m_children.append(adoptRef(new TracedLayoutObject(*child)));
    }
}

PassRefPtr<JSONObject> TracedLayoutObject::toJSON() const
{
    RefPtr<JSONObject> json(JSONObject::create());
    json->setNumber("address", m_address);
    json->setString("name", m_name);
    if (!m_tag.isEmpty())
        json->setString("tag", m_tag);
    if (!m_id.isEmpty())
        json->setString("htmlId", m_id);
    if (m_classNames.size()) {
        RefPtr<JSONArray> classNames(JSONArray::create());
        for (const auto& className : m_classNames) {
            classNames->pushString(className);
        }
        json->setArray("classNames", classNames);
    }
    json->setNumber("absX", m_absRect.x());
    json->setNumber("absY", m_absRect.y());
    json->setNumber("relX", m_rect.x());
    json->setNumber("relY", m_rect.y());
    json->setNumber("width", m_rect.width());
    json->setNumber("height", m_rect.height());
    if (m_isPositioned)
        json->setBoolean("positioned", m_isPositioned);
    if (m_selfNeeds)
        json->setBoolean("selfNeeds", m_selfNeeds);
    if (m_positionedMovement)
        json->setBoolean("positionedMovement", m_positionedMovement);
    if (m_childNeeds)
        json->setBoolean("childNeeds", m_childNeeds);
    if (m_posChildNeeds)
        json->setBoolean("posChildNeeds", m_posChildNeeds);
    if (m_isTableCell) {
        json->setNumber("row", m_row);
        json->setNumber("col", m_col);
        if (m_rowSpan != 1)
            json->setNumber("rowSpan", m_rowSpan);
        if (m_colSpan != 1)
            json->setNumber("colSpan", m_colSpan);
    }
    if (m_isAnonymous)
        json->setBoolean("anonymous", m_isAnonymous);
    if (m_isRelPositioned)
        json->setBoolean("relativePositioned", m_isRelPositioned);
    if (m_isFloating)
        json->setBoolean("float", m_isFloating);
    if (m_children.size()) {
        RefPtr<JSONArray> children(JSONArray::create());
        for (const auto& child : m_children) {
            children->pushObject(child->toJSON());
        }
        json->setArray("children", children);
    }
    return json.release();
}

} // namespace blink
