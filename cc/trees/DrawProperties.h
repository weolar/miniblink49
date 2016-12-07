// Copyright 2012 The Chromium Authors weolar. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_DRAW_PROPERTIES_H_
#define CC_LAYERS_DRAW_PROPERTIES_H_

#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"
#include "third_party/skia/include/utils/SkMatrix44.h"

namespace cc {

// Container for properties that layers need to compute before they can be
// drawn.
struct DrawProperties {
    DrawProperties()
    {
        opacity = 0;
    }

    // Transforms objects from content space to screen space (viewport space).
    SkMatrix44 screenSpaceTransform;
    SkMatrix44 targetSpaceTransform;
    SkMatrix44 currentTransform;
	float opacity;
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
    }

    void copyDrawProperties(const DrawProperties& other)
    {
        screenSpaceTransform = other.screenSpaceTransform;
        targetSpaceTransform = other.targetSpaceTransform;
        currentTransform = other.currentTransform;
		clip = other.clip;
		opacity = other.opacity;
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
    }

    blink::IntSize bounds;
    blink::FloatPoint position;
	bool masksToBounds;
	bool drawsContent;
    bool opaque;
    int maskLayerId;
    int replicaLayerId;
};

}  // namespace cc

#endif  // CC_LAYERS_DRAW_PROPERTIES_H_
