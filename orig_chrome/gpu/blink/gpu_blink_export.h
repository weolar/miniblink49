// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_BLINK_GPU_BLINK_EXPORT_H_
#define GPU_BLINK_GPU_BLINK_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(GPU_BLINK_IMPLEMENTATION)
#define GPU_BLINK_EXPORT __declspec(dllexport)
#else
#define GPU_BLINK_EXPORT __declspec(dllimport)
#endif // defined(GPU_BLINK_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(GPU_BLINK_IMPLEMENTATION)
#define GPU_BLINK_EXPORT __attribute__((visibility("default")))
#else
#define GPU_BLINK_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define GPU_BLINK_EXPORT
#endif

#endif // GPU_BLINK_GPU_BLINK_EXPORT_H_
