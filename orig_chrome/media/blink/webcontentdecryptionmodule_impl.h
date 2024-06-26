// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_WEBCONTENTDECRYPTIONMODULE_IMPL_H_
#define MEDIA_BLINK_WEBCONTENTDECRYPTIONMODULE_IMPL_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "media/blink/media_blink_export.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModule.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModuleResult.h"

namespace blink {
#if defined(ENABLE_PEPPER_CDMS)
class WebLocalFrame;
#endif
class WebSecurityOrigin;
}

namespace media {

struct CdmConfig;
class CdmContext;
class CdmFactory;
class CdmSessionAdapter;
class WebContentDecryptionModuleSessionImpl;

class MEDIA_BLINK_EXPORT WebContentDecryptionModuleImpl
    : public blink::WebContentDecryptionModule {
public:
    static void Create(
        CdmFactory* cdm_factory,
        const base::string16& key_system,
        const blink::WebSecurityOrigin& security_origin,
        const CdmConfig& cdm_config,
        scoped_ptr<blink::WebContentDecryptionModuleResult> result);

    ~WebContentDecryptionModuleImpl() override;

    // blink::WebContentDecryptionModule implementation.
    blink::WebContentDecryptionModuleSession* createSession() override;

    void setServerCertificate(
        const uint8* server_certificate,
        size_t server_certificate_length,
        blink::WebContentDecryptionModuleResult result) override;

    // Returns the CdmContext associated with this CDM, which must not be nullptr.
    // TODO(jrummell): Figure out lifetimes, as WMPI may still use the decryptor
    // after WebContentDecryptionModule is freed. http://crbug.com/330324
    CdmContext* GetCdmContext();

private:
    friend CdmSessionAdapter;

    // Takes reference to |adapter|.
    WebContentDecryptionModuleImpl(scoped_refptr<CdmSessionAdapter> adapter);

    scoped_refptr<CdmSessionAdapter> adapter_;

    DISALLOW_COPY_AND_ASSIGN(WebContentDecryptionModuleImpl);
};

// Allow typecasting from blink type as this is the only implementation.
inline WebContentDecryptionModuleImpl* ToWebContentDecryptionModuleImpl(
    blink::WebContentDecryptionModule* cdm)
{
    return static_cast<WebContentDecryptionModuleImpl*>(cdm);
}

} // namespace media

#endif // MEDIA_BLINK_WEBCONTENTDECRYPTIONMODULE_IMPL_H_
