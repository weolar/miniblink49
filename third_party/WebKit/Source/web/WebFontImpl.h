/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebFontImpl_h
#define WebFontImpl_h

#include "platform/fonts/Font.h"
#include "platform/graphics/paint/DisplayItemClient.h"
#include "public/web/WebFont.h"

namespace blink {

class FontDescription;

class WebFontImpl final : public WebFont {
public:
    explicit WebFontImpl(const FontDescription&);

    WebFontDescription fontDescription() const override;

    int ascent() const override;
    int descent() const override;
    int height() const override;
    int lineSpacing() const override;
    float xHeight() const override;

    void drawText(WebCanvas*, const WebTextRun&, const WebFloatPoint& leftBaseline, WebColor,
        const WebRect& clip) const override;

    int calculateWidth(const WebTextRun&) const override;
    int offsetForPosition(const WebTextRun&, float position) const override;
    WebFloatRect selectionRectForText(const WebTextRun&, const WebFloatPoint& leftBaseline,
        int height, int from = 0, int to = -1) const override;

    DisplayItemClient displayItemClient() const { return toDisplayItemClient(this); }
    String debugName() const { return "WebFontImpl"; }

private:
    Font m_font;
};

} // namespace blink

#endif
