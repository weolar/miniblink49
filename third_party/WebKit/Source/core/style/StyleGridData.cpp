/*
 * Copyright (C) 2011 Google Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"
#include "core/style/StyleGridData.h"

#include "core/style/ComputedStyle.h"

namespace blink {

StyleGridData::StyleGridData()
    : m_gridTemplateColumns(ComputedStyle::initialGridTemplateColumns())
    , m_gridTemplateRows(ComputedStyle::initialGridTemplateRows())
    , m_namedGridColumnLines(ComputedStyle::initialNamedGridColumnLines())
    , m_namedGridRowLines(ComputedStyle::initialNamedGridRowLines())
    , m_orderedNamedGridColumnLines(ComputedStyle::initialOrderedNamedGridColumnLines())
    , m_orderedNamedGridRowLines(ComputedStyle::initialOrderedNamedGridRowLines())
    , m_gridAutoFlow(ComputedStyle::initialGridAutoFlow())
    , m_gridAutoRows(ComputedStyle::initialGridAutoRows())
    , m_gridAutoColumns(ComputedStyle::initialGridAutoColumns())
    , m_namedGridArea(ComputedStyle::initialNamedGridArea())
    , m_namedGridAreaRowCount(ComputedStyle::initialNamedGridAreaCount())
    , m_namedGridAreaColumnCount(ComputedStyle::initialNamedGridAreaCount())
{
}

StyleGridData::StyleGridData(const StyleGridData& o)
    : RefCounted<StyleGridData>()
    , m_gridTemplateColumns(o.m_gridTemplateColumns)
    , m_gridTemplateRows(o.m_gridTemplateRows)
    , m_namedGridColumnLines(o.m_namedGridColumnLines)
    , m_namedGridRowLines(o.m_namedGridRowLines)
    , m_orderedNamedGridColumnLines(o.m_orderedNamedGridColumnLines)
    , m_orderedNamedGridRowLines(o.m_orderedNamedGridRowLines)
    , m_gridAutoFlow(o.m_gridAutoFlow)
    , m_gridAutoRows(o.m_gridAutoRows)
    , m_gridAutoColumns(o.m_gridAutoColumns)
    , m_namedGridArea(o.m_namedGridArea)
    , m_namedGridAreaRowCount(o.m_namedGridAreaRowCount)
    , m_namedGridAreaColumnCount(o.m_namedGridAreaColumnCount)
{
}

} // namespace blink

