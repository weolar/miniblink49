// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/root_window_gtk.h"

#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <X11/Xlib.h>
#undef Success     // Definition conflicts with cef_message_router.h
#undef RootWindow  // Definition conflicts with root_window.h

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "cefclient/browser/browser_window_osr_gtk.h"
#include "cefclient/browser/browser_window_std_gtk.h"
#include "cefclient/browser/main_context.h"
#include "cefclient/browser/main_message_loop.h"
#include "cefclient/browser/resource.h"
#include "cefclient/browser/temp_window.h"
#include "cefclient/browser/window_test.h"
#include "cefclient/common/client_switches.h"

namespace client {

namespace {

const char kMenuIdKey[] = "menu_id";

bool IsWindowMaximized(GtkWindow* window) {
  GdkWindow* gdk_window = gtk_widget_get_window(GTK_WIDGET(window));
  gint state = gdk_window_get_state(gdk_window);
  return (state & GDK_WINDOW_STATE_MAXIMIZED) ? true : false;
}

void MinimizeWindow(GtkWindow* window) {
  // Unmaximize the window before minimizing so restore behaves correctly.
  if (IsWindowMaximized(window))
    gtk_window_unmaximize(window);

  gtk_window_iconify(window);
}

void MaximizeWindow(GtkWindow* window) {
  gtk_window_maximize(window);
}

}  // namespace

RootWindowGtk::RootWindowGtk()
    : delegate_(NULL),
      with_controls_(false),
      with_osr_(false),
      is_popup_(false),
      initialized_(false),
      window_(NULL),
      back_button_(NULL),
      forward_button_(NULL),
      reload_button_(NULL),
      stop_button_(NULL),
      url_entry_(NULL),
      toolbar_height_(0),
      menubar_height_(0),
      force_close_(false),
      window_destroyed_(false),
      browser_destroyed_(false) {
}

RootWindowGtk::~RootWindowGtk() {
  REQUIRE_MAIN_THREAD();

  // The window and browser should already have been destroyed.
  DCHECK(window_destroyed_);
  DCHECK(browser_destroyed_);
}

void RootWindowGtk::Init(RootWindow::Delegate* delegate,
                         bool with_controls,
                         bool with_osr,
                         const CefRect& bounds,
                         const CefBrowserSettings& settings,
                         const std::string& url) {
  DCHECK(delegate);
  DCHECK(!initialized_);

  delegate_ = delegate;
  with_controls_ = with_controls;
  with_osr_ = with_osr;
  start_rect_ = bounds;

  CreateBrowserWindow(url);

  initialized_ = true;

  // Create the native root window on the main thread.
  if (CURRENTLY_ON_MAIN_THREAD()) {
    CreateRootWindow(settings);
  } else {
    MAIN_POST_CLOSURE(
        base::Bind(&RootWindowGtk::CreateRootWindow, this, settings));
  }
}

void RootWindowGtk::InitAsPopup(RootWindow::Delegate* delegate,
                                bool with_controls,
                                bool with_osr,
                                const CefPopupFeatures& popupFeatures,
                                CefWindowInfo& windowInfo,
                                CefRefPtr<CefClient>& client,
                                CefBrowserSettings& settings) {
  DCHECK(delegate);
  DCHECK(!initialized_);

  delegate_ = delegate;
  with_controls_ = with_controls;
  with_osr_ = with_osr;
  is_popup_ = true;

  if (popupFeatures.xSet)
    start_rect_.x = popupFeatures.x;
  if (popupFeatures.ySet)
    start_rect_.y = popupFeatures.y;
  if (popupFeatures.widthSet)
    start_rect_.width = popupFeatures.width;
  if (popupFeatures.heightSet)
    start_rect_.height = popupFeatures.height;

  CreateBrowserWindow(std::string());

  initialized_ = true;

  // The new popup is initially parented to a temporary window. The native root
  // window will be created after the browser is created and the popup window
  // will be re-parented to it at that time.
  browser_window_->GetPopupConfig(TempWindow::GetWindowHandle(),
                                  windowInfo, client, settings);
}

void RootWindowGtk::Show(ShowMode mode) {
  REQUIRE_MAIN_THREAD();

  if (!window_)
    return;

  // Show the GTK window.
  gtk_widget_show_all(window_);

  if (mode == ShowMinimized)
    MinimizeWindow(GTK_WINDOW(window_));
  else if (mode == ShowMaximized)
    MaximizeWindow(GTK_WINDOW(window_));
}

void RootWindowGtk::Hide() {
  REQUIRE_MAIN_THREAD();

  if (window_)
    gtk_widget_hide(window_);
}

void RootWindowGtk::SetBounds(int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();

  if (!window_)
    return;

  GtkWindow* window = GTK_WINDOW(window_);
  GdkWindow* gdk_window = gtk_widget_get_window(window_);

  // Make sure the window isn't minimized or maximized.
  if (IsWindowMaximized(window))
    gtk_window_unmaximize(window);
  else
    gtk_window_present(window);

  // Retrieve information about the display that contains the window.
  GdkScreen* screen = gdk_screen_get_default();
  const gint monitor = gdk_screen_get_monitor_at_window(screen, gdk_window);
  GdkRectangle rect;
  gdk_screen_get_monitor_geometry(screen, monitor, &rect);

  gdk_window_move_resize(gdk_window, rect.x, rect.y, rect.width, rect.height);
}

void RootWindowGtk::Close(bool force) {
  REQUIRE_MAIN_THREAD();

  if (window_) {
    force_close_ = force;
    gtk_widget_destroy(window_);
  }
}

void RootWindowGtk::SetDeviceScaleFactor(float device_scale_factor) {
  REQUIRE_MAIN_THREAD();

  if (browser_window_)
    browser_window_->SetDeviceScaleFactor(device_scale_factor);
}

float RootWindowGtk::GetDeviceScaleFactor() const {
  REQUIRE_MAIN_THREAD();

  if (browser_window_)
    return browser_window_->GetDeviceScaleFactor();
  return 1.0f;
}

CefRefPtr<CefBrowser> RootWindowGtk::GetBrowser() const {
  REQUIRE_MAIN_THREAD();

  if (browser_window_)
    return browser_window_->GetBrowser();
  return NULL;
}

ClientWindowHandle RootWindowGtk::GetWindowHandle() const {
  REQUIRE_MAIN_THREAD();
  return window_;
}

void RootWindowGtk::CreateBrowserWindow(const std::string& startup_url) {
  if (with_osr_) {
    OsrRenderer::Settings settings;
    MainContext::Get()->PopulateOsrSettings(&settings);
    browser_window_.reset(new BrowserWindowOsrGtk(this, startup_url, settings));
  } else {
    browser_window_.reset(new BrowserWindowStdGtk(this, startup_url));
  }
}

void RootWindowGtk::CreateRootWindow(const CefBrowserSettings& settings) {
  REQUIRE_MAIN_THREAD();
  DCHECK(!window_);

  // TODO(port): If no x,y position is specified the window will always appear
  // in the upper-left corner. Maybe there's a better default place to put it?
  int x = start_rect_.x;
  int y = start_rect_.y;
  int width, height;
  if (start_rect_.IsEmpty()) {
    // TODO(port): Also, maybe there's a better way to choose the default size.
    width = 800;
    height = 600;
  } else {
    width = start_rect_.width;
    height = start_rect_.height;
  }

  window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window_), width, height);
  g_signal_connect(G_OBJECT(window_), "focus-in-event",
                   G_CALLBACK(&RootWindowGtk::WindowFocusIn), this);
  g_signal_connect(G_OBJECT(window_), "window-state-event", 
                   G_CALLBACK(&RootWindowGtk::WindowState), this);
  g_signal_connect(G_OBJECT(window_), "configure-event",
                   G_CALLBACK(&RootWindowGtk::WindowConfigure), this);
  g_signal_connect(G_OBJECT(window_), "destroy",
                   G_CALLBACK(&RootWindowGtk::WindowDestroy), this);
  g_signal_connect(G_OBJECT(window_), "delete_event",
                   G_CALLBACK(&RootWindowGtk::WindowDelete), this);

  const cef_color_t background_color = MainContext::Get()->GetBackgroundColor();
  GdkColor color = {0};
  color.red = CefColorGetR(background_color) * 65535 / 255;
  color.green = CefColorGetG(background_color) * 65535 / 255;
  color.blue = CefColorGetB(background_color) * 65535 / 255;
  gtk_widget_modify_bg(window_, GTK_STATE_NORMAL, &color);

  GtkWidget* vbox = gtk_vbox_new(FALSE, 0);
  g_signal_connect(vbox, "size-allocate",
                   G_CALLBACK(&RootWindowGtk::VboxSizeAllocated), this);
  gtk_container_add(GTK_CONTAINER(window_), vbox);

  if (with_controls_) {
    GtkWidget* menu_bar = CreateMenuBar();
    g_signal_connect(menu_bar, "size-allocate",
                     G_CALLBACK(&RootWindowGtk::MenubarSizeAllocated), this);

    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    GtkWidget* toolbar = gtk_toolbar_new();
    // Turn off the labels on the toolbar buttons.
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    g_signal_connect(toolbar, "size-allocate",
                     G_CALLBACK(&RootWindowGtk::ToolbarSizeAllocated), this);

    back_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
    g_signal_connect(back_button_, "clicked",
                     G_CALLBACK(&RootWindowGtk::BackButtonClicked), this);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), back_button_, -1 /* append */);

    forward_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
    g_signal_connect(forward_button_, "clicked",
                     G_CALLBACK(&RootWindowGtk::ForwardButtonClicked), this);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), forward_button_, -1 /* append */);

    reload_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
    g_signal_connect(reload_button_, "clicked",
                     G_CALLBACK(&RootWindowGtk::ReloadButtonClicked), this);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), reload_button_, -1 /* append */);

    stop_button_ = gtk_tool_button_new_from_stock(GTK_STOCK_STOP);
    g_signal_connect(stop_button_, "clicked",
                     G_CALLBACK(&RootWindowGtk::StopButtonClicked), this);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), stop_button_, -1 /* append */);

    url_entry_ = gtk_entry_new();
    g_signal_connect(url_entry_, "activate",
                     G_CALLBACK(&RootWindowGtk::URLEntryActivate), this);
    g_signal_connect(url_entry_, "button-press-event",
                     G_CALLBACK(&RootWindowGtk::URLEntryButtonPress), this);

    GtkToolItem* tool_item = gtk_tool_item_new();
    gtk_container_add(GTK_CONTAINER(tool_item), url_entry_);
    gtk_tool_item_set_expand(tool_item, TRUE);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item, -1);  // append

    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
  }

  // Realize (show) the GTK widget. This must be done before the browser is
  // created because the underlying X11 Window is required. |browser_bounds_|
  // will be set at this point based on the GTK *SizeAllocated signal callbacks.
  Show(ShowNormal);

  // Most window managers ignore requests for initial window positions (instead
  // using a user-defined placement algorithm) and honor requests after the
  // window has already been shown.
  gtk_window_move(GTK_WINDOW(window_), x, y);

  // Windowed browsers are parented to the X11 Window underlying the GtkWindow*
  // and must be sized manually. The OSR GTK widget, on the other hand, can be
  // added to the Vbox container for automatic layout-based sizing.
  GtkWidget* parent = with_osr_ ? vbox : window_;

  if (!is_popup_) {
    // Create the browser window.
    browser_window_->CreateBrowser(parent, browser_bounds_, settings,
                                   delegate_->GetRequestContext(this));
  } else {
    // With popups we already have a browser window. Parent the browser window
    // to the root window and show it in the correct location.
    browser_window_->ShowPopup(parent, browser_bounds_.x, browser_bounds_.y,
                               browser_bounds_.width, browser_bounds_.height);
  }
}

