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
#include "core/html/shadow/MediaControlElements.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/InputTypeNames.h"
#include "core/dom/DOMTokenList.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/MouseEvent.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLVideoElement.h"
#include "core/html/MediaController.h"
#include "core/html/TimeRanges.h"
#include "core/html/shadow/MediaControls.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutSlider.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/LayoutVideo.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

using namespace HTMLNames;

// This is the duration from mediaControls.css
static const double fadeOutDuration = 0.3;

static bool isUserInteractionEvent(Event* event)
{
    const AtomicString& type = event->type();
    return type == EventTypeNames::mousedown
        || type == EventTypeNames::mouseup
        || type == EventTypeNames::click
        || type == EventTypeNames::dblclick
        || event->isKeyboardEvent()
        || event->isTouchEvent();
}

// Sliders (the volume control and timeline) need to capture some additional events used when dragging the thumb.
static bool isUserInteractionEventForSlider(Event* event)
{
    const AtomicString& type = event->type();
    return type == EventTypeNames::mousedown
        || type == EventTypeNames::mouseup
        || type == EventTypeNames::click
        || type == EventTypeNames::dblclick
        || type == EventTypeNames::mouseover
        || type == EventTypeNames::mouseout
        || type == EventTypeNames::mousemove
        || event->isKeyboardEvent()
        || event->isTouchEvent();
}


MediaControlPanelElement::MediaControlPanelElement(MediaControls& mediaControls)
    : MediaControlDivElement(mediaControls, MediaControlsPanel)
    , m_isDisplayed(false)
    , m_opaque(true)
    , m_transitionTimer(this, &MediaControlPanelElement::transitionTimerFired)
{
}

PassRefPtrWillBeRawPtr<MediaControlPanelElement> MediaControlPanelElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlPanelElement> panel = adoptRefWillBeNoop(new MediaControlPanelElement(mediaControls));
    panel->setShadowPseudoId(AtomicString("-webkit-media-controls-panel", AtomicString::ConstructFromLiteral));
    return panel.release();
}

void MediaControlPanelElement::defaultEventHandler(Event* event)
{
    // Suppress the media element activation behavior (toggle play/pause) when
    // any part of the control panel is clicked.
    if (event->type() == EventTypeNames::click) {
        event->setDefaultHandled();
        return;
    }
    HTMLDivElement::defaultEventHandler(event);
}

void MediaControlPanelElement::startTimer()
{
    stopTimer();

    // The timer is required to set the property display:'none' on the panel,
    // such that captions are correctly displayed at the bottom of the video
    // at the end of the fadeout transition.
    // FIXME: Racing a transition with a setTimeout like this is wrong.
    m_transitionTimer.startOneShot(fadeOutDuration, FROM_HERE);
}

void MediaControlPanelElement::stopTimer()
{
    if (m_transitionTimer.isActive())
        m_transitionTimer.stop();
}

void MediaControlPanelElement::transitionTimerFired(Timer<MediaControlPanelElement>*)
{
    if (!m_opaque)
        hide();

    stopTimer();
}

void MediaControlPanelElement::didBecomeVisible()
{
    ASSERT(m_isDisplayed && m_opaque);
    mediaElement().mediaControlsDidBecomeVisible();
}

void MediaControlPanelElement::makeOpaque()
{
    if (m_opaque)
        return;

    setInlineStyleProperty(CSSPropertyOpacity, 1.0, CSSPrimitiveValue::CSS_NUMBER);
    m_opaque = true;

    if (m_isDisplayed) {
        show();
        didBecomeVisible();
    }
}

void MediaControlPanelElement::makeTransparent()
{
    if (!m_opaque)
        return;

    setInlineStyleProperty(CSSPropertyOpacity, 0.0, CSSPrimitiveValue::CSS_NUMBER);

    m_opaque = false;
    startTimer();
}

void MediaControlPanelElement::setIsDisplayed(bool isDisplayed)
{
    if (m_isDisplayed == isDisplayed)
        return;

    m_isDisplayed = isDisplayed;
    if (m_isDisplayed && m_opaque)
        didBecomeVisible();
}

bool MediaControlPanelElement::keepEventInNode(Event* event)
{
    return isUserInteractionEvent(event);
}

