// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webcontentdecryptionmodulesession_impl.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "media/base/cdm_key_information.h"
#include "media/base/cdm_promise.h"
#include "media/base/key_systems.h"
#include "media/base/limits.h"
#include "media/base/media_keys.h"
#include "media/blink/cdm_result_promise.h"
#include "media/blink/cdm_session_adapter.h"
#include "media/blink/webmediaplayer_util.h"
#include "media/cdm/json_web_key.h"
#include "media/cdm/key_system_names.h"
#include "third_party/WebKit/public/platform/WebData.h"
#include "third_party/WebKit/public/platform/WebEncryptedMediaKeyInformation.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/public/platform/WebVector.h"

#if defined(USE_PROPRIETARY_CODECS)
#include "media/cdm/cenc_utils.h"
#endif

namespace media {

const char kCloseSessionUMAName[] = "CloseSession";
const char kGenerateRequestUMAName[] = "GenerateRequest";
const char kLoadSessionUMAName[] = "LoadSession";
const char kRemoveSessionUMAName[] = "RemoveSession";
const char kUpdateSessionUMAName[] = "UpdateSession";

static blink::WebContentDecryptionModuleSession::Client::MessageType
convertMessageType(MediaKeys::MessageType message_type)
{
    switch (message_type) {
    case media::MediaKeys::LICENSE_REQUEST:
        return blink::WebContentDecryptionModuleSession::Client::MessageType::
            LicenseRequest;
    case media::MediaKeys::LICENSE_RENEWAL:
        return blink::WebContentDecryptionModuleSession::Client::MessageType::
            LicenseRenewal;
    case media::MediaKeys::LICENSE_RELEASE:
        return blink::WebContentDecryptionModuleSession::Client::MessageType::
            LicenseRelease;
    }

    NOTREACHED();
    return blink::WebContentDecryptionModuleSession::Client::MessageType::
        LicenseRequest;
}

static blink::WebEncryptedMediaKeyInformation::KeyStatus convertStatus(
    media::CdmKeyInformation::KeyStatus status)
{
    switch (status) {
    case media::CdmKeyInformation::USABLE:
        return blink::WebEncryptedMediaKeyInformation::KeyStatus::Usable;
    case media::CdmKeyInformation::INTERNAL_ERROR:
        return blink::WebEncryptedMediaKeyInformation::KeyStatus::InternalError;
    case media::CdmKeyInformation::EXPIRED:
        return blink::WebEncryptedMediaKeyInformation::KeyStatus::Expired;
    case media::CdmKeyInformation::OUTPUT_RESTRICTED:
        return blink::WebEncryptedMediaKeyInformation::KeyStatus::OutputRestricted;
    case media::CdmKeyInformation::OUTPUT_DOWNSCALED:
        return blink::WebEncryptedMediaKeyInformation::KeyStatus::
            OutputDownscaled;
    case media::CdmKeyInformation::KEY_STATUS_PENDING:
        return blink::WebEncryptedMediaKeyInformation::KeyStatus::StatusPending;
    case media::CdmKeyInformation::RELEASED:
        return blink::WebEncryptedMediaKeyInformation::KeyStatus::Released;
    }

    NOTREACHED();
    return blink::WebEncryptedMediaKeyInformation::KeyStatus::InternalError;
}

static MediaKeys::SessionType convertSessionType(
    blink::WebEncryptedMediaSessionType session_type)
{
    switch (session_type) {
    case blink::WebEncryptedMediaSessionType::Temporary:
        return MediaKeys::TEMPORARY_SESSION;
    case blink::WebEncryptedMediaSessionType::PersistentLicense:
        return MediaKeys::PERSISTENT_LICENSE_SESSION;
    case blink::WebEncryptedMediaSessionType::PersistentReleaseMessage:
        return MediaKeys::PERSISTENT_RELEASE_MESSAGE_SESSION;
    case blink::WebEncryptedMediaSessionType::Unknown:
        break;
    }

    NOTREACHED();
    return MediaKeys::TEMPORARY_SESSION;
}

static bool SanitizeInitData(EmeInitDataType init_data_type,
    const unsigned char* init_data,
    size_t init_data_length,
    std::vector<uint8>* sanitized_init_data,
    std::string* error_message)
{
    if (init_data_length > limits::kMaxInitDataLength) {
        error_message->assign("Initialization data too long.");
        return false;
    }

    switch (init_data_type) {
    case EmeInitDataType::WEBM:
        sanitized_init_data->assign(init_data, init_data + init_data_length);
        return true;

    case EmeInitDataType::CENC:
#if defined(USE_PROPRIETARY_CODECS)
        sanitized_init_data->assign(init_data, init_data + init_data_length);
        if (!ValidatePsshInput(*sanitized_init_data)) {
            error_message->assign("Initialization data for CENC is incorrect.");
            return false;
        }
        return true;
#else
        error_message->assign("Initialization data type CENC is not supported.");
        return false;
#endif

    case EmeInitDataType::KEYIDS: {
        // Extract the keys and then rebuild the message. This ensures that any
        // extra data in the provided JSON is dropped.
        //std::string init_data_string(init_data, init_data + init_data_length);
        std::string init_data_string;
        for (size_t i = 0; i < init_data_length; ++i) {
            init_data_string += init_data[i];
        }

        KeyIdList key_ids;
        if (!ExtractKeyIdsFromKeyIdsInitData(init_data_string, &key_ids, error_message))
            return false;

        for (const auto& key_id : key_ids) {
            if (key_id.size() < limits::kMinKeyIdLength || key_id.size() > limits::kMaxKeyIdLength) {
                error_message->assign("Incorrect key size.");
                return false;
            }
        }

        CreateKeyIdsInitData(key_ids, sanitized_init_data);
        return true;
    }

    case EmeInitDataType::UNKNOWN:
        break;
    }

    NOTREACHED();
    error_message->assign("Initialization data type is not supported.");
    return false;
}

static bool SanitizeSessionId(const blink::WebString& session_id,
    std::string* sanitized_session_id)
{
    // The user agent should thoroughly validate the sessionId value before
    // passing it to the CDM. At a minimum, this should include checking that
    // the length and value (e.g. alphanumeric) are reasonable.
    if (!base::IsStringASCII(session_id))
        return false;

    sanitized_session_id->assign(base::UTF16ToASCII(base::string16(session_id)));
    if (sanitized_session_id->length() > limits::kMaxSessionIdLength)
        return false;

    for (const char c : *sanitized_session_id) {
        if (!base::IsAsciiAlpha(c) && !base::IsAsciiDigit(c))
            return false;
    }

    return true;
}

static bool SanitizeResponse(const std::string& key_system,
    const uint8* response,
    size_t response_length,
    std::vector<uint8>* sanitized_response)
{
    // The user agent should thoroughly validate the response before passing it
    // to the CDM. This may include verifying values are within reasonable limits,
    // stripping irrelevant data or fields, pre-parsing it, sanitizing it,
    // and/or generating a fully sanitized version. The user agent should check
    // that the length and values of fields are reasonable. Unknown fields should
    // be rejected or removed.
    if (response_length > limits::kMaxSessionResponseLength)
        return false;

    if (IsClearKey(key_system) || IsExternalClearKey(key_system)) {
        //std::string key_string(response, response + response_length);
        std::string key_string;
        for (size_t i = 0; i < response_length; ++i) {
            key_string += response[i];
        }
        KeyIdAndKeyPairs keys;
        MediaKeys::SessionType session_type = MediaKeys::TEMPORARY_SESSION;
        if (!ExtractKeysFromJWKSet(key_string, &keys, &session_type))
            return false;

        // Must contain at least one key.
        if (keys.empty())
            return false;

        for (const auto key_pair : keys) {
            if (key_pair.first.size() < limits::kMinKeyIdLength || key_pair.first.size() > limits::kMaxKeyIdLength) {
                return false;
            }
        }

        std::string sanitized_data = GenerateJWKSet(keys, session_type);
        //sanitized_response->assign(sanitized_data.begin(), sanitized_data.end());
        sanitized_response->assign(sanitized_data.size());
        memcpy(sanitized_response->data(), sanitized_data.c_str(), sanitized_data.size());
        return true;
    }

    // TODO(jrummell): Verify responses for Widevine.
    //sanitized_response->assign(response, response + response_length);
    sanitized_response->assign(response_length);
    memcpy(sanitized_response->data(), response, response_length);

    return true;
}

WebContentDecryptionModuleSessionImpl::WebContentDecryptionModuleSessionImpl(
    const scoped_refptr<CdmSessionAdapter>& adapter)
    : adapter_(adapter)
    , is_closed_(false)
    , weak_ptr_factory_(this)
{
}

WebContentDecryptionModuleSessionImpl::
    ~WebContentDecryptionModuleSessionImpl()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    if (!session_id_.empty())
        adapter_->UnregisterSession(session_id_);
}

