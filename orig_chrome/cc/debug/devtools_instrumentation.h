// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_DEVTOOLS_INSTRUMENTATION_H_
#define CC_DEBUG_DEVTOOLS_INSTRUMENTATION_H_

#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"

namespace cc {
namespace devtools_instrumentation {

    namespace internal {
        const char kCategory[] = TRACE_DISABLED_BY_DEFAULT("devtools.timeline");
        const char kCategoryFrame[] = TRACE_DISABLED_BY_DEFAULT("devtools.timeline.frame");
        const char kData[] = "data";
        const char kFrameId[] = "frameId";
        const char kLayerId[] = "layerId";
        const char kLayerTreeId[] = "layerTreeId";
        const char kPixelRefId[] = "pixelRefId";

        const char kImageDecodeTask[] = "ImageDecodeTask";
        const char kBeginFrame[] = "BeginFrame";
        const char kNeedsBeginFrameChanged[] = "NeedsBeginFrameChanged";
        const char kActivateLayerTree[] = "ActivateLayerTree";
        const char kRequestMainThreadFrame[] = "RequestMainThreadFrame";
        const char kBeginMainThreadFrame[] = "BeginMainThreadFrame";
        const char kDrawFrame[] = "DrawFrame";
        const char kCompositeLayers[] = "CompositeLayers";
    } // namespace internal

    const char kPaintSetup[] = "PaintSetup";
    const char kUpdateLayer[] = "UpdateLayer";

    class ScopedLayerTask {
    public:
        ScopedLayerTask(const char* event_name, int layer_id)
            : event_name_(event_name)
        {
            TRACE_EVENT_BEGIN1(internal::kCategory, event_name_,
                internal::kLayerId, layer_id);
        }
        ~ScopedLayerTask()
        {
            TRACE_EVENT_END0(internal::kCategory, event_name_);
        }

    private:
        const char* event_name_;

        DISALLOW_COPY_AND_ASSIGN(ScopedLayerTask);
    };

    class ScopedImageDecodeTask {
    public:
        explicit ScopedImageDecodeTask(const void* imagePtr)
        {
            TRACE_EVENT_BEGIN1(internal::kCategory, internal::kImageDecodeTask,
                internal::kPixelRefId,
                reinterpret_cast<uint64>(imagePtr));
        }
        ~ScopedImageDecodeTask()
        {
            TRACE_EVENT_END0(internal::kCategory, internal::kImageDecodeTask);
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(ScopedImageDecodeTask);
    };

    class ScopedLayerTreeTask {
    public:
        ScopedLayerTreeTask(const char* event_name,
            int layer_id,
            int layer_tree_host_id)
            : event_name_(event_name)
        {
            TRACE_EVENT_BEGIN2(internal::kCategory, event_name_,
                internal::kLayerId, layer_id,
                internal::kLayerTreeId, layer_tree_host_id);
        }
        ~ScopedLayerTreeTask()
        {
            TRACE_EVENT_END0(internal::kCategory, event_name_);
        }

    private:
        const char* event_name_;

        DISALLOW_COPY_AND_ASSIGN(ScopedLayerTreeTask);
    };

    struct ScopedCommitTrace {
    public:
        explicit ScopedCommitTrace(int layer_tree_host_id)
        {
            TRACE_EVENT_BEGIN1(internal::kCategory, internal::kCompositeLayers,
                internal::kLayerTreeId, layer_tree_host_id);
        }
        ~ScopedCommitTrace()
        {
            TRACE_EVENT_END0(internal::kCategory, internal::kCompositeLayers);
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(ScopedCommitTrace);
    };

    struct ScopedLayerObjectTracker
        : public base::trace_event::TraceScopedTrackableObject<int> {
        explicit ScopedLayerObjectTracker(int layer_id)
            : base::trace_event::TraceScopedTrackableObject<int>(
                internal::kCategory,
                internal::kLayerId,
                layer_id)
        {
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(ScopedLayerObjectTracker);
    };

    inline void DidActivateLayerTree(int layer_tree_host_id, int frame_id)
    {
        TRACE_EVENT_INSTANT2(internal::kCategoryFrame,
            internal::kActivateLayerTree,
            TRACE_EVENT_SCOPE_THREAD,
            internal::kLayerTreeId,
            layer_tree_host_id,
            internal::kFrameId,
            frame_id);
    }

    inline void DidBeginFrame(int layer_tree_host_id)
    {
        TRACE_EVENT_INSTANT1(internal::kCategoryFrame,
            internal::kBeginFrame,
            TRACE_EVENT_SCOPE_THREAD,
            internal::kLayerTreeId,
            layer_tree_host_id);
    }

    inline void DidDrawFrame(int layer_tree_host_id)
    {
        TRACE_EVENT_INSTANT1(internal::kCategoryFrame,
            internal::kDrawFrame,
            TRACE_EVENT_SCOPE_THREAD,
            internal::kLayerTreeId,
            layer_tree_host_id);
    }

    inline void DidRequestMainThreadFrame(int layer_tree_host_id)
    {
        TRACE_EVENT_INSTANT1(internal::kCategoryFrame,
            internal::kRequestMainThreadFrame,
            TRACE_EVENT_SCOPE_THREAD,
            internal::kLayerTreeId,
            layer_tree_host_id);
    }

    // inline scoped_refptr<base::trace_event::ConvertableToTraceFormat>
    // BeginMainThreadFrameData(int frame_id) {
    //   scoped_refptr<base::trace_event::TracedValue> value =
    //       new base::trace_event::TracedValue();
    //   value->SetInteger("frameId", frame_id);
    //   return value;
    // }

    inline void WillBeginMainThreadFrame(int layer_tree_host_id, int frame_id)
    {
        TRACE_EVENT_INSTANT2(
            internal::kCategoryFrame, internal::kBeginMainThreadFrame,
            TRACE_EVENT_SCOPE_THREAD, internal::kLayerTreeId, layer_tree_host_id,
            internal::kData, BeginMainThreadFrameData(frame_id));
    }

    // inline scoped_refptr<base::trace_event::ConvertableToTraceFormat>
    // NeedsBeginFrameData(bool needs_begin_frame) {
    //   scoped_refptr<base::trace_event::TracedValue> value =
    //       new base::trace_event::TracedValue();
    //   value->SetInteger("needsBeginFrame", needs_begin_frame);
    //   return value;
    // }

    inline void NeedsBeginFrameChanged(int layer_tree_host_id, bool new_value)
    {
        TRACE_EVENT_INSTANT2(
            internal::kCategoryFrame, internal::kNeedsBeginFrameChanged,
            TRACE_EVENT_SCOPE_THREAD, internal::kLayerTreeId, layer_tree_host_id,
            internal::kData, NeedsBeginFrameData(new_value));
    }

} // namespace devtools_instrumentation
} // namespace cc

#endif // CC_DEBUG_DEVTOOLS_INSTRUMENTATION_H_
