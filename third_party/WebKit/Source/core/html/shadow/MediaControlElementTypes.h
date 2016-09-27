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

#ifndef MediaControlElementTypes_h
#define MediaControlElementTypes_h

#include "core/CoreExport.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/layout/LayoutBlock.h"

namespace blink {

class HTMLMediaElement;
class MediaControls;

enum MediaControlElementType {
    MediaEnterFullscreenButton = 0,
    MediaMuteButton,
    MediaPlayButton,
    MediaSlider,
    MediaSliderThumb,
    MediaShowClosedCaptionsButton,
    MediaHideClosedCaptionsButton,
    MediaUnMuteButton,
    MediaPauseButton,
    MediaTimelineContainer,
    MediaCurrentTimeDisplay,
    MediaTimeRemainingDisplay,
    MediaControlsPanel,
    MediaVolumeSliderContainer,
    MediaVolumeSlider,
    MediaVolumeSliderThumb,
    MediaFullScreenVolumeSlider,
    MediaFullScreenVolumeSliderThumb,
    MediaExitFullscreenButton,
    MediaOverlayPlayButton,
    MediaCastOffButton,
    MediaCastOnButton,
    MediaOverlayCastOffButton,
    MediaOverlayCastOnButton,
};

CORE_EXPORT HTMLMediaElement* toParentMediaElement(Node*);
inline HTMLMediaElement* toParentMediaElement(LayoutObject* layoutObject) { return toParentMediaElement(layoutObject->node()); }

CORE_EXPORT MediaControlElementType mediaControlElementType(Node*);

// ----------------------------

class MediaControlElement : public WillBeGarbageCollectedMixin {
public:
    void hide();
    void show();

    MediaControlElementType displayType() { return m_displayType; }

    DECLARE_VIRTUAL_TRACE();

protected:
    MediaControlElement(MediaControls&, MediaControlElementType, HTMLElement*);

    MediaControls& mediaControls() const { return m_mediaControls; }
    HTMLMediaElement& mediaElement() const;

    void setDisplayType(MediaControlElementType);

private:
    MediaControls& m_mediaControls;
    MediaControlElementType m_displayType;
    RawPtrWillBeMember<HTMLElement> m_element;
};

// ----------------------------

class MediaControlDivElement : public HTMLDivElement, public MediaControlElement {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(MediaControlDivElement);
public:
    DECLARE_VIRTUAL_TRACE();

protected:
    MediaControlDivElement(MediaControls&, MediaControlElementType);

private:
    bool isMediaControlElement() const final { return true; }
};

// ----------------------------

class MediaControlInputElement : public HTMLInputElement, public MediaControlElement {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(MediaControlInputElement);
public:
    DECLARE_VIRTUAL_TRACE();

protected:
    MediaControlInputElement(MediaControls&, MediaControlElementType);

private:
    virtual void updateDisplayType() { }
    bool isMediaControlElement() const final { return true; }
    bool isMouseFocusable() const override;
};

// ----------------------------

class MediaControlTimeDisplayElement : public MediaControlDivElement {
public:
    void setCurrentValue(double);
    double currentValue() const { return m_currentValue; }

protected:
    MediaControlTimeDisplayElement(MediaControls&, MediaControlElementType);

private:
    double m_currentValue;
};

} // namespace blink

#endif // MediaControlElementTypes_h
