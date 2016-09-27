// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/browser_window_osr_mac.h"

#include <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#include <OpenGL/gl.h>

#include "include/base/cef_logging.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_closure_task.h"
#include "cefclient/browser/bytes_write_handler.h"
#include "cefclient/browser/geometry_util.h"
#include "cefclient/browser/main_message_loop.h"

// Forward declare methods and constants that are only available with newer SDK
// versions to avoid -Wpartial-availability compiler warnings.

#if !defined(MAC_OS_X_VERSION_10_7) || \
    MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_7

@interface NSEvent (LionSDK)
- (NSEventPhase)phase;
@end

@interface NSView (LionSDK)
- (NSPoint)convertPointFromBacking:(NSPoint)aPoint;
- (NSPoint)convertPointToBacking:(NSPoint)aPoint;
- (NSRect)convertRectFromBacking:(NSRect)aRect;
- (NSRect)convertRectToBacking:(NSRect)aRect;
- (void)setWantsBestResolutionOpenGLSurface:(BOOL)flag;
@end

@interface NSWindow (LionSDK)
- (CGFloat)backingScaleFactor;
@end

extern "C" {
extern NSString* const NSWindowDidChangeBackingPropertiesNotification;
}  // extern "C"

#endif  // MAC_OS_X_VERSION_10_7


@interface BrowserOpenGLView
    : NSOpenGLView <NSDraggingSource, NSDraggingDestination> {
 @private
  NSTrackingArea* tracking_area_;
  client::BrowserWindowOsrMac* browser_window_;
  client::OsrRenderer* renderer_;
  NSPoint last_mouse_pos_;
  NSPoint cur_mouse_pos_;
  bool rotating_;

  bool was_last_mouse_down_on_view_;

  float device_scale_factor_;

  // Drag and drop
  CefRefPtr<CefDragData> current_drag_data_;
  NSDragOperation current_drag_op_;
  NSDragOperation current_allowed_ops_;
  NSPasteboard* pasteboard_;
  CFStringRef fileUTI_;

  // Event monitor for scroll wheel end event.
  id endWheelMonitor_;
}

- (id)initWithFrame:(NSRect)frame
   andBrowserWindow:(client::BrowserWindowOsrMac*)browser_window
        andRenderer:(client::OsrRenderer*)renderer;
- (void)detach;

- (CefRefPtr<CefBrowser>)getBrowser;
- (NSPoint)getClickPointForEvent:(NSEvent*)event;
- (void)getKeyEvent:(CefKeyEvent&)keyEvent forEvent:(NSEvent*)event;
- (void)getMouseEvent:(CefMouseEvent&)mouseEvent forEvent:(NSEvent*)event;
- (void)getMouseEvent:(CefMouseEvent&)mouseEvent
          forDragInfo:(id<NSDraggingInfo>)info;
- (int)getModifiersForEvent:(NSEvent*)event;
- (BOOL)isKeyUpEvent:(NSEvent*)event;
- (BOOL)isKeyPadEvent:(NSEvent*)event;
- (BOOL)startDragging:(CefRefPtr<CefDragData>)drag_data
           allowedOps:(NSDragOperation)ops point:(NSPoint)p;
- (void)setCurrentDragOp:(NSDragOperation)op;

- (void)resetDragDrop;
- (void)fillPasteboard;
- (void)populateDropData:(CefRefPtr<CefDragData>)data
          fromPasteboard:(NSPasteboard*)pboard;
- (NSPoint)flipWindowPointToView:(const NSPoint&)windowPoint;
- (void)resetDeviceScaleFactor;
- (void)setDeviceScaleFactor:(float)device_scale_factor;
- (float)getDeviceScaleFactor;
- (void)windowDidChangeBackingProperties:(NSNotification*)notification;

- (bool)isOverPopupWidgetX:(int)x andY:(int)y;
- (void)applyPopupOffsetToX:(int&)x andY:(int&)y;
- (int)getPopupXOffset;
- (int)getPopupYOffset;

- (void)sendMouseClick:(NSEvent*)event
                button:(CefBrowserHost::MouseButtonType)type
                  isUp:(bool)isUp;

- (NSPoint)convertPointFromBackingInternal:(NSPoint)aPoint;
- (NSPoint)convertPointToBackingInternal:(NSPoint)aPoint;
- (NSRect)convertRectFromBackingInternal:(NSRect)aRect;
- (NSRect)convertRectToBackingInternal:(NSRect)aRect;

@end


namespace {

// This method will return YES for OS X versions 10.7.3 and later, and NO
// otherwise.
// Used to prevent a crash when building with the 10.7 SDK and accessing the
// notification below. See: http://crbug.com/260595.
BOOL SupportsBackingPropertiesChangedNotification() {
  // windowDidChangeBackingProperties: method has been added to the
  // NSWindowDelegate protocol in 10.7.3, at the same time as the
  // NSWindowDidChangeBackingPropertiesNotification notification was added.
  // If the protocol contains this method description, the notification should
  // be supported as well.
  Protocol* windowDelegateProtocol = NSProtocolFromString(@"NSWindowDelegate");
  struct objc_method_description methodDescription =
      protocol_getMethodDescription(
          windowDelegateProtocol,
          @selector(windowDidChangeBackingProperties:),
          NO,
          YES);

  // If the protocol does not contain the method, the returned method
  // description is {NULL, NULL}
  return methodDescription.name != NULL || methodDescription.types != NULL;
}

NSString* const kCEFDragDummyPboardType = @"org.CEF.drag-dummy-type";
NSString* const kNSURLTitlePboardType = @"public.url-name";

class ScopedGLContext {
 public:
  ScopedGLContext(BrowserOpenGLView* view, bool swap_buffers)
    : swap_buffers_(swap_buffers) {
    context_ = [view openGLContext];
    [context_ makeCurrentContext];
  }
  ~ScopedGLContext() {
    [NSOpenGLContext clearCurrentContext];
    if (swap_buffers_)
      [context_ flushBuffer];
  }
 private:
  NSOpenGLContext* context_;
  const bool swap_buffers_;
};

BrowserOpenGLView* GLView(NSView* view) {
  return static_cast<BrowserOpenGLView*>(view);
}

}  // namespace


