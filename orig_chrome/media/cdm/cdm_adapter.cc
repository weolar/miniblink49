// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cdm/cdm_adapter.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/stl_util.h"
#include "base/thread_task_runner_handle.h"
#include "base/time/time.h"
#include "media/base/cdm_context.h"
#include "media/base/cdm_initialized_promise.h"
#include "media/base/cdm_key_information.h"
#include "media/base/limits.h"
#include "media/cdm/cdm_wrapper.h"

namespace media {

namespace {

    cdm::SessionType MediaSessionTypeToCdmSessionType(
        MediaKeys::SessionType session_type)
    {
        switch (session_type) {
        case MediaKeys::TEMPORARY_SESSION:
            return cdm::kTemporary;
        case MediaKeys::PERSISTENT_LICENSE_SESSION:
            return cdm::kPersistentLicense;
        case MediaKeys::PERSISTENT_RELEASE_MESSAGE_SESSION:
            return cdm::kPersistentKeyRelease;
        }

        NOTREACHED();
        return cdm::kTemporary;
    }

    cdm::InitDataType MediaInitDataTypeToCdmInitDataType(
        EmeInitDataType init_data_type)
    {
        switch (init_data_type) {
        case EmeInitDataType::CENC:
            return cdm::kCenc;
        case EmeInitDataType::KEYIDS:
            return cdm::kKeyIds;
        case EmeInitDataType::WEBM:
            return cdm::kWebM;
        case EmeInitDataType::UNKNOWN:
            break;
        }

        NOTREACHED();
        return cdm::kKeyIds;
    }

    MediaKeys::Exception CdmErrorTypeToMediaExceptionType(cdm::Error error)
    {
        switch (error) {
        case cdm::kNotSupportedError:
            return MediaKeys::NOT_SUPPORTED_ERROR;
        case cdm::kInvalidStateError:
            return MediaKeys::INVALID_STATE_ERROR;
        case cdm::kInvalidAccessError:
            return MediaKeys::INVALID_ACCESS_ERROR;
        case cdm::kQuotaExceededError:
            return MediaKeys::QUOTA_EXCEEDED_ERROR;
        case cdm::kUnknownError:
            return MediaKeys::UNKNOWN_ERROR;
        case cdm::kClientError:
            return MediaKeys::CLIENT_ERROR;
        case cdm::kOutputError:
            return MediaKeys::OUTPUT_ERROR;
        }

        NOTREACHED();
        return MediaKeys::UNKNOWN_ERROR;
    }

    MediaKeys::MessageType CdmMessageTypeToMediaMessageType(
        cdm::MessageType message_type)
    {
        switch (message_type) {
        case cdm::kLicenseRequest:
            return MediaKeys::LICENSE_REQUEST;
        case cdm::kLicenseRenewal:
            return MediaKeys::LICENSE_RENEWAL;
        case cdm::kLicenseRelease:
            return MediaKeys::LICENSE_RELEASE;
        }

        NOTREACHED();
        return MediaKeys::LICENSE_REQUEST;
    }

    CdmKeyInformation::KeyStatus CdmKeyStatusToCdmKeyInformationKeyStatus(
        cdm::KeyStatus status)
    {
        switch (status) {
        case cdm::kUsable:
            return CdmKeyInformation::USABLE;
        case cdm::kInternalError:
            return CdmKeyInformation::INTERNAL_ERROR;
        case cdm::kExpired:
            return CdmKeyInformation::EXPIRED;
        case cdm::kOutputRestricted:
            return CdmKeyInformation::OUTPUT_RESTRICTED;
        case cdm::kOutputDownscaled:
            return CdmKeyInformation::OUTPUT_DOWNSCALED;
        case cdm::kStatusPending:
            return CdmKeyInformation::KEY_STATUS_PENDING;
        case cdm::kReleased:
            return CdmKeyInformation::RELEASED;
        }

        NOTREACHED();
        return CdmKeyInformation::INTERNAL_ERROR;
    }