// ----------------------------

MediaControlPanelEnclosureElement::MediaControlPanelEnclosureElement(MediaControls& mediaControls)
    // Mapping onto same MediaControlElementType as panel element, since it has similar properties.
    : MediaControlDivElement(mediaControls, MediaControlsPanel)
{
}

PassRefPtrWillBeRawPtr<MediaControlPanelEnclosureElement> MediaControlPanelEnclosureElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlPanelEnclosureElement> enclosure = adoptRefWillBeNoop(new MediaControlPanelEnclosureElement(mediaControls));
    enclosure->setShadowPseudoId(AtomicString("-webkit-media-controls-enclosure", AtomicString::ConstructFromLiteral));
    return enclosure.release();
}

// ----------------------------

MediaControlOverlayEnclosureElement::MediaControlOverlayEnclosureElement(MediaControls& mediaControls)
    // Mapping onto same MediaControlElementType as panel element, since it has similar properties.
    : MediaControlDivElement(mediaControls, MediaControlsPanel)
{
}

PassRefPtrWillBeRawPtr<MediaControlOverlayEnclosureElement> MediaControlOverlayEnclosureElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlOverlayEnclosureElement> enclosure = adoptRefWillBeNoop(new MediaControlOverlayEnclosureElement(mediaControls));
    enclosure->setShadowPseudoId(AtomicString("-webkit-media-controls-overlay-enclosure", AtomicString::ConstructFromLiteral));
    return enclosure.release();
}

void* MediaControlOverlayEnclosureElement::preDispatchEventHandler(Event* event)
{
    // When the media element is clicked or touched we want to make the overlay cast button visible
    // (if the other requirements are right) even if JavaScript is doing its own handling of the event.
    // Doing it in preDispatchEventHandler prevents any interference from JavaScript.
    // Note that we can't simply test for click, since JS handling of touch events can prevent their translation to click events.
    if (event && (event->type() == EventTypeNames::click || event->type() == EventTypeNames::touchstart) && mediaElement().hasRemoteRoutes() && !mediaElement().shouldShowControls())
        mediaControls().showOverlayCastButton();
    return MediaControlDivElement::preDispatchEventHandler(event);
}


// ----------------------------

MediaControlMuteButtonElement::MediaControlMuteButtonElement(MediaControls& mediaControls)
    : MediaControlInputElement(mediaControls, MediaMuteButton)
{
}

PassRefPtrWillBeRawPtr<MediaControlMuteButtonElement> MediaControlMuteButtonElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlMuteButtonElement> button = adoptRefWillBeNoop(new MediaControlMuteButtonElement(mediaControls));
    button->ensureUserAgentShadowRoot();
    button->setType(InputTypeNames::button);
    button->setShadowPseudoId(AtomicString("-webkit-media-controls-mute-button", AtomicString::ConstructFromLiteral));
    return button.release();
}

void MediaControlMuteButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::click) {
        mediaElement().setMuted(!mediaElement().muted());
        event->setDefaultHandled();
    }

    HTMLInputElement::defaultEventHandler(event);
}

void MediaControlMuteButtonElement::updateDisplayType()
{
    setDisplayType(mediaElement().muted() ? MediaUnMuteButton : MediaMuteButton);
}

// ----------------------------

MediaControlPlayButtonElement::MediaControlPlayButtonElement(MediaControls& mediaControls)
    : MediaControlInputElement(mediaControls, MediaPlayButton)
{
}

PassRefPtrWillBeRawPtr<MediaControlPlayButtonElement> MediaControlPlayButtonElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlPlayButtonElement> button = adoptRefWillBeNoop(new MediaControlPlayButtonElement(mediaControls));
    button->ensureUserAgentShadowRoot();
    button->setType(InputTypeNames::button);
    button->setShadowPseudoId(AtomicString("-webkit-media-controls-play-button", AtomicString::ConstructFromLiteral));
    return button.release();
}

void MediaControlPlayButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::click) {
        mediaElement().togglePlayState();
        updateDisplayType();
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

void MediaControlPlayButtonElement::updateDisplayType()
{
    setDisplayType(mediaElement().togglePlayStateWillPlay() ? MediaPlayButton : MediaPauseButton);
}

// ----------------------------

MediaControlOverlayPlayButtonElement::MediaControlOverlayPlayButtonElement(MediaControls& mediaControls)
    : MediaControlInputElement(mediaControls, MediaOverlayPlayButton)
{
}

PassRefPtrWillBeRawPtr<MediaControlOverlayPlayButtonElement> MediaControlOverlayPlayButtonElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlOverlayPlayButtonElement> button = adoptRefWillBeNoop(new MediaControlOverlayPlayButtonElement(mediaControls));
    button->ensureUserAgentShadowRoot();
    button->setType(InputTypeNames::button);
    button->setShadowPseudoId(AtomicString("-webkit-media-controls-overlay-play-button", AtomicString::ConstructFromLiteral));
    return button.release();
}

void MediaControlOverlayPlayButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::click && mediaElement().togglePlayStateWillPlay()) {
        mediaElement().togglePlayState();
        updateDisplayType();
        event->setDefaultHandled();
    }
}

void MediaControlOverlayPlayButtonElement::updateDisplayType()
{
    if (mediaElement().shouldShowControls() && mediaElement().togglePlayStateWillPlay())
        show();
    else
        hide();
}

bool MediaControlOverlayPlayButtonElement::keepEventInNode(Event* event)
{
    return isUserInteractionEvent(event);
}


// ----------------------------

MediaControlToggleClosedCaptionsButtonElement::MediaControlToggleClosedCaptionsButtonElement(MediaControls& mediaControls)
    : MediaControlInputElement(mediaControls, MediaShowClosedCaptionsButton)
{
}

PassRefPtrWillBeRawPtr<MediaControlToggleClosedCaptionsButtonElement> MediaControlToggleClosedCaptionsButtonElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlToggleClosedCaptionsButtonElement> button = adoptRefWillBeNoop(new MediaControlToggleClosedCaptionsButtonElement(mediaControls));
    button->ensureUserAgentShadowRoot();
    button->setType(InputTypeNames::button);
    button->setShadowPseudoId(AtomicString("-webkit-media-controls-toggle-closed-captions-button", AtomicString::ConstructFromLiteral));
    button->hide();
    return button.release();
}

void MediaControlToggleClosedCaptionsButtonElement::updateDisplayType()
{
    bool captionsVisible = mediaElement().closedCaptionsVisible();
    setDisplayType(captionsVisible ? MediaHideClosedCaptionsButton : MediaShowClosedCaptionsButton);
    setChecked(captionsVisible);
}

void MediaControlToggleClosedCaptionsButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::click) {
        mediaElement().setClosedCaptionsVisible(!mediaElement().closedCaptionsVisible());
        setChecked(mediaElement().closedCaptionsVisible());
        updateDisplayType();
        event->setDefaultHandled();
    }

    HTMLInputElement::defaultEventHandler(event);
}

// ----------------------------

MediaControlTimelineElement::MediaControlTimelineElement(MediaControls& mediaControls)
    : MediaControlInputElement(mediaControls, MediaSlider)
{
}

PassRefPtrWillBeRawPtr<MediaControlTimelineElement> MediaControlTimelineElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlTimelineElement> timeline = adoptRefWillBeNoop(new MediaControlTimelineElement(mediaControls));
    timeline->ensureUserAgentShadowRoot();
    timeline->setType(InputTypeNames::range);
    timeline->setAttribute(stepAttr, "any");
    timeline->setShadowPseudoId(AtomicString("-webkit-media-controls-timeline", AtomicString::ConstructFromLiteral));
    return timeline.release();
}

