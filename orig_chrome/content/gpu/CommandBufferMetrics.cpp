// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/gpu/CommandBufferMetrics.h"

#include "base/metrics/histogram.h"
#include <windows.h>

namespace content {

namespace {

    enum CommandBufferContextLostReason {
        // Don't add new values here.
        CONTEXT_INIT_FAILED,
        CONTEXT_LOST_GPU_CHANNEL_ERROR,
        CONTEXT_PARSE_ERROR_INVALID_SIZE,
        CONTEXT_PARSE_ERROR_OUT_OF_BOUNDS,
        CONTEXT_PARSE_ERROR_UNKNOWN_COMMAND,
        CONTEXT_PARSE_ERROR_INVALID_ARGS,
        CONTEXT_PARSE_ERROR_GENERIC_ERROR,
        CONTEXT_LOST_GUILTY,
        CONTEXT_LOST_INNOCENT,
        CONTEXT_LOST_UNKNOWN,
        CONTEXT_LOST_OUT_OF_MEMORY,
        CONTEXT_LOST_MAKECURRENT_FAILED,
        // Add new values here and update _MAX_ENUM.
        CONTEXT_LOST_REASON_MAX_ENUM = CONTEXT_LOST_MAKECURRENT_FAILED
    };

    CommandBufferContextLostReason GetContextLostReason(
        gpu::error::Error error,
        gpu::error::ContextLostReason reason)
    {
        if (error == gpu::error::kLostContext) {
            switch (reason) {
            case gpu::error::kGuilty:
                return CONTEXT_LOST_GUILTY;
            case gpu::error::kInnocent:
                return CONTEXT_LOST_INNOCENT;
            case gpu::error::kUnknown:
                return CONTEXT_LOST_UNKNOWN;
            case gpu::error::kOutOfMemory:
                return CONTEXT_LOST_OUT_OF_MEMORY;
            case gpu::error::kMakeCurrentFailed:
                return CONTEXT_LOST_MAKECURRENT_FAILED;
            case gpu::error::kGpuChannelLost:
                return CONTEXT_LOST_GPU_CHANNEL_ERROR;
            }
        }
        switch (error) {
        case gpu::error::kInvalidSize:
            return CONTEXT_PARSE_ERROR_INVALID_SIZE;
        case gpu::error::kOutOfBounds:
            return CONTEXT_PARSE_ERROR_OUT_OF_BOUNDS;
        case gpu::error::kUnknownCommand:
            return CONTEXT_PARSE_ERROR_UNKNOWN_COMMAND;
        case gpu::error::kInvalidArguments:
            return CONTEXT_PARSE_ERROR_INVALID_ARGS;
        case gpu::error::kGenericError:
            return CONTEXT_PARSE_ERROR_GENERIC_ERROR;
        case gpu::error::kDeferCommandUntilLater:
        case gpu::error::kNoError:
        case gpu::error::kLostContext:
            DebugBreak();
            return CONTEXT_LOST_UNKNOWN;
        }
        DebugBreak();
        return CONTEXT_LOST_UNKNOWN;
    }

    void RecordContextLost(CommandBufferContextType type,
        CommandBufferContextLostReason reason)
    {
        switch (type) {
        case BROWSER_COMPOSITOR_ONSCREEN_CONTEXT:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.BrowserCompositor", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case BROWSER_OFFSCREEN_MAINTHREAD_CONTEXT:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.BrowserMainThread", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case BROWSER_WORKER_CONTEXT:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.BrowserWorker", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case RENDER_COMPOSITOR_CONTEXT:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.RenderCompositor", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case RENDER_WORKER_CONTEXT:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.RenderWorker", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case RENDERER_MAINTHREAD_CONTEXT:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.RenderMainThread", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case GPU_VIDEO_ACCELERATOR_CONTEXT:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.VideoAccelerator", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case OFFSCREEN_VIDEO_CAPTURE_CONTEXT:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.VideoCapture", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case OFFSCREEN_CONTEXT_FOR_WEBGL:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.WebGL", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        case CONTEXT_TYPE_UNKNOWN:
            UMA_HISTOGRAM_ENUMERATION("GPU.ContextLost.Unknown", reason,
                CONTEXT_LOST_REASON_MAX_ENUM);
            break;
        }
    }

} // anonymous namespace

std::string CommandBufferContextTypeToString(CommandBufferContextType type)
{
    switch (type) {
    case OFFSCREEN_CONTEXT_FOR_TESTING:
        return "Context-For-Testing";
    case BROWSER_COMPOSITOR_ONSCREEN_CONTEXT:
        return "Compositor";
    case BROWSER_OFFSCREEN_MAINTHREAD_CONTEXT:
        return "Offscreen-MainThread";
    case BROWSER_WORKER_CONTEXT:
        return "CompositorWorker";
    case RENDER_COMPOSITOR_CONTEXT:
        return "RenderCompositor";
    case RENDER_WORKER_CONTEXT:
        return "RenderWorker";
    case RENDERER_MAINTHREAD_CONTEXT:
        return "Offscreen-MainThread";
    case GPU_VIDEO_ACCELERATOR_CONTEXT:
        return "GPU-VideoAccelerator-Offscreen";
    case OFFSCREEN_VIDEO_CAPTURE_CONTEXT:
        return "Offscreen-CaptureThread";
    case OFFSCREEN_CONTEXT_FOR_WEBGL:
        return "Offscreen-For-WebGL";
    default:
        DebugBreak();
        return "unknown";
    }
}

// void UmaRecordContextInitFailed(CommandBufferContextType type) {
//   RecordContextLost(type, CONTEXT_INIT_FAILED);
// }
//
// void UmaRecordContextLost(CommandBufferContextType type,
//                           gpu::error::Error error,
//                           gpu::error::ContextLostReason reason) {
//   CommandBufferContextLostReason converted_reason =
//       GetContextLostReason(error, reason);
//   RecordContextLost(type, converted_reason);
// }

} // namespace content