@implementation BrowserOpenGLView

- (id)initWithFrame:(NSRect)frame
   andBrowserWindow:(client::BrowserWindowOsrMac*)browser_window
        andRenderer:(client::OsrRenderer*)renderer {
  NSOpenGLPixelFormat * pixelFormat =
      [[NSOpenGLPixelFormat alloc]
       initWithAttributes:(NSOpenGLPixelFormatAttribute[]) {
           NSOpenGLPFAWindow,
           NSOpenGLPFADoubleBuffer,
           NSOpenGLPFADepthSize,
           32,
           0}];
  [pixelFormat autorelease];

  if (self = [super initWithFrame:frame pixelFormat:pixelFormat]) {
    browser_window_ = browser_window;
    renderer_ = renderer;
    rotating_ = false;
    endWheelMonitor_ = nil;
    device_scale_factor_ = 1.0f;

    tracking_area_ =
        [[NSTrackingArea alloc] initWithRect:frame
                                     options:NSTrackingMouseMoved |
                                             NSTrackingActiveInActiveApp |
                                             NSTrackingInVisibleRect
                                       owner:self
                                    userInfo:nil];
    [self addTrackingArea:tracking_area_];

    if ([self respondsToSelector:
            @selector(setWantsBestResolutionOpenGLSurface:)]) {
      // enable HiDPI buffer
      [self setWantsBestResolutionOpenGLSurface:YES];
    }

    [self resetDragDrop];

    NSArray* types = [NSArray arrayWithObjects:
        kCEFDragDummyPboardType,
        NSStringPboardType,
        NSFilenamesPboardType,
        NSPasteboardTypeString,
        nil];
    [self registerForDraggedTypes:types];
  }

  return self;
}

- (void)dealloc {
  static BOOL supportsBackingPropertiesNotification =
      SupportsBackingPropertiesChangedNotification();
  if (supportsBackingPropertiesNotification) {
    [[NSNotificationCenter defaultCenter]
        removeObserver:self
                  name:NSWindowDidChangeBackingPropertiesNotification
                object:nil];
  }

  [super dealloc];
}

- (void)detach {
  renderer_ = NULL;
  browser_window_ = NULL;
}

- (CefRefPtr<CefBrowser>)getBrowser {
  if (browser_window_)
    return browser_window_->GetBrowser();
  return NULL;
}

- (void)setFrame:(NSRect)frameRect {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return;

  [super setFrame:frameRect];
  browser->GetHost()->WasResized();
}

- (void) sendMouseClick:(NSEvent*)event
                 button:(CefBrowserHost::MouseButtonType)type
                   isUp:(bool)isUp {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return;

  CefMouseEvent mouseEvent;
  [self getMouseEvent:mouseEvent forEvent:event];

  // |point| is in OS X view coordinates.
  NSPoint point = [self getClickPointForEvent:event];

  // Convert to device coordinates.
  point = [self convertPointToBackingInternal:point];

  if (!isUp) {
    was_last_mouse_down_on_view_ = ![self isOverPopupWidgetX:point.x
                                                        andY:point.y];
  } else if (was_last_mouse_down_on_view_ &&
             [self isOverPopupWidgetX:point.x andY:point.y] &&
             ([self getPopupXOffset] || [self getPopupYOffset])) {
    return;
  }

  browser->GetHost()->SendMouseClickEvent(mouseEvent,
                                          type,
                                          isUp,
                                          [event clickCount]);
}

- (void)mouseDown:(NSEvent*)event {
  [self sendMouseClick: event button:MBT_LEFT isUp:false];
}

- (void)rightMouseDown:(NSEvent*)event {
  if ([event modifierFlags] & NSShiftKeyMask) {
    // Start rotation effect.
    last_mouse_pos_ = cur_mouse_pos_ = [self getClickPointForEvent:event];
    rotating_ = true;
    return;
  }

  [self sendMouseClick: event button:MBT_RIGHT isUp:false];
}

- (void)otherMouseDown:(NSEvent*)event {
  [self sendMouseClick: event button:MBT_MIDDLE isUp:false];
}

- (void)mouseUp:(NSEvent*)event {
  [self sendMouseClick: event button: MBT_LEFT isUp: true];
}

- (void)rightMouseUp:(NSEvent*)event {
  if (rotating_) {
    // End rotation effect.
    renderer_->SetSpin(0, 0);
    rotating_ = false;
    [self setNeedsDisplay:YES];
    return;
  }
  [self sendMouseClick: event button: MBT_RIGHT isUp: true];
}

- (void)otherMouseUp:(NSEvent*)event {
  [self sendMouseClick: event button: MBT_MIDDLE isUp: true];
}

- (void)mouseMoved:(NSEvent*)event {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return;

  if (rotating_) {
    // Apply rotation effect.
    cur_mouse_pos_ = [self getClickPointForEvent:event];;
    renderer_->IncrementSpin((cur_mouse_pos_.x - last_mouse_pos_.x),
                             (cur_mouse_pos_.y - last_mouse_pos_.y));
    last_mouse_pos_ = cur_mouse_pos_;
    [self setNeedsDisplay:YES];
    return;
  }

  CefMouseEvent mouseEvent;
  [self getMouseEvent:mouseEvent forEvent:event];

  browser->GetHost()->SendMouseMoveEvent(mouseEvent, false);
}

- (void)mouseDragged:(NSEvent*)event {
  [self mouseMoved:event];
}

- (void)rightMouseDragged:(NSEvent*)event {
  [self mouseMoved:event];
}

- (void)otherMouseDragged:(NSEvent*)event {
  [self mouseMoved:event];
}

- (void)mouseEntered:(NSEvent*)event {
  [self mouseMoved:event];
}

