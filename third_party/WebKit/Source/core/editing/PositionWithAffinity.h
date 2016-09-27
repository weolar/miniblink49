// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file./*

#ifndef PositionWithAffinity_h
#define PositionWithAffinity_h

#include "core/CoreExport.h"
#include "core/dom/Position.h"
#include "core/editing/TextAffinity.h"

namespace blink {

template <typename Position>
class CORE_TEMPLATE_CLASS_EXPORT PositionWithAffinityTemplate {
    DISALLOW_ALLOCATION();
public:
    typedef Position PositionType;

    PositionWithAffinityTemplate(const PositionType&, EAffinity = DOWNSTREAM);
    PositionWithAffinityTemplate();
    ~PositionWithAffinityTemplate();

    EAffinity affinity() const { return m_affinity; }
    const PositionType& position() const { return m_position; }

    // Returns true if both |this| and |other| is null or both |m_position|
    // and |m_affinity| equal.
    bool operator==(const PositionWithAffinityTemplate& other) const;
    bool operator!=(const PositionWithAffinityTemplate& other) const { return !operator==(other); }

    bool isNotNull() const { return m_position.isNotNull(); }
    bool isNull() const { return m_position.isNull(); }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_position);
    }

private:
    PositionType m_position;
    EAffinity m_affinity;
};

extern template class CORE_EXTERN_TEMPLATE_EXPORT PositionWithAffinityTemplate<Position>;
extern template class CORE_EXTERN_TEMPLATE_EXPORT PositionWithAffinityTemplate<PositionInComposedTree>;

using PositionWithAffinity = PositionWithAffinityTemplate<Position>;
using PositionInComposedTreeWithAffinity = PositionWithAffinityTemplate<PositionInComposedTree>;

} // namespace blink

#endif // PositionWithAffinity_h
