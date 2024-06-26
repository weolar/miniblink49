// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cdm/proxy_decryptor.h"

#include <cstring>

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/logging.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "media/base/cdm_callback_promise.h"
#include "media/base/cdm_config.h"
#include "media/base/cdm_factory.h"
#include "media/base/cdm_key_information.h"
#include "media/base/key_systems.h"
#include "media/base/media_permission.h"
#include "media/cdm/json_web_key.h"
#include "media/cdm/key_system_names.h"

namespace media {

// Special system code to signal a closed persistent session in a SessionError()
// call. This is needed because there is no SessionClosed() call in the prefixed
// EME API.
const int kSessionClosedSystemCode = 29127;

ProxyDecryptor::PendingGenerateKeyRequestData::PendingGenerateKeyRequestData(
    EmeInitDataType init_data_type,
    const std::vector<uint8>& init_data)
    : init_data_type(init_data_type)
    , init_data(init_data)
{
}

ProxyDecryptor::PendingGenerateKeyRequestData::
    ~PendingGenerateKeyRequestData()
{
}

ProxyDecryptor::ProxyDecryptor(MediaPermission* media_permission,
    bool use_hw_secure_codecs,
    const KeyAddedCB& key_added_cb,
    const KeyErrorCB& key_error_cb,
    const KeyMessageCB& key_message_cb)
    : is_creating_cdm_(false)
    ,
#if defined(OS_CHROMEOS) || defined(OS_ANDROID)
    media_permission_(media_permission)
    ,
#endif
    use_hw_secure_codecs_(use_hw_secure_codecs)
    , key_added_cb_(key_added_cb)
    , key_error_cb_(key_error_cb)
    , key_message_cb_(key_message_cb)
    , is_clear_key_(false)
    , weak_ptr_factory_(this)
{
    DCHECK(media_permission);
    DCHECK(!key_added_cb_.is_null());
    DCHECK(!key_error_cb_.is_null());
    DCHECK(!key_message_cb_.is_null());
}

ProxyDecryptor::~ProxyDecryptor()
{
    // Destroy the decryptor explicitly before destroying the plugin.
    media_keys_ = nullptr;
}

void ProxyDecryptor::CreateCdm(CdmFactory* cdm_factory,
    const std::string& key_system,
    const blink::WebURL& security_origin,
    const CdmContextReadyCB& cdm_context_ready_cb)
{
    DVLOG(1) << __FUNCTION__ << ": key_system = " << key_system;
    DCHECK(!is_creating_cdm_);
    DCHECK(!media_keys_);

    // TODO(sandersd): Trigger permissions check here and use it to determine
    // distinctive identifier support, instead of always requiring the
    // permission. http://crbug.com/455271
    CdmConfig cdm_config;
    cdm_config.allow_distinctive_identifier = true;
    cdm_config.allow_persistent_state = true;
    cdm_config.use_hw_secure_codecs = use_hw_secure_codecs_;

    is_creating_cdm_ = true;

    base::WeakPtr<ProxyDecryptor> weak_this = weak_ptr_factory_.GetWeakPtr();
    cdm_factory->Create(
        key_system, security_origin, cdm_config,
        base::Bind(&ProxyDecryptor::OnSessionMessage, weak_this),
        base::Bind(&ProxyDecryptor::OnSessionClosed, weak_this),
        base::Bind(&ProxyDecryptor::OnLegacySessionError, weak_this),
        base::Bind(&ProxyDecryptor::OnSessionKeysChange, weak_this),
        base::Bind(&ProxyDecryptor::OnSessionExpirationUpdate, weak_this),
        base::Bind(&ProxyDecryptor::OnCdmCreated, weak_this, key_system,
            security_origin, cdm_context_ready_cb));
}

void ProxyDecryptor::OnCdmCreated(const std::string& key_system,
    const GURL& security_origin,
    const CdmContextReadyCB& cdm_context_ready_cb,
    const scoped_refptr<MediaKeys>& cdm,
    const std::string& /* error_message */)
{
    is_creating_cdm_ = false;

    if (!cdm) {
        cdm_context_ready_cb.Run(nullptr);
    } else {
        key_system_ = key_system;
        security_origin_ = security_origin;
        is_clear_key_ = IsClearKey(key_system) || IsExternalClearKey(key_system);
        media_keys_ = cdm;

        cdm_context_ready_cb.Run(media_keys_->GetCdmContext());
    }

    for (const auto& request : pending_requests_)
        GenerateKeyRequestInternal(request->init_data_type, request->init_data);

    pending_requests_.clear();
}

void ProxyDecryptor::GenerateKeyRequest(EmeInitDataType init_data_type,
    const uint8* init_data,
    int init_data_length)
{
    std::vector<uint8> init_data_vector(init_data, init_data + init_data_length);

    if (is_creating_cdm_) {
        pending_requests_.push_back(
            new PendingGenerateKeyRequestData(init_data_type, init_data_vector));
        return;
    }

    GenerateKeyRequestInternal(init_data_type, init_data_vector);
}

// Returns true if |data| is prefixed with |header| and has data after the
// |header|.
static bool HasHeader(const std::vector<uint8>& data,
    const std::string& header)
{
    return data.size() > header.size() && std::equal(header.begin(), header.end(), data.begin());
}

// Removes the first |length| items from |data|.
static void StripHeader(std::vector<uint8>& data, size_t length)
{
    data.erase(data.begin(), data.begin() + length);
}

void ProxyDecryptor::GenerateKeyRequestInternal(
    EmeInitDataType init_data_type,
    const std::vector<uint8>& init_data)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(!is_creating_cdm_);

