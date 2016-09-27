// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebMediaKeySystemConfiguration_h
#define WebMediaKeySystemConfiguration_h

#include "public/platform/WebEncryptedMediaTypes.h"
#include "public/platform/WebMediaKeySystemMediaCapability.h"
#include "public/platform/WebVector.h"

namespace blink {

struct WebMediaKeySystemConfiguration {
    enum class Requirement {
        Required,
        Optional,
        NotAllowed,
    };

    // As MediaKeySystemConfiguration is a dictionary, some members may be
    // not-present. Because requestMediaKeySystemAccess() distinguishes empty
    // from not-present, we require a presence flag for each member.
    bool hasInitDataTypes = false;
    WebVector<WebEncryptedMediaInitDataType> initDataTypes;

    bool hasAudioCapabilities = false;
    WebVector<WebMediaKeySystemMediaCapability> audioCapabilities;

    bool hasVideoCapabilities = false;
    WebVector<WebMediaKeySystemMediaCapability> videoCapabilities;

    // |distinctiveIdentifier| and |persistentState| are always present because
    // they have default values.
    Requirement distinctiveIdentifier = Requirement::Optional;
    Requirement persistentState = Requirement::Optional;

    bool hasSessionTypes = false;
    WebVector<WebEncryptedMediaSessionType> sessionTypes;

    // |label| may be not-present, but we use a null string to represent that.
    WebString label;
};

} // namespace blink

#endif // WebMediaKeySystemConfiguration_h
