// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/temp_window_x11.h"

#include <X11/Xlib.h>

#include "include/base/cef_logging.h"
#include "include/cef_app.h"

namespace client {

namespace {


// Create the temp window.
::Window CreateTempWindow() {
  ::Display* xdisplay = cef_get_xdisplay();
  ::Window parent_xwindow = DefaultRootWindow(xdisplay);
  
  XSetWindowAttributes swa;
  memset(&swa, 0, sizeof(swa));
  swa.background_pixmap = None;
  swa.override_redirect = false;
  return XCreateWindow(
      xdisplay, parent_xwindow,
      0, 0, 1, 1,      // size (1x1px)
      0,               // border width
      CopyFromParent,  // depth
      InputOutput,
      CopyFromParent,  // visual
      CWBackPixmap | CWOverrideRedirect,
      &swa);
}

// Close the temp window.
void CloseTempWindow(::Window xwindow) {
  ::Display* xdisplay = cef_get_xdisplay();
  XDestroyWindow(xdisplay, xwindow);
}

TempWindowX11* g_temp_window = NULL;

}  // namespace

TempWindowX11::TempWindowX11()
    : xwindow_(kNullWindowHandle) {
  DCHECK(!g_temp_window);
  g_temp_window = this;

  xwindow_ = CreateTempWindow();
  CHECK(xwindow_);
}

TempWindowX11::~TempWindowX11() {
  g_temp_window = NULL;
  DCHECK(xwindow_);
  
  CloseTempWindow(xwindow_);
}

// static
CefWindowHandle TempWindowX11::GetWindowHandle() {
  DCHECK(g_temp_window);
  return g_temp_window->xwindow_;
}

}  // namespace client
