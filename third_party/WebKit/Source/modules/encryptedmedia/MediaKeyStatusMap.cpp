// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/encryptedmedia/MediaKeyStatusMap.h"

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMArrayPiece.h"
#include "public/platform/WebData.h"
#include "wtf/text/WTFString.h"

namespace blink {

// Represents the key ID and associated status.
class MediaKeyStatusMap::MapEntry final : public GarbageCollectedFinalized<MediaKeyStatusMap::MapEntry> {
public:
    static MapEntry* create(WebData keyId, const String& status)
    {
        return new MapEntry(keyId, status);
    }

    virtual ~MapEntry() { }

    DOMArrayBuffer* keyId() const
    {
        return m_keyId.get();
    }

    const String& status() const
    {
        return m_status;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
    }

private:
    MapEntry(WebData keyId, const String& status)
        : m_keyId(DOMArrayBuffer::create(keyId.data(), keyId.size()))
        , m_status(status)
    {
    }

    RefPtr<DOMArrayBuffer> m_keyId;
    const String m_status;
};

// Represents an Iterator that loops through the set of MapEntrys.
class MapIterationSource final : public PairIterable<ArrayBufferOrArrayBufferView, String>::IterationSource {
public:
    MapIterationSource(MediaKeyStatusMap* map)
        : m_map(map)
        , m_current(0)
    {
    }

    bool next(ScriptState* scriptState, ArrayBufferOrArrayBufferView& key, String& value, ExceptionState&) override
    {
        // This simply advances an index and returns the next value if any,
        // so if the iterated object is mutated values may be skipped.
        if (m_current >= m_map->size())
            return false;

        const auto& entry = m_map->at(m_current++);
        key.setArrayBuffer(entry.keyId());
        value = entry.status();
        return true;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_map);
        PairIterable<ArrayBufferOrArrayBufferView, String>::IterationSource::trace(visitor);
    }

private:
    // m_map is stored just for keeping it alive. It needs to be kept
    // alive while JavaScript holds the iterator to it.
    const Member<const MediaKeyStatusMap> m_map;
    size_t m_current;
};

void MediaKeyStatusMap::clear()
{
    m_entries.clear();
}

void MediaKeyStatusMap::addEntry(WebData keyId, const String& status)
{
    m_entries.append(MapEntry::create(keyId, status));
}

const MediaKeyStatusMap::MapEntry& MediaKeyStatusMap::at(size_t index) const
{
    BLINK_ASSERT(index < m_entries.size());
    return *m_entries.at(index);
}

size_t MediaKeyStatusMap::indexOf(const DOMArrayPiece& key) const
{
    for (size_t index = 0; index < m_entries.size(); ++index) {
        const auto& current = m_entries.at(index)->keyId();
        if (key == *current)
            return index;
    }

    // Not found, so return an index outside the valid range.
    return m_entries.size();
}

PairIterable<ArrayBufferOrArrayBufferView, String>::IterationSource* MediaKeyStatusMap::startIteration(ScriptState*, ExceptionState&)
{
    return new MapIterationSource(this);
}

bool MediaKeyStatusMap::getMapEntry(ScriptState*, const ArrayBufferOrArrayBufferView& key, String& value, ExceptionState&)
{
    size_t index = indexOf(key);
    if (index < m_entries.size()) {
        value = at(index).status();
        return true;
    }
    return false;
}

DEFINE_TRACE(MediaKeyStatusMap)
{
    visitor->trace(m_entries);
}

} // namespace blink
