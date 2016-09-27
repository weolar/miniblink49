// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/track/CueTimeline.h"

#include "core/events/Event.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLTrackElement.h"
#include "core/html/track/LoadableTextTrack.h"
#include "core/html/track/TextTrack.h"
#include "core/html/track/TextTrackCue.h"
#include "core/html/track/TextTrackCueList.h"
#include "wtf/NonCopyingSort.h"

namespace blink {

CueTimeline::CueTimeline(HTMLMediaElement& mediaElement)
    : m_mediaElement(&mediaElement)
    , m_lastUpdateTime(-1)
    , m_ignoreUpdate(0)
{
}

void CueTimeline::addCues(TextTrack* track, const TextTrackCueList* cues)
{
    ASSERT(track->mode() != TextTrack::disabledKeyword());
    for (size_t i = 0; i < cues->length(); ++i)
        addCueInternal(cues->item(i));
    updateActiveCues(mediaElement().currentTime());
}

void CueTimeline::addCue(TextTrack* track, PassRefPtrWillBeRawPtr<TextTrackCue> cue)
{
    ASSERT(track->mode() != TextTrack::disabledKeyword());
    addCueInternal(cue);
    updateActiveCues(mediaElement().currentTime());
}

void CueTimeline::addCueInternal(PassRefPtrWillBeRawPtr<TextTrackCue> cue)
{
    // Negative duration cues need be treated in the interval tree as
    // zero-length cues.
    double endTime = std::max(cue->startTime(), cue->endTime());

    CueInterval interval = m_cueTree.createInterval(cue->startTime(), endTime, cue.get());
    if (!m_cueTree.contains(interval))
        m_cueTree.add(interval);
}

void CueTimeline::removeCues(TextTrack*, const TextTrackCueList* cues)
{
    for (size_t i = 0; i < cues->length(); ++i)
        removeCueInternal(cues->item(i));
    updateActiveCues(mediaElement().currentTime());
}

void CueTimeline::removeCue(TextTrack*, PassRefPtrWillBeRawPtr<TextTrackCue> cue)
{
    removeCueInternal(cue);
    updateActiveCues(mediaElement().currentTime());
}

void CueTimeline::removeCueInternal(PassRefPtrWillBeRawPtr<TextTrackCue> cue)
{
    // Negative duration cues need to be treated in the interval tree as
    // zero-length cues.
    double endTime = std::max(cue->startTime(), cue->endTime());

    CueInterval interval = m_cueTree.createInterval(cue->startTime(), endTime, cue.get());
    m_cueTree.remove(interval);

    size_t index = m_currentlyActiveCues.find(interval);
    if (index != kNotFound) {
        ASSERT(cue->isActive());
        m_currentlyActiveCues.remove(index);
        cue->setIsActive(false);
        // Since the cue will be removed from the media element and likely the
        // TextTrack might also be destructed, notifying the region of the cue
        // removal shouldn't be done.
        cue->removeDisplayTree(TextTrackCue::DontNotifyRegion);
    }
}

void CueTimeline::hideCues(TextTrack*, const TextTrackCueList* cues)
{
    for (size_t i = 0; i < cues->length(); ++i)
        cues->item(i)->removeDisplayTree();
}

static bool trackIndexCompare(TextTrack* a, TextTrack* b)
{
    return a->trackIndex() - b->trackIndex() < 0;
}

static bool eventTimeCueCompare(const std::pair<double, TextTrackCue*>& a, const std::pair<double, TextTrackCue*>& b)
{
    // 12 - Sort the tasks in events in ascending time order (tasks with earlier
    // times first).
    if (a.first != b.first)
        return a.first - b.first < 0;

    // If the cues belong to different text tracks, it doesn't make sense to
    // compare the two tracks by the relative cue order, so return the relative
    // track order.
    if (a.second->track() != b.second->track())
        return trackIndexCompare(a.second->track(), b.second->track());

    // 12 - Further sort tasks in events that have the same time by the
    // relative text track cue order of the text track cues associated
    // with these tasks.
    return a.second->cueIndex() < b.second->cueIndex();
}

static PassRefPtrWillBeRawPtr<Event> createEventWithTarget(const AtomicString& eventName, PassRefPtrWillBeRawPtr<EventTarget> eventTarget)
{
    RefPtrWillBeRawPtr<Event> event = Event::create(eventName);
    event->setTarget(eventTarget);
    return event.release();
}

void CueTimeline::updateActiveCues(double movieTime)
{
    // 4.8.10.8 Playing the media resource

    //  If the current playback position changes while the steps are running,
    //  then the user agent must wait for the steps to complete, and then must
    //  immediately rerun the steps.
    if (ignoreUpdateRequests())
        return;

    HTMLMediaElement& mediaElement = this->mediaElement();

#if !ENABLE(OILPAN)
    // Don't run the "time marches on" algorithm if the document has been
    // detached. This primarily guards against dispatch of events w/
    // HTMLTrackElement targets.
    if (mediaElement.document().isDetached())
        return;
#endif

    // https://html.spec.whatwg.org/#time-marches-on

    // 1 - Let current cues be a list of cues, initialized to contain all the
    // cues of all the hidden, showing, or showing by default text tracks of the
    // media element (not the disabled ones) whose start times are less than or
    // equal to the current playback position and whose end times are greater
    // than the current playback position.
    CueList currentCues;

    // The user agent must synchronously unset [the text track cue active] flag
    // whenever ... the media element's readyState is changed back to HAVE_NOTHING.
    if (mediaElement.readyState() != HTMLMediaElement::HAVE_NOTHING && mediaElement.webMediaPlayer())
        currentCues = m_cueTree.allOverlaps(m_cueTree.createInterval(movieTime, movieTime));

    CueList previousCues;
    CueList missedCues;

    // 2 - Let other cues be a list of cues, initialized to contain all the cues
    // of hidden, showing, and showing by default text tracks of the media
    // element that are not present in current cues.
    previousCues = m_currentlyActiveCues;

    // 3 - Let last time be the current playback position at the time this
    // algorithm was last run for this media element, if this is not the first
    // time it has run.
    double lastTime = m_lastUpdateTime;
    double lastSeekTime = mediaElement.lastSeekTime();

    // 4 - If the current playback position has, since the last time this
    // algorithm was run, only changed through its usual monotonic increase
    // during normal playback, then let missed cues be the list of cues in other
    // cues whose start times are greater than or equal to last time and whose
    // end times are less than or equal to the current playback position.
    // Otherwise, let missed cues be an empty list.
    if (lastTime >= 0 && lastSeekTime < movieTime) {
        CueList potentiallySkippedCues =
            m_cueTree.allOverlaps(m_cueTree.createInterval(lastTime, movieTime));

        for (CueInterval cue : potentiallySkippedCues) {
            // Consider cues that may have been missed since the last seek time.
            if (cue.low() > std::max(lastSeekTime, lastTime) && cue.high() < movieTime)
                missedCues.append(cue);
        }
    }

    m_lastUpdateTime = movieTime;

    // 5 - If the time was reached through the usual monotonic increase of the
    // current playback position during normal playback, and if the user agent
    // has not fired a timeupdate event at the element in the past 15 to 250ms
    // and is not still running event handlers for such an event, then the user
    // agent must queue a task to fire a simple event named timeupdate at the
    // element. (In the other cases, such as explicit seeks, relevant events get
    // fired as part of the overall process of changing the current playback
    // position.)
    if (!mediaElement.seeking() && lastSeekTime < lastTime)
        mediaElement.scheduleTimeupdateEvent(true);

    // Explicitly cache vector sizes, as their content is constant from here.
    size_t currentCuesSize = currentCues.size();
    size_t missedCuesSize = missedCues.size();
    size_t previousCuesSize = previousCues.size();

    // 6 - If all of the cues in current cues have their text track cue active
    // flag set, none of the cues in other cues have their text track cue active
    // flag set, and missed cues is empty, then abort these steps.
    bool activeSetChanged = missedCuesSize;

    for (size_t i = 0; !activeSetChanged && i < previousCuesSize; ++i) {
        if (!currentCues.contains(previousCues[i]) && previousCues[i].data()->isActive())
            activeSetChanged = true;
    }

    for (CueInterval currentCue : currentCues) {
        // Notify any cues that are already active of the current time to mark
        // past and future nodes. Any inactive cues have an empty display state;
        // they will be notified of the current time when the display state is
        // updated.
        if (currentCue.data()->isActive())
            currentCue.data()->updatePastAndFutureNodes(movieTime);
        else
            activeSetChanged = true;
    }

    if (!activeSetChanged)
        return;

    // 7 - If the time was reached through the usual monotonic increase of the
    // current playback position during normal playback, and there are cues in
    // other cues that have their text track cue pause-on-exi flag set and that
    // either have their text track cue active flag set or are also in missed
    // cues, then immediately pause the media element.
    for (size_t i = 0; !mediaElement.paused() && i < previousCuesSize; ++i) {
        if (previousCues[i].data()->pauseOnExit()
            && previousCues[i].data()->isActive()
            && !currentCues.contains(previousCues[i]))
            mediaElement.pause();
    }

    for (size_t i = 0; !mediaElement.paused() && i < missedCuesSize; ++i) {
        if (missedCues[i].data()->pauseOnExit())
            mediaElement.pause();
    }

    // 8 - Let events be a list of tasks, initially empty. Each task in this
    // list will be associated with a text track, a text track cue, and a time,
    // which are used to sort the list before the tasks are queued.
    WillBeHeapVector<std::pair<double, RawPtrWillBeMember<TextTrackCue>>> eventTasks;

    // 8 - Let affected tracks be a list of text tracks, initially empty.
    WillBeHeapVector<RawPtrWillBeMember<TextTrack>> affectedTracks;

    for (size_t i = 0; i < missedCuesSize; ++i) {
        // 9 - For each text track cue in missed cues, prepare an event named enter
        // for the TextTrackCue object with the text track cue start time.
        eventTasks.append(std::make_pair(missedCues[i].data()->startTime(), missedCues[i].data()));

        // 10 - For each text track [...] in missed cues, prepare an event
        // named exit for the TextTrackCue object with the  with the later of
        // the text track cue end time and the text track cue start time.

        // Note: An explicit task is added only if the cue is NOT a zero or
        // negative length cue. Otherwise, the need for an exit event is
        // checked when these tasks are actually queued below. This doesn't
        // affect sorting events before dispatch either, because the exit
        // event has the same time as the enter event.
        if (missedCues[i].data()->startTime() < missedCues[i].data()->endTime())
            eventTasks.append(std::make_pair(missedCues[i].data()->endTime(), missedCues[i].data()));
    }

    for (size_t i = 0; i < previousCuesSize; ++i) {
        // 10 - For each text track cue in other cues that has its text
        // track cue active flag set prepare an event named exit for the
        // TextTrackCue object with the text track cue end time.
        if (!currentCues.contains(previousCues[i]))
            eventTasks.append(std::make_pair(previousCues[i].data()->endTime(), previousCues[i].data()));
    }

    for (size_t i = 0; i < currentCuesSize; ++i) {
        // 11 - For each text track cue in current cues that does not have its
        // text track cue active flag set, prepare an event named enter for the
        // TextTrackCue object with the text track cue start time.
        if (!previousCues.contains(currentCues[i]))
            eventTasks.append(std::make_pair(currentCues[i].data()->startTime(), currentCues[i].data()));
    }

    // 12 - Sort the tasks in events in ascending time order (tasks with earlier
    // times first).
    nonCopyingSort(eventTasks.begin(), eventTasks.end(), eventTimeCueCompare);

    for (size_t i = 0; i < eventTasks.size(); ++i) {
        if (!affectedTracks.contains(eventTasks[i].second->track()))
            affectedTracks.append(eventTasks[i].second->track());

        // 13 - Queue each task in events, in list order.

        // Each event in eventTasks may be either an enterEvent or an exitEvent,
        // depending on the time that is associated with the event. This
        // correctly identifies the type of the event, if the startTime is
        // less than the endTime in the cue.
        if (eventTasks[i].second->startTime() >= eventTasks[i].second->endTime()) {
            mediaElement.scheduleEvent(createEventWithTarget(EventTypeNames::enter, eventTasks[i].second));
            mediaElement.scheduleEvent(createEventWithTarget(EventTypeNames::exit, eventTasks[i].second));
        } else {
            bool isEnterEvent = eventTasks[i].first == eventTasks[i].second->startTime();
            AtomicString eventName = isEnterEvent ? EventTypeNames::enter : EventTypeNames::exit;
            mediaElement.scheduleEvent(createEventWithTarget(eventName, eventTasks[i].second));
        }
    }

    // 14 - Sort affected tracks in the same order as the text tracks appear in
    // the media element's list of text tracks, and remove duplicates.
    nonCopyingSort(affectedTracks.begin(), affectedTracks.end(), trackIndexCompare);

    // 15 - For each text track in affected tracks, in the list order, queue a
    // task to fire a simple event named cuechange at the TextTrack object, and, ...
    for (size_t i = 0; i < affectedTracks.size(); ++i) {
        mediaElement.scheduleEvent(createEventWithTarget(EventTypeNames::cuechange, affectedTracks[i]));

        // ... if the text track has a corresponding track element, to then fire a
        // simple event named cuechange at the track element as well.
        if (affectedTracks[i]->trackType() == TextTrack::TrackElement) {
            HTMLTrackElement* trackElement = static_cast<LoadableTextTrack*>(affectedTracks[i].get())->trackElement();
            ASSERT(trackElement);
            mediaElement.scheduleEvent(createEventWithTarget(EventTypeNames::cuechange, trackElement));
        }
    }

    // 16 - Set the text track cue active flag of all the cues in the current
    // cues, and unset the text track cue active flag of all the cues in the
    // other cues.
    for (size_t i = 0; i < currentCuesSize; ++i)
        currentCues[i].data()->setIsActive(true);

    for (size_t i = 0; i < previousCuesSize; ++i) {
        if (!currentCues.contains(previousCues[i])) {
            TextTrackCue* cue = previousCues[i].data();
            cue->setIsActive(false);
            cue->removeDisplayTree();
        }
    }

    // Update the current active cues.
    m_currentlyActiveCues = currentCues;
    mediaElement.updateTextTrackDisplay();
}

void CueTimeline::beginIgnoringUpdateRequests()
{
    ++m_ignoreUpdate;
}

void CueTimeline::endIgnoringUpdateRequests()
{
    ASSERT(m_ignoreUpdate);
    --m_ignoreUpdate;
    if (!m_ignoreUpdate)
        updateActiveCues(mediaElement().currentTime());
}

DEFINE_TRACE(CueTimeline)
{
    visitor->trace(m_mediaElement);
}

} // namespace blink