    static void* GetCdmHost(int host_interface_version, void* user_data)
    {
        if (!host_interface_version || !user_data)
            return nullptr;

        static_assert(
            cdm::ContentDecryptionModule::Host::kVersion == cdm::Host_8::kVersion,
            "update the code below");

        // Ensure IsSupportedCdmHostVersion matches implementation of this function.
        // Always update this DCHECK when updating this function.
        // If this check fails, update this function and DCHECK or update
        // IsSupportedCdmHostVersion.

        DCHECK(
            // Future version is not supported.
            !IsSupportedCdmHostVersion(cdm::Host_8::kVersion + 1) &&
            // Current version is supported.
            IsSupportedCdmHostVersion(cdm::Host_8::kVersion) &&
            // Include all previous supported versions (if any) here.
            IsSupportedCdmHostVersion(cdm::Host_7::kVersion) &&
            // One older than the oldest supported version is not supported.
            !IsSupportedCdmHostVersion(cdm::Host_7::kVersion - 1));
        DCHECK(IsSupportedCdmHostVersion(host_interface_version));

        CdmAdapter* cdm_adapter = static_cast<CdmAdapter*>(user_data);
        DVLOG(1) << "Create CDM Host with version " << host_interface_version;
        switch (host_interface_version) {
        case cdm::Host_8::kVersion:
            return static_cast<cdm::Host_8*>(cdm_adapter);
        case cdm::Host_7::kVersion:
            return static_cast<cdm::Host_7*>(cdm_adapter);
        default:
            NOTREACHED();
            return nullptr;
        }
    }

} // namespace

// static
void CdmAdapter::Create(
    const std::string& key_system,
    const base::FilePath& cdm_path,
    const CdmConfig& cdm_config,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const LegacySessionErrorCB& legacy_session_error_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb,
    const CdmCreatedCB& cdm_created_cb)
{
    DCHECK(!key_system.empty());
    DCHECK(!session_message_cb.is_null());
    DCHECK(!session_closed_cb.is_null());
    DCHECK(!legacy_session_error_cb.is_null());
    DCHECK(!session_keys_change_cb.is_null());
    DCHECK(!session_expiration_update_cb.is_null());

    scoped_refptr<CdmAdapter> cdm = new CdmAdapter(key_system, cdm_config, session_message_cb,
        session_closed_cb, legacy_session_error_cb,
        session_keys_change_cb, session_expiration_update_cb);

    // |cdm| ownership passed to the promise.
    scoped_ptr<CdmInitializedPromise> cdm_created_promise(
        new CdmInitializedPromise(cdm_created_cb, cdm));

    cdm->Initialize(cdm_path, cdm_created_promise.Pass());
}

CdmAdapter::CdmAdapter(
    const std::string& key_system,
    const CdmConfig& cdm_config,
    const SessionMessageCB& session_message_cb,
    const SessionClosedCB& session_closed_cb,
    const LegacySessionErrorCB& legacy_session_error_cb,
    const SessionKeysChangeCB& session_keys_change_cb,
    const SessionExpirationUpdateCB& session_expiration_update_cb)
    : key_system_(key_system)
    , cdm_config_(cdm_config)
    , session_message_cb_(session_message_cb)
    , session_closed_cb_(session_closed_cb)
    , legacy_session_error_cb_(legacy_session_error_cb)
    , session_keys_change_cb_(session_keys_change_cb)
    , session_expiration_update_cb_(session_expiration_update_cb)
    , task_runner_(base::ThreadTaskRunnerHandle::Get())
    , weak_factory_(this)
{
    DCHECK(!key_system_.empty());
    DCHECK(!session_message_cb_.is_null());
    DCHECK(!session_closed_cb_.is_null());
    DCHECK(!legacy_session_error_cb_.is_null());
    DCHECK(!session_keys_change_cb_.is_null());
    DCHECK(!session_expiration_update_cb_.is_null());
}

CdmAdapter::~CdmAdapter() { }

CdmWrapper* CdmAdapter::CreateCdmInstance(const std::string& key_system,
    const base::FilePath& cdm_path)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // TODO(jrummell): We need to call INITIALIZE_CDM_MODULE() and
    // DeinitializeCdmModule(). However, that should only be done once for the
    // library.
    base::NativeLibraryLoadError error;
    library_.Reset(base::LoadNativeLibrary(cdm_path, &error));
    if (!library_.is_valid()) {
        DVLOG(1) << "CDM instance for " + key_system + " could not be created. "
                 << error.ToString();
        return nullptr;
    }

    CreateCdmFunc create_cdm_func = reinterpret_cast<CreateCdmFunc>(
        library_.GetFunctionPointer("CreateCdmInstance"));
    if (!create_cdm_func) {
        DVLOG(1) << "No CreateCdmInstance() in library for " + key_system;
        return nullptr;
    }

    CdmWrapper* cdm = CdmWrapper::Create(create_cdm_func, key_system.data(),
        key_system.size(), GetCdmHost, this);

