// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/webcontentdecryptionmoduleaccess_impl.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/memory/scoped_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "media/blink/webencryptedmediaclient_impl.h"

namespace media {

// The caller owns the created cdm (passed back using |result|).
static void CreateCdm(
    const base::WeakPtr<WebEncryptedMediaClientImpl>& client,
    const blink::WebString& key_system,
    const blink::WebSecurityOrigin& security_origin,
    const CdmConfig& cdm_config,
    scoped_ptr<blink::WebContentDecryptionModuleResult> result)
{
    // If |client| is gone (due to the frame getting destroyed), it is
    // impossible to create the CDM, so fail.
    if (!client) {
        result->completeWithError(
            blink::WebContentDecryptionModuleExceptionInvalidStateError, 0,
            "Failed to create CDM.");
        return;
    }

    client->CreateCdm(key_system, security_origin, cdm_config, result.Pass());
}

WebContentDecryptionModuleAccessImpl*
WebContentDecryptionModuleAccessImpl::Create(
    const blink::WebString& key_system,
    const blink::WebSecurityOrigin& security_origin,
    const blink::WebMediaKeySystemConfiguration& configuration,
    const CdmConfig& cdm_config,
    const base::WeakPtr<WebEncryptedMediaClientImpl>& client)
{
    return new WebContentDecryptionModuleAccessImpl(
        key_system, security_origin, configuration, cdm_config, client);
}

WebContentDecryptionModuleAccessImpl::WebContentDecryptionModuleAccessImpl(
    const blink::WebString& key_system,
    const blink::WebSecurityOrigin& security_origin,
    const blink::WebMediaKeySystemConfiguration& configuration,
    const CdmConfig& cdm_config,
    const base::WeakPtr<WebEncryptedMediaClientImpl>& client)
    : key_system_(key_system)
    , security_origin_(security_origin)
    , configuration_(configuration)
    , cdm_config_(cdm_config)
    , client_(client)
{
}

WebContentDecryptionModuleAccessImpl::~WebContentDecryptionModuleAccessImpl()
{
}

blink::WebMediaKeySystemConfiguration
WebContentDecryptionModuleAccessImpl::getConfiguration()
{
    return configuration_;
}

void WebContentDecryptionModuleAccessImpl::createContentDecryptionModule(
    blink::WebContentDecryptionModuleResult result)
{
    // This method needs to run asynchronously, as it may need to load the CDM.
    // As this object's lifetime is controlled by MediaKeySystemAccess on the
    // blink side, copy all values needed by CreateCdm() in case the blink object
    // gets garbage-collected.
    scoped_ptr<blink::WebContentDecryptionModuleResult> result_copy(
        new blink::WebContentDecryptionModuleResult(result));
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::Bind(&CreateCdm, client_, key_system_, security_origin_, cdm_config_, base::Passed(&result_copy)));
}

} // namespace media
