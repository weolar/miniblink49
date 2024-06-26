// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_KEY_SYSTEM_INFO_H_
#define MEDIA_BASE_KEY_SYSTEM_INFO_H_

#include <string>

#include "build/build_config.h"
#include "media/base/eme_constants.h"
#include "media/base/media_export.h"

// Definitions:
// * Key system
//    https://dvcs.w3.org/hg/html-media/raw-file/default/encrypted-media/encrypted-media.html#key-system
// * Concrete key system
//    A key system string that can be instantiated, such as
//    via the MediaKeys constructor. Examples include "org.w3.clearkey" and
//    "com.widevine.alpha".
// * Abstract key system
//    A key system string that cannot be instantiated like a concrete key system
//    but is otherwise useful, such as in discovery using isTypeSupported().
// * Parent key system
//    A key system string that is one level up from the child key system. It may
//    be an abstract key system.
//    As an example, "com.example" is the parent of "com.example.foo".

namespace media {

// Contains information about an EME key system as well as how to instantiate
// the corresponding CDM.
struct MEDIA_EXPORT KeySystemInfo {
    KeySystemInfo();
    ~KeySystemInfo();

    std::string key_system;

    InitDataTypeMask supported_init_data_types = kInitDataTypeMaskNone;
    SupportedCodecs supported_codecs = EME_CODEC_NONE;
#if defined(OS_ANDROID)
    SupportedCodecs supported_secure_codecs = EME_CODEC_NONE;
#endif // defined(OS_ANDROID)
    EmeRobustness max_audio_robustness = EmeRobustness::INVALID;
    EmeRobustness max_video_robustness = EmeRobustness::INVALID;
    EmeSessionTypeSupport persistent_license_support = EmeSessionTypeSupport::INVALID;
    EmeSessionTypeSupport persistent_release_message_support = EmeSessionTypeSupport::INVALID;
    EmeFeatureSupport persistent_state_support = EmeFeatureSupport::INVALID;
    EmeFeatureSupport distinctive_identifier_support = EmeFeatureSupport::INVALID;

    // A hierarchical parent for |key_system|. This value can be used to check
    // supported types but cannot be used to instantiate a MediaKeys object.
    // Only one parent key system is currently supported per concrete key system.
    std::string parent_key_system;

    // The following indicate how the corresponding CDM should be instantiated.
    bool use_aes_decryptor = false;
#if defined(ENABLE_PEPPER_CDMS)
    std::string pepper_type;
#endif
};

} // namespace media

#endif // MEDIA_BASE_KEY_SYSTEM_INFO_H_
