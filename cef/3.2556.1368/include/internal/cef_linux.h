// Copyright (c) 2010 Marshall A. Greenblatt. All rights reserved.
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


#ifndef CEF_INCLUDE_INTERNAL_CEF_LINUX_H_
#define CEF_INCLUDE_INTERNAL_CEF_LINUX_H_
#pragma once

#include "include/internal/cef_types_linux.h"
#include "include/internal/cef_types_wrappers.h"

// Handle types.
#define CefCursorHandle cef_cursor_handle_t
#define CefEventHandle cef_event_handle_t
#define CefWindowHandle cef_window_handle_t
#define CefTextInputContext cef_text_input_context_t

struct CefMainArgsTraits {
  typedef cef_main_args_t struct_type;

  static inline void init(struct_type* s) {}
  static inline void clear(struct_type* s) {}

  static inline void set(const struct_type* src, struct_type* target,
      bool copy) {
    target->argc = src->argc;
    target->argv = src->argv;
  }
};

// Class representing CefExecuteProcess arguments.
class CefMainArgs : public CefStructBase<CefMainArgsTraits> {
 public:
  typedef CefStructBase<CefMainArgsTraits> parent;

  CefMainArgs() : parent() {}
  explicit CefMainArgs(const cef_main_args_t& r) : parent(r) {}
  explicit CefMainArgs(const CefMainArgs& r) : parent(r) {}
  CefMainArgs(int argc_arg, char** argv_arg) : parent() {
    argc = argc_arg;
    argv = argv_arg;
  }
};

struct CefWindowInfoTraits {
  typedef cef_window_info_t struct_type;

  static inline void init(struct_type* s) {}
  static inline void clear(struct_type* s) {}

  static inline void set(const struct_type* src, struct_type* target,
      bool copy) {
    target->x = src->x;
    target->y = src->y;
    target->width = src->width;
    target->height = src->height;
    target->parent_window = src->parent_window;
    target->windowless_rendering_enabled = src->windowless_rendering_enabled;
    target->transparent_painting_enabled = src->transparent_painting_enabled;
    target->window = src->window;
  }
};

// Class representing window information.
class CefWindowInfo : public CefStructBase<CefWindowInfoTraits> {
 public:
  typedef CefStructBase<CefWindowInfoTraits> parent;

  CefWindowInfo() : parent() {}
  explicit CefWindowInfo(const cef_window_info_t& r) : parent(r) {}
  explicit CefWindowInfo(const CefWindowInfo& r) : parent(r) {}

  ///
  // Create the browser as a child window.
  ///
  void SetAsChild(CefWindowHandle parent,
                  const CefRect& windowRect) {
    parent_window = parent;
    x = windowRect.x;
    y = windowRect.y;
    width = windowRect.width;
    height = windowRect.height;
  }

  ///
  // Create the browser using windowless (off-screen) rendering. No window
  // will be created for the browser and all rendering will occur via the
  // CefRenderHandler interface. The |parent| value will be used to identify
  // monitor info and to act as the parent window for dialogs, context menus,
  // etc. If |parent| is not provided then the main screen monitor will be used
  // and some functionality that requires a parent window may not function
  // correctly. If |transparent| is true a transparent background color will be
  // used (RGBA=0x00000000). If |transparent| is false the background will be
  // white and opaque. In order to create windowless browsers the
  // CefSettings.windowless_rendering_enabled value must be set to true.
  ///
  void SetAsWindowless(CefWindowHandle parent, bool transparent) {
    windowless_rendering_enabled = true;
    parent_window = parent;
    transparent_painting_enabled = transparent;
  }
};

#endif  // CEF_INCLUDE_INTERNAL_CEF_LINUX_H_
