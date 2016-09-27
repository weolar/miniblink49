// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "public/web/WebSelection.h"

#include "core/editing/SelectionType.h"
#include "core/layout/compositing/CompositedSelection.h"

namespace blink {

static WebSelectionBound getWebSelectionBound(const CompositedSelection& selection, bool isStart)
{
    ASSERT(selection.type != NoSelection);
    const CompositedSelectionBound& bound = isStart ? selection.start : selection.end;
    ASSERT(bound.layer);

    WebSelectionBound::Type type = WebSelectionBound::Caret;
    if (selection.type == RangeSelection) {
        if (isStart)
            type = bound.isTextDirectionRTL ? WebSelectionBound::SelectionRight : WebSelectionBound::SelectionLeft;
        else
            type = bound.isTextDirectionRTL ? WebSelectionBound::SelectionLeft : WebSelectionBound::SelectionRight;
    }

    WebSelectionBound result(type);
    result.layerId = bound.layer->platformLayer()->id();
    result.edgeTopInLayer = roundedIntPoint(bound.edgeTopInLayer);
    result.edgeBottomInLayer = roundedIntPoint(bound.edgeBottomInLayer);
    result.isTextDirectionRTL = bound.isTextDirectionRTL;
    return result;
}

// SelectionType enums have the same values; enforced in AssertMatchingEnums.cpp.
WebSelection::WebSelection(const CompositedSelection& selection)
    : m_selectionType(static_cast<WebSelection::SelectionType>(selection.type))
    , m_start(getWebSelectionBound(selection, true))
    , m_end(getWebSelectionBound(selection, false))
    , m_isEditable(selection.isEditable)
    , m_isEmptyTextFormControl(selection.isEmptyTextFormControl)
{
}

WebSelection::WebSelection(const WebSelection& other)
    : m_selectionType(other.m_selectionType)
    , m_start(other.m_start)
    , m_end(other.m_end)
    , m_isEditable(other.m_isEditable)
    , m_isEmptyTextFormControl(other.m_isEmptyTextFormControl)
{
}

} // namespace blink
