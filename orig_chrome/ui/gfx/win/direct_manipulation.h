// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_WIN_DIRECT_MANIPULATION_H_
#define UI_GFX_WIN_DIRECT_MANIPULATION_H_

#include <directmanipulation.h>

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/win/scoped_comptr.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/gfx_export.h"

namespace gfx {
namespace win {

    // Windows 10 provides a new API called Direct Manipulation which generates
    // smooth scroll events via WM_MOUSEWHEEL messages with predictable deltas
    // on high precision touch pads. This basically requires the application window
    // to register as a Direct Manipulation consumer. The way mouse wheel messages
    // are dispatched is
    // 1. The foreground window is checked to see if it is a Direct Manipulation
    //    consumer.
    // 2. If it is then Direct Manipulation takes over and sends the following
    //    messages. WM_POINTERACTIVATE, WM_POINTERDOWN and DM_POINTERHITTEST.
    // 3. It then posts WM_MOUSEWHEEL messages with precision deltas which vary
    //    based on the amount of the scroll.
    // 4. If the foreground window is not a Direct Manipulation consumer, it
    //    then takes a fallback route where it posts WM_MOUSEWHEEL messages
    //    with precision but varying deltas to the window. There is a also
    //    a slight delay in receiving the first set of mouse wheel messages.
    //    This causes scrolling to appear janky and jumpy.
    // Our approach for addressing this is to do the absolute minimum to
    // register our window as a Direct Manipulation consumer. This class
    // provides the necessary functionality to register the passed in HWND as a
    // Direct Manipulation consumer. We don't rely on Direct manipulation
    // to do the smooth scrolling in the background thread as documented on
    // msdn.
    class GFX_EXPORT DirectManipulationHelper {
    public:
        // Creates an instance of this class if Direct Manipulation is enabled on
        // the platform. If not returns NULL.
        static scoped_ptr<DirectManipulationHelper> CreateInstance();

        // This function instantiates Direct Manipulation and creates a viewport for
        // the passed in |window|.
        // consumer. Most of the code is boiler plate and is based on the sample.
        void Initialize(HWND window);

        // Sets the bounds of the fake Direct manipulation viewport to match those
        // of the legacy window.
        void SetBounds(const gfx::Rect& bounds);

        // Registers and activates the passed in |window| as a Direct Manipulation
        // consumer.
        void Activate(HWND window);

        // Deactivates Direct Manipulation processing on the passed in |window|.
        void Deactivate(HWND window);

        // Passes the WM_MOUSEWHEEL messages to Direct Manipulation. This is for
        // logistics purposes.
        void HandleMouseWheel(HWND window, UINT message, WPARAM w_param,
            LPARAM l_param);

        ~DirectManipulationHelper();

    private:
        DirectManipulationHelper();

        base::win::ScopedComPtr<IDirectManipulationManager2> manager_;
        base::win::ScopedComPtr<IDirectManipulationCompositor> compositor_;
        base::win::ScopedComPtr<IDirectManipulationUpdateManager> update_manager_;
        base::win::ScopedComPtr<IDirectManipulationFrameInfoProvider> frame_info_;
        base::win::ScopedComPtr<IDirectManipulationViewport2> view_port_outer_;

        DISALLOW_COPY_AND_ASSIGN(DirectManipulationHelper);
    };

} // namespace win
} // namespace gfx

#endif // UI_GFX_WIN_DIRECT_MANIPULATION_H_
