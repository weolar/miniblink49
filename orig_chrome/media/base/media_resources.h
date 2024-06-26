// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_MEDIA_RESOURCES_H_
#define MEDIA_BASE_MEDIA_RESOURCES_H_

#include <string>

#include "base/strings/string16.h"
#include "media/base/media_export.h"

namespace media {

// The media layer can't access Chrome's resource bundle directly. This facility
// allows clients to provide indirect access.

// IDs that will get mapped to corresponding entries with IDS_ prefixes in
// chrome/app/generated_resources.grd.
enum MessageId {
    DEFAULT_AUDIO_DEVICE_NAME,
#if defined(OS_WIN)
    COMMUNICATIONS_AUDIO_DEVICE_NAME,
#endif
#if defined(OS_CHROMEOS)
    BEAMFORMING_ON_DEFAULT_AUDIO_INPUT_DEVICE_NAME,
    BEAMFORMING_OFF_DEFAULT_AUDIO_INPUT_DEVICE_NAME,
#endif
};

// Implementations are expected to convert MessageIds to generated_resources.grd
// IDs and extract the matching string from Chrome's resource bundle (e.g.
// through l10n_util::GetStringUTF16).
using LocalizedStringProvider = base::string16 (*)(MessageId message_id);

// Initializes the global LocalizedStringProvider function.
MEDIA_EXPORT void SetLocalizedStringProvider(LocalizedStringProvider func);

#if !defined(OS_IOS)
// The LocalizedStringProvider has probably not been initialized on iOS. This
// will give an early compile warning for clients attempting to use it.

// Returns a resource string corresponding to |message_id|. See l10n_util.h.
// Returns an empty string if the LocalizedStringProvider has not been
// initialized or if the ID is unrecognized.
std::string GetLocalizedStringUTF8(MessageId message_id);
base::string16 GetLocalizedStringUTF16(MessageId message_id);
#endif

} // namespace media

#endif // MEDIA_BASE_MEDIA_RESOURCES_H_
