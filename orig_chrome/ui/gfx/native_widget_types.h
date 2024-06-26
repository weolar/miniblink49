// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_NATIVE_WIDGET_TYPES_H_
#define UI_GFX_NATIVE_WIDGET_TYPES_H_

#include <stdint.h>

#include "base/logging.h"
#include "build/build_config.h"
#include "ui/gfx/gfx_export.h"

#if defined(OS_ANDROID)
#include <jni.h>
#endif

// This file provides cross platform typedefs for native widget types.
//   NativeWindow: this is a handle to a native, top-level window
//   NativeView: this is a handle to a native UI element. It may be the
//     same type as a NativeWindow on some platforms.
//   NativeViewId: Often, in our cross process model, we need to pass around a
//     reference to a "window". This reference will, say, be echoed back from a
//     renderer to the browser when it wishes to query its size. On Windows we
//     use an HWND for this.
//
//     As a rule of thumb - if you're in the renderer, you should be dealing
//     with NativeViewIds. This should remind you that you shouldn't be doing
//     direct operations on platform widgets from the renderer process.
//
//     If you're in the browser, you're probably dealing with NativeViews,
//     unless you're in the IPC layer, which will be translating between
//     NativeViewIds from the renderer and NativeViews.
//
//   NativeImage: The platform-specific image type used for drawing UI elements
//     in the browser.
//
// The name 'View' here meshes with OS X where the UI elements are called
// 'views' and with our Chrome UI code where the elements are also called
// 'views'.

#if defined(USE_AURA)
class SkRegion;
namespace aura {
class Window;
}
namespace ui {
class Cursor;
class Event;
}
#endif // defined(USE_AURA)

#if defined(OS_WIN)
#include <windows.h> // NOLINT
typedef struct HFONT__* HFONT;
struct IAccessible;
#elif defined(OS_IOS)
struct CGContext;
#ifdef __OBJC__
@class UIEvent;
@class UIFont;
@class UIImage;
@class UIView;
@class UIWindow;
@class UITextField;
#else
class UIEvent;
class UIFont;
class UIImage;
class UIView;
class UIWindow;
class UITextField;
#endif // __OBJC__
#elif defined(OS_MACOSX)
struct CGContext;
#ifdef __OBJC__
@class NSCursor;
@class NSEvent;
@class NSFont;
@class NSImage;
@class NSView;
@class NSWindow;
@class NSTextField;
#else
class NSCursor;
class NSEvent;
class NSFont;
class NSImage;
struct NSView;
class NSWindow;
class NSTextField;
#endif // __OBJC__
#elif defined(OS_POSIX)
typedef struct _cairo cairo_t;
#endif

#if defined(OS_ANDROID)
struct ANativeWindow;
namespace ui {
class WindowAndroid;
class ViewAndroid;
}
#endif
class SkBitmap;

#if defined(USE_X11) && !defined(OS_CHROMEOS)
extern "C" {
struct _AtkObject;
typedef struct _AtkObject AtkObject;
}
#endif

