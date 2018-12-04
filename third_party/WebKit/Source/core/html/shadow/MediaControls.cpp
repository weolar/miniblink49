/*
 * Copyright (C) 2011, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2011, 2012 Google Inc. All rights reserved.
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
#include "core/html/shadow/MediaControls.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/ClientRect.h"
#include "core/events/MouseEvent.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/MediaController.h"
#include "core/html/track/TextTrackContainer.h"
#include "core/layout/LayoutTheme.h"

namespace blink {

// If you change this value, then also update the corresponding value in
// LayoutTests/media/media-controls.js.
static const double timeWithoutMouseMovementBeforeHidingMediaControls = 3;

static bool fullscreenIsSupported(const Document& document)
{
    return !document.settings() || document.settings()->fullscreenSupported();
}

MediaControls::MediaControls(HTMLMediaElement& mediaElement)
    : HTMLDivElement(mediaElement.document())
    , m_mediaElement(&mediaElement)
    , m_overlayEnclosure(nullptr)
    , m_overlayPlayButton(nullptr)
    , m_overlayCastButton(nullptr)
    , m_enclosure(nullptr)
    , m_panel(nullptr)
    , m_playButton(nullptr)
    , m_timeline(nullptr)
    , m_currentTimeDisplay(nullptr)
    , m_durationDisplay(nullptr)
    , m_muteButton(nullptr)
    , m_volumeSlider(nullptr)
    , m_toggleClosedCaptionsButton(nullptr)
    , m_castButton(nullptr)
    , m_fullScreenButton(nullptr)
    , m_hideMediaControlsTimer(this, &MediaControls::hideMediaControlsTimerFired)
    , m_hideTimerBehaviorFlags(IgnoreNone)
    , m_isMouseOverControls(false)
    , m_isPausedForScrubbing(false)
{
}

PassRefPtrWillBeRawPtr<MediaControls> MediaControls::create(HTMLMediaElement& mediaElement)
{
    RefPtrWillBeRawPtr<MediaControls> controls = adoptRefWillBeNoop(new MediaControls(mediaElement));
    controls->setShadowPseudoId(AtomicString("-webkit-media-controls", AtomicString::ConstructFromLiteral));
    controls->initializeControls();
    return controls.release();
}

// The media controls DOM structure looks like:
//
// MediaControls                                       (-webkit-media-controls)
// +-MediaControlOverlayEnclosureElement               (-webkit-media-controls-overlay-enclosure)
// | +-MediaControlOverlayPlayButtonElement            (-webkit-media-controls-overlay-play-button)
// | | {if mediaControlsOverlayPlayButtonEnabled}
// | \-MediaControlCastButtonElement                   (-internal-media-controls-overlay-cast-button)
// \-MediaControlPanelEnclosureElement                 (-webkit-media-controls-enclosure)
//   \-MediaControlPanelElement                        (-webkit-media-controls-panel)
//     +-MediaControlPlayButtonElement                 (-webkit-media-controls-play-button)
//     +-MediaControlTimelineElement                   (-webkit-media-controls-timeline)
//     +-MediaControlCurrentTimeDisplayElement         (-webkit-media-controls-current-time-display)
//     +-MediaControlTimeRemainingDisplayElement       (-webkit-media-controls-time-remaining-display)
//     +-MediaControlMuteButtonElement                 (-webkit-media-controls-mute-button)
//     +-MediaControlVolumeSliderElement               (-webkit-media-controls-volume-slider)
//     +-MediaControlToggleClosedCaptionsButtonElement (-webkit-media-controls-toggle-closed-captions-button)
//     +-MediaControlCastButtonElement                 (-internal-media-controls-cast-button)
//     \-MediaControlFullscreenButtonElement           (-webkit-media-controls-fullscreen-button)
void MediaControls::initializeControls()
{
    RefPtrWillBeRawPtr<MediaControlOverlayEnclosureElement> overlayEnclosure = MediaControlOverlayEnclosureElement::create(*this);

    if (document().settings() && document().settings()->mediaControlsOverlayPlayButtonEnabled()) {
        RefPtrWillBeRawPtr<MediaControlOverlayPlayButtonElement> overlayPlayButton = MediaControlOverlayPlayButtonElement::create(*this);
        m_overlayPlayButton = overlayPlayButton.get();
        overlayEnclosure->appendChild(overlayPlayButton.release());
    }

    RefPtrWillBeRawPtr<MediaControlCastButtonElement> overlayCastButton = MediaControlCastButtonElement::create(*this, true);
    m_overlayCastButton = overlayCastButton.get();
    overlayEnclosure->appendChild(overlayCastButton.release());

    m_overlayEnclosure = overlayEnclosure.get();
    appendChild(overlayEnclosure.release());

    // Create an enclosing element for the panel so we can visually offset the controls correctly.
    RefPtrWillBeRawPtr<MediaControlPanelEnclosureElement> enclosure = MediaControlPanelEnclosureElement::create(*this);

    RefPtrWillBeRawPtr<MediaControlPanelElement> panel = MediaControlPanelElement::create(*this);

    RefPtrWillBeRawPtr<MediaControlPlayButtonElement> playButton = MediaControlPlayButtonElement::create(*this);
    m_playButton = playButton.get();
    panel->appendChild(playButton.release());

    RefPtrWillBeRawPtr<MediaControlTimelineElement> timeline = MediaControlTimelineElement::create(*this);
    m_timeline = timeline.get();
    panel->appendChild(timeline.release());

    RefPtrWillBeRawPtr<MediaControlCurrentTimeDisplayElement> currentTimeDisplay = MediaControlCurrentTimeDisplayElement::create(*this);
    m_currentTimeDisplay = currentTimeDisplay.get();
    m_currentTimeDisplay->hide();
    panel->appendChild(currentTimeDisplay.release());

    RefPtrWillBeRawPtr<MediaControlTimeRemainingDisplayElement> durationDisplay = MediaControlTimeRemainingDisplayElement::create(*this);
    m_durationDisplay = durationDisplay.get();
    panel->appendChild(durationDisplay.release());

    RefPtrWillBeRawPtr<MediaControlMuteButtonElement> muteButton = MediaControlMuteButtonElement::create(*this);
    m_muteButton = muteButton.get();
    panel->appendChild(muteButton.release());

    RefPtrWillBeRawPtr<MediaControlVolumeSliderElement> slider = MediaControlVolumeSliderElement::create(*this);
    m_volumeSlider = slider.get();
    panel->appendChild(slider.release());

    RefPtrWillBeRawPtr<MediaControlToggleClosedCaptionsButtonElement> toggleClosedCaptionsButton = MediaControlToggleClosedCaptionsButtonElement::create(*this);
    m_toggleClosedCaptionsButton = toggleClosedCaptionsButton.get();
    panel->appendChild(toggleClosedCaptionsButton.release());

    RefPtrWillBeRawPtr<MediaControlCastButtonElement> castButton = MediaControlCastButtonElement::create(*this, false);
    m_castButton = castButton.get();
    panel->appendChild(castButton.release());

    RefPtrWillBeRawPtr<MediaControlFullscreenButtonElement> fullscreenButton = MediaControlFullscreenButtonElement::create(*this);
    m_fullScreenButton = fullscreenButton.get();
    panel->appendChild(fullscreenButton.release());

    m_panel = panel.get();
    enclosure->appendChild(panel.release());

    m_enclosure = enclosure.get();
    appendChild(enclosure.release());
}

void MediaControls::reset()
{
    double duration = mediaElement().duration();
    m_durationDisplay->setInnerText(LayoutTheme::theme().formatMediaControlsTime(duration), ASSERT_NO_EXCEPTION);
    m_durationDisplay->setCurrentValue(duration);

    updatePlayState();

    updateCurrentTimeDisplay();

    m_timeline->setDuration(duration);
    m_timeline->setPosition(mediaElement().currentTime());

    if (!mediaElement().hasAudio())
        m_volumeSlider->hide();
    else
        m_volumeSlider->show();
    updateVolume();

    refreshClosedCaptionsButtonVisibility();

    // Unconditionally allow the user to exit fullscreen if we are in it
    // now.  Especially on android, when we might not yet know if
    // fullscreen is supported, we sometimes guess incorrectly and show
    // the button earlier, and we don't want to remove it here if the
    // user chose to enter fullscreen.  crbug.com/500732 .
    if ((mediaElement().hasVideo() && fullscreenIsSupported(document()))
        || mediaElement().isFullscreen())
        m_fullScreenButton->show();
    else
        m_fullScreenButton->hide();

    refreshCastButtonVisibility();
    makeOpaque();
}

LayoutObject* MediaControls::layoutObjectForTextTrackLayout()
{
    return m_panel->layoutObject();
}

void MediaControls::show()
{
    makeOpaque();
    m_panel->show();
    m_panel->setIsDisplayed(true);
    if (m_overlayPlayButton)
        m_overlayPlayButton->updateDisplayType();

#ifndef MINIBLINK_NO_CHANGE
    WebMediaPlayer* player = m_mediaElement->webMediaPlayer();
    if (player)
        player->showMediaControls();
#endif
}

void MediaControls::mediaElementFocused()
{
    if (mediaElement().shouldShowControls()) {
        show();
        resetHideMediaControlsTimer();
    }
}

void MediaControls::hide()
{
    m_panel->hide();
    m_panel->setIsDisplayed(false);
    if (m_overlayPlayButton)
        m_overlayPlayButton->hide();

#ifndef MINIBLINK_NO_CHANGE
    WebMediaPlayer* player = m_mediaElement->webMediaPlayer();
    if (player)
        player->hideMediaControls();
#endif
}

void MediaControls::makeOpaque()
{
    m_panel->makeOpaque();
}

void MediaControls::makeTransparent()
{
    m_panel->makeTransparent();
}

bool MediaControls::shouldHideMediaControls(unsigned behaviorFlags) const
{
    // Never hide for a media element without visual representation.
    if (!mediaElement().hasVideo() || mediaElement().isPlayingRemotely())
        return false;
    // Don't hide if the mouse is over the controls.
    const bool ignoreControlsHover = behaviorFlags & IgnoreControlsHover;
    if (!ignoreControlsHover && m_panel->hovered())
        return false;
    // Don't hide if the mouse is over the video area.
    const bool ignoreVideoHover = behaviorFlags & IgnoreVideoHover;
    if (!ignoreVideoHover && m_isMouseOverControls)
        return false;
    // Don't hide if focus is on the HTMLMediaElement or within the
    // controls/shadow tree. (Perform the checks separately to avoid going
    // through all the potential ancestor hosts for the focused element.)
    const bool ignoreFocus = behaviorFlags & IgnoreFocus;
    if (!ignoreFocus && (mediaElement().focused() || contains(document().focusedElement())))
        return false;
    return true;
}

void MediaControls::playbackStarted()
{
    m_currentTimeDisplay->show();
    m_durationDisplay->hide();

    updatePlayState();
    m_timeline->setPosition(mediaElement().currentTime());
    updateCurrentTimeDisplay();

    startHideMediaControlsTimer();
}

void MediaControls::playbackProgressed()
{
    m_timeline->setPosition(mediaElement().currentTime());
    updateCurrentTimeDisplay();

    if (shouldHideMediaControls())
        makeTransparent();
}

void MediaControls::playbackStopped()
{
    updatePlayState();
    m_timeline->setPosition(mediaElement().currentTime());
    updateCurrentTimeDisplay();
    makeOpaque();

    stopHideMediaControlsTimer();
}

void MediaControls::updatePlayState()
{
    if (m_isPausedForScrubbing)
        return;

    if (m_overlayPlayButton)
        m_overlayPlayButton->updateDisplayType();
    m_playButton->updateDisplayType();
}

void MediaControls::beginScrubbing()
{
    if (!mediaElement().togglePlayStateWillPlay()) {
        m_isPausedForScrubbing = true;
        mediaElement().togglePlayState();
    }
}

void MediaControls::endScrubbing()
{
    if (m_isPausedForScrubbing) {
        m_isPausedForScrubbing = false;
        if (mediaElement().togglePlayStateWillPlay())
            mediaElement().togglePlayState();
    }
}

void MediaControls::updateCurrentTimeDisplay()
{
    double now = mediaElement().currentTime();
    double duration = mediaElement().duration();

    // After seek, hide duration display and show current time.
    if (now > 0) {
        m_currentTimeDisplay->show();
        m_durationDisplay->hide();
    }

    // Allow the theme to format the time.
    m_currentTimeDisplay->setInnerText(LayoutTheme::theme().formatMediaControlsCurrentTime(now, duration), IGNORE_EXCEPTION);
    m_currentTimeDisplay->setCurrentValue(now);
}

void MediaControls::updateVolume()
{
    m_muteButton->updateDisplayType();
    // Invalidate the mute button because it paints differently according to volume.
    if (LayoutObject* layoutObject = m_muteButton->layoutObject())
        layoutObject->setShouldDoFullPaintInvalidation();

    if (mediaElement().muted())
        m_volumeSlider->setVolume(0);
    else
        m_volumeSlider->setVolume(mediaElement().volume());
    // Invalidate the volume slider because it paints differently according to volume.
    if (LayoutObject* layoutObject = m_volumeSlider->layoutObject())
        layoutObject->setShouldDoFullPaintInvalidation();
}

void MediaControls::changedClosedCaptionsVisibility()
{
    m_toggleClosedCaptionsButton->updateDisplayType();
}

void MediaControls::refreshClosedCaptionsButtonVisibility()
{
    if (mediaElement().hasClosedCaptions())
        m_toggleClosedCaptionsButton->show();
    else
        m_toggleClosedCaptionsButton->hide();
}

static Element* elementFromCenter(Element& element)
{
    ClientRect* clientRect = element.getBoundingClientRect();
    int centerX = static_cast<int>((clientRect->left() + clientRect->right()) / 2);
    int centerY = static_cast<int>((clientRect->top() + clientRect->bottom()) / 2);

    return element.document().elementFromPoint(centerX , centerY);
}

void MediaControls::tryShowOverlayCastButton()
{
    // The element needs to be shown to have its dimensions and position.
    m_overlayCastButton->show();

    if (elementFromCenter(*m_overlayCastButton) != &mediaElement())
        m_overlayCastButton->hide();
}

void MediaControls::refreshCastButtonVisibility()
{
    if (mediaElement().hasRemoteRoutes()) {
        // The reason for the autoplay test is that some pages (e.g. vimeo.com) have an autoplay background video, which
        // doesn't autoplay on Chrome for Android (we prevent it) so starts paused. In such cases we don't want to automatically
        // show the cast button, since it looks strange and is unlikely to correspond with anything the user wants to do.
        // If a user does want to cast a paused autoplay video then they can still do so by touching or clicking on the
        // video, which will cause the cast button to appear.
        if (!mediaElement().shouldShowControls() && !mediaElement().autoplay() && mediaElement().paused()) {
            showOverlayCastButton();
        } else if (mediaElement().shouldShowControls()) {
            m_overlayCastButton->hide();
            m_castButton->show();
            // Check that the cast button actually fits on the bar.
            if (m_fullScreenButton->getBoundingClientRect()->right() > m_panel->getBoundingClientRect()->right()) {
                m_castButton->hide();
                tryShowOverlayCastButton();
            }
        }
    } else {
        m_castButton->hide();
        m_overlayCastButton->hide();
    }
}

void MediaControls::showOverlayCastButton()
{
    tryShowOverlayCastButton();
    resetHideMediaControlsTimer();
}

void MediaControls::enteredFullscreen()
{
    m_fullScreenButton->setIsFullscreen(true);
    stopHideMediaControlsTimer();
    startHideMediaControlsTimer();
}

void MediaControls::exitedFullscreen()
{
    m_fullScreenButton->setIsFullscreen(false);
    stopHideMediaControlsTimer();
    startHideMediaControlsTimer();
}

void MediaControls::startedCasting()
{
    m_castButton->setIsPlayingRemotely(true);
    m_overlayCastButton->setIsPlayingRemotely(true);
}

void MediaControls::stoppedCasting()
{
    m_castButton->setIsPlayingRemotely(false);
    m_overlayCastButton->setIsPlayingRemotely(false);
}

void MediaControls::defaultEventHandler(Event* event)
{
    HTMLDivElement::defaultEventHandler(event);

    // Add IgnoreControlsHover to m_hideTimerBehaviorFlags when we see a touch event,
    // to allow the hide-timer to do the right thing when it fires.
    // FIXME: Preferably we would only do this when we're actually handling the event
    // here ourselves.
    bool wasLastEventTouch = event->isTouchEvent() || event->isGestureEvent()
        || (event->isMouseEvent() && toMouseEvent(event)->fromTouch());
    m_hideTimerBehaviorFlags |= wasLastEventTouch ? IgnoreControlsHover : IgnoreNone;

    if (event->type() == EventTypeNames::mouseover) {
        if (!containsRelatedTarget(event)) {
            m_isMouseOverControls = true;
            if (!mediaElement().togglePlayStateWillPlay()) {
                makeOpaque();
                if (shouldHideMediaControls())
                    startHideMediaControlsTimer();
            }
        }
        return;
    }

    if (event->type() == EventTypeNames::mouseout) {
        if (!containsRelatedTarget(event)) {
            m_isMouseOverControls = false;
            stopHideMediaControlsTimer();
        }
        return;
    }

    if (event->type() == EventTypeNames::mousemove) {
        // When we get a mouse move, show the media controls, and start a timer
        // that will hide the media controls after a 3 seconds without a mouse move.
        makeOpaque();
        refreshCastButtonVisibility();
        if (shouldHideMediaControls(IgnoreVideoHover))
            startHideMediaControlsTimer();
        return;
    }
}

void MediaControls::hideMediaControlsTimerFired(Timer<MediaControls>*)
{
    unsigned behaviorFlags = m_hideTimerBehaviorFlags | IgnoreFocus | IgnoreVideoHover;
    m_hideTimerBehaviorFlags = IgnoreNone;

    if (mediaElement().togglePlayStateWillPlay())
        return;

    if (!shouldHideMediaControls(behaviorFlags))
        return;

    makeTransparent();
    m_overlayCastButton->hide();
}

void MediaControls::startHideMediaControlsTimer()
{
    m_hideMediaControlsTimer.startOneShot(timeWithoutMouseMovementBeforeHidingMediaControls, FROM_HERE);
}

void MediaControls::stopHideMediaControlsTimer()
{
    m_hideMediaControlsTimer.stop();
}

void MediaControls::resetHideMediaControlsTimer()
{
    stopHideMediaControlsTimer();
    if (!mediaElement().paused())
        startHideMediaControlsTimer();
}


bool MediaControls::containsRelatedTarget(Event* event)
{
    if (!event->isMouseEvent())
        return false;
    EventTarget* relatedTarget = toMouseEvent(event)->relatedTarget();
    if (!relatedTarget)
        return false;
    return contains(relatedTarget->toNode());
}

DEFINE_TRACE(MediaControls)
{
    visitor->trace(m_mediaElement);
    visitor->trace(m_panel);
    visitor->trace(m_overlayPlayButton);
    visitor->trace(m_overlayEnclosure);
    visitor->trace(m_playButton);
    visitor->trace(m_currentTimeDisplay);
    visitor->trace(m_timeline);
    visitor->trace(m_muteButton);
    visitor->trace(m_volumeSlider);
    visitor->trace(m_toggleClosedCaptionsButton);
    visitor->trace(m_fullScreenButton);
    visitor->trace(m_durationDisplay);
    visitor->trace(m_enclosure);
    visitor->trace(m_castButton);
    visitor->trace(m_overlayCastButton);
    HTMLDivElement::trace(visitor);
}

}
