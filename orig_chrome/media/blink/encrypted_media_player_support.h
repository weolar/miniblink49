// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_ENCRYPTED_MEDIA_PLAYER_SUPPORT_H_
#define MEDIA_BLINK_ENCRYPTED_MEDIA_PLAYER_SUPPORT_H_

#include <string>
#include <vector>

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "media/base/cdm_context.h"
#include "media/base/cdm_factory.h"
#include "media/base/demuxer.h"
#include "media/base/eme_constants.h"
#include "media/cdm/proxy_decryptor.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModuleResult.h"
#include "third_party/WebKit/public/platform/WebMediaPlayer.h"

namespace blink {
class WebContentDecryptionModule;
class WebLocalFrame;
class WebMediaPlayerEncryptedMediaClient;
class WebString;
}

namespace media {

class MediaPermission;
class WebContentDecryptionModuleImpl;

// Provides support to prefixed EME implementation.
// Do NOT add unprefixed EME functionality to this class!
// TODO(xhwang): When deprecating prefixed EME support, drop this whole file.
class EncryptedMediaPlayerSupport
    : public base::SupportsWeakPtr<EncryptedMediaPlayerSupport> {
public:
    using CdmContextReadyCB = ProxyDecryptor::CdmContextReadyCB;

    // |cdm_context_ready_cb| is called when the CDM instance creation completes.
    EncryptedMediaPlayerSupport(CdmFactory* cdm_factory,
        blink::WebMediaPlayerEncryptedMediaClient* client,
        MediaPermission* media_permission,
        const CdmContextReadyCB& cdm_context_ready_cb);
    ~EncryptedMediaPlayerSupport();

    blink::WebMediaPlayer::MediaKeyException GenerateKeyRequest(
        blink::WebLocalFrame* frame,
        const blink::WebString& key_system,
        const unsigned char* init_data,
        unsigned init_data_length);

    blink::WebMediaPlayer::MediaKeyException AddKey(
        const blink::WebString& key_system,
        const unsigned char* key,
        unsigned key_length,
        const unsigned char* init_data,
        unsigned init_data_length,
        const blink::WebString& session_id);

    blink::WebMediaPlayer::MediaKeyException CancelKeyRequest(
        const blink::WebString& key_system,
        const blink::WebString& session_id);

    void SetInitDataType(EmeInitDataType init_data_type);

private:
    blink::WebMediaPlayer::MediaKeyException GenerateKeyRequestInternal(
        blink::WebLocalFrame* frame,
        const std::string& key_system,
        const unsigned char* init_data,
        unsigned init_data_length);

    blink::WebMediaPlayer::MediaKeyException AddKeyInternal(
        const std::string& key_system,
        const unsigned char* key,
        unsigned key_length,
        const unsigned char* init_data,
        unsigned init_data_length,
        const std::string& session_id);

    blink::WebMediaPlayer::MediaKeyException CancelKeyRequestInternal(
        const std::string& key_system,
        const std::string& session_id);

    void OnKeyAdded(const std::string& session_id);
    void OnKeyError(const std::string& session_id,
        MediaKeys::KeyError error_code,
        uint32 system_code);
    void OnKeyMessage(const std::string& session_id,
        const std::vector<uint8>& message,
        const blink::WebURL& destination_url);

    CdmFactory* cdm_factory_;

    blink::WebMediaPlayerEncryptedMediaClient* client_;

    MediaPermission* media_permission_;

    // The currently selected key system. Empty string means that no key system
    // has been selected.
    std::string current_key_system_;

    // We assume all streams are from the same container, thus have the same
    // init data type.
    EmeInitDataType init_data_type_;

    CdmContextReadyCB cdm_context_ready_cb_;

    // Manages decryption keys and decrypts encrypted frames.
    scoped_ptr<ProxyDecryptor> proxy_decryptor_;

    DISALLOW_COPY_AND_ASSIGN(EncryptedMediaPlayerSupport);
};

} // namespace media

#endif // MEDIA_BLINK_ENCRYPTED_MEDIA_PLAYER_SUPPORT_H_
