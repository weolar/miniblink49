// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/events/PointerIdManager.h"

namespace blink {

PointerIdManager::PointerIdManager()
{
    clear();
}

PointerIdManager::~PointerIdManager()
{
    clear();
}

void PointerIdManager::clear()
{
    for (int type = 0; type < static_cast<int>(PointerTypeLastEntry); type++) {
        m_ids[type].clear();
        m_hasPrimaryId[type] = false;
    }
}

void PointerIdManager::add(PointerType type, unsigned id)
{
    if (m_ids[type].isEmpty())
        m_hasPrimaryId[type] = true;
    m_ids[type].add(id);
}

void PointerIdManager::remove(PointerType type, unsigned id)
{
    if (isPrimary(type, id)) {
        m_ids[type].removeFirst();
        m_hasPrimaryId[type] = false;
    } else {
        // Note that simply counting the number of ids instead of storing all of them is not enough.
        // When id is absent, remove() should be a no-op.
        m_ids[type].remove(id);
    }
}

bool PointerIdManager::isPrimary(PointerType type, unsigned id)
{
    return m_hasPrimaryId[type] && m_ids[type].first() == id;
}

} // namespace blink
