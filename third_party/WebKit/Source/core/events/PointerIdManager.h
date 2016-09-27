// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PointerIdManager_h
#define PointerIdManager_h

#include "wtf/ListHashSet.h"

namespace blink {

/**
  Helper class for tracking the primary pointer id for each type of PointerEvents.
*/
class PointerIdManager {
public:
    // TODO(mustaq): Move this enum to PointerEvent.h? Change the spec to use enums?
    enum PointerType {
        PointerTypeUnknown = 0,
        PointerTypeMouse,
        PointerTypePen,
        PointerTypeTouch,
        PointerTypeLastEntry // Must be the last entry in the list
    };

    PointerIdManager();
    ~PointerIdManager();
    void clear();
    void add(PointerType, unsigned);
    void remove(PointerType, unsigned);
    bool isPrimary(PointerType, unsigned);

private:
    ListHashSet<unsigned> m_ids[PointerTypeLastEntry];
    bool m_hasPrimaryId[PointerTypeLastEntry];
};

} // namespace blink

#endif // PointerIdManager_h
