// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TrackDefaultList_h
#define TrackDefaultList_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/mediasource/TrackDefault.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExceptionState;

class TrackDefaultList final : public GarbageCollected<TrackDefaultList>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static TrackDefaultList* create(); // Creates an empty TrackDefaultList.

    // Implement the IDL
    static TrackDefaultList* create(const HeapVector<Member<TrackDefault>>&, ExceptionState&);

    unsigned length() const { return m_trackDefaults.size(); }
    TrackDefault* item(unsigned) const;

    DECLARE_TRACE();

private:
    TrackDefaultList();

    explicit TrackDefaultList(const HeapVector<Member<TrackDefault>>&);

    const HeapVector<Member<TrackDefault>> m_trackDefaults;
};

} // namespace blink

#endif // TrackDefaultList_h
