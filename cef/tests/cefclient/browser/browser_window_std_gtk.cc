// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/browser_window_std_gtk.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <X11/Xlib.h>
#undef Success     // Definition conflicts with cef_message_router.h
#undef RootWindow  // Definition conflicts with root_window.h

#include "include/base/cef_logging.h"
#include "cefclient/browser/client_handler_std.h"
#include "cefclient/browser/main_message_loop.h"

namespace client {

namespace {

::Window GetXWindowForWidget(GtkWidget* widget) {
  // The GTK window must be visible before we can retrieve the XID.
  ::Window xwindow = GDK_WINDOW_XID(gtk_widget_get_window(widget));
  DCHECK(xwindow);
  return xwindow;
}

void SetXWindowVisible(::Window xwindow, bool visible) {
  ::Display* xdisplay = cef_get_xdisplay();

  // Retrieve the atoms required by the below XChangeProperty call.
  const char* kAtoms[] = {
    "_NET_WM_STATE",
    "ATOM",
    "_NET_WM_STATE_HIDDEN"
  };
  Atom atoms[3];
  int result = XInternAtoms(xdisplay, const_cast<char**>(kAtoms), 3, false,
                            atoms);
  if (!result)
    NOTREACHED();

  if (!visible) {
    // Set the hidden property state value.
    scoped_ptr<Atom[]> data(new Atom[1]);
    data[0] = atoms[2];

    XChangeProperty(xdisplay,
                    xwindow,
                    atoms[0],  // name
                    atoms[1],  // type
                    32,  // size in bits of items in 'value'
                    PropModeReplace,
                    reinterpret_cast<const unsigned char*>(data.get()),
                    1);  // num items
  } else {
    // Set an empty array of property state values.
    XChangeProperty(xdisplay,
                    xwindow,
                    atoms[0],  // name
                    atoms[1],  // type
                    32,  // size in bits of items in 'value'
                    PropModeReplace,
                    NULL,
                    0);  // num items
  }
}

void SetXWindowBounds(::Window xwindow,
                      int x, int y, size_t width, size_t height) {
  ::Display* xdisplay = cef_get_xdisplay();
  XWindowChanges changes = {0};
  changes.x = x;
  changes.y = y;
  changes.width = static_cast<int>(width);
  changes.height = static_cast<int>(height);
  XConfigureWindow(xdisplay, xwindow,
                   CWX | CWY | CWHeight | CWWidth, &changes);
}

}  // namespace

BrowserWindowStdGtk::BrowserWindowStdGtk(Delegate* delegate,
                                         const std::string& startup_url)
    : BrowserWindow(delegate) {
  client_handler_ = new ClientHandlerStd(this, startup_url);
}

void BrowserWindowStdGtk::CreateBrowser(
    ClientWindowHandle parent_handle,
    const CefRect& rect,
    const CefBrowserSettings& settings,
    CefRefPtr<CefRequestContext> request_context) {
  REQUIRE_MAIN_THREAD();

  CefWindowInfo window_info;
  window_info.SetAsChild(GetXWindowForWidget(parent_handle), rect);

  CefBrowserHost::CreateBrowser(window_info, client_handler_,
                                client_handler_->startup_url(),
                                settings, request_context);
}

void BrowserWindowStdGtk::GetPopupConfig(CefWindowHandle temp_handle,
                                         CefWindowInfo& windowInfo,
                                         CefRefPtr<CefClient>& client,
                                         CefBrowserSettings& settings) {
  // Note: This method may be called on any thread.
  // The window will be properly sized after the browser is created.
  windowInfo.SetAsChild(temp_handle, CefRect());
  client = client_handler_;
}

void BrowserWindowStdGtk::ShowPopup(ClientWindowHandle parent_handle,
                                    int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();

  if (browser_) {
    ::Window parent_xwindow = GetXWindowForWidget(parent_handle);
    ::Display* xdisplay = cef_get_xdisplay();
    ::Window xwindow = browser_->GetHost()->GetWindowHandle();
    DCHECK(xwindow);

    XReparentWindow(xdisplay, xwindow, parent_xwindow, x, y);
    
    SetXWindowBounds(xwindow, x, y, width, height);
    SetXWindowVisible(xwindow, true);
  }
}

void BrowserWindowStdGtk::Show() {
  REQUIRE_MAIN_THREAD();

  if (browser_) {
    ::Window xwindow = browser_->GetHost()->GetWindowHandle();
    DCHECK(xwindow);
    SetXWindowVisible(xwindow, true);
  }
}

void BrowserWindowStdGtk::Hide() {
  REQUIRE_MAIN_THREAD();

  if (browser_) {
    ::Window xwindow = browser_->GetHost()->GetWindowHandle();
    DCHECK(xwindow);
    SetXWindowVisible(xwindow, false);
  }
}

void BrowserWindowStdGtk::SetBounds(int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();

  if (browser_) {
    ::Window xwindow = browser_->GetHost()->GetWindowHandle();
    DCHECK(xwindow);
    SetXWindowBounds(xwindow, x, y, width, height);
  }
}

void BrowserWindowStdGtk::SetFocus(bool focus) {
  REQUIRE_MAIN_THREAD();

  if (browser_)
    browser_->GetHost()->SetFocus(focus);
}

ClientWindowHandle BrowserWindowStdGtk::GetWindowHandle() const {
  REQUIRE_MAIN_THREAD();

  // There is no GtkWidget* representation of this object.
  NOTREACHED();
  return NULL;
}


}  // namespace client
