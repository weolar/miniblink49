/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/canvas/CanvasRenderingContext.h"

#include "core/html/canvas/CanvasImageSource.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

CanvasRenderingContext::CanvasRenderingContext(HTMLCanvasElement* canvas)
    : ActiveDOMObject(&canvas->document())
    , m_canvas(canvas)
{
    suspendIfNeeded();
}

CanvasRenderingContext::ContextType CanvasRenderingContext::contextTypeFromId(const String& id)
{
    if (id == "2d")
        return Context2d;
    if (id == "experimental-webgl")
        return ContextExperimentalWebgl;
    if (id == "webgl")
        return ContextWebgl;
    if (id == "webgl2")
        return ContextWebgl2;

    return ContextTypeCount;
}

CanvasRenderingContext::ContextType CanvasRenderingContext::resolveContextTypeAliases(CanvasRenderingContext::ContextType type)
{
    if (type == ContextExperimentalWebgl)
        return ContextWebgl;
    return type;
}

bool CanvasRenderingContext::wouldTaintOrigin(CanvasImageSource* imageSource)
{
    const KURL& sourceURL = imageSource->sourceURL();
    bool hasURL = (sourceURL.isValid() && !sourceURL.isAboutBlankURL());

    if (hasURL) {
        if (sourceURL.protocolIsData() || m_cleanURLs.contains(sourceURL.string()))
            return false;
        if (m_dirtyURLs.contains(sourceURL.string()))
            return true;
    }

    bool taintOrigin = imageSource->wouldTaintOrigin(canvas()->securityOrigin());

    if (hasURL) {
        if (taintOrigin)
            m_dirtyURLs.add(sourceURL.string());
        else
            m_cleanURLs.add(sourceURL.string());
    }
    return taintOrigin;
}

DEFINE_TRACE(CanvasRenderingContext)
{
    visitor->trace(m_canvas);
    ActiveDOMObject::trace(visitor);
}

bool CanvasRenderingContext::hasPendingActivity() const
{
    return false;
}

void CanvasRenderingContext::didMoveToNewDocument(Document* document)
{
    ActiveDOMObject::didMoveToNewExecutionContext(document);
}

} // namespace blink
