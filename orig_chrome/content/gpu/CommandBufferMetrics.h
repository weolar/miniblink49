// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_CLIENT_COMMAND_BUFFER_METRICS_H_
#define CONTENT_COMMON_GPU_CLIENT_COMMAND_BUFFER_METRICS_H_

#include <string>

#include "gpu/command_buffer/common/constants.h"

namespace content {

enum CommandBufferContextType {
    BROWSER_COMPOSITOR_ONSCREEN_CONTEXT,
    BROWSER_OFFSCREEN_MAINTHREAD_CONTEXT,
    BROWSER_WORKER_CONTEXT,
    RENDER_COMPOSITOR_CONTEXT,
    RENDER_WORKER_CONTEXT,
    RENDERER_MAINTHREAD_CONTEXT,
    GPU_VIDEO_ACCELERATOR_CONTEXT,
    OFFSCREEN_VIDEO_CAPTURE_CONTEXT,
    OFFSCREEN_CONTEXT_FOR_WEBGL,
    CONTEXT_TYPE_UNKNOWN,
    OFFSCREEN_CONTEXT_FOR_TESTING = CONTEXT_TYPE_UNKNOWN,
};

std::string CommandBufferContextTypeToString(CommandBufferContextType type);

// void UmaRecordContextInitFailed(CommandBufferContextType type);
//
// void UmaRecordContextLost(CommandBufferContextType type,
//                           gpu::error::Error error,
//                           gpu::error::ContextLostReason reason);

} // namespace content

#endif // CONTENT_COMMON_GPU_CLIENT_COMMAND_BUFFER_METRICS_H_
