// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/mediasource/TrackDefaultList.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "wtf/text/AtomicStringHash.h"
#include "wtf/text/StringHash.h"

namespace blink {

TrackDefaultList* TrackDefaultList::create()
{
    return new TrackDefaultList();
}

TrackDefaultList* TrackDefaultList::create(const HeapVector<Member<TrackDefault>>& trackDefaults, ExceptionState& exceptionState)
{
    // Per 11 Dec 2014 Editor's Draft
    // https://w3c.github.io/media-source/#trackdefaultlist
    // When this method is invoked, the user agent must run the following steps:
    // 1. If |trackDefaults| contains two or more TrackDefault objects with the
    //    same type and the same byteStreamTrackID, then throw an
    //    InvalidAccessError and abort these steps.
    //    Note: This also applies when byteStreamTrackID contains an empty
    //    string and ensures that there is only one "byteStreamTrackID
    //    independent" default for each TrackDefaultType value.
    using TypeAndID = std::pair<AtomicString, String>;
    using TypeAndIDToTrackDefaultMap = HeapHashMap<TypeAndID, Member<TrackDefault>>;
    TypeAndIDToTrackDefaultMap typeAndIDToTrackDefaultMap;

    for (const auto& trackDefault : trackDefaults) {
        TypeAndID key = TypeAndID(trackDefault->type(), trackDefault->byteStreamTrackID());
        if (!typeAndIDToTrackDefaultMap.add(key, trackDefault).isNewEntry) {
            exceptionState.throwDOMException(InvalidAccessError, "Duplicate TrackDefault type (" + key.first + ") and byteStreamTrackID (" + key.second + ")");
            return nullptr;
        }
    }

    // 2. Store a shallow copy of |trackDefaults| in this new object so the values can
    //    be returned by the accessor methods.
    // This step is done in constructor initializer.
    return new TrackDefaultList(trackDefaults);
}

TrackDefault* TrackDefaultList::item(unsigned index) const
{
    // Per 11 Dec 2014 Editor's Draft
    // https://w3c.github.io/media-source/#trackdefaultlist
    // When this method is invoked, the user agent must run the following steps:
    // 1. If |index| is greater than or equal to the length attribute then
    //    return undefined and abort these steps.
    if (index >= m_trackDefaults.size())
        return 0;

    // 2. Return the |index|'th TrackDefault object in the list.
    return m_trackDefaults[index].get();
}

TrackDefaultList::TrackDefaultList()
{
}

TrackDefaultList::TrackDefaultList(const HeapVector<Member<TrackDefault>>& trackDefaults)
    : m_trackDefaults(trackDefaults)
{
}

DEFINE_TRACE(TrackDefaultList)
{
    visitor->trace(m_trackDefaults);
}

} // namespace blink
