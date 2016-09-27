// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_BROWSER_ROOT_WINDOW_GTK_H_
#define CEF_TESTS_CEFCLIENT_BROWSER_ROOT_WINDOW_GTK_H_
#pragma once

#include <gtk/gtk.h>
#include <string>

#include "include/base/cef_scoped_ptr.h"
#include "cefclient/browser/browser_window.h"
#include "cefclient/browser/root_window.h"

namespace client {

// GTK implementation of a top-level native window in the browser process.
// The methods of this class must be called on the main thread unless otherwise
// indicated.
class RootWindowGtk : public RootWindow,
                      public BrowserWindow::Delegate {
 public:
  // Constructor may be called on any thread.
  RootWindowGtk();
  ~RootWindowGtk();

  // RootWindow methods.
  void Init(RootWindow::Delegate* delegate,
            bool with_controls,
            bool with_osr,
            const CefRect& rect,
            const CefBrowserSettings& settings,
            const std::string& url) OVERRIDE;
  void InitAsPopup(RootWindow::Delegate* delegate,
                   bool with_controls,
                   bool with_osr,
                   const CefPopupFeatures& popupFeatures,
                   CefWindowInfo& windowInfo,
                   CefRefPtr<CefClient>& client,
                   CefBrowserSettings& settings) OVERRIDE;
  void Show(ShowMode mode) OVERRIDE;
  void Hide() OVERRIDE;
  void SetBounds(int x, int y, size_t width, size_t height) OVERRIDE;
  void Close(bool force) OVERRIDE;
  void SetDeviceScaleFactor(float device_scale_factor) OVERRIDE;
  float GetDeviceScaleFactor() const OVERRIDE;
  CefRefPtr<CefBrowser> GetBrowser() const OVERRIDE;
  ClientWindowHandle GetWindowHandle() const OVERRIDE;

 private:
  void CreateBrowserWindow(const std::string& startup_url);
  void CreateRootWindow(const CefBrowserSettings& settings);

  // BrowserWindow::Delegate methods.
  void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  void OnBrowserWindowDestroyed() OVERRIDE;
  void OnSetAddress(const std::string& url) OVERRIDE;
  void OnSetTitle(const std::string& title) OVERRIDE;
  void OnSetFullscreen(bool fullscreen) OVERRIDE;
  void OnSetLoadingState(bool isLoading,
                         bool canGoBack,
                         bool canGoForward) OVERRIDE;
  void OnSetDraggableRegions(
      const std::vector<CefDraggableRegion>& regions) OVERRIDE;

  void NotifyDestroyedIfDone();

  GtkWidget* CreateMenuBar();
  GtkWidget* CreateMenu(GtkWidget* menu_bar, const char* text);
  GtkWidget* AddMenuEntry(GtkWidget* menu_widget, const char* text, int id);

  // Signal handlers for the top-level GTK window.
  static gboolean WindowFocusIn(GtkWidget* widget,
                                GdkEventFocus* event,
                                RootWindowGtk* self);
  static gboolean WindowState(GtkWidget* widget,
                              GdkEventWindowState* event,
                              RootWindowGtk* self);
  static gboolean WindowConfigure(GtkWindow* window, 
                                  GdkEvent* event,
                                  RootWindowGtk* self);
  static void WindowDestroy(GtkWidget* widget, RootWindowGtk* self);
  static gboolean WindowDelete(GtkWidget* widget,
                               GdkEvent* event,
                               RootWindowGtk* self);

  // Signal handlers for the GTK Vbox containing all UX elements.
  static void VboxSizeAllocated(GtkWidget* widget,
                                GtkAllocation* allocation,
                                RootWindowGtk* self);

  // Signal handlers for the GTK menu bar.
  static void MenubarSizeAllocated(GtkWidget* widget,
                                   GtkAllocation* allocation,
                                   RootWindowGtk* self);
  static gboolean MenuItemActivated(GtkWidget* widget,
                                    RootWindowGtk* self);

  // Signal handlers for the GTK toolbar.
  static void ToolbarSizeAllocated(GtkWidget* widget,
                                   GtkAllocation* allocation,
                                   RootWindowGtk* self);
  static void BackButtonClicked(GtkButton* button,
                                RootWindowGtk* self);
  static void ForwardButtonClicked(GtkButton* button,
                                   RootWindowGtk* self);
  static void StopButtonClicked(GtkButton* button,
                                RootWindowGtk* self);
  static void ReloadButtonClicked(GtkButton* button,
                                  RootWindowGtk* self);

  // Signal handlers for the GTK URL entry field.
  static void URLEntryActivate(GtkEntry* entry,
                               RootWindowGtk* self);
  static gboolean URLEntryButtonPress(GtkWidget* widget,
                                      GdkEventButton* event,
                                      RootWindowGtk* self);

  // After initialization all members are only accessed on the main thread.
  // Members set during initialization.
  RootWindow::Delegate* delegate_;
  bool with_controls_;
  bool with_osr_;
  bool is_popup_;
  CefRect start_rect_;
  scoped_ptr<BrowserWindow> browser_window_;
  bool initialized_;

  // Main window.
  GtkWidget* window_;

  // Buttons.
  GtkToolItem* back_button_;
  GtkToolItem* forward_button_;
  GtkToolItem* reload_button_;
  GtkToolItem* stop_button_;

  // URL text field.
  GtkWidget* url_entry_;

  // Height of UX controls that affect browser window placement.
  int toolbar_height_;
  int menubar_height_;

  CefRect browser_bounds_;

  bool force_close_;
  bool window_destroyed_;
  bool browser_destroyed_;

  DISALLOW_COPY_AND_ASSIGN(RootWindowGtk);
};

}  // namespace client

#endif  // CEF_TESTS_CEFCLIENT_BROWSER_ROOT_WINDOW_GTK_H_
