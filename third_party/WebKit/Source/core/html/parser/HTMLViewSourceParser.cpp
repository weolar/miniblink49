/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/parser/HTMLViewSourceParser.h"

#include "core/dom/DOMImplementation.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/parser/HTMLParserOptions.h"
#include "core/html/parser/HTMLToken.h"
#include "core/html/parser/XSSAuditorDelegate.h"

namespace blink {

HTMLViewSourceParser::HTMLViewSourceParser(HTMLViewSourceDocument& document, const String& mimeType)
    : DecodedDataDocumentParser(document)
    , m_tokenizer(HTMLTokenizer::create(HTMLParserOptions(&document)))
{
    if (mimeType != "text/html" && !DOMImplementation::isXMLMIMEType(mimeType))
        m_tokenizer->setState(HTMLTokenizer::PLAINTEXTState);
}

void HTMLViewSourceParser::pumpTokenizer()
{
    m_xssAuditor.init(document(), 0);

    while (true) {
        m_sourceTracker.start(m_input.current(), m_tokenizer.get(), m_token);
        if (!m_tokenizer->nextToken(m_input.current(), m_token))
            return;
        m_sourceTracker.end(m_input.current(), m_tokenizer.get(), m_token);

        OwnPtr<XSSInfo> xssInfo = m_xssAuditor.filterToken(FilterTokenRequest(m_token, m_sourceTracker, m_tokenizer->shouldAllowCDATA()));
        HTMLViewSourceDocument::SourceAnnotation annotation = xssInfo ? HTMLViewSourceDocument::AnnotateSourceAsXSS : HTMLViewSourceDocument::AnnotateSourceAsSafe;
        document()->addSource(m_sourceTracker.sourceForToken(m_token), m_token, annotation);

        // FIXME: The tokenizer should do this work for us.
        if (m_token.type() == HTMLToken::StartTag)
            m_tokenizer->updateStateFor(attemptStaticStringCreation(m_token.name(), Likely8Bit));
        m_token.clear();
    }
}

void HTMLViewSourceParser::append(const String& input)
{
    m_input.appendToEnd(input);
    pumpTokenizer();
}

void HTMLViewSourceParser::finish()
{
    if (!m_input.haveSeenEndOfFile())
        m_input.markEndOfFile();
    pumpTokenizer();
    document()->finishedParsing();
}

}
