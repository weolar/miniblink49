// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ModulesExport_h
#define ModulesExport_h

#if !defined(LINK_CORE_MODULES_SEPARATELY)
#define LINK_CORE_MODULES_SEPARATELY 0
#endif

#if LINK_CORE_MODULES_SEPARATELY && defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(BLINK_MODULES_IMPLEMENTATION) && BLINK_MODULES_IMPLEMENTATION
#define MODULES_EXPORT __declspec(dllexport)
#else
#define MODULES_EXPORT __declspec(dllimport)
#endif // defined(BLINK_MODULES_IMPLEMENTATION) && BLINK_MODULES_IMPLEMENTATION

#else // defined(WIN32)
#if defined(BLINK_MODULES_IMPLEMENTATION) && BLINK_MODULES_IMPLEMENTATION
#define MODULES_EXPORT __attribute__((visibility("default")))
#else
#define MODULES_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define MODULES_EXPORT
#endif

#endif // ModulesExport_h
