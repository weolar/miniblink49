// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebContentDecryptionModuleResult.h"

#include "platform/ContentDecryptionModuleResult.h"

namespace blink {

void WebContentDecryptionModuleResult::complete()
{
    m_impl->complete();
    reset();
}

void WebContentDecryptionModuleResult::completeWithContentDecryptionModule(WebContentDecryptionModule* cdm)
{
    m_impl->completeWithContentDecryptionModule(cdm);
    reset();
}

void WebContentDecryptionModuleResult::completeWithSession(SessionStatus status)
{
    m_impl->completeWithSession(status);
    reset();
}

void WebContentDecryptionModuleResult::completeWithError(WebContentDecryptionModuleException exception, unsigned long systemCode, const WebString& errorMessage)
{
    m_impl->completeWithError(exception, systemCode, errorMessage);
    reset();
}

WebContentDecryptionModuleResult::WebContentDecryptionModuleResult(ContentDecryptionModuleResult* impl)
    : m_impl(impl)
{
    ASSERT(m_impl.get());
}

void WebContentDecryptionModuleResult::reset()
{
    m_impl.reset();
}

void WebContentDecryptionModuleResult::assign(const WebContentDecryptionModuleResult& o)
{
    m_impl = o.m_impl;
}

} // namespace blink