- (void)mouseExited:(NSEvent*)event {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return;

  CefMouseEvent mouseEvent;
  [self getMouseEvent:mouseEvent forEvent:event];

  browser->GetHost()->SendMouseMoveEvent(mouseEvent, true);
}

- (void)keyDown:(NSEvent*)event {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return;

  if ([event type] != NSFlagsChanged) {
    browser->GetHost()->HandleKeyEventBeforeTextInputClient(event);

    // The return value of this method seems to always be set to YES,
    // thus we ignore it and ask the host view whether IME is active
    // or not.
    [[self inputContext] handleEvent:event];

    browser->GetHost()->HandleKeyEventAfterTextInputClient(event);
  }
}

- (void)keyUp:(NSEvent*)event {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return;

  CefKeyEvent keyEvent;
  [self getKeyEvent:keyEvent forEvent:event];

  keyEvent.type = KEYEVENT_KEYUP;
  browser->GetHost()->SendKeyEvent(keyEvent);
}

- (void)flagsChanged:(NSEvent*)event {
  if ([self isKeyUpEvent:event])
    [self keyUp:event];
  else
    [self keyDown:event];
}

- (void)shortCircuitScrollWheelEvent:(NSEvent*)event {
  // Phase is only supported in OS-X 10.7 and newer.
  if ([event phase] != NSEventPhaseEnded &&
      [event phase] != NSEventPhaseCancelled)
    return;

  [self sendScrollWheelEvet:event];

  if (endWheelMonitor_) {
    [NSEvent removeMonitor:endWheelMonitor_];
    endWheelMonitor_ = nil;
  }
}

- (void)scrollWheel:(NSEvent*)event {
  // Phase is only supported in OS-X 10.7 and newer.
  // Use an NSEvent monitor to listen for the wheel-end end. This ensures that
  // the event is received even when the mouse cursor is no longer over the
  // view when the scrolling ends. Also it avoids sending duplicate scroll
  // events to the renderer.
  if ([event respondsToSelector:@selector(phase)] &&
      [event phase] == NSEventPhaseBegan && !endWheelMonitor_) {
    endWheelMonitor_ =
        [NSEvent addLocalMonitorForEventsMatchingMask:NSScrollWheelMask
            handler:^(NSEvent* blockEvent) {
              [self shortCircuitScrollWheelEvent:blockEvent];
              return blockEvent;
            }];
  }

  [self sendScrollWheelEvet:event];
}

- (void)sendScrollWheelEvet:(NSEvent*)event {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return;

  CGEventRef cgEvent = [event CGEvent];
  DCHECK(cgEvent);

  int deltaX =
      CGEventGetIntegerValueField(cgEvent, kCGScrollWheelEventPointDeltaAxis2);
  int deltaY =
      CGEventGetIntegerValueField(cgEvent, kCGScrollWheelEventPointDeltaAxis1);

  CefMouseEvent mouseEvent;
  [self getMouseEvent:mouseEvent forEvent:event];

  browser->GetHost()->SendMouseWheelEvent(mouseEvent, deltaX, deltaY);
}

- (BOOL)canBecomeKeyView {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  return (browser.get() != NULL);
}

- (BOOL)acceptsFirstResponder {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  return (browser.get() != NULL);
}

- (BOOL)becomeFirstResponder {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get()) {
    browser->GetHost()->SendFocusEvent(true);
    return [super becomeFirstResponder];
  }

  return NO;
}

- (BOOL)resignFirstResponder {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get()) {
    browser->GetHost()->SendFocusEvent(false);
    return [super resignFirstResponder];
  }

  return NO;
}

- (void)undo:(id)sender {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    browser->GetFocusedFrame()->Undo();
}

- (void)redo:(id)sender {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    browser->GetFocusedFrame()->Redo();
}

- (void)cut:(id)sender {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    browser->GetFocusedFrame()->Cut();
}

- (void)copy:(id)sender {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    browser->GetFocusedFrame()->Copy();
}

- (void)paste:(id)sender {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    browser->GetFocusedFrame()->Paste();
}

- (void)delete:(id)sender {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    browser->GetFocusedFrame()->Delete();
}

- (void)selectAll:(id)sender {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    browser->GetFocusedFrame()->SelectAll();
}

- (NSPoint)getClickPointForEvent:(NSEvent*)event {
  NSPoint windowLocal = [event locationInWindow];
  NSPoint contentLocal = [self convertPoint:windowLocal fromView:nil];

  NSPoint point;
  point.x = contentLocal.x;
  point.y = [self frame].size.height - contentLocal.y;  // Flip y.
  return point;
}

- (void)getKeyEvent:(CefKeyEvent &)keyEvent forEvent:(NSEvent *)event {
  if ([event type] == NSKeyDown || [event type] == NSKeyUp) {
    NSString* s = [event characters];
    if ([s length] > 0)
      keyEvent.character = [s characterAtIndex:0];

    s = [event charactersIgnoringModifiers];
    if ([s length] > 0)
      keyEvent.unmodified_character = [s characterAtIndex:0];
  }

  if ([event type] == NSFlagsChanged) {
    keyEvent.character = 0;
    keyEvent.unmodified_character = 0;
  }

  keyEvent.native_key_code = [event keyCode];

  keyEvent.modifiers = [self getModifiersForEvent:event];
}

- (NSTextInputContext*)inputContext {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    return browser->GetHost()->GetNSTextInputContext();
  return NULL;
}

- (void)getMouseEvent:(CefMouseEvent&)mouseEvent forEvent:(NSEvent*)event {
  const float device_scale_factor = [self getDeviceScaleFactor];

  // |point| is in OS X view coordinates.
  NSPoint point = [self getClickPointForEvent:event];

  // Convert to device coordinates.
  point = [self convertPointToBackingInternal:point];

  int device_x = point.x;
  int device_y = point.y;
  if ([self isOverPopupWidgetX:device_x andY:device_y])
    [self applyPopupOffsetToX:device_x andY:device_y];

  // Convert to browser view coordinates.
  mouseEvent.x = client::DeviceToLogical(device_x, device_scale_factor);
  mouseEvent.y = client::DeviceToLogical(device_y, device_scale_factor);

  mouseEvent.modifiers = [self getModifiersForEvent:event];
}

