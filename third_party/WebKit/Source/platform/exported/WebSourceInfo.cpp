/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "public/platform/WebSourceInfo.h"

#include "public/platform/WebString.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class WebSourceInfoPrivate final : public RefCounted<WebSourceInfoPrivate> {
public:
    static PassRefPtr<WebSourceInfoPrivate> create(const WebString& id, WebSourceInfo::SourceKind, const WebString& label, WebSourceInfo::VideoFacingMode);

    const WebString& id() const { return m_id; }
    WebSourceInfo::SourceKind kind() const { return m_kind; }
    const WebString& label() const { return m_label; }
    WebSourceInfo::VideoFacingMode facing() const { return m_facing; }

private:
    WebSourceInfoPrivate(const WebString& id, WebSourceInfo::SourceKind, const WebString& label, WebSourceInfo::VideoFacingMode);

    WebString m_id;
    WebSourceInfo::SourceKind m_kind;
    WebString m_label;
    WebSourceInfo::VideoFacingMode m_facing;
};

PassRefPtr<WebSourceInfoPrivate> WebSourceInfoPrivate::create(const WebString& id, WebSourceInfo::SourceKind kind, const WebString& label, WebSourceInfo::VideoFacingMode facing)
{
    return adoptRef(new WebSourceInfoPrivate(id, kind, label, facing));
}

WebSourceInfoPrivate::WebSourceInfoPrivate(const WebString& id, WebSourceInfo::SourceKind kind, const WebString& label, WebSourceInfo::VideoFacingMode facing)
    : m_id(id)
    , m_kind(kind)
    , m_label(label)
    , m_facing(facing)
{
}

void WebSourceInfo::assign(const WebSourceInfo& other)
{
    m_private = other.m_private;
}

void WebSourceInfo::reset()
{
    m_private.reset();
}

void WebSourceInfo::initialize(const WebString& id, WebSourceInfo::SourceKind kind, const WebString& label, WebSourceInfo::VideoFacingMode facing)
{
    m_private = WebSourceInfoPrivate::create(id, kind, label, facing);
}

WebString WebSourceInfo::id() const
{
    ASSERT(!m_private.isNull());
    return m_private->id();
}

WebSourceInfo::SourceKind WebSourceInfo::kind() const
{
    ASSERT(!m_private.isNull());
    return m_private->kind();
}

WebString WebSourceInfo::label() const
{
    ASSERT(!m_private.isNull());
    return m_private->label();
}

WebSourceInfo::VideoFacingMode WebSourceInfo::facing() const
{
    ASSERT(!m_private.isNull());
    return m_private->facing();
}

} // namespace blink

