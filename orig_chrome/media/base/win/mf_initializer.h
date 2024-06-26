// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_WIN_MF_INITIALIZER_H_
#define MEDIA_BASE_WIN_MF_INITIALIZER_H_

#include "media/base/media_export.h"

namespace media {

// Makes sure MFStartup() is called exactly once, and that this call is paired
// by a call to MFShutdown().
MEDIA_EXPORT void InitializeMediaFoundation();

} // namespace media

#endif // MEDIA_BASE_WIN_MF_INITIALIZER_H_