    DVLOG(1) << "CDM instance for " + key_system + (cdm ? "" : " could not be") + " created.";
    return cdm;
}

void CdmAdapter::Initialize(const base::FilePath& cdm_path,
    scoped_ptr<media::SimpleCdmPromise> promise)
{
    cdm_.reset(CreateCdmInstance(key_system_, cdm_path));
    if (!cdm_) {
        promise->reject(MediaKeys::INVALID_ACCESS_ERROR, 0,
            "Unable to create CDM.");
        return;
    }

    cdm_->Initialize(cdm_config_.allow_distinctive_identifier,
        cdm_config_.allow_persistent_state);
    promise->resolve();
}

void CdmAdapter::SetServerCertificate(const std::vector<uint8_t>& certificate,
    scoped_ptr<SimpleCdmPromise> promise)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (certificate.size() < limits::kMinCertificateLength || certificate.size() > limits::kMaxCertificateLength) {
        promise->reject(MediaKeys::INVALID_ACCESS_ERROR, 0,
            "Incorrect certificate.");
        return;
    }

    uint32_t promise_id = cdm_promise_adapter_.SavePromise(promise.Pass());
    cdm_->SetServerCertificate(promise_id, vector_as_array(&certificate),
        certificate.size());
}

void CdmAdapter::CreateSessionAndGenerateRequest(
    SessionType session_type,
    EmeInitDataType init_data_type,
    const std::vector<uint8_t>& init_data,
    scoped_ptr<NewSessionCdmPromise> promise)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    uint32_t promise_id = cdm_promise_adapter_.SavePromise(promise.Pass());
    cdm_->CreateSessionAndGenerateRequest(
        promise_id, MediaSessionTypeToCdmSessionType(session_type),
        MediaInitDataTypeToCdmInitDataType(init_data_type),
        vector_as_array(&init_data), init_data.size());
}

void CdmAdapter::LoadSession(SessionType session_type,
    const std::string& session_id,
    scoped_ptr<NewSessionCdmPromise> promise)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    uint32_t promise_id = cdm_promise_adapter_.SavePromise(promise.Pass());
    cdm_->LoadSession(promise_id, MediaSessionTypeToCdmSessionType(session_type),
        session_id.data(), session_id.size());
}

void CdmAdapter::UpdateSession(const std::string& session_id,
    const std::vector<uint8_t>& response,
    scoped_ptr<SimpleCdmPromise> promise)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!session_id.empty());
    DCHECK(!response.empty());

    uint32_t promise_id = cdm_promise_adapter_.SavePromise(promise.Pass());
    cdm_->UpdateSession(promise_id, session_id.data(), session_id.size(),
        vector_as_array(&response), response.size());
}

void CdmAdapter::CloseSession(const std::string& session_id,
    scoped_ptr<SimpleCdmPromise> promise)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!session_id.empty());

    uint32_t promise_id = cdm_promise_adapter_.SavePromise(promise.Pass());
    cdm_->CloseSession(promise_id, session_id.data(), session_id.size());
}

void CdmAdapter::RemoveSession(const std::string& session_id,
    scoped_ptr<SimpleCdmPromise> promise)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!session_id.empty());

    uint32_t promise_id = cdm_promise_adapter_.SavePromise(promise.Pass());
    cdm_->RemoveSession(promise_id, session_id.data(), session_id.size());
}

CdmContext* CdmAdapter::GetCdmContext()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // TODO(jrummell): Support the Decryptor interface.
    NOTIMPLEMENTED();
    return nullptr;
}

cdm::Buffer* CdmAdapter::Allocate(uint32_t capacity)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // TODO(jrummell): Figure out how memory should be passed around when
    // decrypting.
    NOTIMPLEMENTED();
    return nullptr;
}

void CdmAdapter::SetTimer(int64_t delay_ms, void* context)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    task_runner_->PostDelayedTask(FROM_HERE,
        base::Bind(&CdmAdapter::TimerExpired,
            weak_factory_.GetWeakPtr(), context),
        base::TimeDelta::FromMilliseconds(delay_ms));
}

void CdmAdapter::TimerExpired(void* context)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    cdm_->TimerExpired(context);
}

cdm::Time CdmAdapter::GetCurrentWallTime()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return base::Time::Now().ToDoubleT();
}

void CdmAdapter::OnResolvePromise(uint32_t promise_id)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    cdm_promise_adapter_.ResolvePromise(promise_id);
}