    if (!media_keys_) {
        OnLegacySessionError(std::string(), MediaKeys::NOT_SUPPORTED_ERROR, 0,
            "CDM creation failed.");
        return;
    }

    const char kPrefixedApiPersistentSessionHeader[] = "PERSISTENT|";
    const char kPrefixedApiLoadSessionHeader[] = "LOAD_SESSION|";

    SessionCreationType session_creation_type = TemporarySession;
    std::vector<uint8> stripped_init_data = init_data;
    if (HasHeader(init_data, kPrefixedApiLoadSessionHeader)) {
        session_creation_type = LoadSession;
        StripHeader(stripped_init_data, strlen(kPrefixedApiLoadSessionHeader));
    } else if (HasHeader(init_data, kPrefixedApiPersistentSessionHeader)) {
        session_creation_type = PersistentSession;
        StripHeader(stripped_init_data,
            strlen(kPrefixedApiPersistentSessionHeader));
    }

    scoped_ptr<NewSessionCdmPromise> promise(new CdmCallbackPromise<std::string>(
        base::Bind(&ProxyDecryptor::SetSessionId, weak_ptr_factory_.GetWeakPtr(),
            session_creation_type),
        base::Bind(&ProxyDecryptor::OnLegacySessionError,
            weak_ptr_factory_.GetWeakPtr(),
            std::string()))); // No session id until created.

    if (session_creation_type == LoadSession) {
        media_keys_->LoadSession(
            MediaKeys::PERSISTENT_LICENSE_SESSION,
            std::string(
                reinterpret_cast<const char*>(vector_as_array(&stripped_init_data)),
                stripped_init_data.size()),
            promise.Pass());
        return;
    }

    MediaKeys::SessionType session_type = session_creation_type == PersistentSession
        ? MediaKeys::PERSISTENT_LICENSE_SESSION
        : MediaKeys::TEMPORARY_SESSION;

    // No permission required when AesDecryptor is used or when the key system is
    // external clear key.
    DCHECK(!key_system_.empty());
    if (CanUseAesDecryptor(key_system_) || IsExternalClearKey(key_system_)) {
        OnPermissionStatus(session_type, init_data_type, stripped_init_data,
            promise.Pass(), true /* granted */);
        return;
    }

#if defined(OS_CHROMEOS) || defined(OS_ANDROID)
    media_permission_->RequestPermission(
        MediaPermission::PROTECTED_MEDIA_IDENTIFIER, security_origin_,
        base::Bind(&ProxyDecryptor::OnPermissionStatus,
            weak_ptr_factory_.GetWeakPtr(), session_type, init_data_type,
            stripped_init_data, base::Passed(&promise)));
#else
    OnPermissionStatus(session_type, init_data_type, stripped_init_data,
        promise.Pass(), true /* granted */);
#endif
}