void RootWindowGtk::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_MAIN_THREAD();

  // For popup browsers create the root window once the browser has been
  // created.
  if (is_popup_)
    CreateRootWindow(CefBrowserSettings());
}

void RootWindowGtk::OnBrowserWindowDestroyed() {
  REQUIRE_MAIN_THREAD();

  browser_window_.reset();

  if (!window_destroyed_) {
    // The browser was destroyed first. This could be due to the use of
    // off-screen rendering or execution of JavaScript window.close().
    // Close the RootWindow.
    Close(true);
  }

  browser_destroyed_ = true;
  NotifyDestroyedIfDone();
}

void RootWindowGtk::OnSetAddress(const std::string& url) {
  REQUIRE_MAIN_THREAD();

  if (url_entry_) {
    std::string urlStr(url);
    gtk_entry_set_text(GTK_ENTRY(url_entry_), urlStr.c_str());
  }
}

void RootWindowGtk::OnSetTitle(const std::string& title) {
  REQUIRE_MAIN_THREAD();

  if (window_) {
    std::string titleStr(title);
    gtk_window_set_title(GTK_WINDOW(window_), titleStr.c_str());
  }
}

void RootWindowGtk::OnSetFullscreen(bool fullscreen) {
  REQUIRE_MAIN_THREAD();

  CefRefPtr<CefBrowser> browser = GetBrowser();
  if (browser) {
    if (fullscreen)
      window_test::Maximize(browser);
    else
      window_test::Restore(browser);
  }
}

