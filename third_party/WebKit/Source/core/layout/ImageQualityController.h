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

#ifndef ImageQualityController_h
#define ImageQualityController_h

#include "core/CoreExport.h"
#include "core/layout/LayoutObject.h"
#include "platform/geometry/IntSize.h"
#include "platform/geometry/LayoutSize.h"
#include "platform/graphics/Image.h"
#include "platform/graphics/ImageOrientation.h"
#include "platform/graphics/ImageSource.h"
#include "wtf/HashMap.h"

namespace blink {

typedef HashMap<const void*, LayoutSize> LayerSizeMap;
typedef HashMap<LayoutObject*, LayerSizeMap> ObjectLayerSizeMap;

class CORE_EXPORT ImageQualityController final {
    WTF_MAKE_NONCOPYABLE(ImageQualityController); WTF_MAKE_FAST_ALLOCATED(ImageQualityController);
public:
    ~ImageQualityController();

    static ImageQualityController* imageQualityController();

    static void remove(LayoutObject*);

    InterpolationQuality chooseInterpolationQuality(GraphicsContext*, LayoutObject*, Image*, const void* layer, const LayoutSize&);

private:
    ImageQualityController();

    static bool has(LayoutObject*);
    void set(LayoutObject*, LayerSizeMap* innerMap, const void* layer, const LayoutSize&);

    bool shouldPaintAtLowQuality(GraphicsContext*, LayoutObject*, Image*, const void* layer, const LayoutSize&);
    void removeLayer(LayoutObject*, LayerSizeMap* innerMap, const void* layer);
    void objectDestroyed(LayoutObject*);
    bool isEmpty() { return m_objectLayerSizeMap.isEmpty(); }

    void highQualityRepaintTimerFired(Timer<ImageQualityController>*);
    void restartTimer();

    // Only for use in testing.
    void setTimer(Timer<ImageQualityController>*);

    ObjectLayerSizeMap m_objectLayerSizeMap;
    OwnPtr<Timer<ImageQualityController>> m_timer;
    bool m_animatedResizeIsActive;
    bool m_liveResizeOptimizationIsActive;

    // For calling set().
    friend class LayoutPartTest_DestroyUpdatesImageQualityController_Test;

    // For calling setTimer(),
    friend class ImageQualityControllerTest_LowQualityFilterForLiveResize_Test;
    friend class ImageQualityControllerTest_LowQualityFilterForResizingImage_Test;
    friend class ImageQualityControllerTest_DontKickTheAnimationTimerWhenPaintingAtTheSameSize_Test;
};

} // namespace blink

#endif
