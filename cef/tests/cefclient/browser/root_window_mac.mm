// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/root_window_mac.h"

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_application_mac.h"
#include "cefclient/browser/browser_window_osr_mac.h"
#include "cefclient/browser/browser_window_std_mac.h"
#include "cefclient/browser/main_context.h"
#include "cefclient/browser/main_message_loop.h"
#include "cefclient/browser/temp_window.h"
#include "cefclient/browser/window_test.h"
#include "cefclient/common/client_switches.h"

// Receives notifications from controls and the browser window. Will delete
// itself when done.
@interface RootWindowDelegate : NSObject <NSWindowDelegate> {
 @private
  NSWindow* window_;
  client::RootWindowMac* root_window_;
  bool force_close_;
}

@property (nonatomic, readonly) client::RootWindowMac* root_window;
@property (nonatomic, readwrite) bool force_close;

- (id)initWithWindow:(NSWindow*)window
       andRootWindow:(client::RootWindowMac*)root_window;
- (IBAction)goBack:(id)sender;
- (IBAction)goForward:(id)sender;
- (IBAction)reload:(id)sender;
- (IBAction)stopLoading:(id)sender;
- (IBAction)takeURLStringValueFrom:(NSTextField *)sender;
@end

@implementation RootWindowDelegate

@synthesize root_window = root_window_;
@synthesize force_close = force_close_;

- (id)initWithWindow:(NSWindow*)window
       andRootWindow:(client::RootWindowMac*)root_window {
  if (self = [super init]) {
    window_ = window;
    [window_ setDelegate:self];
    root_window_ = root_window;
    force_close_ = false;

    // Register for application hide/unhide notifications.
    [[NSNotificationCenter defaultCenter]
         addObserver:self
            selector:@selector(applicationDidHide:)
                name:NSApplicationDidHideNotification
              object:nil];
    [[NSNotificationCenter defaultCenter]
         addObserver:self
            selector:@selector(applicationDidUnhide:)
                name:NSApplicationDidUnhideNotification
              object:nil];
  }
  return self;
}

- (void)dealloc {
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  [super dealloc];
}

- (IBAction)goBack:(id)sender {
  CefRefPtr<CefBrowser> browser = root_window_->GetBrowser();
  if (browser.get())
    browser->GoBack();
}

- (IBAction)goForward:(id)sender {
  CefRefPtr<CefBrowser> browser = root_window_->GetBrowser();
  if (browser.get())
    browser->GoForward();
}

- (IBAction)reload:(id)sender {
  CefRefPtr<CefBrowser> browser = root_window_->GetBrowser();
  if (browser.get())
    browser->Reload();
}

- (IBAction)stopLoading:(id)sender {
  CefRefPtr<CefBrowser> browser = root_window_->GetBrowser();
  if (browser.get())
    browser->StopLoad();
}

- (IBAction)takeURLStringValueFrom:(NSTextField *)sender {
  CefRefPtr<CefBrowser> browser = root_window_->GetBrowser();
  if (!browser.get())
    return;

  NSString *url = [sender stringValue];

  // if it doesn't already have a prefix, add http. If we can't parse it,
  // just don't bother rather than making things worse.
  NSURL* tempUrl = [NSURL URLWithString:url];
  if (tempUrl && ![tempUrl scheme])
    url = [@"http://" stringByAppendingString:url];

  std::string urlStr = [url UTF8String];
  browser->GetMainFrame()->LoadURL(urlStr);
}

// Called when we are activated (when we gain focus).
- (void)windowDidBecomeKey:(NSNotification*)notification {
  client::BrowserWindow* browser_window = root_window_->browser_window();
  if (browser_window)
    browser_window->SetFocus(true);
}

// Called when we are deactivated (when we lose focus).
- (void)windowDidResignKey:(NSNotification*)notification {
  client::BrowserWindow* browser_window = root_window_->browser_window();
  if (browser_window)
    browser_window->SetFocus(false);
}

// Called when we have been minimized.
- (void)windowDidMiniaturize:(NSNotification *)notification {
  client::BrowserWindow* browser_window = root_window_->browser_window();
  if (browser_window)
    browser_window->Hide();
}

// Called when we have been unminimized.
- (void)windowDidDeminiaturize:(NSNotification *)notification {
  client::BrowserWindow* browser_window = root_window_->browser_window();
  if (browser_window)
    browser_window->Show();
}

// Called when the application has been hidden.
- (void)applicationDidHide:(NSNotification *)notification {
  // If the window is miniaturized then nothing has really changed.
  if (![window_ isMiniaturized]) {
    client::BrowserWindow* browser_window = root_window_->browser_window();
    if (browser_window)
      browser_window->Hide();
  }
}