void RootWindowGtk::OnSetLoadingState(bool isLoading,
                                      bool canGoBack,
                                      bool canGoForward) {
  REQUIRE_MAIN_THREAD();

  if (with_controls_) {
    gtk_widget_set_sensitive(GTK_WIDGET(stop_button_), isLoading);
    gtk_widget_set_sensitive(GTK_WIDGET(reload_button_), !isLoading);
    gtk_widget_set_sensitive(GTK_WIDGET(back_button_), canGoBack);
    gtk_widget_set_sensitive(GTK_WIDGET(forward_button_), canGoForward);
  }
}

void RootWindowGtk::OnSetDraggableRegions(
    const std::vector<CefDraggableRegion>& regions) {
  REQUIRE_MAIN_THREAD();
  // TODO(cef): Implement support for draggable regions on this platform.
}

void RootWindowGtk::NotifyDestroyedIfDone() {
  // Notify once both the window and the browser have been destroyed.
  if (window_destroyed_ && browser_destroyed_)
    delegate_->OnRootWindowDestroyed(this);
}

// static
gboolean RootWindowGtk::WindowFocusIn(GtkWidget* widget,
                                      GdkEventFocus* event,
                                      RootWindowGtk* self) {
  if (event->in && self->browser_window_.get()) {
    self->browser_window_->SetFocus(true);
    // Return true for a windowed browser so that focus is not passed to GTK.
    return self->with_osr_ ? FALSE : TRUE;
  }

  return FALSE;
}

