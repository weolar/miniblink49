// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_MEDIA_CEF_KEY_SYSTEMS_H_
#define CEF_LIBCEF_RENDERER_MEDIA_CEF_KEY_SYSTEMS_H_

#include <vector>

#include "media/base/key_system_info.h"

void AddCefKeySystems(std::vector<media::KeySystemInfo>* key_systems_info);

#endif  // CEF_LIBCEF_RENDERER_MEDIA_CEF_KEY_SYSTEMS_H_
