// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVENTS_OZONE_EXPORT_H_
#define UI_EVENTS_OZONE_EVENTS_OZONE_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(EVENTS_OZONE_IMPLEMENTATION)
#define EVENTS_OZONE_EXPORT __declspec(dllexport)
#else
#define EVENTS_OZONE_EXPORT __declspec(dllimport)
#endif // defined(EVENTS_OZONE_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(EVENTS_OZONE_IMPLEMENTATION)
#define EVENTS_OZONE_EXPORT __attribute__((visibility("default")))
#else
#define EVENTS_OZONE_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define EVENTS_OZONE_EXPORT
#endif

#endif // UI_EVENTS_OZONE_EVENTS_OZONE_EXPORT_H_