void WebContentDecryptionModuleSessionImpl::setClientInterface(Client* client)
{
    client_ = client;
}

blink::WebString WebContentDecryptionModuleSessionImpl::sessionId() const
{
    return blink::WebString::fromUTF8(session_id_);
}

void WebContentDecryptionModuleSessionImpl::initializeNewSession(
    blink::WebEncryptedMediaInitDataType init_data_type,
    const unsigned char* init_data,
    size_t init_data_length,
    blink::WebEncryptedMediaSessionType session_type,
    blink::WebContentDecryptionModuleResult result)
{
    DCHECK(init_data);
    DCHECK(session_id_.empty());
    DCHECK(thread_checker_.CalledOnValidThread());

    // From https://w3c.github.io/encrypted-media/#generateRequest.
    // 5. If the Key System implementation represented by this object's cdm
    //    implementation value does not support initDataType as an Initialization
    //    Data Type, return a promise rejected with a new DOMException whose name
    //    is NotSupportedError. String comparison is case-sensitive.
    EmeInitDataType eme_init_data_type = ConvertToEmeInitDataType(init_data_type);
    if (!IsSupportedKeySystemWithInitDataType(adapter_->GetKeySystem(),
            eme_init_data_type)) {
        std::string message = "The initialization data type is not supported by the key system.";
        result.completeWithError(
            blink::WebContentDecryptionModuleExceptionNotSupportedError, 0,
            blink::WebString::fromUTF8(message));
        return;
    }

    // 9.1 If the init data is not valid for initDataType, reject promise with a
    //     new DOMException whose name is InvalidAccessError.
    // 9.2 Let sanitized init data be a validated and sanitized version of init
    //     data. The user agent must thoroughly validate the Initialization Data
    //     before passing it to the CDM. This includes verifying that the length
    //     and values of fields are reasonable, verifying that values are within
    //     reasonable limits, and stripping irrelevant, unsupported, or unknown
    //     data or fields. It is recommended that user agents pre-parse, sanitize,
    //     and/or generate a fully sanitized version of the Initialization Data.
    //     If the Initialization Data format specified by initDataType support
    //     multiple entries, the user agent should remove entries that are not
    //     needed by the CDM.
    // 9.3 If the previous step failed, reject promise with a new DOMException
    //     whose name is InvalidAccessError.
    std::vector<uint8> sanitized_init_data;
    std::string message;
    if (!SanitizeInitData(eme_init_data_type, init_data, init_data_length,
            &sanitized_init_data, &message)) {
        result.completeWithError(
            blink::WebContentDecryptionModuleExceptionInvalidAccessError, 0,
            blink::WebString::fromUTF8(message));
        return;
    }

    // 9.4 Let session id be the empty string.
    //     (Done in constructor.)

    // 9.5 Let message be null.
    //     (Done by CDM.)

    // 9.6 Let cdm be the CDM instance represented by this object's cdm
    //     instance value.
    // 9.7 Use the cdm to execute the following steps:
    adapter_->InitializeNewSession(
        eme_init_data_type, sanitized_init_data, convertSessionType(session_type),
        scoped_ptr<NewSessionCdmPromise>(new NewSessionCdmResultPromise(
            result, adapter_->GetKeySystemUMAPrefix() + kGenerateRequestUMAName,
            base::Bind(
                &WebContentDecryptionModuleSessionImpl::OnSessionInitialized,
                weak_ptr_factory_.GetWeakPtr()))));
}

