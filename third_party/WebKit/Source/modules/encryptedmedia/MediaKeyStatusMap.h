// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MediaKeyStatusMap_h
#define MediaKeyStatusMap_h

#include "bindings/core/v8/Maplike.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "core/dom/DOMArrayPiece.h"
#include "platform/heap/Heap.h"

namespace blink {

class ExceptionState;
class Iterator;
class ScriptState;
class WebData;

// Represents a read-only map (to JavaScript) of key IDs and their current
// status known to a particular session. Since it can be updated any time there
// is a keychange event, iteration order and completeness is not guaranteed
// if the event loop runs.
class MediaKeyStatusMap final : public GarbageCollected<MediaKeyStatusMap>, public ScriptWrappable, public Maplike<ArrayBufferOrArrayBufferView, String> {
    DEFINE_WRAPPERTYPEINFO();
private:
    // MapEntry holds the keyId (DOMArrayBuffer) and status (MediaKeyStatus as
    // String) for each entry.
    class MapEntry;

    // As the number of keys per session should be small, the key ids and
    // their status are kept in a list rather than a proper map.
    typedef HeapVector<Member<MapEntry>> MapType;

public:
    MediaKeyStatusMap() { }

    void clear();
    void addEntry(WebData source, const String& status);
    const MapEntry& at(size_t) const;

    // IDL attributes / methods
    size_t size() const { return m_entries.size(); }

    DECLARE_VIRTUAL_TRACE();

private:
    IterationSource* startIteration(ScriptState*, ExceptionState&) override;
    bool getMapEntry(ScriptState*, const ArrayBufferOrArrayBufferView&, String&, ExceptionState&) override;

    size_t indexOf(const DOMArrayPiece& key) const;

    MapType m_entries;
};

} // namespace blink

#endif // MediaKeyStatusMap_h
