// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_MEDIA_BLINK_EXPORT_H_
#define MEDIA_BLINK_MEDIA_BLINK_EXPORT_H_

// Define MEDIA_BLINK_EXPORT so that functionality implemented by the
// media/blink module can be exported to consumers.

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(MEDIA_BLINK_IMPLEMENTATION)
#define MEDIA_BLINK_EXPORT __declspec(dllexport)
#else
#define MEDIA_BLINK_EXPORT __declspec(dllimport)
#endif // defined(MEDIA_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(MEDIA_BLINK_IMPLEMENTATION)
#define MEDIA_BLINK_EXPORT __attribute__((visibility("default")))
#else
#define MEDIA_BLINK_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define MEDIA_BLINK_EXPORT
#endif

#endif // MEDIA_BLINK_MEDIA_BLINK_EXPORT_H_
