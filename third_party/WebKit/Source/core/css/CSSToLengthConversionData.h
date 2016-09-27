/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef CSSToLengthConversionData_h
#define CSSToLengthConversionData_h

#include "core/CoreExport.h"
#include "wtf/Assertions.h"
#include "wtf/MathExtras.h"
#include <limits>

namespace blink {

class ComputedStyle;
class LayoutView;
class Font;

class CORE_EXPORT CSSToLengthConversionData {
public:

    class FontSizes {
    public:
        FontSizes() : m_em(0), m_rem(0), m_font(nullptr) { }
        FontSizes(float em, float rem, const Font*);
        FontSizes(const ComputedStyle*, const ComputedStyle* rootStyle);

        float em() const { return m_em; }
        float rem() const { return m_rem; }
        float ex() const;
        float ch() const;
    private:
        float m_em;
        float m_rem;
        const Font* m_font;
    };

    class ViewportSize {
    public:
        ViewportSize() : m_width(0), m_height(0) { }
        ViewportSize(double width, double height) : m_width(width), m_height(height) { }
        explicit ViewportSize(const LayoutView*);

        double width() const { return m_width; }
        double height() const { return m_height; }
    private:
        double m_width;
        double m_height;
    };

    CSSToLengthConversionData() { }
    CSSToLengthConversionData(const ComputedStyle*, const FontSizes&, const ViewportSize&, float zoom);
    CSSToLengthConversionData(const ComputedStyle* currStyle, const ComputedStyle* rootStyle, const LayoutView*, float zoom);

    float zoom() const { return m_zoom; }

    float emFontSize() const { return m_fontSizes.em(); }
    float remFontSize() const { return m_fontSizes.rem(); }
    float exFontSize() const { return m_fontSizes.ex(); }
    float chFontSize() const { return m_fontSizes.ch(); }

    // Accessing these marks the style as having viewport units
    double viewportWidthPercent() const;
    double viewportHeightPercent() const;
    double viewportMinPercent() const;
    double viewportMaxPercent() const;

    void setFontSizes(const FontSizes& fontSizes) { m_fontSizes = fontSizes; }
    void setZoom(float zoom)
    {
        ASSERT(std::isfinite(zoom) && zoom > 0);
        m_zoom = zoom;
    }

    CSSToLengthConversionData copyWithAdjustedZoom(float newZoom) const
    {
        return CSSToLengthConversionData(m_style, m_fontSizes, m_viewportSize, newZoom);
    }

private:
    const ComputedStyle* m_style;
    FontSizes m_fontSizes;
    ViewportSize m_viewportSize;
    float m_zoom;
};

} // namespace blink

#endif
