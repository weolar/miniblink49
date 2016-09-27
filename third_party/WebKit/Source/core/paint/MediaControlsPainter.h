/*
 * Copyright (C) 2009 Apple Inc.
 * Copyright (C) 2009 Google Inc.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MediaControlsPainter_h
#define MediaControlsPainter_h

namespace blink {

struct PaintInfo;

class ComputedStyle;
class IntRect;
class LayoutObject;

class MediaControlsPainter {
public:
    static bool paintMediaMuteButton(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaPlayButton(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaToggleClosedCaptionsButton(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaSlider(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaSliderThumb(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaVolumeSlider(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaVolumeSliderThumb(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaFullscreenButton(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaOverlayPlayButton(LayoutObject*, const PaintInfo&, const IntRect&);
    static bool paintMediaCastButton(LayoutObject*, const PaintInfo&, const IntRect&);
    static void adjustMediaSliderThumbSize(ComputedStyle&);
};

} // namespace blink

#endif // MediaControlsPainter_h