// static
gboolean RootWindowGtk::WindowState(GtkWidget* widget,
                                    GdkEventWindowState* event,
                                    RootWindowGtk* self) {
  // Called when the root window is iconified or restored. Hide the browser
  // window when the root window is iconified to reduce resource usage.
  if ((event->changed_mask & GDK_WINDOW_STATE_ICONIFIED) &&
      self->browser_window_.get()) {
    if (event->new_window_state & GDK_WINDOW_STATE_ICONIFIED)
      self->browser_window_->Hide();
    else
      self->browser_window_->Show();
  }

  return TRUE;
}

// static
gboolean RootWindowGtk::WindowConfigure(GtkWindow* window, 
                                        GdkEvent* event,
                                        RootWindowGtk* self) {
  // Called when size, position or stack order changes.
  CefRefPtr<CefBrowser> browser = self->GetBrowser();
  if (browser.get()) {
    // Notify the browser of move/resize events so that:
    // - Popup windows are displayed in the correct location and dismissed
    //   when the window moves.
    // - Drag&drop areas are updated accordingly.
    browser->GetHost()->NotifyMoveOrResizeStarted();
  }

  return FALSE;  // Don't stop this message.
}

// static
void RootWindowGtk::WindowDestroy(GtkWidget* widget, RootWindowGtk* self) {
  // Called when the root window is destroyed.
  self->window_destroyed_ = true;
  self->NotifyDestroyedIfDone();
}

