// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CoreExport_h
#define CoreExport_h

#if !defined(LINK_CORE_MODULES_SEPARATELY)
#define LINK_CORE_MODULES_SEPARATELY 0
#endif

#if LINK_CORE_MODULES_SEPARATELY && defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(BLINK_CORE_IMPLEMENTATION) && BLINK_CORE_IMPLEMENTATION
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT __declspec(dllimport)
#endif // defined(BLINK_CORE_IMPLEMENTATION) && BLINK_CORE_IMPLEMENTATION

#else // defined(WIN32)
#if defined(BLINK_CORE_IMPLEMENTATION) && BLINK_CORE_IMPLEMENTATION
#define CORE_EXPORT __attribute__((visibility("default")))
#else
#define CORE_EXPORT
#endif
#endif

#else // defined(COMPONENT_BUILD)
#define CORE_EXPORT
#endif

#if defined(BLINK_CORE_IMPLEMENTATION) && BLINK_CORE_IMPLEMENTATION
#if COMPILER(MSVC)
#define CORE_TEMPLATE_CLASS_EXPORT
#define CORE_EXTERN_TEMPLATE_EXPORT CORE_EXPORT
#define CORE_TEMPLATE_EXPORT CORE_EXPORT
#elif COMPILER(GCC)
#define CORE_TEMPLATE_CLASS_EXPORT CORE_EXPORT
#define CORE_EXTERN_TEMPLATE_EXPORT CORE_EXPORT
#define CORE_TEMPLATE_EXPORT
#else
#error Unknown compiler
#endif
#else // !BLINK_CORE_IMPLEMENTATION
#define CORE_TEMPLATE_CLASS_EXPORT
#define CORE_EXTERN_TEMPLATE_EXPORT CORE_EXPORT
#define CORE_TEMPLATE_EXPORT
#endif

#endif // CoreExport_h
