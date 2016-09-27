// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AudioTrack_h
#define AudioTrack_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/html/track/TrackBase.h"

namespace blink {

class CORE_EXPORT AudioTrack final : public NoBaseWillBeGarbageCollectedFinalized<AudioTrack>, public TrackBase, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(AudioTrack);
public:
    static PassRefPtrWillBeRawPtr<AudioTrack> create(const String& id, const AtomicString& kind, const AtomicString& label, const AtomicString& language, bool enabled)
    {
        return adoptRefWillBeNoop(new AudioTrack(id, kind, label, language, enabled));
    }

    ~AudioTrack() override;
    DECLARE_VIRTUAL_TRACE();

    bool enabled() const { return m_enabled; }
    void setEnabled(bool);

    // Valid kind keywords.
    static const AtomicString& alternativeKeyword();
    static const AtomicString& descriptionsKeyword();
    static const AtomicString& mainKeyword();
    static const AtomicString& mainDescriptionsKeyword();
    static const AtomicString& translationKeyword();
    static const AtomicString& commentaryKeyword();

    static bool isValidKindKeyword(const String&);

private:
    AudioTrack(const String& id, const AtomicString& kind, const AtomicString& label, const AtomicString& language, bool enabled);

    // TrackBase
    bool isValidKind(const AtomicString& kind) const override { return isValidKindKeyword(kind); }
    AtomicString defaultKind() const override;

    bool m_enabled;
};

DEFINE_TRACK_TYPE_CASTS(AudioTrack, TrackBase::AudioTrack);

} // namespace blink

#endif // AudioTrack_h