void WebContentDecryptionModuleSessionImpl::load(
    const blink::WebString& session_id,
    blink::WebContentDecryptionModuleResult result)
{
    DCHECK(!session_id.isEmpty());
    DCHECK(session_id_.empty());
    DCHECK(thread_checker_.CalledOnValidThread());

    std::string sanitized_session_id;
    if (!SanitizeSessionId(session_id, &sanitized_session_id)) {
        result.completeWithError(
            blink::WebContentDecryptionModuleExceptionInvalidAccessError, 0,
            "Invalid session ID.");
        return;
    }

    // TODO(jrummell): Now that there are 2 types of persistent sessions, the
    // session type should be passed from blink. Type should also be passed in the
    // constructor (and removed from initializeNewSession()).
    adapter_->LoadSession(
        MediaKeys::PERSISTENT_LICENSE_SESSION, sanitized_session_id,
        scoped_ptr<NewSessionCdmPromise>(new NewSessionCdmResultPromise(
            result, adapter_->GetKeySystemUMAPrefix() + kLoadSessionUMAName,
            base::Bind(
                &WebContentDecryptionModuleSessionImpl::OnSessionInitialized,
                weak_ptr_factory_.GetWeakPtr()))));
}

void WebContentDecryptionModuleSessionImpl::update(
    const uint8* response,
    size_t response_length,
    blink::WebContentDecryptionModuleResult result)
{
    DCHECK(response);
    DCHECK(!session_id_.empty());
    DCHECK(thread_checker_.CalledOnValidThread());

    std::vector<uint8> sanitized_response;
    if (!SanitizeResponse(adapter_->GetKeySystem(), response, response_length,
            &sanitized_response)) {
        result.completeWithError(
            blink::WebContentDecryptionModuleExceptionInvalidAccessError, 0,
            "Invalid response.");
        return;
    }

    adapter_->UpdateSession(
        session_id_, sanitized_response,
        scoped_ptr<SimpleCdmPromise>(new CdmResultPromise<>(
            result, adapter_->GetKeySystemUMAPrefix() + kUpdateSessionUMAName)));
}