// Called when the application has been unhidden.
- (void)applicationDidUnhide:(NSNotification *)notification {
  // If the window is miniaturized then nothing has really changed.
  if (![window_ isMiniaturized]) {
    client::BrowserWindow* browser_window = root_window_->browser_window();
    if (browser_window)
      browser_window->Show();
  }
}

// Called when the window is about to close. Perform the self-destruction
// sequence by getting rid of the window. By returning YES, we allow the window
// to be removed from the screen.
- (BOOL)windowShouldClose:(id)window {
  if (!force_close_) {
    client::BrowserWindow* browser_window = root_window_->browser_window();
    if (browser_window && !browser_window->IsClosing()) {
      CefRefPtr<CefBrowser> browser = browser_window->GetBrowser();
      if (browser.get()) {
        // Notify the browser window that we would like to close it. This
        // will result in a call to ClientHandler::DoClose() if the
        // JavaScript 'onbeforeunload' event handler allows it.
        browser->GetHost()->CloseBrowser(false);

        // Cancel the close.
        return NO;
      }
    }
  }

  // Try to make the window go away.
  [window autorelease];

  // Clean ourselves up after clearing the stack of anything that might have the
  // window on it.
  [self performSelectorOnMainThread:@selector(cleanup:)
                         withObject:window
                      waitUntilDone:NO];

  // Allow the close.
  return YES;
}

// Deletes itself.
- (void)cleanup:(id)window {
  root_window_->WindowDestroyed();
  [self release];
}

@end


namespace client {

namespace {

// Sizes for URL bar layout.
#define BUTTON_HEIGHT   22
#define BUTTON_WIDTH    72
#define BUTTON_MARGIN   8
#define URLBAR_HEIGHT   32

NSButton* MakeButton(NSRect* rect, NSString* title, NSView* parent) {
  NSButton* button = [[[NSButton alloc] initWithFrame:*rect] autorelease];
  [button setTitle:title];
  [button setBezelStyle:NSSmallSquareBezelStyle];
  [button setAutoresizingMask:(NSViewMaxXMargin | NSViewMinYMargin)];
  [parent addSubview:button];
  rect->origin.x += BUTTON_WIDTH;
  return button;
}

NSRect GetScreenRectForWindow(NSWindow* window) {
  NSScreen* screen = [window screen];
  if (screen == nil)
    screen = [NSScreen mainScreen];
  return [screen visibleFrame];
}

}  // namespace

RootWindowMac::RootWindowMac()
    : delegate_(NULL),
      with_controls_(false),
      with_osr_(false),
      is_popup_(false),
      initialized_(false),
      window_(nil),
      back_button_(nil),
      forward_button_(nil),
      reload_button_(nil),
      stop_button_(nil),
      url_textfield_(nil),
      window_destroyed_(false),
      browser_destroyed_(false) {
}

RootWindowMac::~RootWindowMac() {
  REQUIRE_MAIN_THREAD();

  // The window and browser should already have been destroyed.
  DCHECK(window_destroyed_);
  DCHECK(browser_destroyed_);
}

void RootWindowMac::Init(RootWindow::Delegate* delegate,
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
        base::Bind(&RootWindowMac::CreateRootWindow, this, settings));
  }
}

