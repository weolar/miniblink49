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

#include "config.h"
#include "core/html/HTMLTrackElement.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/track/LoadableTextTrack.h"
#include "platform/Logging.h"

namespace blink {

using namespace HTMLNames;

#if !LOG_DISABLED
static String urlForLoggingTrack(const KURL& url)
{
    static const unsigned maximumURLLengthForLogging = 128;

    if (url.string().length() < maximumURLLengthForLogging)
        return url.string();
    return url.string().substring(0, maximumURLLengthForLogging) + "...";
}
#endif

inline HTMLTrackElement::HTMLTrackElement(Document& document)
    : HTMLElement(trackTag, document)
    , m_loadTimer(this, &HTMLTrackElement::loadTimerFired)
{
    WTF_LOG(Media, "HTMLTrackElement::HTMLTrackElement - %p", this);
}

DEFINE_NODE_FACTORY(HTMLTrackElement)

HTMLTrackElement::~HTMLTrackElement()
{
#if !ENABLE(OILPAN)
    if (m_track)
        m_track->clearTrackElement();
#endif
}

Node::InsertionNotificationRequest HTMLTrackElement::insertedInto(ContainerNode* insertionPoint)
{
    WTF_LOG(Media, "HTMLTrackElement::insertedInto");

    // Since we've moved to a new parent, we may now be able to load.
    scheduleLoad();

    HTMLElement::insertedInto(insertionPoint);
    HTMLMediaElement* parent = mediaElement();
    if (insertionPoint == parent)
        parent->didAddTrackElement(this);
    return InsertionDone;
}

void HTMLTrackElement::removedFrom(ContainerNode* insertionPoint)
{
    if (!parentNode() && isHTMLMediaElement(*insertionPoint))
        toHTMLMediaElement(insertionPoint)->didRemoveTrackElement(this);
    HTMLElement::removedFrom(insertionPoint);
}

void HTMLTrackElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == srcAttr) {
        if (!value.isEmpty())
            scheduleLoad();
        else if (m_track)
            m_track->removeAllCues();

    // 4.8.10.12.3 Sourcing out-of-band text tracks
    // As the kind, label, and srclang attributes are set, changed, or removed, the text track must update accordingly...
    } else if (name == kindAttr) {
        track()->setKind(value.lower());
    } else if (name == labelAttr) {
        track()->setLabel(value);
    } else if (name == srclangAttr) {
        track()->setLanguage(value);
    } else if (name == idAttr) {
        track()->setId(value);
    }

    HTMLElement::parseAttribute(name, value);
}

const AtomicString& HTMLTrackElement::kind()
{
    return track()->kind();
}

void HTMLTrackElement::setKind(const AtomicString& kind)
{
    setAttribute(kindAttr, kind);
}

LoadableTextTrack* HTMLTrackElement::ensureTrack()
{
    if (!m_track) {
        // kind, label and language are updated by parseAttribute
        m_track = LoadableTextTrack::create(this);
    }
    return m_track.get();
}

TextTrack* HTMLTrackElement::track()
{
    return ensureTrack();
}

bool HTMLTrackElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == srcAttr || HTMLElement::isURLAttribute(attribute);
}

void HTMLTrackElement::scheduleLoad()
{
    WTF_LOG(Media, "HTMLTrackElement::scheduleLoad");

    // 1. If another occurrence of this algorithm is already running for this text track and its track element,
    // abort these steps, letting that other algorithm take care of this element.
    if (m_loadTimer.isActive())
        return;

    // 2. If the text track's text track mode is not set to one of hidden or showing, abort these steps.
    if (ensureTrack()->mode() != TextTrack::hiddenKeyword() && ensureTrack()->mode() != TextTrack::showingKeyword())
        return;

    // 3. If the text track's track element does not have a media element as a parent, abort these steps.
    if (!mediaElement())
        return;

    // 4. Run the remainder of these steps in parallel, allowing whatever caused these steps to run to continue.
    m_loadTimer.startOneShot(0, FROM_HERE);

    // 5. Top: Await a stable state. The synchronous section consists of the following steps. (The steps in the
    // synchronous section are marked with [X])
    // FIXME: We use a timer to approximate a "stable state" - i.e. this is not 100% per spec.
}

void HTMLTrackElement::loadTimerFired(Timer<HTMLTrackElement>*)
{
    WTF_LOG(Media, "HTMLTrackElement::loadTimerFired");

    // 6. [X] Set the text track readiness state to loading.
    setReadyState(LOADING);

    // 7. [X] Let URL be the track URL of the track element.
    KURL url = getNonEmptyURLAttribute(srcAttr);

    // 8. [X] If the track element's parent is a media element then let CORS mode be the state of the parent media
    // element's crossorigin content attribute. Otherwise, let CORS mode be No CORS.
    const AtomicString& corsMode = mediaElementCrossOriginAttribute();

    // 9. End the synchronous section, continuing the remaining steps in parallel.

    // 10. If URL is not the empty string, perform a potentially CORS-enabled fetch of URL, with the mode being CORS
    // mode, the origin being the origin of the track element's node document, and the default origin behaviour set to
    // fail.
    if (!canLoadUrl(url)) {
        didCompleteLoad(Failure);
        return;
    }

    if (url == m_url) {
        ASSERT(m_loader);
        switch (m_loader->loadState()) {
        case TextTrackLoader::Idle:
        case TextTrackLoader::Loading:
            // If loading of the resource from this URL is in progress, return early.
            break;
        case TextTrackLoader::Finished:
            didCompleteLoad(Success);
            break;
        case TextTrackLoader::Failed:
            didCompleteLoad(Failure);
            break;
        default:
            ASSERT_NOT_REACHED();
        }
        return;
    }

    m_url = url;

    if (m_loader)
        m_loader->cancelLoad();

    m_loader = TextTrackLoader::create(*this, document());
    if (!m_loader->load(m_url, corsMode))
        didCompleteLoad(Failure);
}

