// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_DISPLAY_ITEM_LIST_SETTINGS_H_
#define CC_PLAYBACK_DISPLAY_ITEM_LIST_SETTINGS_H_

#include <cstddef>

#include "cc/base/cc_export.h"

namespace cc {

class CC_EXPORT DisplayItemListSettings {
public:
    DisplayItemListSettings();
    ~DisplayItemListSettings();

    // If set, a picture will be cached inside the DisplayItemList.
    bool use_cached_picture;
};

} // namespace cc

#endif // CC_PLAYBACK_DISPLAY_ITEM_LIST_SETTINGS_H_