void RootWindowMac::InitAsPopup(RootWindow::Delegate* delegate,
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

void RootWindowMac::Show(ShowMode mode) {
  REQUIRE_MAIN_THREAD();

  if (!window_)
    return;
  
  const bool is_visible = [window_ isVisible];
  const bool is_minimized = [window_ isMiniaturized];
  const bool is_maximized = [window_ isZoomed];

  if ((mode == ShowMinimized && is_minimized) ||
      (mode == ShowMaximized && is_maximized) ||
      (mode == ShowNormal && is_visible)) {
    // The window is already in the desired state.
    return;
  }

  // Undo the previous state since it's not the desired state.
  if (is_minimized)
    [window_ deminiaturize:nil];
  else if (is_maximized)
    [window_ performZoom:nil];

  // Window visibility may change after (for example) deminiaturizing the
  // window.
  if (![window_ isVisible])
    [window_ makeKeyAndOrderFront: nil];

  if (mode == ShowMinimized)
    [window_ performMiniaturize:nil];
  else if (mode == ShowMaximized)
    [window_ performZoom:nil];
}

void RootWindowMac::Hide() {
  REQUIRE_MAIN_THREAD();

  if (!window_)
    return;

  // Undo miniaturization, if any, so the window will actually be hidden.
  if ([window_ isMiniaturized])
    [window_ deminiaturize:nil];

  // Hide the window.
  [window_ orderOut:nil];
}

void RootWindowMac::SetBounds(int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();

  if (!window_)
    return;

  NSRect screen_rect = GetScreenRectForWindow(window_);

  // Desired content rectangle.
  NSRect content_rect;
  content_rect.size.width = static_cast<int>(width);
  content_rect.size.height = static_cast<int>(height) +
                             (with_controls_ ? URLBAR_HEIGHT : 0);

  // Convert to a frame rectangle.
  NSRect frame_rect = [window_ frameRectForContentRect:content_rect];
  frame_rect.origin.x = x;
  frame_rect.origin.y = screen_rect.size.height - y;

  [window_ setFrame:frame_rect display:YES];
}

void RootWindowMac::Close(bool force) {
  REQUIRE_MAIN_THREAD();

  if (window_) {
    static_cast<RootWindowDelegate*>([window_ delegate]).force_close = force;
    [window_ performClose:nil];
  }
}

void RootWindowMac::SetDeviceScaleFactor(float device_scale_factor) {
  REQUIRE_MAIN_THREAD();

  if (browser_window_)
    browser_window_->SetDeviceScaleFactor(device_scale_factor);
}

float RootWindowMac::GetDeviceScaleFactor() const {
  REQUIRE_MAIN_THREAD();

  if (browser_window_)
    return browser_window_->GetDeviceScaleFactor();
  return 1.0f;
}

CefRefPtr<CefBrowser> RootWindowMac::GetBrowser() const {
  REQUIRE_MAIN_THREAD();

  if (browser_window_)
    return browser_window_->GetBrowser();
  return NULL;
}

ClientWindowHandle RootWindowMac::GetWindowHandle() const {
  REQUIRE_MAIN_THREAD();
  return [window_ contentView];
}

void RootWindowMac::WindowDestroyed() {
  window_ = nil;
  window_destroyed_ = true;
  NotifyDestroyedIfDone();
}

void RootWindowMac::CreateBrowserWindow(const std::string& startup_url) {
  if (with_osr_) {
    OsrRenderer::Settings settings;
    MainContext::Get()->PopulateOsrSettings(&settings);
    browser_window_.reset(new BrowserWindowOsrMac(this, startup_url, settings));
  } else {
    browser_window_.reset(new BrowserWindowStdMac(this, startup_url));
  }
}

void RootWindowMac::CreateRootWindow(const CefBrowserSettings& settings) {
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

  // Create the main window.
  NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
  NSRect window_rect =
      NSMakeRect(x, screen_rect.size.height - y, width, height);
  window_ = [[UnderlayOpenGLHostingWindow alloc]
                initWithContentRect:window_rect
                styleMask:(NSTitledWindowMask |
                           NSClosableWindowMask |
                           NSMiniaturizableWindowMask |
                           NSResizableWindowMask )
                backing:NSBackingStoreBuffered
                defer:NO];
  [window_ setTitle:@"cefclient"];

  // Create the delegate for control and browser window events.
  RootWindowDelegate* delegate =
      [[RootWindowDelegate alloc] initWithWindow:window_ andRootWindow:this];

  // Rely on the window delegate to clean us up rather than immediately
  // releasing when the window gets closed. We use the delegate to do
  // everything from the autorelease pool so the window isn't on the stack
  // during cleanup (ie, a window close from javascript).
  [window_ setReleasedWhenClosed:NO];

  const cef_color_t background_color = MainContext::Get()->GetBackgroundColor();
  [window_ setBackgroundColor:
      [NSColor colorWithCalibratedRed:
                            float(CefColorGetR(background_color)) / 255.0f
                      green:float(CefColorGetG(background_color)) / 255.0f
                       blue:float(CefColorGetB(background_color)) / 255.0f
                      alpha:1.f]];

  NSView* contentView = [window_ contentView];
  NSRect contentBounds = [contentView bounds];

  if (with_controls_) {
    // Create the buttons.
    NSRect button_rect = contentBounds;
    button_rect.origin.y = window_rect.size.height - URLBAR_HEIGHT +
        (URLBAR_HEIGHT - BUTTON_HEIGHT) / 2;
    button_rect.size.height = BUTTON_HEIGHT;
    button_rect.origin.x += BUTTON_MARGIN;
    button_rect.size.width = BUTTON_WIDTH;

    contentBounds.size.height -= URLBAR_HEIGHT;

    back_button_ = MakeButton(&button_rect, @"Back", contentView);
    [back_button_ setTarget:delegate];
    [back_button_ setAction:@selector(goBack:)];
    [back_button_ setEnabled:NO];

    forward_button_ = MakeButton(&button_rect, @"Forward", contentView);
    [forward_button_ setTarget:delegate];
    [forward_button_ setAction:@selector(goForward:)];
    [forward_button_ setEnabled:NO];

    reload_button_ = MakeButton(&button_rect, @"Reload", contentView);
    [reload_button_ setTarget:delegate];
    [reload_button_ setAction:@selector(reload:)];
    [reload_button_ setEnabled:NO];

    stop_button_ = MakeButton(&button_rect, @"Stop", contentView);
    [stop_button_ setTarget:delegate];
    [stop_button_ setAction:@selector(stopLoading:)];
    [stop_button_ setEnabled:NO];

    // Create the URL text field.
    button_rect.origin.x += BUTTON_MARGIN;
    button_rect.size.width = [contentView bounds].size.width -
        button_rect.origin.x - BUTTON_MARGIN;
    url_textfield_ = [[NSTextField alloc] initWithFrame:button_rect];
    [contentView addSubview:url_textfield_];
    [url_textfield_ setAutoresizingMask:
        (NSViewWidthSizable | NSViewMinYMargin)];
    [url_textfield_ setTarget:delegate];
    [url_textfield_ setAction:@selector(takeURLStringValueFrom:)];
    [url_textfield_ setEnabled:NO];
    [[url_textfield_ cell] setWraps:NO];
    [[url_textfield_ cell] setScrollable:YES];
  }

  if (!is_popup_) {
    // Create the browser window.
    browser_window_->CreateBrowser(contentView, CefRect(0, 0, width, height),
                                   settings,
                                   delegate_->GetRequestContext(this));
  } else {
    // With popups we already have a browser window. Parent the browser window
    // to the root window and show it in the correct location.
    browser_window_->ShowPopup(contentView, 0, 0, contentBounds.size.width,
                               contentBounds.size.height);
  }

  // Show the window.
  Show(ShowNormal);

  // Size the window.
  SetBounds(x, y, width, height);
}

void RootWindowMac::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_MAIN_THREAD();

  // For popup browsers create the root window once the browser has been
  // created.
  if (is_popup_)
    CreateRootWindow(CefBrowserSettings());
}

