// Copyright 2012 The Chromium Authors weolar. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_DRAW_PROPERTIES_H_
#define CC_LAYERS_DRAW_PROPERTIES_H_

#include "cc/base/bdcolor.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"
#include "third_party/skia/include/utils/SkMatrix44.h"

namespace cc {

// Container for properties that layers need to compute before they can be
// drawn.
struct DrawProperties {
    DrawProperties()
    {
        
    }

    // Transforms objects from content space to screen space (viewport space).
    SkMatrix44 screenSpaceTransform;
    SkMatrix44 targetSpaceTransform;
    SkMatrix44 currentTransform;
    blink::IntRect clip;
};

struct DrawToCanvasProperties : public DrawProperties {
    DrawToCanvasProperties()
    {
        maskLayerId = -2;
        replicaLayerId = -2;
        masksToBounds = true;
        drawsContent = true;
        opaque = true;
        opacity = 0;
        backgroundColor = s_kBgColor;
        useParentBackfaceVisibility = false;
        isDoubleSided = false;
    }

    void copyDrawProperties(const DrawProperties& other, float otherOpacity)
    {
        screenSpaceTransform = other.screenSpaceTransform;
        targetSpaceTransform = other.targetSpaceTransform;
        currentTransform = other.currentTransform;
        clip = other.clip;
        opacity = otherOpacity;
    }

    void copy(const DrawToCanvasProperties& other)
    {
        screenSpaceTransform = other.screenSpaceTransform;
        targetSpaceTransform = other.targetSpaceTransform;
        currentTransform = other.currentTransform;
        clip = other.clip;
        bounds = other.bounds;
        position = other.position;
        masksToBounds = other.masksToBounds;
        drawsContent = other.drawsContent;
        opaque = other.opaque;
        opacity = other.opacity;
        maskLayerId = other.maskLayerId;
        replicaLayerId = other.replicaLayerId;
        backgroundColor = other.backgroundColor;
        useParentBackfaceVisibility = other.useParentBackfaceVisibility;
        isDoubleSided = other.isDoubleSided;
    }

    blink::IntSize bounds;
    blink::FloatPoint position;
    bool masksToBounds;
    bool drawsContent;
    bool opaque;
    float opacity;
    int maskLayerId;
    int replicaLayerId;
    SkColor backgroundColor;
    bool useParentBackfaceVisibility;
    bool isDoubleSided;
};

}  // namespace cc

#endif  // CC_LAYERS_DRAW_PROPERTIES_H_
