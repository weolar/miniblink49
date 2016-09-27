/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Geoposition_h
#define Geoposition_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/EventModules.h"
#include "modules/geolocation/Coordinates.h"
#include "platform/heap/Handle.h"

namespace blink {

class Geoposition final : public GarbageCollected<Geoposition>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static Geoposition* create(Coordinates* coordinates, DOMTimeStamp timestamp)
    {
        return new Geoposition(coordinates, timestamp);
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_coordinates);
    }

    DOMTimeStamp timestamp() const { return m_timestamp; }
    Coordinates* coords() const { return m_coordinates.get(); }

private:
    Geoposition(Coordinates* coordinates, DOMTimeStamp timestamp)
        : m_coordinates(coordinates)
        , m_timestamp(timestamp)
    {
        ASSERT(m_coordinates);
    }

    Member<Coordinates> m_coordinates;
    DOMTimeStamp m_timestamp;
};

} // namespace blink

#endif // Geoposition_h
