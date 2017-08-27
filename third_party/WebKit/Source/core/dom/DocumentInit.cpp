/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "core/dom/DocumentInit.h"

#include "core/dom/Document.h"
#include "core/dom/custom/CustomElementRegistrationContext.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/imports/HTMLImportsController.h"
#include "core/loader/DocumentLoader.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "public/platform/Platform.h"

namespace blink {

// FIXME: Broken with OOPI.
static Document* parentDocument(LocalFrame* frame)
{
    if (!frame)
        return 0;
    Element* ownerElement = frame->deprecatedLocalOwner();
    if (!ownerElement)
        return 0;
    return &ownerElement->document();
}


static Document* ownerDocument(LocalFrame* frame)
{
    if (!frame)
        return 0;

    Frame* ownerFrame = frame->tree().parent();
    if (!ownerFrame)
        ownerFrame = frame->loader().opener();
    if (!ownerFrame || !ownerFrame->isLocalFrame())
        return 0;
    return toLocalFrame(ownerFrame)->document();
}

DocumentInit::DocumentInit(const KURL& url, LocalFrame* frame, WeakPtrWillBeRawPtr<Document> contextDocument, HTMLImportsController* importsController)
    : m_url(url)
    , m_frame(frame)
    , m_parent(parentDocument(frame))
    , m_owner(ownerDocument(frame))
    , m_contextDocument(contextDocument)
    , m_importsController(importsController)
    , m_createNewRegistrationContext(false)
    , m_shouldReuseDefaultView(frame && frame->shouldReuseDefaultView(url))
    , m_shouldInheritSecurityOriginFromOwner(url.isEmpty() || url.protocolIsAbout())
{
}

DocumentInit::DocumentInit(const DocumentInit& other)
    : m_url(other.m_url)
    , m_frame(other.m_frame)
    , m_parent(other.m_parent)
    , m_owner(other.m_owner)
    , m_contextDocument(other.m_contextDocument)
    , m_importsController(other.m_importsController)
    , m_registrationContext(other.m_registrationContext)
    , m_createNewRegistrationContext(other.m_createNewRegistrationContext)
    , m_shouldReuseDefaultView(other.m_shouldReuseDefaultView)
    , m_shouldInheritSecurityOriginFromOwner(other.m_shouldInheritSecurityOriginFromOwner)
{
}

DocumentInit::~DocumentInit()
{
}

bool DocumentInit::shouldSetURL() const
{
    LocalFrame* frame = frameForSecurityContext();
    return (frame && frame->owner()) || !m_url.isEmpty();
}

bool DocumentInit::shouldTreatURLAsSrcdocDocument() const
{
    return m_parent && m_frame->loader().shouldTreatURLAsSrcdocDocument(m_url);
}

LocalFrame* DocumentInit::frameForSecurityContext() const
{
    if (m_frame)
        return m_frame;
    if (m_importsController)
        return m_importsController->master()->frame();
    return 0;
}

SandboxFlags DocumentInit::sandboxFlags() const
{
    ASSERT(frameForSecurityContext());
    return frameForSecurityContext()->loader().effectiveSandboxFlags();
}

bool DocumentInit::shouldEnforceStrictMixedContentChecking() const
{
    ASSERT(frameForSecurityContext());
    return frameForSecurityContext()->loader().shouldEnforceStrictMixedContentChecking();
}

SecurityContext::InsecureRequestsPolicy DocumentInit::insecureRequestsPolicy() const
{
    ASSERT(frameForSecurityContext());
    return frameForSecurityContext()->loader().insecureRequestsPolicy();
}

SecurityContext::InsecureNavigationsSet* DocumentInit::insecureNavigationsToUpgrade() const
{
    ASSERT(frameForSecurityContext());
    return frameForSecurityContext()->loader().insecureNavigationsToUpgrade();
}

bool DocumentInit::isHostedInReservedIPRange() const
{
    if (LocalFrame* frame = frameForSecurityContext()) {
        if (DocumentLoader* loader = frame->loader().provisionalDocumentLoader() ? frame->loader().provisionalDocumentLoader() : frame->loader().documentLoader()) {
            if (!loader->response().remoteIPAddress().isEmpty())
                return Platform::current()->isReservedIPAddress(loader->response().remoteIPAddress());
        }
    }
    return false;
}

Settings* DocumentInit::settings() const
{
    ASSERT(frameForSecurityContext());
    return frameForSecurityContext()->settings();
}

KURL DocumentInit::parentBaseURL() const
{
    return m_parent->baseURL();
}

DocumentInit& DocumentInit::withRegistrationContext(CustomElementRegistrationContext* registrationContext)
{
    ASSERT(!m_createNewRegistrationContext && !m_registrationContext);
    m_registrationContext = registrationContext;
    return *this;
}

DocumentInit& DocumentInit::withNewRegistrationContext()
{
    ASSERT(!m_createNewRegistrationContext && !m_registrationContext);
    m_createNewRegistrationContext = true;
    return *this;
}

PassRefPtrWillBeRawPtr<CustomElementRegistrationContext> DocumentInit::registrationContext(Document* document) const
{
    if (!document->isHTMLDocument() && !document->isXHTMLDocument())
        return nullptr;

    if (m_createNewRegistrationContext)
        return CustomElementRegistrationContext::create();

    return m_registrationContext.get();
}

WeakPtrWillBeRawPtr<Document> DocumentInit::contextDocument() const
{
    return m_contextDocument;
}

DocumentInit DocumentInit::fromContext(WeakPtrWillBeRawPtr<Document> contextDocument, const KURL& url)
{
    return DocumentInit(url, 0, contextDocument, 0);
}

DocumentInit& DocumentInit::withoutInheritingSecurityOrigin()
{
    m_shouldInheritSecurityOriginFromOwner = false;
    return *this;
}

} // namespace blink

