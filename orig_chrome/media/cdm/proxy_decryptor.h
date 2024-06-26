// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CDM_PROXY_DECRYPTOR_H_
#define MEDIA_CDM_PROXY_DECRYPTOR_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/memory/weak_ptr.h"
#include "media/base/cdm_context.h"
#include "media/base/decryptor.h"
#include "media/base/eme_constants.h"
#include "media/base/media_export.h"
#include "media/base/media_keys.h"
//#include "url/gurl.h"
#include "third_party/WebKit/public/platform/WebURL.h"

namespace media {

class CdmFactory;
class MediaPermission;

// ProxyDecryptor is for EME v0.1b only. It should not be used for the WD API.
// A decryptor proxy that creates a real decryptor object on demand and
// forwards decryptor calls to it.
//
// TODO(xhwang): Currently we don't support run-time switching among decryptor
// objects. Fix this when needed.
// TODO(xhwang): The ProxyDecryptor is not a Decryptor. Find a better name!
class MEDIA_EXPORT ProxyDecryptor {
public:
    // Callback to provide a CdmContext when the CDM creation is finished.
    // If CDM creation failed, |cdm_context| will be null.
    typedef base::Callback<void(CdmContext* cdm_context)> CdmContextReadyCB;

    // These are similar to the callbacks in media_keys.h, but pass back the
    // session ID rather than the internal session ID.
    typedef base::Callback<void(const std::string& session_id)> KeyAddedCB;
    typedef base::Callback<void(const std::string& session_id,
        MediaKeys::KeyError error_code,
        uint32 system_code)>
        KeyErrorCB;
    typedef base::Callback<void(const std::string& session_id,
        const std::vector<uint8>& message,
        const blink::WebURL& destination_url)>
        KeyMessageCB;

    ProxyDecryptor(MediaPermission* media_permission,
        bool use_hw_secure_codecs,
        const KeyAddedCB& key_added_cb,
        const KeyErrorCB& key_error_cb,
        const KeyMessageCB& key_message_cb);
    virtual ~ProxyDecryptor();

    // Creates the CDM and fires |cdm_created_cb|. This method should only be
    // called once. If CDM creation failed, all following GenerateKeyRequest,
    // AddKey and CancelKeyRequest calls will result in a KeyError.
    void CreateCdm(CdmFactory* cdm_factory,
        const std::string& key_system,
        const blink::WebURL& security_origin,
        const CdmContextReadyCB& cdm_context_ready_cb);

    // May only be called after CreateCDM().
    void GenerateKeyRequest(EmeInitDataType init_data_type,
        const uint8* init_data,
        int init_data_length);
    void AddKey(const uint8* key, int key_length,
        const uint8* init_data, int init_data_length,
        const std::string& session_id);
    void CancelKeyRequest(const std::string& session_id);

private:
    // Callback for CreateCdm().
    void OnCdmCreated(const std::string& key_system,
        const blink::WebURL& security_origin,
        const CdmContextReadyCB& cdm_context_ready_cb,
        const scoped_refptr<MediaKeys>& cdm,
        const std::string& error_message);

    void GenerateKeyRequestInternal(EmeInitDataType init_data_type,
        const std::vector<uint8>& init_data);

    // Callbacks for firing session events.
    void OnSessionMessage(const std::string& session_id,
        MediaKeys::MessageType message_type,
        const std::vector<uint8>& message,
        const blink::WebURL& legacy_destination_url);
    void OnSessionKeysChange(const std::string& session_id,
        bool has_additional_usable_key,
        CdmKeysInfo keys_info);
    void OnSessionExpirationUpdate(const std::string& session_id,
        const base::Time& new_expiry_time);
    void GenerateKeyAdded(const std::string& session_id);
    void OnSessionClosed(const std::string& session_id);
    void OnLegacySessionError(const std::string& session_id,
        MediaKeys::Exception exception_code,
        uint32 system_code,
        const std::string& error_message);

    // Callback for permission request.
    void OnPermissionStatus(MediaKeys::SessionType session_type,
        EmeInitDataType init_data_type,
        const std::vector<uint8>& init_data,
        scoped_ptr<NewSessionCdmPromise> promise,
        bool granted);

    enum SessionCreationType {
        TemporarySession,
        PersistentSession,
        LoadSession
    };

    // Called when a session is actually created or loaded.
    void SetSessionId(SessionCreationType session_type,
        const std::string& session_id);

    struct PendingGenerateKeyRequestData {
        PendingGenerateKeyRequestData(EmeInitDataType init_data_type,
            const std::vector<uint8>& init_data);
        ~PendingGenerateKeyRequestData();

        const EmeInitDataType init_data_type;
        const std::vector<uint8> init_data;
    };

    bool is_creating_cdm_;

    // The real MediaKeys that manages key operations for the ProxyDecryptor.
    scoped_refptr<MediaKeys> media_keys_;

#if defined(OS_CHROMEOS) || defined(OS_ANDROID)
    MediaPermission* media_permission_;
#endif

    bool use_hw_secure_codecs_;

    // Callbacks for firing key events.
    KeyAddedCB key_added_cb_;
    KeyErrorCB key_error_cb_;
    KeyMessageCB key_message_cb_;

    std::string key_system_;
    blink::WebURL security_origin_;

    // Keep track of both persistent and non-persistent sessions.
    base::hash_map<std::string, bool> active_sessions_;

    bool is_clear_key_;

    ScopedVector<PendingGenerateKeyRequestData> pending_requests_;

    // NOTE: Weak pointers must be invalidated before all other member variables.
    base::WeakPtrFactory<ProxyDecryptor> weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(ProxyDecryptor);
};

} // namespace media

#endif // MEDIA_CDM_PROXY_DECRYPTOR_H_