- (void)getMouseEvent:(CefMouseEvent&)mouseEvent
          forDragInfo:(id<NSDraggingInfo>)info {
  const float device_scale_factor = [self getDeviceScaleFactor];

  // |point| is in OS X view coordinates.
  NSPoint windowPoint = [info draggingLocation];
  NSPoint point = [self flipWindowPointToView:windowPoint];

  // Convert to device coordinates.
  point = [self convertPointToBackingInternal:point];

  // Convert to browser view coordinates.
  mouseEvent.x = client::DeviceToLogical(point.x, device_scale_factor);
  mouseEvent.y = client::DeviceToLogical(point.y, device_scale_factor);

  mouseEvent.modifiers = [NSEvent modifierFlags];
}

- (int)getModifiersForEvent:(NSEvent*)event {
  int modifiers = 0;

  if ([event modifierFlags] & NSControlKeyMask)
    modifiers |= EVENTFLAG_CONTROL_DOWN;
  if ([event modifierFlags] & NSShiftKeyMask)
    modifiers |= EVENTFLAG_SHIFT_DOWN;
  if ([event modifierFlags] & NSAlternateKeyMask)
    modifiers |= EVENTFLAG_ALT_DOWN;
  if ([event modifierFlags] & NSCommandKeyMask)
    modifiers |= EVENTFLAG_COMMAND_DOWN;
  if ([event modifierFlags] & NSAlphaShiftKeyMask)
    modifiers |= EVENTFLAG_CAPS_LOCK_ON;

  if ([event type] == NSKeyUp ||
      [event type] == NSKeyDown ||
      [event type] == NSFlagsChanged) {
    // Only perform this check for key events
    if ([self isKeyPadEvent:event])
      modifiers |= EVENTFLAG_IS_KEY_PAD;
  }

  // OS X does not have a modifier for NumLock, so I'm not entirely sure how to
  // set EVENTFLAG_NUM_LOCK_ON;
  //
  // There is no EVENTFLAG for the function key either.

  // Mouse buttons
  switch ([event type]) {
    case NSLeftMouseDragged:
    case NSLeftMouseDown:
    case NSLeftMouseUp:
      modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    break;
    case NSRightMouseDragged:
    case NSRightMouseDown:
    case NSRightMouseUp:
      modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
    break;
    case NSOtherMouseDragged:
    case NSOtherMouseDown:
    case NSOtherMouseUp:
      modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    break;
    default:
    break;
  }

  return modifiers;
}

- (BOOL)isKeyUpEvent:(NSEvent*)event {
  if ([event type] != NSFlagsChanged)
    return [event type] == NSKeyUp;

  // FIXME: This logic fails if the user presses both Shift keys at once, for
  // example: we treat releasing one of them as keyDown.
  switch ([event keyCode]) {
    case 54: // Right Command
    case 55: // Left Command
      return ([event modifierFlags] & NSCommandKeyMask) == 0;

    case 57: // Capslock
      return ([event modifierFlags] & NSAlphaShiftKeyMask) == 0;

    case 56: // Left Shift
    case 60: // Right Shift
      return ([event modifierFlags] & NSShiftKeyMask) == 0;

    case 58: // Left Alt
    case 61: // Right Alt
     return ([event modifierFlags] & NSAlternateKeyMask) == 0;

    case 59: // Left Ctrl
    case 62: // Right Ctrl
      return ([event modifierFlags] & NSControlKeyMask) == 0;

    case 63: // Function
      return ([event modifierFlags] & NSFunctionKeyMask) == 0;
  }
  return false;
}

- (BOOL)isKeyPadEvent:(NSEvent*)event {
  if ([event modifierFlags] & NSNumericPadKeyMask)
    return true;

  switch ([event keyCode]) {
    case 71: // Clear
    case 81: // =
    case 75: // /
    case 67: // *
    case 78: // -
    case 69: // +
    case 76: // Enter
    case 65: // .
    case 82: // 0
    case 83: // 1
    case 84: // 2
    case 85: // 3
    case 86: // 4
    case 87: // 5
    case 88: // 6
    case 89: // 7
    case 91: // 8
    case 92: // 9
      return true;
  }

  return false;
}

- (void)windowDidChangeBackingProperties:(NSNotification*)notification {
  // This delegate method is only called on 10.7 and later, so don't worry about
  // other backing changes calling it on 10.6 or earlier
  [self resetDeviceScaleFactor];
}

- (void)drawRect: (NSRect) dirtyRect {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if ([self inLiveResize] || !browser.get()) {
    // Fill with the background color.
    const cef_color_t background_color = renderer_->GetBackgroundColor();
    NSColor* color =
        [NSColor colorWithCalibratedRed:
                              float(CefColorGetR(background_color)) / 255.0f
                        green:float(CefColorGetG(background_color)) / 255.0f
                         blue:float(CefColorGetB(background_color)) / 255.0f
                        alpha:1.f];
    [color setFill];
    NSRectFill(dirtyRect);
  }

  // The Invalidate below fixes flicker when resizing.
  if ([self inLiveResize] && browser.get())
    browser->GetHost()->Invalidate(PET_VIEW);
}

// Drag and drop

- (BOOL)startDragging:(CefRefPtr<CefDragData>)drag_data
           allowedOps:(NSDragOperation)ops
                point:(NSPoint)position {
  DCHECK(!pasteboard_);
  DCHECK(!fileUTI_);
  DCHECK(!current_drag_data_.get());

  [self resetDragDrop];

  current_allowed_ops_ = ops;
  current_drag_data_ = drag_data;

  [self fillPasteboard];

  NSEvent* currentEvent = [[NSApplication sharedApplication] currentEvent];
  NSWindow* window = [self window];
  NSTimeInterval eventTime = [currentEvent timestamp];

  NSEvent* dragEvent = [NSEvent mouseEventWithType:NSLeftMouseDragged
                                          location:position
                                     modifierFlags:NSLeftMouseDraggedMask
                                         timestamp:eventTime
                                      windowNumber:[window windowNumber]
                                           context:nil
                                       eventNumber:0
                                        clickCount:1
                                          pressure:1.0];

  [window dragImage:nil
                 at:position
             offset:NSZeroSize
              event:dragEvent
         pasteboard:pasteboard_
             source:self
          slideBack:YES];
  return YES;
}

