// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/win/mf_initializer.h"

#include <mfapi.h>

#include "base/lazy_instance.h"
#include "base/macros.h"

namespace media {

namespace {

    // LazyInstance to initialize the Media Foundation Library.
    class MFInitializer {
    public:
        MFInitializer()
            : mf_started_(MFStartup(MF_VERSION, MFSTARTUP_LITE) == S_OK)
        {
        }

        ~MFInitializer()
        {
            if (mf_started_)
                MFShutdown();
        }

    private:
        const bool mf_started_;

        DISALLOW_COPY_AND_ASSIGN(MFInitializer);
    };

    base::LazyInstance<MFInitializer> g_mf_initializer = LAZY_INSTANCE_INITIALIZER;

} // namespace

void InitializeMediaFoundation()
{
    g_mf_initializer.Get();
}

} // namespace media
