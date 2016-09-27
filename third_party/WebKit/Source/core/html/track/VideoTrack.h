// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VideoTrack_h
#define VideoTrack_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/html/track/TrackBase.h"

namespace blink {

class CORE_EXPORT VideoTrack final : public NoBaseWillBeGarbageCollectedFinalized<VideoTrack>, public TrackBase, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(VideoTrack);
public:
    static PassRefPtrWillBeRawPtr<VideoTrack> create(const String& id, const AtomicString& kind, const AtomicString& label, const AtomicString& language, bool selected)
    {
        return adoptRefWillBeNoop(new VideoTrack(id, kind, label, language, selected));
    }

    ~VideoTrack() override;
    DECLARE_VIRTUAL_TRACE();

    bool selected() const { return m_selected; }
    void setSelected(bool);

    // Set selected to false without notifying the owner media element. Used when
    // another video track is selected, implicitly deselecting this one.
    void clearSelected() { m_selected = false; }

    // Valid kind keywords.
    static const AtomicString& alternativeKeyword();
    static const AtomicString& captionsKeyword();
    static const AtomicString& mainKeyword();
    static const AtomicString& signKeyword();
    static const AtomicString& subtitlesKeyword();
    static const AtomicString& commentaryKeyword();

    static bool isValidKindKeyword(const String&);

private:
    VideoTrack(const String& id, const AtomicString& kind, const AtomicString& label, const AtomicString& language, bool selected);

    // TrackBase
    bool isValidKind(const AtomicString& kind) const override { return isValidKindKeyword(kind); }
    AtomicString defaultKind() const override;

    bool m_selected;
};

DEFINE_TRACK_TYPE_CASTS(VideoTrack, TrackBase::VideoTrack);

} // namespace blink

#endif // VideoTrack_h
