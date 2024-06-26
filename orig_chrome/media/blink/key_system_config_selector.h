// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_KEY_SYSTEM_CONFIG_SELECTOR_H_
#define MEDIA_BLINK_KEY_SYSTEM_CONFIG_SELECTOR_H_

#include <string>
#include <vector>

#include "base/bind.h"
#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "media/base/eme_constants.h"
#include "media/blink/media_blink_export.h"
#include "third_party/WebKit/public/platform/WebVector.h"

namespace blink {

struct WebMediaKeySystemConfiguration;
struct WebMediaKeySystemMediaCapability;
class WebSecurityOrigin;
class WebString;

} // namespace blink

namespace media {

struct CdmConfig;
class KeySystems;
class MediaPermission;

class MEDIA_BLINK_EXPORT KeySystemConfigSelector {
public:
    KeySystemConfigSelector(
        const KeySystems* key_systems,
        MediaPermission* media_permission);

    ~KeySystemConfigSelector();

    void SelectConfig(
        const blink::WebString& key_system,
        const blink::WebVector<blink::WebMediaKeySystemConfiguration>&
            candidate_configurations,
        const blink::WebSecurityOrigin& security_origin,
        bool are_secure_codecs_supported,
        base::Callback<void(const blink::WebMediaKeySystemConfiguration&,
            const CdmConfig&)>
            succeeded_cb,
        base::Callback<void(const blink::WebString&)> not_supported_cb);

private:
    struct SelectionRequest;
    class ConfigState;

    enum ConfigurationSupport {
        CONFIGURATION_NOT_SUPPORTED,
        CONFIGURATION_REQUIRES_PERMISSION,
        CONFIGURATION_SUPPORTED,
    };

    void SelectConfigInternal(scoped_ptr<SelectionRequest> request);

    void OnPermissionResult(scoped_ptr<SelectionRequest> request,
        bool is_permission_granted);

    ConfigurationSupport GetSupportedConfiguration(
        const std::string& key_system,
        const blink::WebMediaKeySystemConfiguration& candidate,
        ConfigState* config_state,
        blink::WebMediaKeySystemConfiguration* accumulated_configuration);

    bool GetSupportedCapabilities(
        const std::string& key_system,
        EmeMediaType media_type,
        const blink::WebVector<blink::WebMediaKeySystemMediaCapability>&
            requested_media_capabilities,
        ConfigState* config_state,
        std::vector<blink::WebMediaKeySystemMediaCapability>*
            supported_media_capabilities);

    bool IsSupportedContentType(const std::string& key_system,
        EmeMediaType media_type,
        const std::string& container_mime_type,
        const std::string& codecs,
        ConfigState* config_state);

    const KeySystems* key_systems_;
    MediaPermission* media_permission_;
    base::WeakPtrFactory<KeySystemConfigSelector> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(KeySystemConfigSelector);
};

} // namespace media

#endif // MEDIA_BLINK_KEY_SYSTEM_CONFIG_SELECTOR_H_