// static
gboolean RootWindowGtk::WindowDelete(GtkWidget* widget,
                                     GdkEvent* event,
                                     RootWindowGtk* self) {
  // Called to query whether the root window should be closed.
  if (self->force_close_)
    return FALSE;  // Allow the close.

  if (self->browser_window_.get() && !self->browser_window_->IsClosing()) {
    CefRefPtr<CefBrowser> browser = self->GetBrowser();
    if (browser) {
      // Notify the browser window that we would like to close it. This
      // will result in a call to ClientHandler::DoClose() if the
      // JavaScript 'onbeforeunload' event handler allows it.
      browser->GetHost()->CloseBrowser(false);

      // Cancel the close.
      return TRUE;
    }
  }

  // Allow the close.
  return FALSE;
}

// static
void RootWindowGtk::VboxSizeAllocated(GtkWidget* widget,
                                      GtkAllocation* allocation,
                                      RootWindowGtk* self) {
  // Offset browser positioning by any controls that will appear in the client
  // area.
  const int ux_height = self->toolbar_height_ + self->menubar_height_;
  const int x = allocation->x;
  const int y = allocation->y + ux_height;
  const int width = allocation->width;
  const int height = allocation->height - ux_height;

  // Size the browser window to match the GTK widget.
  self->browser_bounds_ = CefRect(x, y, width, height);
  if (self->browser_window_.get())
    self->browser_window_->SetBounds(x, y, width, height);
}

// static
void RootWindowGtk::MenubarSizeAllocated(GtkWidget* widget,
                                         GtkAllocation* allocation,
                                         RootWindowGtk* self) {
  self->menubar_height_ = allocation->height;
}

// static
gboolean RootWindowGtk::MenuItemActivated(GtkWidget* widget,
                                          RootWindowGtk* self) {
  // Retrieve the menu ID set in AddMenuEntry.
  int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), kMenuIdKey));
  // Run the test.
  if (self->delegate_)
    self->delegate_->OnTest(self, id);

  return FALSE;  // Don't stop this message.
}

// static
void RootWindowGtk::ToolbarSizeAllocated(GtkWidget* widget,
                                         GtkAllocation* allocation,
                                         RootWindowGtk* self) {
  self->toolbar_height_ = allocation->height;
}

// static
void RootWindowGtk::BackButtonClicked(GtkButton* button,
                                      RootWindowGtk* self) {
  CefRefPtr<CefBrowser> browser = self->GetBrowser();
  if (browser.get())
    browser->GoBack();
}

// static
void RootWindowGtk::ForwardButtonClicked(GtkButton* button,
                                         RootWindowGtk* self) {
  CefRefPtr<CefBrowser> browser = self->GetBrowser();
  if (browser.get())
    browser->GoForward();
}

// static
void RootWindowGtk::StopButtonClicked(GtkButton* button,
                                      RootWindowGtk* self) {
  CefRefPtr<CefBrowser> browser = self->GetBrowser();
  if (browser.get())
    browser->StopLoad();
}

// static
void RootWindowGtk::ReloadButtonClicked(GtkButton* button,
                                        RootWindowGtk* self) {
  CefRefPtr<CefBrowser> browser = self->GetBrowser();
  if (browser.get())
    browser->Reload();
}

// static
void RootWindowGtk::URLEntryActivate(GtkEntry* entry,
                                     RootWindowGtk* self) {
  CefRefPtr<CefBrowser> browser = self->GetBrowser();
  if (browser.get()) {
    const gchar* url = gtk_entry_get_text(entry);
    browser->GetMainFrame()->LoadURL(std::string(url).c_str());
  }
}

