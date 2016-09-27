// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EphemeralRange_h
#define EphemeralRange_h

#include "core/dom/Position.h"

namespace blink {

class Document;
class Range;

// Unlike |Range| objects, |EphemeralRangeTemplate| objects aren't relocated.
// You should not use |EphemeralRangeTemplate| objects after DOM modification.
//
// EphemeralRangeTemplate is supposed to use returning or passing start and end
// position.
//
//  Example usage:
//    RefPtrWillBeRawPtr<Range> range = produceRange();
//    consumeRange(range.get());
//    ... no DOM modification ...
//    consumeRange2(range.get());
//
//  Above code should be:
//    EphemeralRangeTemplate range = produceRange();
//    consumeRange(range);
//    ... no DOM modification ...
//    consumeRange2(range);
//
//  Because of |Range| objects consume heap memory and inserted into |Range|
//  object list in |Document| for relocation. These operations are redundant
//  if |Range| objects doesn't live after DOM mutation.
//
template <typename Strategy>
class CORE_TEMPLATE_CLASS_EXPORT EphemeralRangeTemplate final {
    STACK_ALLOCATED();
public:
    EphemeralRangeTemplate(const PositionAlgorithm<Strategy>& start, const PositionAlgorithm<Strategy>& end);
    EphemeralRangeTemplate(const EphemeralRangeTemplate& other);
    // |position| should be null or in-document.
    explicit EphemeralRangeTemplate(const PositionAlgorithm<Strategy>& /* position */);
    // When |range| is nullptr, |EphemeralRangeTemplate| is null.
    explicit EphemeralRangeTemplate(const Range* /* range */);
    EphemeralRangeTemplate();
    ~EphemeralRangeTemplate();

    EphemeralRangeTemplate<Strategy>& operator=(const EphemeralRangeTemplate<Strategy>& other);

    Document& document() const;
    PositionAlgorithm<Strategy> startPosition() const;
    PositionAlgorithm<Strategy> endPosition() const;

    // Returns true if |m_startPositoin| == |m_endPosition| or |isNull()|.
    bool isCollapsed() const;
    bool isNull() const { return !isNotNull(); }
    bool isNotNull() const;

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_startPosition);
        visitor->trace(m_endPosition);
    }

    // |node| should be in-document and valid for anchor node of
    // |PositionAlgorithm<Strategy>|.
    static EphemeralRangeTemplate<Strategy> rangeOfContents(const Node& /* node */);

private:
    bool isValid() const;

    PositionAlgorithm<Strategy> m_startPosition;
    PositionAlgorithm<Strategy> m_endPosition;
#if ENABLE(ASSERT)
    uint64_t m_domTreeVersion;
#endif
};

extern template class CORE_EXTERN_TEMPLATE_EXPORT EphemeralRangeTemplate<EditingStrategy>;
using EphemeralRange = EphemeralRangeTemplate<EditingStrategy>;

} // namespace blink

#endif
