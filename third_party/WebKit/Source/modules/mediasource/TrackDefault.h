// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TrackDefault_h
#define TrackDefault_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExceptionState;

class TrackDefault final : public GarbageCollectedFinalized<TrackDefault>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static TrackDefault* create(const AtomicString& type, const String& language, const String& label, const Vector<String>& kinds, const String& byteStreamTrackID, ExceptionState&);

    virtual ~TrackDefault();

    // Implement the IDL
    AtomicString type() const { return m_type; }
    String byteStreamTrackID() const { return m_byteStreamTrackID; }
    String language() const { return m_language; }
    String label() const { return m_label; }
    const Vector<String>& kinds() const { return m_kinds; }

    DEFINE_INLINE_TRACE() { }

private:
    TrackDefault(const AtomicString& type, const String& language, const String& label, const Vector<String>& kinds, const String& byteStreamTrackID);

    const AtomicString m_type;
    const String m_byteStreamTrackID;
    const String m_language;
    const String m_label;
    const Vector<String> m_kinds;
};

} // namespace blink

#endif // TrackDefault_h
