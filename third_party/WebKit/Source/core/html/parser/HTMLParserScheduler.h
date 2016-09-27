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

#ifndef HTMLParserScheduler_h
#define HTMLParserScheduler_h

#include "core/html/parser/NestingLevelIncrementer.h"
#include "platform/scheduler/CancellableTaskFactory.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class Document;
class HTMLDocumentParser;

class ActiveParserSession : public NestingLevelIncrementer {
    STACK_ALLOCATED();
public:
    ActiveParserSession(unsigned& nestingLevel, Document*);
    ~ActiveParserSession();

private:
    RefPtrWillBeMember<Document> m_document;
};

class PumpSession : public ActiveParserSession {
    STACK_ALLOCATED();
public:
    PumpSession(unsigned& nestingLevel, Document*);
    ~PumpSession();
};

class SpeculationsPumpSession : public ActiveParserSession {
public:
    SpeculationsPumpSession(unsigned& nestingLevel, Document*);
    ~SpeculationsPumpSession();

    double elapsedTime() const;
    void addedElementTokens(size_t count);
    size_t processedElementTokens() const { return m_processedElementTokens; }

private:
    double m_startTime;
    size_t m_processedElementTokens;
};

class HTMLParserScheduler {
    WTF_MAKE_NONCOPYABLE(HTMLParserScheduler); WTF_MAKE_FAST_ALLOCATED(HTMLParserScheduler);
public:
    static PassOwnPtr<HTMLParserScheduler> create(HTMLDocumentParser* parser)
    {
        return adoptPtr(new HTMLParserScheduler(parser));
    }
    ~HTMLParserScheduler();

    bool isScheduledForResume() const { return m_isSuspendedWithActiveTimer || m_cancellableContinueParse.isPending(); }

    void scheduleForResume();
    bool yieldIfNeeded(const SpeculationsPumpSession&, bool startingScript);

    /**
     * Can only be called if this scheduler is suspended. If this is called,
     * then after the scheduler is resumed by calling resume(), this call
     * ensures that HTMLDocumentParser::resumeAfterYield will be called. Used to
     * signal this scheduler that the background html parser sent chunks to
     * HTMLDocumentParser while it was suspended.
     */
    void forceResumeAfterYield();

    void suspend();
    void resume();

private:
    explicit HTMLParserScheduler(HTMLDocumentParser*);

    bool shouldYield(const SpeculationsPumpSession&, bool startingScript) const;
    void continueParsing();

    HTMLDocumentParser* m_parser;

    CancellableTaskFactory m_cancellableContinueParse;
    bool m_isSuspendedWithActiveTimer;
};

}

#endif