- (void)setCurrentDragOp:(NSDragOperation)op {
  current_drag_op_ = op;
}

// NSDraggingSource Protocol

- (NSDragOperation)draggingSession:(NSDraggingSession *)session
    sourceOperationMaskForDraggingContext:(NSDraggingContext)context {
  switch(context) {
    case NSDraggingContextOutsideApplication:
      return current_allowed_ops_;

    case NSDraggingContextWithinApplication:
    default:
      return current_allowed_ops_;
  }
}

- (NSArray*)namesOfPromisedFilesDroppedAtDestination:(NSURL*)dropDest {
  if (![dropDest isFileURL])
    return nil;

  if (!current_drag_data_)
    return nil;

  size_t expected_size = current_drag_data_->GetFileContents(NULL);
  if (expected_size == 0)
    return nil;

  std::string path = [[dropDest path] UTF8String];
  path.append("/");
  path.append(current_drag_data_->GetFileName().ToString());

  CefRefPtr<CefStreamWriter> writer = CefStreamWriter::CreateForFile(path);
  if (!writer)
    return nil;

  if (current_drag_data_->GetFileContents(writer) != expected_size)
    return nil;

  return @[ [NSString stringWithUTF8String:path.c_str()] ];
}

- (void)draggedImage:(NSImage*)anImage
             endedAt:(NSPoint)screenPoint
           operation:(NSDragOperation)operation {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return;

  if (operation == (NSDragOperationMove | NSDragOperationCopy))
    operation &= ~NSDragOperationMove;

  NSPoint windowPoint = [[self window] convertScreenToBase: screenPoint];
  NSPoint pt = [self flipWindowPointToView:windowPoint];
  CefRenderHandler::DragOperation op =
      static_cast<CefRenderHandler::DragOperation>(operation);
  browser->GetHost()->DragSourceEndedAt(pt.x, pt.y, op);
  browser->GetHost()->DragSourceSystemDragEnded();
  [self resetDragDrop];
}

// NSDraggingDestination Protocol

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)info {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return NSDragOperationNone;

  CefRefPtr<CefDragData> drag_data;
  if (!current_drag_data_) {
    drag_data = CefDragData::Create();
    [self populateDropData:drag_data
            fromPasteboard:[info draggingPasteboard]];
  } else {
    drag_data = current_drag_data_->Clone();
    drag_data->ResetFileContents();
  }

  CefMouseEvent mouseEvent;
  [self getMouseEvent:mouseEvent forDragInfo:info];

  NSDragOperation mask = [info draggingSourceOperationMask];
  CefBrowserHost::DragOperationsMask allowed_ops =
      static_cast<CefBrowserHost::DragOperationsMask>(mask);

  browser->GetHost()->DragTargetDragEnter(drag_data, mouseEvent, allowed_ops);
  browser->GetHost()->DragTargetDragOver(mouseEvent, allowed_ops);

  current_drag_op_ = NSDragOperationCopy;
  return current_drag_op_;
}

- (void)draggingExited:(id<NSDraggingInfo>)sender {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser.get())
    browser->GetHost()->DragTargetDragLeave();
}

- (BOOL)prepareForDragOperation:(id<NSDraggingInfo>)info {
  return YES;
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)info {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return NO;

  CefMouseEvent mouseEvent;
  [self getMouseEvent:mouseEvent forDragInfo:info];

  browser->GetHost()->DragTargetDrop(mouseEvent);

  return YES;
}

- (NSDragOperation)draggingUpdated:(id<NSDraggingInfo>)info {
  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (!browser.get())
    return NSDragOperationNone;

  CefMouseEvent mouseEvent;
  [self getMouseEvent:mouseEvent forDragInfo:info];

  NSDragOperation mask = [info draggingSourceOperationMask];
  CefBrowserHost::DragOperationsMask allowed_ops =
      static_cast<CefBrowserHost::DragOperationsMask>(mask);

  browser->GetHost()->DragTargetDragOver(mouseEvent, allowed_ops);

  return current_drag_op_;
}

// NSPasteboardOwner Protocol

- (void)pasteboard:(NSPasteboard *)pboard provideDataForType:(NSString *)type {
  if (!current_drag_data_) {
    return;
  }

  // URL.
  if ([type isEqualToString:NSURLPboardType]) {
    DCHECK(current_drag_data_->IsLink());
    NSString* strUrl = [NSString stringWithUTF8String:
        current_drag_data_->GetLinkURL().ToString().c_str()];
    NSURL* url = [NSURL URLWithString:strUrl];
    [url writeToPasteboard:pboard];
  // URL title.
  } else if ([type isEqualToString:kNSURLTitlePboardType]) {
    NSString* strTitle = [NSString stringWithUTF8String:
        current_drag_data_->GetLinkTitle().ToString().c_str()];
    [pboard setString:strTitle forType:kNSURLTitlePboardType];

  // File contents.
  } else if ([type isEqualToString:(NSString*)fileUTI_]) {
    size_t size = current_drag_data_->GetFileContents(NULL);
    DCHECK_GT(size, 0U);
    CefRefPtr<client::BytesWriteHandler> handler =
        new client::BytesWriteHandler(size);
    CefRefPtr<CefStreamWriter> writer =
        CefStreamWriter::CreateForHandler(handler.get());
    current_drag_data_->GetFileContents(writer);
    DCHECK_EQ(handler->GetDataSize(), static_cast<int64>(size));

    [pboard setData:[NSData dataWithBytes:handler->GetData()
                                   length:handler->GetDataSize()]
                                  forType:(NSString*)fileUTI_];

  // Plain text.
  } else if ([type isEqualToString:NSStringPboardType]) {
    NSString* strTitle = [NSString stringWithUTF8String:
        current_drag_data_->GetFragmentText().ToString().c_str()];
    [pboard setString:strTitle forType:NSStringPboardType];

  } else if ([type isEqualToString:kCEFDragDummyPboardType]) {
    // The dummy type _was_ promised and someone decided to call the bluff.
    [pboard setData:[NSData data]
            forType:kCEFDragDummyPboardType];

  }
}

