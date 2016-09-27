// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/GraphicsContextState.h"

namespace blink {

GraphicsContextState::GraphicsContextState()
    : m_strokeColor(Color::black)
    , m_fillColor(Color::black)
    , m_textDrawingMode(TextModeFill)
    , m_interpolationQuality(InterpolationDefault)
    , m_saveCount(0)
    , m_shouldAntialias(true)
{
    m_strokePaint.setStyle(SkPaint::kStroke_Style);
    m_strokePaint.setStrokeWidth(SkFloatToScalar(m_strokeData.thickness()));
    m_strokePaint.setColor(m_strokeColor.rgb());
    m_strokePaint.setStrokeCap(SkPaint::kDefault_Cap);
    m_strokePaint.setStrokeJoin(SkPaint::kDefault_Join);
    m_strokePaint.setStrokeMiter(SkFloatToScalar(m_strokeData.miterLimit()));
    m_strokePaint.setFilterQuality(WebCoreInterpolationQualityToSkFilterQuality(m_interpolationQuality));
    m_strokePaint.setAntiAlias(m_shouldAntialias);
    m_fillPaint.setColor(m_fillColor.rgb());
    m_fillPaint.setFilterQuality(WebCoreInterpolationQualityToSkFilterQuality(m_interpolationQuality));
    m_fillPaint.setAntiAlias(m_shouldAntialias);
}

GraphicsContextState::GraphicsContextState(const GraphicsContextState& other)
    : m_strokePaint(other.m_strokePaint)
    , m_fillPaint(other.m_fillPaint)
    , m_strokeData(other.m_strokeData)
    , m_strokeColor(other.m_strokeColor)
    , m_strokeGradient(other.m_strokeGradient)
    , m_fillColor(other.m_fillColor)
    , m_fillGradient(other.m_fillGradient)
    , m_looper(other.m_looper)
    , m_textDrawingMode(other.m_textDrawingMode)
    , m_colorFilter(other.m_colorFilter)
    , m_interpolationQuality(other.m_interpolationQuality)
    , m_saveCount(0)
    , m_shouldAntialias(other.m_shouldAntialias) { }

void GraphicsContextState::copy(const GraphicsContextState& source)
{
    this->~GraphicsContextState();
    new (this) GraphicsContextState(source);
}

const SkPaint& GraphicsContextState::strokePaint(int strokedPathLength) const
{
    if (m_strokeGradient && m_strokeGradient->shaderChanged())
        m_strokePaint.setShader(m_strokeGradient->shader());
    m_strokeData.setupPaintDashPathEffect(&m_strokePaint, strokedPathLength);
    return m_strokePaint;
}

const SkPaint& GraphicsContextState::fillPaint() const
{
    if (m_fillGradient && m_fillGradient->shaderChanged())
        m_fillPaint.setShader(m_fillGradient->shader());
    return m_fillPaint;
}

void GraphicsContextState::setStrokeStyle(StrokeStyle style)
{
    m_strokeData.setStyle(style);
}

void GraphicsContextState::setStrokeThickness(float thickness)
{
    m_strokeData.setThickness(thickness);
    m_strokePaint.setStrokeWidth(SkFloatToScalar(thickness));
}

void GraphicsContextState::setStrokeColor(const Color& color)
{
    m_strokeGradient.clear();
    m_strokeColor = color;
    m_strokePaint.setColor(color.rgb());
    m_strokePaint.setShader(0);
}

void GraphicsContextState::setStrokeGradient(const PassRefPtr<Gradient> gradient, float alpha)
{
    m_strokeColor = Color::black;
    m_strokeGradient = gradient;
    m_strokePaint.setColor(scaleAlpha(SK_ColorBLACK, alpha));
    m_strokePaint.setShader(m_strokeGradient->shader());
}

void GraphicsContextState::setLineCap(LineCap cap)
{
    m_strokeData.setLineCap(cap);
    m_strokePaint.setStrokeCap((SkPaint::Cap)cap);
}

void GraphicsContextState::setLineJoin(LineJoin join)
{
    m_strokeData.setLineJoin(join);
    m_strokePaint.setStrokeJoin((SkPaint::Join)join);
}

void GraphicsContextState::setMiterLimit(float miterLimit)
{
    m_strokeData.setMiterLimit(miterLimit);
    m_strokePaint.setStrokeMiter(SkFloatToScalar(miterLimit));
}

void GraphicsContextState::setFillColor(const Color& color)
{
    m_fillColor = color;
    m_fillGradient.clear();
    m_fillPaint.setColor(color.rgb());
    m_fillPaint.setShader(0);
}

void GraphicsContextState::setFillGradient(const PassRefPtr<Gradient> gradient, float alpha)
{
    m_fillColor = Color::black;
    m_fillGradient = gradient;
    m_fillPaint.setColor(scaleAlpha(SK_ColorBLACK, alpha));
    m_fillPaint.setShader(m_fillGradient->shader());
}

// Shadow. (This will need tweaking if we use draw loopers for other things.)
void GraphicsContextState::setDrawLooper(PassRefPtr<SkDrawLooper> drawLooper)
{
    m_looper = drawLooper;
    m_strokePaint.setLooper(m_looper.get());
    m_fillPaint.setLooper(m_looper.get());
}

void GraphicsContextState::clearDrawLooper()
{
    m_looper.clear();
    m_strokePaint.setLooper(0);
    m_fillPaint.setLooper(0);
}

void GraphicsContextState::setLineDash(const DashArray& dashes, float dashOffset)
{
    m_strokeData.setLineDash(dashes, dashOffset);
}

void GraphicsContextState::setColorFilter(PassRefPtr<SkColorFilter> colorFilter)
{
    m_colorFilter = colorFilter;
    m_strokePaint.setColorFilter(m_colorFilter.get());
    m_fillPaint.setColorFilter(m_colorFilter.get());
}

void GraphicsContextState::setInterpolationQuality(InterpolationQuality quality)
{
    m_interpolationQuality = quality;
    m_strokePaint.setFilterQuality(WebCoreInterpolationQualityToSkFilterQuality(quality));
    m_fillPaint.setFilterQuality(WebCoreInterpolationQualityToSkFilterQuality(quality));
}

void GraphicsContextState::setShouldAntialias(bool shouldAntialias)
{
    m_shouldAntialias = shouldAntialias;
    m_strokePaint.setAntiAlias(shouldAntialias);
    m_fillPaint.setAntiAlias(shouldAntialias);
}

} // namespace blink
