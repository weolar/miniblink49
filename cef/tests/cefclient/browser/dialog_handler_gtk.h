// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_BROWSER_DIALOG_HANDLER_GTK_H_
#define CEF_TESTS_CEFCLIENT_BROWSER_DIALOG_HANDLER_GTK_H_
#pragma once

#include <gtk/gtk.h>

#include "include/cef_dialog_handler.h"
#include "include/cef_jsdialog_handler.h"

namespace client {

class ClientDialogHandlerGtk : public CefDialogHandler,
                               public CefJSDialogHandler {
 public:
  ClientDialogHandlerGtk();

  // CefDialogHandler methods.
  bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                    FileDialogMode mode,
                    const CefString& title,
                    const CefString& default_file_path,
                    const std::vector<CefString>& accept_filters,
                    int selected_accept_filter,
                    CefRefPtr<CefFileDialogCallback> callback) OVERRIDE;

  // CefJSDialogHandler methods.
  bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                  const CefString& origin_url,
                  const CefString& accept_lang,
                  JSDialogType dialog_type,
                  const CefString& message_text,
                  const CefString& default_prompt_text,
                  CefRefPtr<CefJSDialogCallback> callback,
                  bool& suppress_message) OVERRIDE;
  bool OnBeforeUnloadDialog(
      CefRefPtr<CefBrowser> browser,
      const CefString& message_text,
      bool is_reload,
      CefRefPtr<CefJSDialogCallback> callback) OVERRIDE;
  void OnResetDialogState(CefRefPtr<CefBrowser> browser) OVERRIDE;

 private:
  static void OnDialogResponse(GtkDialog *dialog,
                               gint response_id,
                               ClientDialogHandlerGtk* handler);

  GtkWidget* gtk_dialog_;
  CefRefPtr<CefJSDialogCallback> js_dialog_callback_;

  IMPLEMENT_REFCOUNTING(ClientDialogHandlerGtk);
  DISALLOW_COPY_AND_ASSIGN(ClientDialogHandlerGtk);
};

}  // namespace client

#endif  // CEF_TESTS_CEFCLIENT_BROWSER_DIALOG_HANDLER_GTK_H_
