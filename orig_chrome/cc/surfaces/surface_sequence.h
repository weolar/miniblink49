// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_SURFACE_SEQUENCE_H_
#define CC_SURFACES_SURFACE_SEQUENCE_H_

#include "base/containers/hash_tables.h"

namespace cc {

// A per-surface-namespace sequence number that's used to coordinate
// dependencies between frames. A sequence number may be satisfied once, and
// may be depended on once.
struct SurfaceSequence {
    SurfaceSequence()
        : id_namespace(0u)
        , sequence(0u)
    {
    }
    SurfaceSequence(uint32_t id_namespace, uint32_t sequence)
        : id_namespace(id_namespace)
        , sequence(sequence)
    {
    }
    bool is_null() const { return id_namespace == 0u && sequence == 0u; }

    uint32_t id_namespace;
    uint32_t sequence;
};

inline bool operator==(const SurfaceSequence& a, const SurfaceSequence& b)
{
    return a.id_namespace == b.id_namespace && a.sequence == b.sequence;
}

inline bool operator!=(const SurfaceSequence& a, const SurfaceSequence& b)
{
    return !(a == b);
}

inline bool operator<(const SurfaceSequence& a, const SurfaceSequence& b)
{
    if (a.id_namespace != b.id_namespace)
        return a.id_namespace < b.id_namespace;
    return a.sequence < b.sequence;
}

} // namespace cc

namespace BASE_HASH_NAMESPACE {
#if USING_VC6RT != 1
template <>
struct hash<cc::SurfaceSequence> {
    size_t operator()(cc::SurfaceSequence key) const
    {
        return base::HashPair(key.id_namespace, key.sequence);
    }
};
#endif
} // namespace BASE_HASH_NAMESPACE

#endif // CC_SURFACES_SURFACE_SEQUENCE_H_
