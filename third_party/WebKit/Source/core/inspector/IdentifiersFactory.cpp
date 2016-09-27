/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
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
#include "core/inspector/IdentifiersFactory.h"

#include "core/dom/WeakIdentifierMap.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/DocumentLoader.h"
#include "wtf/Assertions.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

namespace {

static long s_lastUsedIdentifier = 0;

// static
String& processIdPrefix()
{
    DEFINE_STATIC_LOCAL(String, s_processIdPrefix, ());
    return s_processIdPrefix;
}

} // namespace


// static
void IdentifiersFactory::setProcessId(long processId)
{
    StringBuilder builder;
    builder.appendNumber(processId);
    builder.append('.');
    ASSERT(processIdPrefix().isEmpty() || processIdPrefix() == builder.toString());
    processIdPrefix() = builder.toString();
}

// static
String IdentifiersFactory::createIdentifier()
{
    return addProcessIdPrefixTo(++s_lastUsedIdentifier);
}

// static
String IdentifiersFactory::requestId(unsigned long identifier)
{
    return identifier ? addProcessIdPrefixTo(identifier) : String();
}

// static
String IdentifiersFactory::frameId(LocalFrame* frame)
{
    return addProcessIdPrefixTo(WeakIdentifierMap<LocalFrame>::identifier(frame));
}

// static
LocalFrame* IdentifiersFactory::frameById(const String& frameId)
{
    bool ok;
    int id = removeProcessIdPrefixFrom(frameId, &ok);
    return ok ? WeakIdentifierMap<LocalFrame>::lookup(id) : nullptr;
}

// static
String IdentifiersFactory::loaderId(DocumentLoader* loader)
{
    return addProcessIdPrefixTo(WeakIdentifierMap<DocumentLoader>::identifier(loader));
}

// static
DocumentLoader* IdentifiersFactory::loaderById(const String& loaderId)
{
    bool ok;
    int id = removeProcessIdPrefixFrom(loaderId, &ok);
    return ok ? WeakIdentifierMap<DocumentLoader>::lookup(id) : nullptr;
}

// static
String IdentifiersFactory::addProcessIdPrefixTo(int id)
{
    ASSERT(!processIdPrefix().isEmpty());
    return processIdPrefix() + String::number(id);
}

// static
int IdentifiersFactory::removeProcessIdPrefixFrom(const String& id, bool* ok)
{
    if (id.length() < processIdPrefix().length()) {
        *ok = false;
        return 0;
    }
    return id.substring(processIdPrefix().length()).toInt(ok);
}

} // namespace blink

