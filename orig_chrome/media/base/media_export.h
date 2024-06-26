// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_MEDIA_EXPORT_H_
#define MEDIA_BASE_MEDIA_EXPORT_H_

// Define MEDIA_EXPORT so that functionality implemented by the Media module
// can be exported to consumers.

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(MEDIA_IMPLEMENTATION)
#define MEDIA_EXPORT __declspec(dllexport)
#else
#define MEDIA_EXPORT __declspec(dllimport)
#endif // defined(MEDIA_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(MEDIA_IMPLEMENTATION)
#define MEDIA_EXPORT __attribute__((visibility("default")))
#else
#define MEDIA_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define MEDIA_EXPORT
#endif

#endif // MEDIA_BASE_MEDIA_EXPORT_H_