void CdmAdapter::OnResolveNewSessionPromise(uint32_t promise_id,
    const char* session_id,
    uint32_t session_id_size)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    cdm_promise_adapter_.ResolvePromise(promise_id,
        std::string(session_id, session_id_size));
}

void CdmAdapter::OnRejectPromise(uint32_t promise_id,
    cdm::Error error,
    uint32_t system_code,
    const char* error_message,
    uint32_t error_message_size)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    cdm_promise_adapter_.RejectPromise(
        promise_id, CdmErrorTypeToMediaExceptionType(error), system_code,
        std::string(error_message, error_message_size));
}

void CdmAdapter::OnSessionMessage(const char* session_id,
    uint32_t session_id_size,
    cdm::MessageType message_type,
    const char* message,
    uint32_t message_size,
    const char* legacy_destination_url,
    uint32_t legacy_destination_url_size)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(legacy_destination_url_size == 0 || message_type != cdm::MessageType::kLicenseRequest);

    GURL verified_gurl = GURL(std::string(legacy_destination_url, legacy_destination_url_size));
    if (!verified_gurl.is_valid()) {
        DLOG(WARNING) << "SessionMessage legacy_destination_url is invalid : "
                      << verified_gurl.possibly_invalid_spec();
        verified_gurl = GURL::EmptyGURL(); // Replace invalid destination_url.
    }

    const uint8_t* message_ptr = reinterpret_cast<const uint8*>(message);
    session_message_cb_.Run(
        std::string(session_id, session_id_size),
        CdmMessageTypeToMediaMessageType(message_type),
        std::vector<uint8_t>(message_ptr, message_ptr + message_size),
        verified_gurl);
}

void CdmAdapter::OnSessionKeysChange(const char* session_id,
    uint32_t session_id_size,
    bool has_additional_usable_key,
    const cdm::KeyInformation* keys_info,
    uint32_t keys_info_count)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    CdmKeysInfo keys;
    keys.reserve(keys_info_count);
    for (uint32_t i = 0; i < keys_info_count; ++i) {
        const auto& info = keys_info[i];
        keys.push_back(new CdmKeyInformation(
            info.key_id, info.key_id_size,
            CdmKeyStatusToCdmKeyInformationKeyStatus(info.status),
            info.system_code));
    }

    session_keys_change_cb_.Run(std::string(session_id, session_id_size),
        has_additional_usable_key, keys.Pass());
}

void CdmAdapter::OnExpirationChange(const char* session_id,
    uint32_t session_id_size,
    cdm::Time new_expiry_time)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    session_expiration_update_cb_.Run(std::string(session_id, session_id_size),
        base::Time::FromDoubleT(new_expiry_time));
}

void CdmAdapter::OnSessionClosed(const char* session_id,
    uint32_t session_id_size)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    session_closed_cb_.Run(std::string(session_id, session_id_size));
}

void CdmAdapter::OnLegacySessionError(const char* session_id,
    uint32_t session_id_size,
    cdm::Error error,
    uint32_t system_code,
    const char* error_message,
    uint32_t error_message_size)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    legacy_session_error_cb_.Run(std::string(session_id, session_id_size),
        CdmErrorTypeToMediaExceptionType(error),
        system_code,
        std::string(error_message, error_message_size));
}

void CdmAdapter::SendPlatformChallenge(const char* service_id,
    uint32_t service_id_size,
    const char* challenge,
    uint32_t challenge_size)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // TODO(jrummell): If platform verification is available, use it.
    NOTIMPLEMENTED();
    cdm::PlatformChallengeResponse platform_challenge_response = {};
    cdm_->OnPlatformChallengeResponse(platform_challenge_response);
}

void CdmAdapter::EnableOutputProtection(uint32_t desired_protection_mask)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // TODO(jrummell): If output protection is available, use it.
    NOTIMPLEMENTED();
}

void CdmAdapter::QueryOutputProtectionStatus()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // TODO(jrummell): If output protection is available, use it.
    NOTIMPLEMENTED();
    cdm_->OnQueryOutputProtectionStatus(cdm::kQueryFailed, 0, 0);
}

void CdmAdapter::OnDeferredInitializationDone(cdm::StreamType stream_type,
    cdm::Status decoder_status)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Not initializing a decoder, so this should never happen.
    NOTREACHED();
}

// The CDM owns the returned object and must call FileIO::Close() to release it.
cdm::FileIO* CdmAdapter::CreateFileIO(cdm::FileIOClient* client)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // TODO(jrummell): This should use the mojo FileIO client.
    NOTIMPLEMENTED();
    return nullptr;
}

} // namespace media
