/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef CSSToStyleMap_h
#define CSSToStyleMap_h

#include "core/CSSPropertyNames.h"
#include "core/animation/Timing.h"
#include "core/animation/css/CSSTransitionData.h"
#include "core/style/ComputedStyleConstants.h"
#include "platform/animation/TimingFunction.h"

namespace blink {

class FillLayer;
class CSSValue;
class StyleResolverState;
class NinePieceImage;
class BorderImageLengthBox;

class CSSToStyleMap {
public:
    static void mapFillAttachment(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillClip(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillComposite(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillBlendMode(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillOrigin(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillImage(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillRepeatX(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillRepeatY(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillSize(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillXPosition(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillYPosition(StyleResolverState&, FillLayer*, CSSValue*);
    static void mapFillMaskSourceType(StyleResolverState&, FillLayer*, CSSValue*);

    static double mapAnimationDelay(CSSValue*);
    static Timing::PlaybackDirection mapAnimationDirection(CSSValue*);
    static double mapAnimationDuration(CSSValue*);
    static Timing::FillMode mapAnimationFillMode(CSSValue*);
    static double mapAnimationIterationCount(CSSValue*);
    static AtomicString mapAnimationName(CSSValue*);
    static EAnimPlayState mapAnimationPlayState(CSSValue*);
    static CSSTransitionData::TransitionProperty mapAnimationProperty(CSSValue*);
    static PassRefPtr<TimingFunction> mapAnimationTimingFunction(CSSValue*, bool allowStepMiddle = false);

    static void mapNinePieceImage(StyleResolverState&, CSSPropertyID, CSSValue*, NinePieceImage&);
    static void mapNinePieceImageSlice(StyleResolverState&, CSSValue*, NinePieceImage&);
    static BorderImageLengthBox mapNinePieceImageQuad(StyleResolverState&, CSSValue*);
    static void mapNinePieceImageRepeat(StyleResolverState&, CSSValue*, NinePieceImage&);
};

}

#endif
