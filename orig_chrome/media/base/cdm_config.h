// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_CDM_CONFIG_H_
#define MEDIA_BASE_CDM_CONFIG_H_

namespace media {

// The runtime configuration for new CDM instances as computed by
// |requestMediaKeySystemAccess|. This is in some sense the Chromium-side
// counterpart of Blink's WebMediaKeySystemConfiguration.
struct CdmConfig {
    // Allow access to a distinctive identifier.
    bool allow_distinctive_identifier = false;

    // Allow access to persistent state.
    bool allow_persistent_state = false;

    // Use hardware-secure codecs. This flag is only used on Android, it should
    // always be false on other platforms.
    bool use_hw_secure_codecs = false;
};

} // namespace media

#endif // MEDIA_BASE_CDM_CONFIG_H_