void ProxyDecryptor::OnPermissionStatus(
    MediaKeys::SessionType session_type,
    EmeInitDataType init_data_type,
    const std::vector<uint8>& init_data,
    scoped_ptr<NewSessionCdmPromise> promise,
    bool granted)
{
    // ProxyDecryptor is only used by Prefixed EME, where RequestPermission() is
    // only for triggering the permission UI. Later CheckPermission() will be
    // called (e.g. in PlatformVerificationFlow on ChromeOS; in BrowserCdmManager
    // on Android) and the permission status will be evaluated then.
    DVLOG_IF(1, !granted) << "Permission request rejected.";

    media_keys_->CreateSessionAndGenerateRequest(session_type, init_data_type,
        init_data, promise.Pass());
}

void ProxyDecryptor::AddKey(const uint8* key,
    int key_length,
    const uint8* init_data,
    int init_data_length,
    const std::string& session_id)
{
    DVLOG(1) << "AddKey()";

    if (!media_keys_) {
        OnLegacySessionError(std::string(), MediaKeys::INVALID_STATE_ERROR, 0,
            "CDM is not available.");
        return;
    }

    // In the prefixed API, the session parameter provided to addKey() is
    // optional, so use the single existing session if it exists.
    std::string new_session_id(session_id);
    if (new_session_id.empty()) {
        if (active_sessions_.size() == 1) {
            base::hash_map<std::string, bool>::iterator it = active_sessions_.begin();
            new_session_id = it->first;
        } else {
            OnLegacySessionError(std::string(), MediaKeys::NOT_SUPPORTED_ERROR, 0,
                "SessionId not specified.");
            return;
        }
    }

    scoped_ptr<SimpleCdmPromise> promise(new CdmCallbackPromise<>(
        base::Bind(&ProxyDecryptor::GenerateKeyAdded,
            weak_ptr_factory_.GetWeakPtr(), session_id),
        base::Bind(&ProxyDecryptor::OnLegacySessionError,
            weak_ptr_factory_.GetWeakPtr(), session_id)));

    // EME WD spec only supports a single array passed to the CDM. For
    // Clear Key using v0.1b, both arrays are used (|init_data| is key_id).
    // Since the EME WD spec supports the key as a JSON Web Key,
    // convert the 2 arrays to a JWK and pass it as the single array.
    if (is_clear_key_) {
        // Decryptor doesn't support empty key ID (see http://crbug.com/123265).
        // So ensure a non-empty value is passed.
        if (!init_data) {
            static const uint8 kDummyInitData[1] = { 0 };
            init_data = kDummyInitData;
            init_data_length = arraysize(kDummyInitData);
        }

        std::string jwk = GenerateJWKSet(key, key_length, init_data, init_data_length);
        DCHECK(!jwk.empty());
        media_keys_->UpdateSession(new_session_id,
            std::vector<uint8_t>(jwk.begin(), jwk.end()),
            promise.Pass());
        return;
    }

    media_keys_->UpdateSession(new_session_id,
        std::vector<uint8_t>(key, key + key_length),
        promise.Pass());
}

void ProxyDecryptor::CancelKeyRequest(const std::string& session_id)
{
    DVLOG(1) << "CancelKeyRequest()";

    if (!media_keys_) {
        OnLegacySessionError(std::string(), MediaKeys::INVALID_STATE_ERROR, 0,
            "CDM is not available.");
        return;
    }

    scoped_ptr<SimpleCdmPromise> promise(new CdmCallbackPromise<>(
        base::Bind(&ProxyDecryptor::OnSessionClosed,
            weak_ptr_factory_.GetWeakPtr(), session_id),
        base::Bind(&ProxyDecryptor::OnLegacySessionError,
            weak_ptr_factory_.GetWeakPtr(), session_id)));
    media_keys_->RemoveSession(session_id, promise.Pass());
}

