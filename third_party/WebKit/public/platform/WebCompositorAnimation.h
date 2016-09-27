// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebCompositorAnimation_h
#define WebCompositorAnimation_h

namespace blink {

// A compositor driven animation.
class WebCompositorAnimation {
public:
    enum TargetProperty {
        TargetPropertyTransform = 0,
        TargetPropertyOpacity,
        TargetPropertyFilter,
        TargetPropertyScrollOffset
    };

    enum Direction {
        DirectionNormal = 0,
        DirectionReverse,
        DirectionAlternate,
        DirectionAlternateReverse
    };

    enum FillMode {
        FillModeNone = 0,
        FillModeForwards,
        FillModeBackwards,
        FillModeBoth
    };

    virtual ~WebCompositorAnimation() { }

    // An id must be unique.
    virtual int id() = 0;
    virtual int group() = 0;

    virtual TargetProperty targetProperty() const = 0;

    // This is the number of times that the animation will play. If this
    // value is zero the animation will not play. If it is negative, then
    // the animation will loop indefinitely.
    virtual double iterations() const = 0;
    virtual void setIterations(double) = 0;

    virtual double startTime() const = 0;
    virtual void setStartTime(double monotonicTime) = 0;

    virtual double timeOffset() const = 0;
    virtual void setTimeOffset(double monotonicTime) = 0;

    virtual Direction direction() const = 0;
    virtual void setDirection(Direction) = 0;

    virtual double playbackRate() const = 0;
    virtual void setPlaybackRate(double) = 0;

    virtual FillMode fillMode() const = 0;
    virtual void setFillMode(FillMode)  = 0;

    virtual double iterationStart() const = 0;
    virtual void setIterationStart(double) = 0;
};

} // namespace blink

#endif // WebCompositorAnimation_h
