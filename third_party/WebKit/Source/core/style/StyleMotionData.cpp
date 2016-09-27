// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/style/StyleMotionData.h"

#include "core/style/PathStyleMotionPath.h"

namespace blink {

bool StyleMotionData::operator==(const StyleMotionData& o) const
{
    if (m_offset != o.m_offset || m_rotation != o.m_rotation || m_rotationType != o.m_rotationType)
        return false;

    if (!m_path || !o.m_path)
        return !m_path && !o.m_path;

    if (m_path->isPathStyleMotionPath() && o.m_path->isPathStyleMotionPath())
        return toPathStyleMotionPath(*m_path).equals(toPathStyleMotionPath(*o.m_path));

    return false;
}

} // namespace blink
