// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/track/AudioTrack.h"

#include "core/html/HTMLMediaElement.h"

namespace blink {

AudioTrack::AudioTrack(const String& id, const AtomicString& kind, const AtomicString& label, const AtomicString& language, bool enabled)
    : TrackBase(TrackBase::AudioTrack, label, language, id)
    , m_enabled(enabled)
{
    setKind(kind);
}

AudioTrack::~AudioTrack()
{
}

DEFINE_TRACE(AudioTrack)
{
    TrackBase::trace(visitor);
}

void AudioTrack::setEnabled(bool enabled)
{
    if (enabled == m_enabled)
        return;

    m_enabled = enabled;

    if (mediaElement())
        mediaElement()->audioTrackChanged();
}

const AtomicString& AudioTrack::alternativeKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, keyword, ("alternative", AtomicString::ConstructFromLiteral));
    return keyword;
}

const AtomicString& AudioTrack::descriptionsKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, keyword, ("descriptions", AtomicString::ConstructFromLiteral));
    return keyword;
}

const AtomicString& AudioTrack::mainKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, keyword, ("main", AtomicString::ConstructFromLiteral));
    return keyword;
}

const AtomicString& AudioTrack::mainDescriptionsKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, keyword, ("main-desc", AtomicString::ConstructFromLiteral));
    return keyword;
}

const AtomicString& AudioTrack::translationKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, keyword, ("translation", AtomicString::ConstructFromLiteral));
    return keyword;
}

const AtomicString& AudioTrack::commentaryKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, keyword, ("commentary", AtomicString::ConstructFromLiteral));
    return keyword;
}

bool AudioTrack::isValidKindKeyword(const String& kind)
{
    return (kind == alternativeKeyword())
        || (kind == descriptionsKeyword())
        || (kind == mainKeyword())
        || (kind == mainDescriptionsKeyword())
        || (kind == translationKeyword())
        || (kind == commentaryKeyword())
        || (kind == emptyAtom);
}

AtomicString AudioTrack::defaultKind() const
{
    return emptyAtom;
}

}
