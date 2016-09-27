/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HTMLTrackElement_h
#define HTMLTrackElement_h

#include "core/html/HTMLElement.h"
#include "core/html/track/TextTrack.h"
#include "core/loader/TextTrackLoader.h"
#include "platform/heap/Handle.h"

namespace blink {

class HTMLMediaElement;
class LoadableTextTrack;

class HTMLTrackElement final : public HTMLElement, private TextTrackLoaderClient {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(HTMLTrackElement);

    const AtomicString& kind();
    void setKind(const AtomicString&);

    enum ReadyState {
        NONE = 0,
        LOADING = 1,
        LOADED = 2,
        TRACK_ERROR = 3
    };
    ReadyState readyState();
    void scheduleLoad();

    TextTrack* track();

    DECLARE_VIRTUAL_TRACE();

private:
    explicit HTMLTrackElement(Document&);
    ~HTMLTrackElement() override;

    void parseAttribute(const QualifiedName&, const AtomicString&) override;

    InsertionNotificationRequest insertedInto(ContainerNode*) override;

    void removedFrom(ContainerNode*) override;
    bool isURLAttribute(const Attribute&) const override;

    // TextTrackLoaderClient
    void newCuesAvailable(TextTrackLoader*) override;
    void cueLoadingCompleted(TextTrackLoader*, bool loadingFailed) override;
    void newRegionsAvailable(TextTrackLoader*) override;

    void setReadyState(ReadyState);

    const AtomicString& mediaElementCrossOriginAttribute() const;
    bool canLoadUrl(const KURL&);
    void loadTimerFired(Timer<HTMLTrackElement>*);

    enum LoadStatus {
        Failure,
        Success
    };
    void didCompleteLoad(LoadStatus);

    HTMLMediaElement* mediaElement() const;

    LoadableTextTrack* ensureTrack();

    RefPtrWillBeMember<LoadableTextTrack> m_track;
    OwnPtrWillBeMember<TextTrackLoader> m_loader;
    Timer<HTMLTrackElement> m_loadTimer;
    KURL m_url;
};

} // namespace blink

#endif // HTMLTrackElement_h
