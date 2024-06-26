// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/cdm_session_adapter.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/metrics/histogram.h"
#include "base/stl_util.h"
#include "base/trace_event/trace_event.h"
#include "media/base/cdm_factory.h"
#include "media/base/cdm_key_information.h"
#include "media/base/cdm_promise.h"
#include "media/base/key_systems.h"
#include "media/blink/webcontentdecryptionmodule_impl.h"
#include "media/blink/webcontentdecryptionmodulesession_impl.h"
//#include "url/gurl.h"
#include "third_party/WebKit/public/platform/WebURL.h"

namespace media {

const char kMediaEME[] = "Media.EME.";
const char kDot[] = ".";
const char kTimeToCreateCdmUMAName[] = "CreateCdmTime";

CdmSessionAdapter::CdmSessionAdapter()
    : trace_id_(0)
    , weak_ptr_factory_(this)
{
}

CdmSessionAdapter::~CdmSessionAdapter() { }

void CdmSessionAdapter::CreateCdm(
    CdmFactory* cdm_factory,
    const std::string& key_system,
    const blink::WebURL& security_origin,
    const CdmConfig& cdm_config,
    scoped_ptr<blink::WebContentDecryptionModuleResult> result)
{
    TRACE_EVENT_ASYNC_BEGIN0("media", "CdmSessionAdapter::CreateCdm", ++trace_id_);

    base::TimeTicks start_time = base::TimeTicks::Now();

    // Note: WebContentDecryptionModuleImpl::Create() calls this method without
    // holding a reference to the CdmSessionAdapter. Bind OnCdmCreated() with
    // |this| instead of |weak_this| to prevent |this| from being destructed.
    base::WeakPtr<CdmSessionAdapter> weak_this = weak_ptr_factory_.GetWeakPtr();

    DCHECK(!cdm_created_result_);
    cdm_created_result_ = result.Pass();

    cdm_factory->Create(
        key_system, security_origin, cdm_config,
        base::Bind(&CdmSessionAdapter::OnSessionMessage, weak_this),
        base::Bind(&CdmSessionAdapter::OnSessionClosed, weak_this),
        base::Bind(&CdmSessionAdapter::OnLegacySessionError, weak_this),
        base::Bind(&CdmSessionAdapter::OnSessionKeysChange, weak_this),
        base::Bind(&CdmSessionAdapter::OnSessionExpirationUpdate, weak_this),
        base::Bind(&CdmSessionAdapter::OnCdmCreated, this, key_system,
            start_time));
}

void CdmSessionAdapter::SetServerCertificate(
    const std::vector<uint8_t>& certificate,
    scoped_ptr<SimpleCdmPromise> promise)
{
    cdm_->SetServerCertificate(certificate, promise.Pass());
}

WebContentDecryptionModuleSessionImpl* CdmSessionAdapter::CreateSession()
{
    return new WebContentDecryptionModuleSessionImpl(this);
}

bool CdmSessionAdapter::RegisterSession(
    const std::string& session_id,
    base::WeakPtr<WebContentDecryptionModuleSessionImpl> session)
{
    // If this session ID is already registered, don't register it again.
    if (ContainsKey(sessions_, session_id))
        return false;

    sessions_[session_id] = session;
    return true;
}

void CdmSessionAdapter::UnregisterSession(const std::string& session_id)
{
    DCHECK(ContainsKey(sessions_, session_id));
    sessions_.erase(session_id);
}

void CdmSessionAdapter::InitializeNewSession(
    EmeInitDataType init_data_type,
    const std::vector<uint8_t>& init_data,
    MediaKeys::SessionType session_type,
    scoped_ptr<NewSessionCdmPromise> promise)
{
    cdm_->CreateSessionAndGenerateRequest(session_type, init_data_type, init_data,
        promise.Pass());
}

void CdmSessionAdapter::LoadSession(MediaKeys::SessionType session_type,
    const std::string& session_id,
    scoped_ptr<NewSessionCdmPromise> promise)
{
    cdm_->LoadSession(session_type, session_id, promise.Pass());
}

void CdmSessionAdapter::UpdateSession(const std::string& session_id,
    const std::vector<uint8_t>& response,
    scoped_ptr<SimpleCdmPromise> promise)
{
    cdm_->UpdateSession(session_id, response, promise.Pass());
}

void CdmSessionAdapter::CloseSession(const std::string& session_id,
    scoped_ptr<SimpleCdmPromise> promise)
{
    cdm_->CloseSession(session_id, promise.Pass());
}

void CdmSessionAdapter::RemoveSession(const std::string& session_id,
    scoped_ptr<SimpleCdmPromise> promise)
{
    cdm_->RemoveSession(session_id, promise.Pass());
}

CdmContext* CdmSessionAdapter::GetCdmContext()
{
    return cdm_->GetCdmContext();
}

const std::string& CdmSessionAdapter::GetKeySystem() const
{
    return key_system_;
}

const std::string& CdmSessionAdapter::GetKeySystemUMAPrefix() const
{
    DCHECK(!key_system_uma_prefix_.empty());
    return key_system_uma_prefix_;
}

void CdmSessionAdapter::OnCdmCreated(const std::string& key_system,
    base::TimeTicks start_time,
    const scoped_refptr<MediaKeys>& cdm,
    const std::string& error_message)
{
    DVLOG(2) << __FUNCTION__;
    DCHECK(!cdm_);

    TRACE_EVENT_ASYNC_END2("media", "CdmSessionAdapter::CreateCdm", trace_id_,
        "success", (cdm ? "true" : "false"), "error_message",
        error_message);

    if (!cdm) {
        cdm_created_result_->completeWithError(
            blink::WebContentDecryptionModuleExceptionNotSupportedError, 0,
            blink::WebString::fromUTF8(error_message));
        cdm_created_result_.reset();
        return;
    }

    key_system_ = key_system;
    key_system_uma_prefix_ = kMediaEME + GetKeySystemNameForUMA(key_system) + kDot;

    // Only report time for successful CDM creation.
    ReportTimeToCreateCdmUMA(base::TimeTicks::Now() - start_time);

    cdm_ = cdm;

    cdm_created_result_->completeWithContentDecryptionModule(
        new WebContentDecryptionModuleImpl(this));
    cdm_created_result_.reset();
}

void CdmSessionAdapter::OnSessionMessage(
    const std::string& session_id,
    MediaKeys::MessageType message_type,
    const std::vector<uint8_t>& message,
    const blink::WebURL& /* legacy_destination_url */)
{
    WebContentDecryptionModuleSessionImpl* session = GetSession(session_id);
    DLOG_IF(WARNING, !session) << __FUNCTION__ << " for unknown session "
                               << session_id;
    if (session)
        session->OnSessionMessage(message_type, message);
}

void CdmSessionAdapter::OnSessionKeysChange(const std::string& session_id,
    bool has_additional_usable_key,
    CdmKeysInfo keys_info)
{
    // TODO(jrummell): Pass |keys_info| on.
    WebContentDecryptionModuleSessionImpl* session = GetSession(session_id);
    DLOG_IF(WARNING, !session) << __FUNCTION__ << " for unknown session "
                               << session_id;
    if (session)
        session->OnSessionKeysChange(has_additional_usable_key, keys_info.Pass());
}

void CdmSessionAdapter::OnSessionExpirationUpdate(
    const std::string& session_id,
    const base::Time& new_expiry_time)
{
    WebContentDecryptionModuleSessionImpl* session = GetSession(session_id);
    DLOG_IF(WARNING, !session) << __FUNCTION__ << " for unknown session "
                               << session_id;
    if (session)
        session->OnSessionExpirationUpdate(new_expiry_time);
}

void CdmSessionAdapter::OnSessionClosed(const std::string& session_id)
{
    WebContentDecryptionModuleSessionImpl* session = GetSession(session_id);
    DLOG_IF(WARNING, !session) << __FUNCTION__ << " for unknown session "
                               << session_id;
    if (session)
        session->OnSessionClosed();
}

void CdmSessionAdapter::OnLegacySessionError(
    const std::string& session_id,
    MediaKeys::Exception exception_code,
    uint32_t system_code,
    const std::string& error_message)
{
    // Error events not used by unprefixed EME.
    // TODO(jrummell): Remove when prefixed EME removed.
}

WebContentDecryptionModuleSessionImpl* CdmSessionAdapter::GetSession(
    const std::string& session_id)
{
    // Since session objects may get garbage collected, it is possible that there
    // are events coming back from the CDM and the session has been unregistered.
    // We can not tell if the CDM is firing events at sessions that never existed.
    SessionMap::iterator session = sessions_.find(session_id);
    return (session != sessions_.end()) ? session->second.get() : NULL;
}

void CdmSessionAdapter::ReportTimeToCreateCdmUMA(base::TimeDelta time) const
{
    // Note: This leaks memory, which is expected behavior.
    //     base::HistogramBase* histogram = base::Histogram::FactoryTimeGet(
    //         GetKeySystemUMAPrefix() + kTimeToCreateCdmUMAName,
    //         base::TimeDelta::FromMilliseconds(1), base::TimeDelta::FromSeconds(10),
    //         50, base::HistogramBase::kUmaTargetedHistogramFlag);
    //
    //     histogram->AddTime(time);
    DebugBreak();
}

} // namespace media