// static
gboolean RootWindowGtk::URLEntryButtonPress(GtkWidget* widget,
                                            GdkEventButton* event,
                                            RootWindowGtk* self) {
  // Give focus to the GTK window. This is a work-around for bad focus-related
  // interaction between the root window managed by GTK and the browser managed
  // by X11.
  GtkWidget* window = gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW);
  GdkWindow* gdk_window = gtk_widget_get_window(window);
  ::Display* xdisplay = GDK_WINDOW_XDISPLAY(gdk_window);
  ::Window xwindow = GDK_WINDOW_XID(gdk_window);

  // Retrieve the atoms required by the below XSendEvent call.
  const char* kAtoms[] = {
    "WM_PROTOCOLS",
    "WM_TAKE_FOCUS"
  };
  Atom atoms[2];
  int result = XInternAtoms(xdisplay, const_cast<char**>(kAtoms), 2, false,
                            atoms);
  if (!result)
    NOTREACHED();

  XEvent e;
  e.type = ClientMessage;
  e.xany.display = xdisplay;
  e.xany.window = xwindow;
  e.xclient.format = 32;
  e.xclient.message_type = atoms[0];
  e.xclient.data.l[0] = atoms[1];
  e.xclient.data.l[1] = CurrentTime;
  e.xclient.data.l[2] = 0;
  e.xclient.data.l[3] = 0;
  e.xclient.data.l[4] = 0;

  XSendEvent(xdisplay, xwindow, false, 0, &e);

  return FALSE;
}

GtkWidget* RootWindowGtk::CreateMenuBar() {
  GtkWidget* menu_bar = gtk_menu_bar_new();

  // Create the test menu.
  GtkWidget* test_menu = CreateMenu(menu_bar, "Tests");
  AddMenuEntry(test_menu, "Get Source",    ID_TESTS_GETSOURCE);
  AddMenuEntry(test_menu, "Get Text",      ID_TESTS_GETTEXT);
  AddMenuEntry(test_menu, "New Window",    ID_TESTS_WINDOW_NEW);
  AddMenuEntry(test_menu, "Popup Window",  ID_TESTS_WINDOW_POPUP);
  AddMenuEntry(test_menu, "Request",       ID_TESTS_REQUEST);
  AddMenuEntry(test_menu, "Plugin Info",   ID_TESTS_PLUGIN_INFO);
  AddMenuEntry(test_menu, "Zoom In",       ID_TESTS_ZOOM_IN);
  AddMenuEntry(test_menu, "Zoom Out",      ID_TESTS_ZOOM_OUT);
  AddMenuEntry(test_menu, "Zoom Reset",    ID_TESTS_ZOOM_RESET);
  if (with_osr_) {
    AddMenuEntry(test_menu, "Set FPS",          ID_TESTS_OSR_FPS);
    AddMenuEntry(test_menu, "Set Scale Factor", ID_TESTS_OSR_DSF);
  }
  AddMenuEntry(test_menu, "Begin Tracing", ID_TESTS_TRACING_BEGIN);
  AddMenuEntry(test_menu, "End Tracing",   ID_TESTS_TRACING_END);
  AddMenuEntry(test_menu, "Print",         ID_TESTS_PRINT);
  AddMenuEntry(test_menu, "Print to PDF",  ID_TESTS_PRINT_TO_PDF);
  AddMenuEntry(test_menu, "Other Tests",   ID_TESTS_OTHER_TESTS);

  return menu_bar;
}

GtkWidget* RootWindowGtk::CreateMenu(GtkWidget* menu_bar, const char* text) {
  GtkWidget* menu_widget = gtk_menu_new();
  GtkWidget* menu_header = gtk_menu_item_new_with_label(text);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_header), menu_widget);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_header);
  return menu_widget;
}

GtkWidget* RootWindowGtk::AddMenuEntry(GtkWidget* menu_widget,
                                       const char* text,
                                       int id) {
  GtkWidget* entry = gtk_menu_item_new_with_label(text);
  g_signal_connect(entry, "activate",
                   G_CALLBACK(&RootWindowGtk::MenuItemActivated), this);

  // Set the menu ID that will be retrieved in MenuItemActivated.
  g_object_set_data(G_OBJECT(entry), kMenuIdKey, GINT_TO_POINTER(id));

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), entry);
  return entry;
}

// static
scoped_refptr<RootWindow> RootWindow::Create() {
  return new RootWindowGtk();
}

}  // namespace client
