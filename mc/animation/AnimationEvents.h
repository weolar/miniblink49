// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_AnimationEvents_h
#define mc_animation_AnimationEvents_h

#include <vector>

#include "mc/animation/AnimationObj.h"
//#include "mc/animation/FilterOperationsWrap.h"
#include "third_party/skia/include/utils/SkMatrix44.h"

namespace mc {

struct AnimationEvent {
    enum Type {
        STARTED, FINISHED, ABORTED, PROPERTY_UPDATE
    };

    AnimationEvent(
        Type typeParam,
        int layerIdParam,
        int groupIdParam,
        AnimationObj::TargetProperty targetPropertyParam,
        base::TimeTicks monotonicTimeParam
        )
        : type(typeParam)
        , layerId(layerIdParam)
        , groupId(groupIdParam)
        , targetProperty(targetPropertyParam)
        , monotonicTime(monotonicTimeParam)
        , isImplOnly(false)
        , opacity(0.0)
    {

    }

    Type type;
    int layerId;
    int groupId;
    AnimationObj::TargetProperty targetProperty;
    base::TimeTicks monotonicTime;
    bool isImplOnly;
    float opacity;
    SkMatrix44 transform;
    //FilterOperations filters;    
};

typedef std::vector<AnimationEvent> AnimationEventsVector;

}  // namespace mc

#endif  // mc_animation_AnimationEvents_h