// Utility methods.
- (void)resetDragDrop {
  current_drag_op_ = NSDragOperationNone;
  current_allowed_ops_ = NSDragOperationNone;
  current_drag_data_ = NULL;
  if (fileUTI_) {
    CFRelease(fileUTI_);
    fileUTI_ = NULL;
  }
  if (pasteboard_) {
    [pasteboard_ release];
    pasteboard_ = nil;
  }
}

- (void)fillPasteboard {
  DCHECK(!pasteboard_);
  pasteboard_ = [[NSPasteboard pasteboardWithName:NSDragPboard] retain];

  [pasteboard_ declareTypes:@[ kCEFDragDummyPboardType ]
                      owner:self];

  // URL (and title).
  if (current_drag_data_->IsLink()) {
    [pasteboard_ addTypes:@[ NSURLPboardType, kNSURLTitlePboardType ]
                    owner:self];
  }

  // MIME type.
  CefString mimeType;
  size_t contents_size = current_drag_data_->GetFileContents(NULL);
  CefString download_metadata = current_drag_data_->GetLinkMetadata();
  CefString file_name = current_drag_data_->GetFileName();

  // File.
  if (contents_size > 0) {
      std::string file_name = current_drag_data_->GetFileName().ToString();
      size_t sep = file_name.find_last_of(".");
      CefString extension = file_name.substr(sep + 1);

      mimeType = CefGetMimeType(extension);

    if (!mimeType.empty()) {
      CFStringRef mimeTypeCF;
      mimeTypeCF = CFStringCreateWithCString(kCFAllocatorDefault,
          mimeType.ToString().c_str(), kCFStringEncodingUTF8);
      fileUTI_ = UTTypeCreatePreferredIdentifierForTag(kUTTagClassMIMEType,
          mimeTypeCF, NULL);
      CFRelease(mimeTypeCF);
      // File (HFS) promise.
      NSArray* fileUTIList = @[ (NSString*)fileUTI_ ];
      [pasteboard_ addTypes:@[ NSFilesPromisePboardType ] owner:self];
      [pasteboard_ setPropertyList:fileUTIList
                           forType:NSFilesPromisePboardType];

      [pasteboard_ addTypes:fileUTIList owner:self];
    }
  }

  // Plain text.
  if (!current_drag_data_->GetFragmentText().empty()) {
    [pasteboard_ addTypes:@[ NSStringPboardType ]
                    owner:self];
  }
}

- (void)populateDropData:(CefRefPtr<CefDragData>)data
          fromPasteboard:(NSPasteboard*)pboard {
  DCHECK(data);
  DCHECK(pboard);
  DCHECK(data && !data->IsReadOnly());
  NSArray* types = [pboard types];

  // Get plain text.
  if ([types containsObject:NSStringPboardType]) {
    data->SetFragmentText(
        [[pboard stringForType:NSStringPboardType] UTF8String]);
  }

  // Get files.
  if ([types containsObject:NSFilenamesPboardType]) {
    NSArray* files = [pboard propertyListForType:NSFilenamesPboardType];
    if ([files isKindOfClass:[NSArray class]] && [files count]) {
      for (NSUInteger i = 0; i < [files count]; i++) {
        NSString* filename = [files objectAtIndex:i];
        BOOL exists = [[NSFileManager defaultManager]
            fileExistsAtPath:filename];
        if (exists) {
          data->AddFile([filename UTF8String], CefString());
        }
      }
    }
  }
}

- (NSPoint)flipWindowPointToView:(const NSPoint&)windowPoint {
  NSPoint viewPoint =  [self convertPoint:windowPoint fromView:nil];
  NSRect viewFrame = [self frame];
  viewPoint.y = viewFrame.size.height - viewPoint.y;
  return viewPoint;
}

- (void)resetDeviceScaleFactor {
  float device_scale_factor = 1.0f;
  NSWindow* window = [self window];
  if (window) {
    if ([window respondsToSelector:@selector(backingScaleFactor)])
      device_scale_factor = [window backingScaleFactor];
    else
      device_scale_factor = [window userSpaceScaleFactor];
  }
  [self setDeviceScaleFactor:device_scale_factor];
}

- (void)setDeviceScaleFactor:(float)device_scale_factor {
  if (device_scale_factor == device_scale_factor_)
    return;

  // Apply some sanity checks.
  if (device_scale_factor < 1.0f || device_scale_factor > 4.0f)
    return;

  device_scale_factor_ = device_scale_factor;

  CefRefPtr<CefBrowser> browser = [self getBrowser];
  if (browser) {
    browser->GetHost()->NotifyScreenInfoChanged();
    browser->GetHost()->WasResized();
  }
}

- (float)getDeviceScaleFactor {
  return device_scale_factor_;
}

- (bool)isOverPopupWidgetX:(int)x andY:(int)y {
  CefRect rc = renderer_->popup_rect();
  int popup_right = rc.x + rc.width;
  int popup_bottom = rc.y + rc.height;
  return (x >= rc.x) && (x < popup_right) &&
         (y >= rc.y) && (y < popup_bottom);
}

- (int)getPopupXOffset {
  return renderer_->original_popup_rect().x - renderer_->popup_rect().x;
}

- (int)getPopupYOffset {
  return renderer_->original_popup_rect().y - renderer_->popup_rect().y;
}

