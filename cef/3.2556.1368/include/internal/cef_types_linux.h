// Copyright (c) 2014 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef CEF_INCLUDE_INTERNAL_CEF_TYPES_LINUX_H_
#define CEF_INCLUDE_INTERNAL_CEF_TYPES_LINUX_H_
#pragma once

#include "include/base/cef_build.h"

#if defined(OS_LINUX)

typedef union _XEvent XEvent;
typedef struct _XDisplay XDisplay;

#include "include/internal/cef_export.h"
#include "include/internal/cef_string.h"

// Handle types.
#define cef_cursor_handle_t unsigned long
#define cef_event_handle_t XEvent*
#define cef_window_handle_t unsigned long

#define kNullCursorHandle 0
#define kNullEventHandle NULL
#define kNullWindowHandle 0

#ifdef __cplusplus
extern "C" {
#endif

///
// Return the singleton X11 display shared with Chromium. The display is not
// thread-safe and must only be accessed on the browser process UI thread.
///
CEF_EXPORT XDisplay* cef_get_xdisplay();
#define cef_text_input_context_t void*

///
// Structure representing CefExecuteProcess arguments.
///
typedef struct _cef_main_args_t {
  int argc;
  char** argv;
} cef_main_args_t;

///
// Class representing window information.
///
typedef struct _cef_window_info_t {
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;

  ///
  // Pointer for the parent window.
  ///
  cef_window_handle_t parent_window;

  ///
  // Set to true (1) to create the browser using windowless (off-screen)
  // rendering. No window will be created for the browser and all rendering will
  // occur via the CefRenderHandler interface. The |parent_window| value will be
  // used to identify monitor info and to act as the parent window for dialogs,
  // context menus, etc. If |parent_window| is not provided then the main screen
  // monitor will be used and some functionality that requires a parent window
  // may not function correctly. In order to create windowless browsers the
  // CefSettings.windowless_rendering_enabled value must be set to true.
  ///
  int windowless_rendering_enabled;

  ///
  // Set to true (1) to enable transparent painting in combination with
  // windowless rendering. When this value is true a transparent background
  // color will be used (RGBA=0x00000000). When this value is false the
  // background will be white and opaque.
  ///
  int transparent_painting_enabled;

  ///
  // Pointer for the new browser window. Only used with windowed rendering.
  ///
  cef_window_handle_t window;
} cef_window_info_t;

#ifdef __cplusplus
}
#endif

#endif  // OS_LINUX

#endif  // CEF_INCLUDE_INTERNAL_CEF_TYPES_LINUX_H_
