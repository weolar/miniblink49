// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/editing/EphemeralRange.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/Range.h"
#include "core/dom/Text.h"

namespace blink {

template <typename Strategy>
EphemeralRangeTemplate<Strategy>::EphemeralRangeTemplate(const PositionAlgorithm<Strategy>& start, const PositionAlgorithm<Strategy>& end)
    : m_startPosition(start)
    , m_endPosition(end)
#if ENABLE(ASSERT)
    , m_domTreeVersion(start.isNull() ? 0 : start.document()->domTreeVersion())
#endif
{
    if (m_startPosition.isNull()) {
        ASSERT(m_endPosition.isNull());
        return;
    }
    ASSERT(m_endPosition.isNotNull());
    ASSERT(m_startPosition.document() == m_endPosition.document());
    ASSERT(m_startPosition.inDocument());
    ASSERT(m_endPosition.inDocument());
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy>::EphemeralRangeTemplate(const EphemeralRangeTemplate<Strategy>& other)
    : EphemeralRangeTemplate(other.m_startPosition, other.m_endPosition)
{
    ASSERT(other.isValid());
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy>::EphemeralRangeTemplate(const PositionAlgorithm<Strategy>& position)
    : EphemeralRangeTemplate(position, position)
{
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy>::EphemeralRangeTemplate(const Range* range)
{
    if (!range)
        return;
    m_startPosition = fromPositionInDOMTree<Strategy>(range->startPosition());
    m_endPosition = fromPositionInDOMTree<Strategy>(range->endPosition());
#if ENABLE(ASSERT)
    m_domTreeVersion = range->ownerDocument().domTreeVersion();
#endif
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy>::EphemeralRangeTemplate()
{
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy>::~EphemeralRangeTemplate()
{
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy>& EphemeralRangeTemplate<Strategy>::operator=(const EphemeralRangeTemplate<Strategy>& other)
{
    ASSERT(other.isValid());
    m_startPosition = other.m_startPosition;
    m_endPosition = other.m_endPosition;
#if ENABLE(ASSERT)
    m_domTreeVersion = other.m_domTreeVersion;
#endif
    return *this;
}

template <typename Strategy>
Document& EphemeralRangeTemplate<Strategy>::document() const
{
    ASSERT(isNotNull());
    return *m_startPosition.document();
}

template <typename Strategy>
PositionAlgorithm<Strategy> EphemeralRangeTemplate<Strategy>::startPosition() const
{
    ASSERT(isValid());
    return m_startPosition;
}

template <typename Strategy>
PositionAlgorithm<Strategy> EphemeralRangeTemplate<Strategy>::endPosition() const
{
    ASSERT(isValid());
    return m_endPosition;
}

template <typename Strategy>
bool EphemeralRangeTemplate<Strategy>::isCollapsed() const
{
    ASSERT(isValid());
    return m_startPosition == m_endPosition;
}

template <typename Strategy>
bool EphemeralRangeTemplate<Strategy>::isNotNull() const
{
    ASSERT(isValid());
    return m_startPosition.isNotNull();
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy> EphemeralRangeTemplate<Strategy>::rangeOfContents(const Node& node)
{
    return EphemeralRangeTemplate<Strategy>(PositionAlgorithm<Strategy>::firstPositionInNode(&const_cast<Node&>(node)), PositionAlgorithm<Strategy>::lastPositionInNode(&const_cast<Node&>(node)));
}

#if ENABLE(ASSERT)
template <typename Strategy>
bool EphemeralRangeTemplate<Strategy>::isValid() const
{
    return m_startPosition.isNull() || m_domTreeVersion == m_startPosition.document()->domTreeVersion();
}
#else
template <typename Strategy>
bool EphemeralRangeTemplate<Strategy>::isValid() const
{
    return true;
}
#endif

template class CORE_TEMPLATE_EXPORT EphemeralRangeTemplate<EditingStrategy>;

} // namespace blink
