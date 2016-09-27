/*
 * Copyright (C) 2013 Google, Inc. All Rights Reserved.
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

#ifndef BackgroundHTMLParser_h
#define BackgroundHTMLParser_h

#include "core/dom/DocumentEncodingData.h"
#include "core/html/parser/BackgroundHTMLInputStream.h"
#include "core/html/parser/CompactHTMLToken.h"
#include "core/html/parser/HTMLParserOptions.h"
#include "core/html/parser/HTMLPreloadScanner.h"
#include "core/html/parser/HTMLSourceTracker.h"
#include "core/html/parser/HTMLTreeBuilderSimulator.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/html/parser/XSSAuditorDelegate.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/WeakPtr.h"

namespace blink {

class HTMLDocumentParser;
class XSSAuditor;
class WebScheduler;

class BackgroundHTMLParser {
    WTF_MAKE_FAST_ALLOCATED(BackgroundHTMLParser);
public:
    struct Configuration {
        Configuration();
        HTMLParserOptions options;
        WeakPtr<HTMLDocumentParser> parser;
        OwnPtr<XSSAuditor> xssAuditor;
        OwnPtr<TokenPreloadScanner> preloadScanner;
        OwnPtr<TextResourceDecoder> decoder;
        // outstandingTokenLimit must be greater than or equal to
        // pendingTokenLimit
        size_t outstandingTokenLimit;
        size_t pendingTokenLimit;
    };

    static void start(PassRefPtr<WeakReference<BackgroundHTMLParser>>, PassOwnPtr<Configuration>, WebScheduler*);

    struct Checkpoint {
        WeakPtr<HTMLDocumentParser> parser;
        OwnPtr<HTMLToken> token;
        OwnPtr<HTMLTokenizer> tokenizer;
        HTMLTreeBuilderSimulator::State treeBuilderState;
        HTMLInputCheckpoint inputCheckpoint;
        TokenPreloadScannerCheckpoint preloadScannerCheckpoint;
        String unparsedInput;
    };

    void appendRawBytesFromParserThread(const char* data, int dataLength);

    void appendRawBytesFromMainThread(PassOwnPtr<Vector<char>>);
    void setDecoder(PassOwnPtr<TextResourceDecoder>);
    void flush();
    void resumeFrom(PassOwnPtr<Checkpoint>);
    void startedChunkWithCheckpoint(HTMLInputCheckpoint);
    void finish();
    void stop();

    void forcePlaintextForTextDocument();

private:
    BackgroundHTMLParser(PassRefPtr<WeakReference<BackgroundHTMLParser>>, PassOwnPtr<Configuration>, WebScheduler*);
    ~BackgroundHTMLParser();

    void appendDecodedBytes(const String&);
    void markEndOfFile();
    void pumpTokenizer();
    void sendTokensToMainThread();
    void updateDocument(const String& decodedData);

    WeakPtrFactory<BackgroundHTMLParser> m_weakFactory;
    BackgroundHTMLInputStream m_input;
    HTMLSourceTracker m_sourceTracker;
    OwnPtr<HTMLToken> m_token;
    OwnPtr<HTMLTokenizer> m_tokenizer;
    HTMLTreeBuilderSimulator m_treeBuilderSimulator;
    HTMLParserOptions m_options;
    const size_t m_outstandingTokenLimit;
    WeakPtr<HTMLDocumentParser> m_parser;

    OwnPtr<CompactHTMLTokenStream> m_pendingTokens;
    const size_t m_pendingTokenLimit;
    PreloadRequestStream m_pendingPreloads;
    XSSInfoStream m_pendingXSSInfos;

    OwnPtr<XSSAuditor> m_xssAuditor;
    OwnPtr<TokenPreloadScanner> m_preloadScanner;
    OwnPtr<TextResourceDecoder> m_decoder;
    DocumentEncodingData m_lastSeenEncodingData;
    WebScheduler* m_scheduler; // NOT OWNED, scheduler will outlive BackgroundHTMLParser

    bool m_startingScript;
};

}

#endif