- (void)applyPopupOffsetToX:(int&)x andY:(int&)y {
  if ([self isOverPopupWidgetX:x andY:y]) {
    x += [self getPopupXOffset];
    y += [self getPopupYOffset];
  }
}

// Convert from scaled coordinates to view coordinates.
- (NSPoint)convertPointFromBackingInternal:(NSPoint)aPoint {
  if ([self respondsToSelector:@selector(convertPointFromBacking:)])
    return [self convertPointFromBacking:aPoint];
  return aPoint;
}

// Convert from view coordinates to scaled coordinates.
- (NSPoint)convertPointToBackingInternal:(NSPoint)aPoint {
  if ([self respondsToSelector:@selector(convertPointToBacking:)])
    return [self convertPointToBacking:aPoint];
  return aPoint;
}

// Convert from scaled coordinates to view coordinates.
- (NSRect)convertRectFromBackingInternal:(NSRect)aRect {
  if ([self respondsToSelector:@selector(convertRectFromBacking:)])
    return [self convertRectFromBacking:aRect];
  return aRect;
}

// Convert from view coordinates to scaled coordinates.
- (NSRect)convertRectToBackingInternal:(NSRect)aRect {
  if ([self respondsToSelector:@selector(convertRectToBacking:)])
    return [self convertRectToBacking:aRect];
  return aRect;
}

@end


namespace client {

BrowserWindowOsrMac::BrowserWindowOsrMac(BrowserWindow::Delegate* delegate,
                                         const std::string& startup_url,
                                         const OsrRenderer::Settings& settings)
    : BrowserWindow(delegate),
      renderer_(settings),
      nsview_(NULL),
      hidden_(false),
      painting_popup_(false) {
  client_handler_ = new ClientHandlerOsr(this, this, startup_url);
}

BrowserWindowOsrMac::~BrowserWindowOsrMac() {
  if (nsview_) {
    // Disassociate the view with |this|.
    [GLView(nsview_) detach];
  }
}

void BrowserWindowOsrMac::CreateBrowser(
    ClientWindowHandle parent_handle,
    const CefRect& rect,
    const CefBrowserSettings& settings,
    CefRefPtr<CefRequestContext> request_context) {
  REQUIRE_MAIN_THREAD();

  // Create the native NSView.
  Create(parent_handle, rect);

  CefWindowInfo window_info;
  window_info.SetAsWindowless(nsview_, renderer_.IsTransparent());

  // Create the browser asynchronously.
  CefBrowserHost::CreateBrowser(window_info, client_handler_,
                                client_handler_->startup_url(),
                                settings, request_context);
}

void BrowserWindowOsrMac::GetPopupConfig(CefWindowHandle temp_handle,
                                         CefWindowInfo& windowInfo,
                                         CefRefPtr<CefClient>& client,
                                         CefBrowserSettings& settings) {
  // Note: This method may be called on any thread.
  windowInfo.SetAsWindowless(temp_handle, renderer_.IsTransparent());
  client = client_handler_;
}

void BrowserWindowOsrMac::ShowPopup(ClientWindowHandle parent_handle,
                                    int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();
  DCHECK(browser_.get());

  // Create the native NSView.
  Create(parent_handle,
         CefRect(x, y, static_cast<int>(width), static_cast<int>(height)));

  // Send resize notification so the compositor is assigned the correct
  // viewport size and begins rendering.
  browser_->GetHost()->WasResized();

  Show();
}

void BrowserWindowOsrMac::Show() {
  REQUIRE_MAIN_THREAD();

  if (hidden_) {
    // Set the browser as visible.
    browser_->GetHost()->SetWindowVisibility(true);
    hidden_ = false;
  }

  // Give focus to the browser.
  browser_->GetHost()->SendFocusEvent(true);
}

void BrowserWindowOsrMac::Hide() {
  REQUIRE_MAIN_THREAD();
  
  if (!browser_.get())
    return;

  // Remove focus from the browser.
  browser_->GetHost()->SendFocusEvent(false);

  if (!hidden_) {
    // Set the browser as hidden.
    browser_->GetHost()->SetWindowVisibility(false);
    hidden_ = true;
  }
}

void BrowserWindowOsrMac::SetBounds(int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();
  // Nothing to do here. GTK will take care of positioning in the container.
}

void BrowserWindowOsrMac::SetFocus(bool focus) {
  REQUIRE_MAIN_THREAD();
  if (nsview_)
    [[nsview_ window] makeFirstResponder:nsview_];
}

void BrowserWindowOsrMac::SetDeviceScaleFactor(float device_scale_factor) {
  REQUIRE_MAIN_THREAD();
  if (nsview_)
    [GLView(nsview_) setDeviceScaleFactor:device_scale_factor];
}

float BrowserWindowOsrMac::GetDeviceScaleFactor() const {
  REQUIRE_MAIN_THREAD();
  if (nsview_)
    return [GLView(nsview_) getDeviceScaleFactor];
  return 1.0f;
}

ClientWindowHandle BrowserWindowOsrMac::GetWindowHandle() const {
  REQUIRE_MAIN_THREAD();
  return nsview_;
}

void BrowserWindowOsrMac::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
}

void BrowserWindowOsrMac::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  // Detach |this| from the ClientHandlerOsr.
  static_cast<ClientHandlerOsr*>(client_handler_.get())->DetachOsrDelegate();
}

bool BrowserWindowOsrMac::GetRootScreenRect(CefRefPtr<CefBrowser> browser,
                                            CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  return false;
}

bool BrowserWindowOsrMac::GetViewRect(CefRefPtr<CefBrowser> browser,
                                      CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  if (!nsview_)
    return false;

  const float device_scale_factor = [GLView(nsview_) getDeviceScaleFactor];

  // |bounds| is in OS X view coordinates.
  NSRect bounds = [nsview_ bounds];

  // Convert to device coordinates.
  bounds = [GLView(nsview_) convertRectToBackingInternal:bounds];

  // Convert to browser view coordinates.
  rect.x = rect.y = 0;
  rect.width = DeviceToLogical(bounds.size.width, device_scale_factor);
  rect.height = DeviceToLogical(bounds.size.height, device_scale_factor);

  return true;
}

