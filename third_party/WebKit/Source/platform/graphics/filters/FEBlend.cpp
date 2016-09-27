/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "platform/graphics/filters/FEBlend.h"

#include "SkXfermodeImageFilter.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/graphics/skia/SkiaUtils.h"
#include "platform/text/TextStream.h"

namespace blink {

FEBlend::FEBlend(Filter* filter, WebBlendMode mode)
    : FilterEffect(filter)
    , m_mode(mode)
{
}

PassRefPtrWillBeRawPtr<FEBlend> FEBlend::create(Filter* filter, WebBlendMode mode)
{
    return adoptRefWillBeNoop(new FEBlend(filter, mode));
}

WebBlendMode FEBlend::blendMode() const
{
    return m_mode;
}

bool FEBlend::setBlendMode(WebBlendMode mode)
{
    if (m_mode == mode)
        return false;
    m_mode = mode;
    return true;
}

PassRefPtr<SkImageFilter> FEBlend::createImageFilter(SkiaImageFilterBuilder* builder)
{
    RefPtr<SkImageFilter> foreground(builder->build(inputEffect(0), operatingColorSpace()));
    RefPtr<SkImageFilter> background(builder->build(inputEffect(1), operatingColorSpace()));
    RefPtr<SkXfermode> mode(adoptRef(SkXfermode::Create(WebCoreCompositeToSkiaComposite(CompositeSourceOver, m_mode))));
    SkImageFilter::CropRect cropRect = getCropRect(builder->cropOffset());
    return adoptRef(SkXfermodeImageFilter::Create(mode.get(), background.get(), foreground.get(), &cropRect));
}

TextStream& FEBlend::externalRepresentation(TextStream& ts, int indent) const
{
    writeIndent(ts, indent);
    ts << "[feBlend";
    FilterEffect::externalRepresentation(ts);
    ts << " mode=\"" << (m_mode == WebBlendModeNormal ? "normal" : compositeOperatorName(CompositeSourceOver, m_mode)) << "\"]\n";
    inputEffect(0)->externalRepresentation(ts, indent + 1);
    inputEffect(1)->externalRepresentation(ts, indent + 1);
    return ts;
}

} // namespace blink
