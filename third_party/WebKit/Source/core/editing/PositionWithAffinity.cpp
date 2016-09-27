// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file./*

#include "config.h"
#include "core/editing/PositionWithAffinity.h"

namespace blink {

template <typename PositionType>
PositionWithAffinityTemplate<PositionType>::PositionWithAffinityTemplate(const PositionType& position, EAffinity affinity)
    : m_position(position)
    , m_affinity(affinity)
{
}

template <typename PositionType>
PositionWithAffinityTemplate<PositionType>::PositionWithAffinityTemplate()
    : m_affinity(DOWNSTREAM)
{
}

template <typename PositionType>
PositionWithAffinityTemplate<PositionType>::~PositionWithAffinityTemplate()
{
}

template <typename PositionType>
bool PositionWithAffinityTemplate<PositionType>::operator==(const PositionWithAffinityTemplate& other) const
{
    if (isNull())
        return other.isNull();
    return m_affinity == other.m_affinity && m_position == other.m_position;
}

template class CORE_TEMPLATE_EXPORT PositionWithAffinityTemplate<Position>;
template class CORE_TEMPLATE_EXPORT PositionWithAffinityTemplate<PositionInComposedTree>;

} // namespace blink