bool BrowserWindowOsrMac::GetScreenPoint(CefRefPtr<CefBrowser> browser,
                                         int viewX,
                                         int viewY,
                                         int& screenX,
                                         int& screenY) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  if (!nsview_)
    return false;

  const float device_scale_factor = [GLView(nsview_) getDeviceScaleFactor];

  // (viewX, viewX) is in browser view coordinates.
  // Convert to device coordinates.
  NSPoint view_pt = NSMakePoint(
      LogicalToDevice(viewX, device_scale_factor),
      LogicalToDevice(viewY, device_scale_factor));

  // Convert to OS X view coordinates.
  view_pt = [GLView(nsview_) convertPointFromBackingInternal:view_pt];

  // Reverse the Y component.
  const NSRect bounds = [nsview_ bounds];
  view_pt.y = bounds.size.height - view_pt.y;

  // Convert to screen coordinates.
  NSPoint window_pt = [nsview_ convertPoint:view_pt toView:nil];
  NSPoint screen_pt = [[nsview_ window] convertBaseToScreen:window_pt];

  screenX = screen_pt.x;
  screenY = screen_pt.y;
  return true;
}

bool BrowserWindowOsrMac::GetScreenInfo(CefRefPtr<CefBrowser> browser,
                                        CefScreenInfo& screen_info) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  if (!nsview_)
    return false;

  CefRect view_rect;
  GetViewRect(browser, view_rect);

  screen_info.device_scale_factor = [GLView(nsview_) getDeviceScaleFactor];

  // The screen info rectangles are used by the renderer to create and position
  // popups. Keep popups inside the view rectangle.
  screen_info.rect = view_rect;
  screen_info.available_rect = view_rect;

  return true;
}

void BrowserWindowOsrMac::OnPopupShow(CefRefPtr<CefBrowser> browser,
                                      bool show) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  if (!nsview_)
    return;

  if (!show) {
    renderer_.ClearPopupRects();
    browser->GetHost()->Invalidate(PET_VIEW);
  }
  renderer_.OnPopupShow(browser, show);
}

void BrowserWindowOsrMac::OnPopupSize(CefRefPtr<CefBrowser> browser,
                                      const CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  if (!nsview_)
    return;

  const float device_scale_factor = [GLView(nsview_) getDeviceScaleFactor];

  // |rect| is in browser view coordinates. Convert to device coordinates.
  CefRect device_rect = LogicalToDevice(rect, device_scale_factor);

  renderer_.OnPopupSize(browser, device_rect);
}

void BrowserWindowOsrMac::OnPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    const void* buffer,
    int width,
    int height) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  if (!nsview_)
    return;

  if (width <= 2 && height <= 2) {
    // Ignore really small buffer sizes while the widget is starting up.
    return;
  }

  if (painting_popup_) {
    renderer_.OnPaint(browser, type, dirtyRects, buffer, width, height);
    return;
  }

  ScopedGLContext scoped_gl_context(GLView(nsview_), true);

  renderer_.OnPaint(browser, type, dirtyRects, buffer, width, height);
  if (type == PET_VIEW && !renderer_.popup_rect().IsEmpty()) {
    painting_popup_ = true;
    browser->GetHost()->Invalidate(PET_POPUP);
    painting_popup_ = false;
  }
  renderer_.Render();
}

void BrowserWindowOsrMac::OnCursorChange(
    CefRefPtr<CefBrowser> browser,
    CefCursorHandle cursor,
    CefRenderHandler::CursorType type,
    const CefCursorInfo& custom_cursor_info) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  [cursor set];
}

bool BrowserWindowOsrMac::StartDragging(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDragData> drag_data,
    CefRenderHandler::DragOperationsMask allowed_ops,
    int x, int y) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  if (!nsview_)
    return false;

  static float device_scale_factor = [GLView(nsview_) getDeviceScaleFactor];

  // |point| is in browser view coordinates.
  NSPoint point = NSMakePoint(x, y);

  // Convert to device coordinates.
  point.x = LogicalToDevice(point.x, device_scale_factor);
  point.y = LogicalToDevice(point.y, device_scale_factor);

  // Convert to OS X view coordinates.
  point = [GLView(nsview_) convertPointFromBackingInternal:point];

  return [GLView(nsview_)
      startDragging:drag_data
         allowedOps:static_cast<NSDragOperation>(allowed_ops)
              point:point];
}

void BrowserWindowOsrMac::UpdateDragCursor(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::DragOperation operation) {
  CEF_REQUIRE_UI_THREAD();
  REQUIRE_MAIN_THREAD();

  if (nsview_)
    [GLView(nsview_) setCurrentDragOp:operation];
}

void BrowserWindowOsrMac::Create(ClientWindowHandle parent_handle,
                                 const CefRect& rect) {
  REQUIRE_MAIN_THREAD();
  DCHECK(!nsview_);

  NSRect window_rect = NSMakeRect(rect.x, rect.y, rect.width, rect.height);
  nsview_ =
      [[BrowserOpenGLView alloc] initWithFrame:window_rect
                              andBrowserWindow:this
                                   andRenderer:&renderer_];
  [nsview_ setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
  [nsview_ setAutoresizesSubviews: true];
  [parent_handle addSubview:nsview_];

  // Determine the default scale factor.
  [GLView(nsview_) resetDeviceScaleFactor];

  // Backing property notifications crash on 10.6 when building with the 10.7
  // SDK, see http://crbug.com/260595.
  static BOOL supportsBackingPropertiesNotification =
      SupportsBackingPropertiesChangedNotification();
  if (supportsBackingPropertiesNotification) {
    [[NSNotificationCenter defaultCenter]
      addObserver:nsview_
         selector:@selector(windowDidChangeBackingProperties:)
             name:NSWindowDidChangeBackingPropertiesNotification
           object:[nsview_ window]];
  }
}

}  // namespace client