bool HTMLTrackElement::canLoadUrl(const KURL& url)
{
    HTMLMediaElement* parent = mediaElement();
    if (!parent)
        return false;

    if (url.isEmpty())
        return false;

    if (!document().contentSecurityPolicy()->allowMediaFromSource(url)) {
        WTF_LOG(Media, "HTMLTrackElement::canLoadUrl(%s) -> rejected by Content Security Policy", urlForLoggingTrack(url).utf8().data());
        return false;
    }

    return true;
}

void HTMLTrackElement::didCompleteLoad(LoadStatus status)
{
    // 10. ... (continued)

    // If the fetching algorithm fails for any reason (network error, the server returns an error code, a cross-origin
    // check fails, etc), or if URL is the empty string, then queue a task to first change the text track readiness
    // state to failed to load and then fire a simple event named error at the track element. This task must use the DOM
    // manipulation task source.
    //
    // (Note: We don't "queue a task" here because this method will only be called from a timer - m_loadTimer or
    // TextTrackLoader::m_cueLoadTimer - which should be a reasonable, and hopefully non-observable, approximation of
    // the spec text. I.e we could consider this to be run from the "networking task source".)
    //
    // If the fetching algorithm does not fail, but the type of the resource is not a supported text track format, or
    // the file was not successfully processed (e.g. the format in question is an XML format and the file contained a
    // well-formedness error that the XML specification requires be detected and reported to the application), then the
    // task that is queued by the networking task source in which the aforementioned problem is found must change the
    // text track readiness state to failed to load and fire a simple event named error at the track element.
    if (status == Failure) {
        setReadyState(TRACK_ERROR);
        dispatchEvent(Event::create(EventTypeNames::error));
        return;
    }

    // If the fetching algorithm does not fail, and the file was successfully processed, then the final task that is
    // queued by the networking task source, after it has finished parsing the data, must change the text track
    // readiness state to loaded, and fire a simple event named load at the track element.
    setReadyState(LOADED);
    dispatchEvent(Event::create(EventTypeNames::load));
}

void HTMLTrackElement::newCuesAvailable(TextTrackLoader* loader)
{
    ASSERT_UNUSED(loader, m_loader == loader);
    ASSERT(m_track);

    WillBeHeapVector<RefPtrWillBeMember<TextTrackCue>> newCues;
    m_loader->getNewCues(newCues);

    m_track->addListOfCues(newCues);
}

void HTMLTrackElement::newRegionsAvailable(TextTrackLoader* loader)
{
    ASSERT_UNUSED(loader, m_loader == loader);
    ASSERT(m_track);

    WillBeHeapVector<RefPtrWillBeMember<VTTRegion>> newRegions;
    m_loader->getNewRegions(newRegions);

    m_track->addRegions(newRegions);
}

void HTMLTrackElement::cueLoadingCompleted(TextTrackLoader* loader, bool loadingFailed)
{
    ASSERT_UNUSED(loader, m_loader == loader);

    didCompleteLoad(loadingFailed ? Failure : Success);
}

// NOTE: The values in the TextTrack::ReadinessState enum must stay in sync with those in HTMLTrackElement::ReadyState.
static_assert(HTMLTrackElement::NONE == static_cast<HTMLTrackElement::ReadyState>(TextTrack::NotLoaded), "HTMLTrackElement::NONE should be in sync with TextTrack::NotLoaded");
static_assert(HTMLTrackElement::LOADING == static_cast<HTMLTrackElement::ReadyState>(TextTrack::Loading), "HTMLTrackElement::LOADING should be in sync with TextTrack::Loading");
static_assert(HTMLTrackElement::LOADED == static_cast<HTMLTrackElement::ReadyState>(TextTrack::Loaded), "HTMLTrackElement::LOADED should be in sync with TextTrack::Loaded");
static_assert(HTMLTrackElement::TRACK_ERROR == static_cast<HTMLTrackElement::ReadyState>(TextTrack::FailedToLoad), "HTMLTrackElement::TRACK_ERROR should be in sync with TextTrack::FailedToLoad");

void HTMLTrackElement::setReadyState(ReadyState state)
{
    ensureTrack()->setReadinessState(static_cast<TextTrack::ReadinessState>(state));
    if (HTMLMediaElement* parent = mediaElement())
        return parent->textTrackReadyStateChanged(m_track.get());
}

HTMLTrackElement::ReadyState HTMLTrackElement::readyState()
{
    return static_cast<ReadyState>(ensureTrack()->readinessState());
}

const AtomicString& HTMLTrackElement::mediaElementCrossOriginAttribute() const
{
    if (HTMLMediaElement* parent = mediaElement())
        return parent->fastGetAttribute(HTMLNames::crossoriginAttr);

    return nullAtom;
}

HTMLMediaElement* HTMLTrackElement::mediaElement() const
{
    Element* parent = parentElement();
    if (isHTMLMediaElement(parent))
        return toHTMLMediaElement(parent);
    return nullptr;
}

DEFINE_TRACE(HTMLTrackElement)
{
    visitor->trace(m_track);
    visitor->trace(m_loader);
    HTMLElement::trace(visitor);
}

}