void WebContentDecryptionModuleSessionImpl::close(
    blink::WebContentDecryptionModuleResult result)
{
    DCHECK(!session_id_.empty());
    DCHECK(thread_checker_.CalledOnValidThread());
    adapter_->CloseSession(
        session_id_,
        scoped_ptr<SimpleCdmPromise>(new CdmResultPromise<>(
            result, adapter_->GetKeySystemUMAPrefix() + kCloseSessionUMAName)));
}

void WebContentDecryptionModuleSessionImpl::remove(
    blink::WebContentDecryptionModuleResult result)
{
    DCHECK(!session_id_.empty());
    DCHECK(thread_checker_.CalledOnValidThread());
    adapter_->RemoveSession(
        session_id_,
        scoped_ptr<SimpleCdmPromise>(new CdmResultPromise<>(
            result, adapter_->GetKeySystemUMAPrefix() + kRemoveSessionUMAName)));
}

void WebContentDecryptionModuleSessionImpl::OnSessionMessage(
    MediaKeys::MessageType message_type,
    const std::vector<uint8>& message)
{
    DCHECK(client_) << "Client not set before message event";
    DCHECK(thread_checker_.CalledOnValidThread());
    client_->message(convertMessageType(message_type), vector_as_array(&message),
        message.size());
}

void WebContentDecryptionModuleSessionImpl::OnSessionKeysChange(
    bool has_additional_usable_key,
    CdmKeysInfo keys_info)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    blink::WebVector<blink::WebEncryptedMediaKeyInformation> keys(
        keys_info.size());
    for (size_t i = 0; i < keys_info.size(); ++i) {
        const auto& key_info = keys_info[i];
        keys[i].setId(blink::WebData(reinterpret_cast<char*>(&key_info->key_id[0]),
            key_info->key_id.size()));
        keys[i].setStatus(convertStatus(key_info->status));
        keys[i].setSystemCode(key_info->system_code);
    }

    // Now send the event to blink.
    client_->keysStatusesChange(keys, has_additional_usable_key);
}

void WebContentDecryptionModuleSessionImpl::OnSessionExpirationUpdate(
    const base::Time& new_expiry_time)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    client_->expirationChanged(new_expiry_time.ToJsTime());
}

void WebContentDecryptionModuleSessionImpl::OnSessionClosed()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    if (is_closed_)
        return;

    is_closed_ = true;
    client_->close();
}

void WebContentDecryptionModuleSessionImpl::OnSessionInitialized(
    const std::string& session_id,
    SessionInitStatus* status)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    // CDM will return NULL if the session to be loaded can't be found.
    if (session_id.empty()) {
        *status = SessionInitStatus::SESSION_NOT_FOUND;
        return;
    }

    DCHECK(session_id_.empty()) << "Session ID may not be changed once set.";
    session_id_ = session_id;
    *status = adapter_->RegisterSession(session_id_, weak_ptr_factory_.GetWeakPtr())
        ? SessionInitStatus::NEW_SESSION
        : SessionInitStatus::SESSION_ALREADY_EXISTS;
}

} // namespace media