void RootWindowMac::OnBrowserWindowDestroyed() {
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

void RootWindowMac::OnSetAddress(const std::string& url) {
  REQUIRE_MAIN_THREAD();

  if (url_textfield_) {
    std::string urlStr(url);
    NSString* str = [NSString stringWithUTF8String:urlStr.c_str()];
    [url_textfield_ setStringValue:str];
  }
}

void RootWindowMac::OnSetDraggableRegions(
    const std::vector<CefDraggableRegion>& regions) {
  REQUIRE_MAIN_THREAD();
  // TODO(cef): Implement support for draggable regions on this platform.
}

void RootWindowMac::OnSetTitle(const std::string& title) {
  REQUIRE_MAIN_THREAD();

  if (window_) {
    std::string titleStr(title);
    NSString* str = [NSString stringWithUTF8String:titleStr.c_str()];
    [window_ setTitle:str];
  }
}

void RootWindowMac::OnSetFullscreen(bool fullscreen) {
  REQUIRE_MAIN_THREAD();

  CefRefPtr<CefBrowser> browser = GetBrowser();
  if (browser) {
    if (fullscreen)
      window_test::Maximize(browser);
    else
      window_test::Restore(browser);
  }
}

void RootWindowMac::OnSetLoadingState(bool isLoading,
                                      bool canGoBack,
                                      bool canGoForward) {
  REQUIRE_MAIN_THREAD();

  if (with_controls_) {
    [url_textfield_ setEnabled:YES];
    [reload_button_ setEnabled:!isLoading];
    [stop_button_ setEnabled:isLoading];
    [back_button_ setEnabled:canGoBack];
    [forward_button_ setEnabled:canGoForward];
  }
}

void RootWindowMac::NotifyDestroyedIfDone() {
  // Notify once both the window and the browser have been destroyed.
  if (window_destroyed_ && browser_destroyed_)
    delegate_->OnRootWindowDestroyed(this);
}

// static
scoped_refptr<RootWindow> RootWindow::Create() {
  return new RootWindowMac();
}

// static
scoped_refptr<RootWindow> RootWindow::GetForNSWindow(NSWindow* window) {
  RootWindowDelegate* delegate =
      static_cast<RootWindowDelegate*>([window delegate]);
  return [delegate root_window];
}

}  // namespace client
