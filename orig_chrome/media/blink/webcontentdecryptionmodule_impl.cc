// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webcontentdecryptionmodule_impl.h"

#include "base/basictypes.h"
#include "base/bind.h"
#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "media/base/cdm_promise.h"
#include "media/base/key_systems.h"
#include "media/base/media_keys.h"
#include "media/blink/cdm_result_promise.h"
#include "media/blink/cdm_session_adapter.h"
#include "media/blink/webcontentdecryptionmodulesession_impl.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/public/web/WebSecurityOrigin.h"
//#include "url/gurl.h"

namespace media {

void WebContentDecryptionModuleImpl::Create(
    media::CdmFactory* cdm_factory,
    const base::string16& key_system,
    const blink::WebSecurityOrigin& security_origin,
    const CdmConfig& cdm_config,
    scoped_ptr<blink::WebContentDecryptionModuleResult> result)
{
    DCHECK(!security_origin.isNull());
    DCHECK(!key_system.empty());

    // TODO(ddorwin): Guard against this in supported types check and remove this.
    // Chromium only supports ASCII key systems.
    if (!base::IsStringASCII(key_system)) {
        NOTREACHED();
        result->completeWithError(
            blink::WebContentDecryptionModuleExceptionNotSupportedError, 0,
            "Invalid keysystem.");
        return;
    }

    // TODO(ddorwin): This should be a DCHECK.
    std::string key_system_ascii = base::UTF16ToASCII(key_system);
    if (!media::KeySystems::GetInstance()->IsSupportedKeySystem(
            key_system_ascii)) {
        std::string message = "Keysystem '" + key_system_ascii + "' is not supported.";
        result->completeWithError(
            blink::WebContentDecryptionModuleExceptionNotSupportedError, 0,
            blink::WebString::fromUTF8(message));
        return;
    }

    // If unique security origin, don't try to create the CDM.
    if (security_origin.isUnique() || security_origin.toString() == "null") {
        result->completeWithError(
            blink::WebContentDecryptionModuleExceptionNotSupportedError, 0,
            "EME use is not allowed on unique origins.");
        return;
    }

    blink::WebURL security_origin_as_gurl(security_origin.toString());

    // CdmSessionAdapter::CreateCdm() will keep a reference to |adapter|. Then
    // if WebContentDecryptionModuleImpl is successfully created (returned in
    // |result|), it will keep a reference to |adapter|. Otherwise, |adapter| will
    // be destructed.
    scoped_refptr<CdmSessionAdapter> adapter(new CdmSessionAdapter());
    adapter->CreateCdm(cdm_factory, key_system_ascii, security_origin_as_gurl, cdm_config, result.Pass());
}

WebContentDecryptionModuleImpl::WebContentDecryptionModuleImpl(
    scoped_refptr<CdmSessionAdapter> adapter)
    : adapter_(adapter)
{
}

WebContentDecryptionModuleImpl::~WebContentDecryptionModuleImpl()
{
}

// The caller owns the created session.
blink::WebContentDecryptionModuleSession*
WebContentDecryptionModuleImpl::createSession()
{
    return adapter_->CreateSession();
}

void WebContentDecryptionModuleImpl::setServerCertificate(
    const uint8* server_certificate,
    size_t server_certificate_length,
    blink::WebContentDecryptionModuleResult result)
{
    DCHECK(server_certificate);
    adapter_->SetServerCertificate(
        std::vector<uint8>(server_certificate,
            server_certificate + server_certificate_length),
        scoped_ptr<SimpleCdmPromise>(
            new CdmResultPromise<>(result, std::string())));
}

CdmContext* WebContentDecryptionModuleImpl::GetCdmContext()
{
    return adapter_->GetCdmContext();
}

} // namespace media