void ProxyDecryptor::OnSessionMessage(const std::string& session_id,
    MediaKeys::MessageType message_type,
    const std::vector<uint8>& message,
    const GURL& legacy_destination_url)
{
    // Assumes that OnSessionCreated() has been called before this.

    // For ClearKey, convert the message from JSON into just passing the key
    // as the message. If unable to extract the key, return the message unchanged.
    if (is_clear_key_) {
        std::vector<uint8> key;
        if (ExtractFirstKeyIdFromLicenseRequest(message, &key)) {
            key_message_cb_.Run(session_id, key, legacy_destination_url);
            return;
        }
    }

    key_message_cb_.Run(session_id, message, legacy_destination_url);
}

void ProxyDecryptor::OnSessionKeysChange(const std::string& session_id,
    bool has_additional_usable_key,
    CdmKeysInfo keys_info)
{
    // EME v0.1b doesn't support this event.
}

void ProxyDecryptor::OnSessionExpirationUpdate(
    const std::string& session_id,
    const base::Time& new_expiry_time)
{
    // EME v0.1b doesn't support this event.
}

void ProxyDecryptor::GenerateKeyAdded(const std::string& session_id)
{
    // EME WD doesn't support this event, but it is needed for EME v0.1b.
    key_added_cb_.Run(session_id);
}

void ProxyDecryptor::OnSessionClosed(const std::string& session_id)
{
    base::hash_map<std::string, bool>::iterator it = active_sessions_.find(session_id);

    // Latest EME spec separates closing a session ("allows an application to
    // indicate that it no longer needs the session") and actually closing the
    // session (done by the CDM at any point "such as in response to a close()
    // call, when the session is no longer needed, or when system resources are
    // lost.") Thus the CDM may cause 2 close() events -- one to resolve the
    // close() promise, and a second to actually close the session. Prefixed EME
    // only expects 1 close event, so drop the second (and subsequent) events.
    // However, this means we can't tell if the CDM is generating spurious close()
    // events.
    if (it == active_sessions_.end())
        return;

    if (it->second) {
        OnLegacySessionError(session_id, MediaKeys::NOT_SUPPORTED_ERROR,
            kSessionClosedSystemCode,
            "Do not close persistent sessions.");
    }
    active_sessions_.erase(it);
}

void ProxyDecryptor::OnLegacySessionError(const std::string& session_id,
    MediaKeys::Exception exception_code,
    uint32 system_code,
    const std::string& error_message)
{
    // Convert |error_name| back to MediaKeys::KeyError if possible. Prefixed
    // EME has different error message, so all the specific error events will
    // get lost.
    MediaKeys::KeyError error_code;
    switch (exception_code) {
    case MediaKeys::CLIENT_ERROR:
        error_code = MediaKeys::kClientError;
        break;
    case MediaKeys::OUTPUT_ERROR:
        error_code = MediaKeys::kOutputError;
        break;
    default:
        // This will include all other CDM4 errors and any error generated
        // by CDM5 or later.
        error_code = MediaKeys::kUnknownError;
        break;
    }
    key_error_cb_.Run(session_id, error_code, system_code);
}

void ProxyDecryptor::SetSessionId(SessionCreationType session_type,
    const std::string& session_id)
{
    // LoadSession() returns empty |session_id| if the session is not found, so
    // convert this into an error.
    if (session_type == LoadSession && session_id.empty()) {
        OnLegacySessionError(session_id, MediaKeys::INVALID_ACCESS_ERROR, 0,
            "Incorrect session id specified for LoadSession().");
        return;
    }

    // Loaded sessions are considered persistent.
    bool is_persistent = session_type == PersistentSession || session_type == LoadSession;
    active_sessions_.insert(std::make_pair(session_id, is_persistent));

    // For LoadSession(), generate the KeyAdded event.
    if (session_type == LoadSession)
        GenerateKeyAdded(session_id);
}

} // namespace media