namespace gfx {

#if defined(USE_AURA)
typedef ui::Cursor NativeCursor;
typedef aura::Window* NativeView;
typedef aura::Window* NativeWindow;
typedef ui::Event* NativeEvent;
#elif defined(OS_IOS)
typedef void* NativeCursor;
typedef UIView* NativeView;
typedef UIWindow* NativeWindow;
typedef UIEvent* NativeEvent;
#elif defined(OS_MACOSX)
typedef NSCursor* NativeCursor;
typedef NSView* NativeView;
typedef NSWindow* NativeWindow;
typedef NSEvent* NativeEvent;
#elif defined(OS_ANDROID)
typedef void* NativeCursor;
typedef ui::ViewAndroid* NativeView;
typedef ui::WindowAndroid* NativeWindow;
typedef jobject NativeEvent;
#else
#error Unknown build environment.
#endif

#if defined(OS_WIN)
typedef HFONT NativeFont;
typedef HDC NativeDrawingContext;
typedef IAccessible* NativeViewAccessible;
#elif defined(OS_IOS)
typedef UIFont* NativeFont;
typedef CGContext* NativeDrawingContext;
#ifdef __OBJC__
typedef id NativeViewAccessible;
#else
typedef void* NativeViewAccessible;
#endif // __OBJC__
#elif defined(OS_MACOSX)
typedef NSFont* NativeFont;
typedef CGContext* NativeDrawingContext;
#ifdef __OBJC__
typedef id NativeViewAccessible;
#else
typedef void* NativeViewAccessible;
#endif // __OBJC__
#else // Android, Linux, Chrome OS, etc.
// Linux doesn't have a native font type.
#if defined(USE_CAIRO)
typedef cairo_t* NativeDrawingContext;
#else
typedef void* NativeDrawingContext;
#endif // defined(USE_CAIRO)
#if defined(USE_X11) && !defined(OS_CHROMEOS)
typedef AtkObject* NativeViewAccessible;
#else
typedef void* NativeViewAccessible;
#endif
#endif

// A constant value to indicate that gfx::NativeCursor refers to no cursor.
#if defined(USE_AURA)
const int kNullCursor = 0;
#else
const gfx::NativeCursor kNullCursor = static_cast<gfx::NativeCursor>(NULL);
#endif

#if defined(OS_IOS)
typedef UIImage NativeImageType;
#elif defined(OS_MACOSX)
typedef NSImage NativeImageType;
#else
typedef SkBitmap NativeImageType;
#endif
typedef NativeImageType* NativeImage;

// Note: for test_shell we're packing a pointer into the NativeViewId. So, if
// you make it a type which is smaller than a pointer, you have to fix
// test_shell.
//
// See comment at the top of the file for usage.
typedef intptr_t NativeViewId;

// PluginWindowHandle is an abstraction wrapping "the types of windows
// used by NPAPI plugins". On Windows it's an HWND, on X it's an X
// window id.
#if defined(OS_WIN)
typedef HWND PluginWindowHandle;
const PluginWindowHandle kNullPluginWindow = NULL;
#elif defined(USE_X11)
typedef unsigned long PluginWindowHandle;
const PluginWindowHandle kNullPluginWindow = 0;
#elif defined(OS_ANDROID)
typedef uint32_t PluginWindowHandle;
const PluginWindowHandle kNullPluginWindow = 0;
#elif defined(USE_OZONE)
typedef intptr_t PluginWindowHandle;
const PluginWindowHandle kNullPluginWindow = 0;
#else
typedef uint32_t PluginWindowHandle;
const PluginWindowHandle kNullPluginWindow = 0;
#endif

enum SurfaceType {
    EMPTY,
    NATIVE_DIRECT,
    NULL_TRANSPORT,
    SURFACE_TYPE_LAST = NULL_TRANSPORT
};

struct GLSurfaceHandle {
    GLSurfaceHandle()
        : handle(kNullPluginWindow)
        , transport_type(EMPTY)
    {
    }
    GLSurfaceHandle(PluginWindowHandle handle_, SurfaceType transport_)
        : handle(handle_)
        , transport_type(transport_)
    {
        DCHECK(!is_null() || handle == kNullPluginWindow);
        DCHECK(transport_type != NULL_TRANSPORT || handle == kNullPluginWindow);
    }
    bool is_null() const { return transport_type == EMPTY; }
    bool is_transport() const
    {
        return transport_type == NULL_TRANSPORT;
    }
    PluginWindowHandle handle;
    SurfaceType transport_type;
};

// AcceleratedWidget provides a surface to compositors to paint pixels.
#if defined(OS_WIN)
typedef HWND AcceleratedWidget;
const AcceleratedWidget kNullAcceleratedWidget = NULL;
#elif defined(USE_X11)
typedef unsigned long AcceleratedWidget;
const AcceleratedWidget kNullAcceleratedWidget = 0;
#elif defined(OS_IOS)
typedef UIView* AcceleratedWidget;
const AcceleratedWidget kNullAcceleratedWidget = 0;
#elif defined(OS_MACOSX)
typedef NSView* AcceleratedWidget;
const AcceleratedWidget kNullAcceleratedWidget = 0;
#elif defined(OS_ANDROID)
typedef ANativeWindow* AcceleratedWidget;
const AcceleratedWidget kNullAcceleratedWidget = 0;
#elif defined(USE_OZONE)
typedef intptr_t AcceleratedWidget;
const AcceleratedWidget kNullAcceleratedWidget = 0;
#else
#error unknown platform
#endif

} // namespace gfx

#endif // UI_GFX_NATIVE_WIDGET_TYPES_H_