void MediaControlTimelineElement::defaultEventHandler(Event* event)
{
    if (event->isMouseEvent() && toMouseEvent(event)->button() != LeftButton)
        return;

    if (!inDocument() || !document().isActive())
        return;

    if (event->type() == EventTypeNames::mousedown)
        mediaControls().beginScrubbing();

    if (event->type() == EventTypeNames::mouseup)
        mediaControls().endScrubbing();

    MediaControlInputElement::defaultEventHandler(event);

    if (event->type() == EventTypeNames::mouseover || event->type() == EventTypeNames::mouseout || event->type() == EventTypeNames::mousemove)
        return;

    double time = value().toDouble();
    if (event->type() == EventTypeNames::input) {
        // FIXME: This will need to take the timeline offset into consideration
        // once that concept is supported, see https://crbug.com/312699
        if (mediaElement().controller()) {
            if (mediaElement().controller()->seekable()->contain(time))
                mediaElement().controller()->setCurrentTime(time);
        } else if (mediaElement().seekable()->contain(time)) {
            mediaElement().setCurrentTime(time, IGNORE_EXCEPTION);
        }
    }

    LayoutSlider* slider = toLayoutSlider(layoutObject());
    if (slider && slider->inDragMode())
        mediaControls().updateCurrentTimeDisplay();
}

bool MediaControlTimelineElement::willRespondToMouseClickEvents()
{
    return inDocument() && document().isActive();
}

void MediaControlTimelineElement::setPosition(double currentTime)
{
    setValue(String::number(currentTime));

    if (LayoutObject* layoutObject = this->layoutObject())
        layoutObject->setShouldDoFullPaintInvalidation();
}

void MediaControlTimelineElement::setDuration(double duration)
{
    setFloatingPointAttribute(maxAttr, std::isfinite(duration) ? duration : 0);

    if (LayoutObject* layoutObject = this->layoutObject())
        layoutObject->setShouldDoFullPaintInvalidation();
}

bool MediaControlTimelineElement::keepEventInNode(Event* event)
{
    return isUserInteractionEventForSlider(event);
}

// ----------------------------

MediaControlVolumeSliderElement::MediaControlVolumeSliderElement(MediaControls& mediaControls)
    : MediaControlInputElement(mediaControls, MediaVolumeSlider)
{
}

PassRefPtrWillBeRawPtr<MediaControlVolumeSliderElement> MediaControlVolumeSliderElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlVolumeSliderElement> slider = adoptRefWillBeNoop(new MediaControlVolumeSliderElement(mediaControls));
    slider->ensureUserAgentShadowRoot();
    slider->setType(InputTypeNames::range);
    slider->setAttribute(stepAttr, "any");
    slider->setAttribute(maxAttr, "1");
    slider->setShadowPseudoId(AtomicString("-webkit-media-controls-volume-slider", AtomicString::ConstructFromLiteral));
    return slider.release();
}

void MediaControlVolumeSliderElement::defaultEventHandler(Event* event)
{
    if (event->isMouseEvent() && toMouseEvent(event)->button() != LeftButton)
        return;

    if (!inDocument() || !document().isActive())
        return;

    MediaControlInputElement::defaultEventHandler(event);

    if (event->type() == EventTypeNames::mouseover || event->type() == EventTypeNames::mouseout || event->type() == EventTypeNames::mousemove)
        return;

    double volume = value().toDouble();
    mediaElement().setVolume(volume, ASSERT_NO_EXCEPTION);
    mediaElement().setMuted(false);
}

bool MediaControlVolumeSliderElement::willRespondToMouseMoveEvents()
{
    if (!inDocument() || !document().isActive())
        return false;

    return MediaControlInputElement::willRespondToMouseMoveEvents();
}

bool MediaControlVolumeSliderElement::willRespondToMouseClickEvents()
{
    if (!inDocument() || !document().isActive())
        return false;

    return MediaControlInputElement::willRespondToMouseClickEvents();
}

void MediaControlVolumeSliderElement::setVolume(double volume)
{
    if (value().toDouble() != volume)
        setValue(String::number(volume));
}

bool MediaControlVolumeSliderElement::keepEventInNode(Event* event)
{
    return isUserInteractionEventForSlider(event);
}

// ----------------------------

MediaControlFullscreenButtonElement::MediaControlFullscreenButtonElement(MediaControls& mediaControls)
    : MediaControlInputElement(mediaControls, MediaEnterFullscreenButton)
{
}

PassRefPtrWillBeRawPtr<MediaControlFullscreenButtonElement> MediaControlFullscreenButtonElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlFullscreenButtonElement> button = adoptRefWillBeNoop(new MediaControlFullscreenButtonElement(mediaControls));
    button->ensureUserAgentShadowRoot();
    button->setType(InputTypeNames::button);
    button->setShadowPseudoId(AtomicString("-webkit-media-controls-fullscreen-button", AtomicString::ConstructFromLiteral));
    button->hide();
    return button.release();
}

void MediaControlFullscreenButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::click) {
        if (mediaElement().isFullscreen())
            mediaElement().exitFullscreen();
        else
            mediaElement().enterFullscreen();
        event->setDefaultHandled();
    }
    HTMLInputElement::defaultEventHandler(event);
}

void MediaControlFullscreenButtonElement::setIsFullscreen(bool isFullscreen)
{
    setDisplayType(isFullscreen ? MediaExitFullscreenButton : MediaEnterFullscreenButton);
}

// ----------------------------

MediaControlCastButtonElement::MediaControlCastButtonElement(MediaControls& mediaControls, bool isOverlayButton)
    : MediaControlInputElement(mediaControls, MediaCastOnButton), m_isOverlayButton(isOverlayButton)
{
    setIsPlayingRemotely(false);
}

PassRefPtrWillBeRawPtr<MediaControlCastButtonElement> MediaControlCastButtonElement::create(MediaControls& mediaControls, bool isOverlayButton)
{
    RefPtrWillBeRawPtr<MediaControlCastButtonElement> button = adoptRefWillBeNoop(new MediaControlCastButtonElement(mediaControls, isOverlayButton));
    button->ensureUserAgentShadowRoot();
    button->setType(InputTypeNames::button);
    return button.release();
}

void MediaControlCastButtonElement::defaultEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::click) {
        if (mediaElement().isPlayingRemotely()) {
            mediaElement().requestRemotePlaybackControl();
        } else {
            mediaElement().requestRemotePlayback();
        }
    }
    HTMLInputElement::defaultEventHandler(event);
}

const AtomicString& MediaControlCastButtonElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(AtomicString, id_nonOverlay, ("-internal-media-controls-cast-button", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(AtomicString, id_overlay, ("-internal-media-controls-overlay-cast-button", AtomicString::ConstructFromLiteral));
    return m_isOverlayButton ? id_overlay : id_nonOverlay;
}

void MediaControlCastButtonElement::setIsPlayingRemotely(bool isPlayingRemotely)
{
    if (isPlayingRemotely) {
        if (m_isOverlayButton) {
            setDisplayType(MediaOverlayCastOnButton);
        } else {
            setDisplayType(MediaCastOnButton);
        }
    } else {
        if (m_isOverlayButton) {
            setDisplayType(MediaOverlayCastOffButton);
        } else {
            setDisplayType(MediaCastOffButton);
        }
    }
}

bool MediaControlCastButtonElement::keepEventInNode(Event* event)
{
    return isUserInteractionEvent(event);
}

// ----------------------------

MediaControlTimeRemainingDisplayElement::MediaControlTimeRemainingDisplayElement(MediaControls& mediaControls)
    : MediaControlTimeDisplayElement(mediaControls, MediaTimeRemainingDisplay)
{
}

PassRefPtrWillBeRawPtr<MediaControlTimeRemainingDisplayElement> MediaControlTimeRemainingDisplayElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlTimeRemainingDisplayElement> element = adoptRefWillBeNoop(new MediaControlTimeRemainingDisplayElement(mediaControls));
    element->setShadowPseudoId(AtomicString("-webkit-media-controls-time-remaining-display", AtomicString::ConstructFromLiteral));
    return element.release();
}

// ----------------------------

MediaControlCurrentTimeDisplayElement::MediaControlCurrentTimeDisplayElement(MediaControls& mediaControls)
    : MediaControlTimeDisplayElement(mediaControls, MediaCurrentTimeDisplay)
{
}

PassRefPtrWillBeRawPtr<MediaControlCurrentTimeDisplayElement> MediaControlCurrentTimeDisplayElement::create(MediaControls& mediaControls)
{
    RefPtrWillBeRawPtr<MediaControlCurrentTimeDisplayElement> element = adoptRefWillBeNoop(new MediaControlCurrentTimeDisplayElement(mediaControls));
    element->setShadowPseudoId(AtomicString("-webkit-media-controls-current-time-display", AtomicString::ConstructFromLiteral));
    return element.release();
}

} // namespace blink
