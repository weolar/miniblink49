/*
 * Copyright (C) 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/track/TextTrackContainer.h"

#include "core/html/HTMLVideoElement.h"
#include "core/html/track/CueTimeline.h"
#include "core/layout/LayoutTextTrackContainer.h"

namespace blink {

TextTrackContainer::TextTrackContainer(Document& document)
    : HTMLDivElement(document)
{
}

PassRefPtrWillBeRawPtr<TextTrackContainer> TextTrackContainer::create(Document& document)
{
    RefPtrWillBeRawPtr<TextTrackContainer> element = adoptRefWillBeNoop(new TextTrackContainer(document));
    element->setShadowPseudoId(AtomicString("-webkit-media-text-track-container", AtomicString::ConstructFromLiteral));
    return element.release();
}

LayoutObject* TextTrackContainer::createLayoutObject(const ComputedStyle&)
{
    return new LayoutTextTrackContainer(this);
}

void TextTrackContainer::updateDisplay(HTMLMediaElement& mediaElement, ExposingControls exposingControls)
{
    if (!mediaElement.closedCaptionsVisible()) {
        removeChildren();
        return;
    }

    // http://dev.w3.org/html5/webvtt/#dfn-rules-for-updating-the-display-of-webvtt-text-tracks

    // 1. If the media element is an audio element, or is another playback
    // mechanism with no rendering area, abort these steps. There is nothing to
    // render.
    if (isHTMLAudioElement(mediaElement))
        return;

    // 2. Let video be the media element or other playback mechanism.
    HTMLVideoElement& video = toHTMLVideoElement(mediaElement);

    // 3. Let output be an empty list of absolutely positioned CSS block boxes.

    // Note: This is a layout algorithm, expressed terms of appending CSS block
    // boxes to output, and the "apply WebVTT cue settings" part is implemented
    // in LayoutVTTCue. Here we merely create the DOM tree from which the layout
    // tree is built and append it to this TextTrackContainer.

    // 4. If the user agent is exposing a user interface for video, add to
    // output one or more completely transparent positioned CSS block boxes that
    // cover the same region as the user interface.

    // Note: Overlap checking for the controls is implemented in LayoutVTTCue
    // without a placeholder box (element or layout object).

    // 5. If the last time these rules were run, the user agent was not exposing
    // a user interface for video, but now it is, optionally let reset be true.
    // Otherwise, let reset be false.
    bool reset = exposingControls == DidStartExposingControls;

    // 6. Let tracks be the subset of video's list of text tracks that have as
    // their rules for updating the text track rendering these rules for
    // updating the display of WebVTT text tracks, and whose text track mode is
    // showing or showing by default.
    // 7. Let cues be an empty list of text track cues.
    // 8. For each track track in tracks, append to cues all the cues from
    // track's list of cues that have their text track cue active flag set.
    const CueList& activeCues = video.cueTimeline().currentlyActiveCues();

    // 9. If reset is false, then, for each text track cue cue in cues: if cue's
    // text track cue display state has a set of CSS boxes, then add those boxes
    // to output, and remove cue from cues.

    // Note: Removing all children will cause them to be re-inserted below,
    // invalidating the layout.
    // effect
    if (reset)
        removeChildren();

    // 10. For each text track cue cue in cues that has not yet had
    // corresponding CSS boxes added to output, in text track cue order, run the
    // following substeps:
    double movieTime = video.currentTime();
    for (size_t i = 0; i < activeCues.size(); ++i) {
        TextTrackCue* cue = activeCues[i].data();

        ASSERT(cue->isActive());
        if (!cue->track() || !cue->track()->isRendered() || !cue->isActive())
            continue;

        cue->updateDisplay(*this);
        cue->updatePastAndFutureNodes(movieTime);
    }

    // 11. Return output.
    // See the note for step 3 for why there is no output to return.
}

} // namespace blink
