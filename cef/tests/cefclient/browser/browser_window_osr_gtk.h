// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_BROWSER_BROWSER_WINDOW_OSR_GTK_H_
#define CEF_TESTS_CEFCLIENT_BROWSER_BROWSER_WINDOW_OSR_GTK_H_
#pragma once

#include "cefclient/browser/browser_window.h"
#include "cefclient/browser/client_handler_osr.h"
#include "cefclient/browser/osr_renderer.h"

namespace client {

// Represents a native child window hosting a single off-screen browser
// instance. The methods of this class must be called on the main thread unless
// otherwise indicated.
class BrowserWindowOsrGtk : public BrowserWindow,
                            public ClientHandlerOsr::OsrDelegate {
 public:
  // Constructor may be called on any thread.
  // |delegate| must outlive this object.
  BrowserWindowOsrGtk(BrowserWindow::Delegate* delegate,
                      const std::string& startup_url,
                      const OsrRenderer::Settings& settings);

  // BrowserWindow methods.
  void CreateBrowser(ClientWindowHandle parent_handle,
                     const CefRect& rect,
                     const CefBrowserSettings& settings,
                     CefRefPtr<CefRequestContext> request_context) OVERRIDE;
  void GetPopupConfig(CefWindowHandle temp_handle,
                      CefWindowInfo& windowInfo,
                      CefRefPtr<CefClient>& client,
                      CefBrowserSettings& settings) OVERRIDE;
  void ShowPopup(ClientWindowHandle parent_handle,
                 int x, int y, size_t width, size_t height) OVERRIDE;
  void Show() OVERRIDE;
  void Hide() OVERRIDE;
  void SetBounds(int x, int y, size_t width, size_t height) OVERRIDE;
  void SetFocus(bool focus) OVERRIDE;
  void SetDeviceScaleFactor(float device_scale_factor) OVERRIDE;
  float GetDeviceScaleFactor() const OVERRIDE;
  ClientWindowHandle GetWindowHandle() const OVERRIDE;

  // ClientHandlerOsr::OsrDelegate methods.
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  bool GetRootScreenRect(CefRefPtr<CefBrowser> browser,
                         CefRect& rect) OVERRIDE;
  bool GetViewRect(CefRefPtr<CefBrowser> browser,
                   CefRect& rect) OVERRIDE;
  bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
                      int viewX,
                      int viewY,
                      int& screenX,
                      int& screenY) OVERRIDE;
  bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
                     CefScreenInfo& screen_info) OVERRIDE;
  void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
  void OnPopupSize(CefRefPtr<CefBrowser> browser,
                   const CefRect& rect) OVERRIDE;
  void OnPaint(CefRefPtr<CefBrowser> browser,
               CefRenderHandler::PaintElementType type,
               const CefRenderHandler::RectList& dirtyRects,
               const void* buffer,
               int width,
               int height) OVERRIDE;
  void OnCursorChange(CefRefPtr<CefBrowser> browser,
                      CefCursorHandle cursor,
                      CefRenderHandler::CursorType type,
                      const CefCursorInfo& custom_cursor_info) OVERRIDE;
  bool StartDragging(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefDragData> drag_data,
                     CefRenderHandler::DragOperationsMask allowed_ops,
                     int x, int y) OVERRIDE;
  void UpdateDragCursor(CefRefPtr<CefBrowser> browser,
                        CefRenderHandler::DragOperation operation) OVERRIDE;

 private:
  // Create the GTK GlArea.
  void Create(ClientWindowHandle parent_handle);

  // Signal handlers for the GTK GlArea.
  static gint SizeAllocation(GtkWidget* widget,
                             GtkAllocation* allocation,
                             BrowserWindowOsrGtk* self);
  static gint ClickEvent(GtkWidget* widget,
                         GdkEventButton* event,
                         BrowserWindowOsrGtk* self);
  static gint KeyEvent(GtkWidget* widget,
                       GdkEventKey* event,
                       BrowserWindowOsrGtk* self);
  static gint MoveEvent(GtkWidget* widget,
                        GdkEventMotion* event,
                        BrowserWindowOsrGtk* self);
  static gint ScrollEvent(GtkWidget* widget,
                          GdkEventScroll* event,
                          BrowserWindowOsrGtk* self);
  static gint FocusEvent(GtkWidget* widget,
                         GdkEventFocus* event,
                         BrowserWindowOsrGtk* self);

  bool IsOverPopupWidget(int x, int y) const;
  int GetPopupXOffset() const;
  int GetPopupYOffset() const;
  void ApplyPopupOffset(int& x, int& y) const;

  void EnableGL();
  void DisableGL();

  // The below members will only be accessed on the main thread which should be
  // the same as the CEF UI thread.
  OsrRenderer renderer_;
  ClientWindowHandle glarea_;
  bool hidden_;
  bool gl_enabled_;
  bool painting_popup_;

  float device_scale_factor_;

  DISALLOW_COPY_AND_ASSIGN(BrowserWindowOsrGtk);
};

}  // namespace client

#endif  // CEF_TESTS_CEFCLIENT_BROWSER_BROWSER_WINDOW_OSR_GTK_H_
