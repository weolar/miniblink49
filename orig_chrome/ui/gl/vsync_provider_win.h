// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/gl/gl_export.h"

namespace gfx {

class GL_EXPORT VSyncProviderWin : public VSyncProvider {
public:
    explicit VSyncProviderWin(gfx::AcceleratedWidget window);
    ~VSyncProviderWin() override;

    // VSyncProvider overrides;
    void GetVSyncParameters(const UpdateVSyncCallback& callback) override;

private:
    gfx::AcceleratedWidget window_;
    bool use_dwm_;

    DISALLOW_COPY_AND_ASSIGN(VSyncProviderWin);
};

} // namespace gfx
