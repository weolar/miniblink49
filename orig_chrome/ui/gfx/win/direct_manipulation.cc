// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/win/direct_manipulation.h"

#include "base/win/windows_version.h"

namespace gfx {
namespace win {

    // static
    scoped_ptr<DirectManipulationHelper>
    DirectManipulationHelper::CreateInstance()
    {
        scoped_ptr<DirectManipulationHelper> instance;

        if (base::win::GetVersion() >= base::win::VERSION_WIN10)
            instance.reset(new DirectManipulationHelper);

        return instance;
    }

    DirectManipulationHelper::DirectManipulationHelper() { }

    DirectManipulationHelper::~DirectManipulationHelper() { }

    void DirectManipulationHelper::Initialize(HWND window)
    {
        DCHECK(::IsWindow(window));

        // TODO(ananta)
        // Remove the CHECK statements here and below and replace them with logs
        // when this code stabilizes.
        HRESULT hr = manager_.CreateInstance(CLSID_DirectManipulationManager,
            nullptr, CLSCTX_INPROC_SERVER);
        CHECK(SUCCEEDED(hr));

        hr = compositor_.CreateInstance(CLSID_DCompManipulationCompositor,
            nullptr, CLSCTX_INPROC_SERVER);
        CHECK(SUCCEEDED(hr));

        hr = manager_->GetUpdateManager(IID_PPV_ARGS(update_manager_.Receive()));
        CHECK(SUCCEEDED(hr));

        hr = compositor_->SetUpdateManager(update_manager_.get());
        CHECK(SUCCEEDED(hr));

        hr = frame_info_.QueryFrom(compositor_.get());
        CHECK(SUCCEEDED(hr));

        hr = manager_->CreateViewport(frame_info_.get(), window,
            IID_PPV_ARGS(view_port_outer_.Receive()));
        CHECK(SUCCEEDED(hr));

        //
        // Enable the desired configuration for each viewport.
        //
        DIRECTMANIPULATION_CONFIGURATION configuration = DIRECTMANIPULATION_CONFIGURATION_INTERACTION
            | DIRECTMANIPULATION_CONFIGURATION_TRANSLATION_X
            | DIRECTMANIPULATION_CONFIGURATION_TRANSLATION_Y
            | DIRECTMANIPULATION_CONFIGURATION_TRANSLATION_INERTIA
            | DIRECTMANIPULATION_CONFIGURATION_RAILS_X
            | DIRECTMANIPULATION_CONFIGURATION_RAILS_Y
            | DIRECTMANIPULATION_CONFIGURATION_SCALING
            | DIRECTMANIPULATION_CONFIGURATION_SCALING_INERTIA;

        hr = view_port_outer_->ActivateConfiguration(configuration);
        CHECK(SUCCEEDED(hr));
    }

    void DirectManipulationHelper::SetBounds(const gfx::Rect& bounds)
    {
        base::win::ScopedComPtr<IDirectManipulationPrimaryContent>
            primary_content_outer;
        HRESULT hr = view_port_outer_->GetPrimaryContent(
            IID_PPV_ARGS(primary_content_outer.Receive()));
        CHECK(SUCCEEDED(hr));

        base::win::ScopedComPtr<IDirectManipulationContent> content_outer;
        hr = content_outer.QueryFrom(primary_content_outer.get());
        CHECK(SUCCEEDED(hr));

        RECT rect = bounds.ToRECT();

        hr = view_port_outer_->SetViewportRect(&rect);
        CHECK(SUCCEEDED(hr));

        hr = content_outer->SetContentRect(&rect);
        CHECK(SUCCEEDED(hr));
    }

    void DirectManipulationHelper::Activate(HWND window)
    {
        DCHECK(::IsWindow(window));
        manager_->Activate(window);
    }

    void DirectManipulationHelper::Deactivate(HWND window)
    {
        DCHECK(::IsWindow(window));
        manager_->Deactivate(window);
    }

    void DirectManipulationHelper::HandleMouseWheel(HWND window, UINT message,
        WPARAM w_param, LPARAM l_param)
    {
        MSG msg = { window, message, w_param, l_param };

        HRESULT hr = view_port_outer_->SetContact(DIRECTMANIPULATION_MOUSEFOCUS);
        if (SUCCEEDED(hr)) {
            BOOL handled = FALSE;
            manager_->ProcessInput(&msg, &handled);
            view_port_outer_->ReleaseContact(DIRECTMANIPULATION_MOUSEFOCUS);
        }
    }

} // namespace win.
} // namespace gfx.
