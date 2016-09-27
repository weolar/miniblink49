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
#include "core/html/parser/HTMLDocumentParser.h"

#include "core/HTMLNames.h"
#include "core/css/MediaValuesCached.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentLifecycleObserver.h"
#include "core/dom/Element.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLDocument.h"
#include "core/html/parser/AtomicHTMLToken.h"
#include "core/html/parser/BackgroundHTMLParser.h"
#include "core/html/parser/HTMLParserScheduler.h"
#include "core/html/parser/HTMLParserThread.h"
#include "core/html/parser/HTMLScriptRunner.h"
#include "core/html/parser/HTMLTreeBuilder.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/loader/DocumentLoader.h"
#include "platform/SharedBuffer.h"
#include "platform/ThreadSafeFunctional.h"
#include "platform/ThreadedDataReceiver.h"
#include "platform/TraceEvent.h"
#include "platform/heap/Handle.h"
#include "public/platform/Platform.h"
#include "public/platform/WebScheduler.h"
#include "public/platform/WebThread.h"
#include "wtf/RefCounted.h"
#include "wtf/TemporaryChange.h"

namespace blink {

using namespace HTMLNames;

// This is a direct transcription of step 4 from:
// http://www.whatwg.org/specs/web-apps/current-work/multipage/the-end.html#fragment-case
static HTMLTokenizer::State tokenizerStateForContextElement(Element* contextElement, bool reportErrors, const HTMLParserOptions& options)
{
    if (!contextElement)
        return HTMLTokenizer::DataState;

    const QualifiedName& contextTag = contextElement->tagQName();

    if (contextTag.matches(titleTag) || contextTag.matches(textareaTag))
        return HTMLTokenizer::RCDATAState;
    if (contextTag.matches(styleTag)
        || contextTag.matches(xmpTag)
        || contextTag.matches(iframeTag)
        || (contextTag.matches(noembedTag) && options.pluginsEnabled)
        || (contextTag.matches(noscriptTag) && options.scriptEnabled)
        || contextTag.matches(noframesTag))
        return reportErrors ? HTMLTokenizer::RAWTEXTState : HTMLTokenizer::PLAINTEXTState;
    if (contextTag.matches(scriptTag))
        return reportErrors ? HTMLTokenizer::ScriptDataState : HTMLTokenizer::PLAINTEXTState;
    if (contextTag.matches(plaintextTag))
        return HTMLTokenizer::PLAINTEXTState;
    return HTMLTokenizer::DataState;
}

class ParserDataReceiver final : public RefCountedWillBeGarbageCollectedFinalized<ParserDataReceiver>, public ThreadedDataReceiver, public DocumentLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ParserDataReceiver);
public:
    static PassRefPtrWillBeRawPtr<ParserDataReceiver> create(WeakPtr<BackgroundHTMLParser> backgroundParser, Document* document)
    {
        return adoptRefWillBeNoop(new ParserDataReceiver(backgroundParser, document));
    }

#if !ENABLE(OILPAN)
    void ref() override { RefCounted<ParserDataReceiver>::ref(); }
    void deref() override { RefCounted<ParserDataReceiver>::deref(); }
#endif

    // ThreadedDataReceiver
    void acceptData(const char* data, int dataLength) override
    {
        ASSERT(backgroundThread() && backgroundThread()->isCurrentThread());
        if (m_backgroundParser.get())
            m_backgroundParser.get()->appendRawBytesFromParserThread(data, dataLength);
    }

    WebThread* backgroundThread() override
    {
        if (HTMLParserThread::shared())
            return &HTMLParserThread::shared()->platformThread();

        return nullptr;
    }

    bool needsMainthreadDataCopy() override { return InspectorInstrumentation::hasFrontends(); }
    void acceptMainthreadDataNotification(const char* data, int dataLength, int encodedDataLength) override
    {
        ASSERT(!data || needsMainthreadDataCopy());
        if (lifecycleContext())
            lifecycleContext()->loader()->acceptDataFromThreadedReceiver(data, dataLength, encodedDataLength);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        DocumentLifecycleObserver::trace(visitor);
    }

private:
    ParserDataReceiver(WeakPtr<BackgroundHTMLParser> backgroundParser, Document* document)
        : DocumentLifecycleObserver(document)
        , m_backgroundParser(backgroundParser)
    {
    }

    WeakPtr<BackgroundHTMLParser> m_backgroundParser;
};

HTMLDocumentParser::HTMLDocumentParser(HTMLDocument& document, bool reportErrors, ParserSynchronizationPolicy syncPolicy)
    : ScriptableDocumentParser(document)
    , m_options(&document)
    , m_token(syncPolicy == ForceSynchronousParsing ? adoptPtr(new HTMLToken) : nullptr)
    , m_tokenizer(syncPolicy == ForceSynchronousParsing ? HTMLTokenizer::create(m_options) : nullptr)
    , m_scriptRunner(HTMLScriptRunner::create(&document, this))
    , m_treeBuilder(HTMLTreeBuilder::create(this, &document, parserContentPolicy(), reportErrors, m_options))
    , m_parserScheduler(HTMLParserScheduler::create(this))
    , m_xssAuditorDelegate(&document)
    , m_weakFactory(this)
    , m_preloader(HTMLResourcePreloader::create(document))
    , m_shouldUseThreading(syncPolicy == AllowAsynchronousParsing)
    , m_endWasDelayed(false)
    , m_haveBackgroundParser(false)
    , m_tasksWereSuspended(false)
    , m_pumpSessionNestingLevel(0)
    , m_pumpSpeculationsSessionNestingLevel(0)
    , m_isParsingAtLineNumber(false)
{
    ASSERT(shouldUseThreading() || (m_token && m_tokenizer));
}

// FIXME: Member variables should be grouped into self-initializing structs to
// minimize code duplication between these constructors.
HTMLDocumentParser::HTMLDocumentParser(DocumentFragment* fragment, Element* contextElement, ParserContentPolicy parserContentPolicy)
    : ScriptableDocumentParser(fragment->document(), parserContentPolicy)
    , m_options(&fragment->document())
    , m_token(adoptPtr(new HTMLToken))
    , m_tokenizer(HTMLTokenizer::create(m_options))
    , m_treeBuilder(HTMLTreeBuilder::create(this, fragment, contextElement, this->parserContentPolicy(), m_options))
    , m_xssAuditorDelegate(&fragment->document())
    , m_weakFactory(this)
    , m_shouldUseThreading(false)
    , m_endWasDelayed(false)
    , m_haveBackgroundParser(false)
    , m_tasksWereSuspended(false)
    , m_pumpSessionNestingLevel(0)
    , m_pumpSpeculationsSessionNestingLevel(0)
{
    bool reportErrors = false; // For now document fragment parsing never reports errors.
    m_tokenizer->setState(tokenizerStateForContextElement(contextElement, reportErrors, m_options));
    m_xssAuditor.initForFragment();
}

HTMLDocumentParser::~HTMLDocumentParser()
{
#if ENABLE(OILPAN)
    if (m_haveBackgroundParser)
        stopBackgroundParser();
    // In Oilpan, HTMLDocumentParser can die together with Document, and
    // detach() is not called in this case.
#else
    ASSERT(!m_parserScheduler);
    ASSERT(!m_pumpSessionNestingLevel);
    ASSERT(!m_preloadScanner);
    ASSERT(!m_insertionPreloadScanner);
    ASSERT(!m_haveBackgroundParser);
    // FIXME: We should be able to ASSERT(m_speculations.isEmpty()),
    // but there are cases where that's not true currently. For example,
    // we we're told to stop parsing before we've consumed all the input.
#endif
}

DEFINE_TRACE(HTMLDocumentParser)
{
    visitor->trace(m_treeBuilder);
    visitor->trace(m_xssAuditorDelegate);
    visitor->trace(m_scriptRunner);
    visitor->trace(m_preloader);
    ScriptableDocumentParser::trace(visitor);
    HTMLScriptRunnerHost::trace(visitor);
}

void HTMLDocumentParser::detach()
{
    if (m_haveBackgroundParser)
        stopBackgroundParser();
    DocumentParser::detach();
    if (m_scriptRunner)
        m_scriptRunner->detach();
    m_treeBuilder->detach();
    // FIXME: It seems wrong that we would have a preload scanner here.
    // Yet during fast/dom/HTMLScriptElement/script-load-events.html we do.
    m_preloadScanner.clear();
    m_insertionPreloadScanner.clear();
    m_parserScheduler.clear(); // Deleting the scheduler will clear any timers.
    // Oilpan: It is important to clear m_token to deallocate backing memory of
    // HTMLToken::m_data and let the allocator reuse the memory for
    // HTMLToken::m_data of a next HTMLDocumentParser. We need to clear
    // m_tokenizer first because m_tokenizer has a raw pointer to m_token.
    m_tokenizer.clear();
    m_token.clear();
}

void HTMLDocumentParser::stopParsing()
{
    DocumentParser::stopParsing();
    m_parserScheduler.clear(); // Deleting the scheduler will clear any timers.
    if (m_haveBackgroundParser)
        stopBackgroundParser();
}

// This kicks off "Once the user agent stops parsing" as described by:
// http://www.whatwg.org/specs/web-apps/current-work/multipage/the-end.html#the-end
void HTMLDocumentParser::prepareToStopParsing()
{
    // FIXME: It may not be correct to disable this for the background parser.
    // That means hasInsertionPoint() may not be correct in some cases.
    ASSERT(!hasInsertionPoint() || m_haveBackgroundParser);

    // pumpTokenizer can cause this parser to be detached from the Document,
    // but we need to ensure it isn't deleted yet.
    RefPtrWillBeRawPtr<HTMLDocumentParser> protect(this);

    // NOTE: This pump should only ever emit buffered character tokens.
    if (m_tokenizer) {
        ASSERT(!m_haveBackgroundParser);
        pumpTokenizerIfPossible();
    }

    if (isStopped())
        return;

    DocumentParser::prepareToStopParsing();

    // We will not have a scriptRunner when parsing a DocumentFragment.
    if (m_scriptRunner)
        document()->setReadyState(Document::Interactive);

    // Setting the ready state above can fire mutation event and detach us
    // from underneath. In that case, just bail out.
    if (isDetached())
        return;

    attemptToRunDeferredScriptsAndEnd();
}

bool HTMLDocumentParser::isParsingFragment() const
{
    return m_treeBuilder->isParsingFragment();
}

bool HTMLDocumentParser::processingData() const
{
    return isScheduledForResume() || inPumpSession() || m_haveBackgroundParser;
}

void HTMLDocumentParser::pumpTokenizerIfPossible()
{
    if (isStopped() || isWaitingForScripts())
        return;

    pumpTokenizer();
}

bool HTMLDocumentParser::isScheduledForResume() const
{
    return m_parserScheduler && m_parserScheduler->isScheduledForResume();
}

// Used by HTMLParserScheduler
void HTMLDocumentParser::resumeParsingAfterYield()
{
    ASSERT(shouldUseThreading());
    ASSERT(m_haveBackgroundParser);

    // pumpPendingSpeculations can cause this parser to be detached from the Document,
    // but we need to ensure it isn't deleted yet.
    RefPtrWillBeRawPtr<HTMLDocumentParser> protect(this);
    pumpPendingSpeculations();
}

void HTMLDocumentParser::runScriptsForPausedTreeBuilder()
{
    ASSERT(scriptingContentIsAllowed(parserContentPolicy()));

    TextPosition scriptStartPosition = TextPosition::belowRangePosition();
    RefPtrWillBeRawPtr<Element> scriptElement = m_treeBuilder->takeScriptToProcess(scriptStartPosition);
    // We will not have a scriptRunner when parsing a DocumentFragment.
    if (m_scriptRunner)
        m_scriptRunner->execute(scriptElement.release(), scriptStartPosition);
}

bool HTMLDocumentParser::canTakeNextToken()
{
    if (isStopped())
        return false;

    if (isWaitingForScripts()) {
        // If we're paused waiting for a script, we try to execute scripts before continuing.
        runScriptsForPausedTreeBuilder();
        if (isStopped())
            return false;
        if (isWaitingForScripts())
            return false;
    }

    // FIXME: It's wrong for the HTMLDocumentParser to reach back to the
    //        LocalFrame, but this approach is how the old parser handled
    //        stopping when the page assigns window.location.  What really
    //        should happen is that assigning window.location causes the
    //        parser to stop parsing cleanly.  The problem is we're not
    //        perpared to do that at every point where we run JavaScript.
    if (!isParsingFragment()
        && document()->frame() && document()->frame()->navigationScheduler().locationChangePending())
        return false;

    return true;
}

void HTMLDocumentParser::didReceiveParsedChunkFromBackgroundParser(PassOwnPtr<ParsedChunk> chunk)
{
    TRACE_EVENT0("blink", "HTMLDocumentParser::didReceiveParsedChunkFromBackgroundParser");

    if (!isParsing())
        return;

    // ApplicationCache needs to be initialized before issuing preloads.
    // We suspend preload until HTMLHTMLElement is inserted and
    // ApplicationCache is initialized.
    if (!document()->documentElement()) {
        for (auto& request : chunk->preloads)
            m_queuedPreloads.append(request.release());
    } else {
        // We can safely assume that there are no queued preloads request after
        // the document element is available, as we empty the queue immediately
        // after the document element is created in pumpPendingSpeculations().
        ASSERT(m_queuedPreloads.isEmpty());
        m_preloader->takeAndPreload(chunk->preloads);
    }

    m_speculations.append(chunk);

    if (!isWaitingForScripts() && !isScheduledForResume()) {
        if (m_tasksWereSuspended)
            m_parserScheduler->forceResumeAfterYield();
        else
            m_parserScheduler->scheduleForResume();
    }
}

void HTMLDocumentParser::didReceiveEncodingDataFromBackgroundParser(const DocumentEncodingData& data)
{
    document()->setEncodingData(data);
}

void HTMLDocumentParser::validateSpeculations(PassOwnPtr<ParsedChunk> chunk)
{
    ASSERT(chunk);
    if (isWaitingForScripts()) {
        // We're waiting on a network script, just save the chunk, we'll get
        // a second validateSpeculations call after the script completes.
        // This call should have been made immediately after runScriptsForPausedTreeBuilder
        // which may have started a network load and left us waiting.
        ASSERT(!m_lastChunkBeforeScript);
        m_lastChunkBeforeScript = chunk;
        return;
    }

    ASSERT(!m_lastChunkBeforeScript);
    OwnPtr<HTMLTokenizer> tokenizer = m_tokenizer.release();
    OwnPtr<HTMLToken> token = m_token.release();

    if (!tokenizer) {
        // There must not have been any changes to the HTMLTokenizer state on
        // the main thread, which means the speculation buffer is correct.
        return;
    }

    // Currently we're only smart enough to reuse the speculation buffer if the tokenizer
    // both starts and ends in the DataState. That state is simplest because the HTMLToken
    // is always in the Uninitialized state. We should consider whether we can reuse the
    // speculation buffer in other states, but we'd likely need to do something more
    // sophisticated with the HTMLToken.
    if (chunk->tokenizerState == HTMLTokenizer::DataState
        && tokenizer->state() == HTMLTokenizer::DataState
        && m_input.current().isEmpty()
        && chunk->treeBuilderState == HTMLTreeBuilderSimulator::stateFor(m_treeBuilder.get())) {
        ASSERT(token->isUninitialized());
        return;
    }

    discardSpeculationsAndResumeFrom(chunk, token.release(), tokenizer.release());
}

void HTMLDocumentParser::discardSpeculationsAndResumeFrom(PassOwnPtr<ParsedChunk> lastChunkBeforeScript, PassOwnPtr<HTMLToken> token, PassOwnPtr<HTMLTokenizer> tokenizer)
{
    m_weakFactory.revokeAll();
    m_speculations.clear();

    OwnPtr<BackgroundHTMLParser::Checkpoint> checkpoint = adoptPtr(new BackgroundHTMLParser::Checkpoint);
    checkpoint->parser = m_weakFactory.createWeakPtr();
    checkpoint->token = token;
    checkpoint->tokenizer = tokenizer;
    checkpoint->treeBuilderState = HTMLTreeBuilderSimulator::stateFor(m_treeBuilder.get());
    checkpoint->inputCheckpoint = lastChunkBeforeScript->inputCheckpoint;
    checkpoint->preloadScannerCheckpoint = lastChunkBeforeScript->preloadScannerCheckpoint;
    checkpoint->unparsedInput = m_input.current().toString().isolatedCopy();
    m_input.current().clear(); // FIXME: This should be passed in instead of cleared.

    ASSERT(checkpoint->unparsedInput.isSafeToSendToAnotherThread());
    HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::resumeFrom, AllowCrossThreadAccess(m_backgroundParser), checkpoint.release()));
}

size_t HTMLDocumentParser::processParsedChunkFromBackgroundParser(PassOwnPtr<ParsedChunk> popChunk)
{
    TRACE_EVENT0("blink", "HTMLDocumentParser::processParsedChunkFromBackgroundParser");
    TemporaryChange<bool> hasLineNumber(m_isParsingAtLineNumber, true);

    ASSERT_WITH_SECURITY_IMPLICATION(document()->activeParserCount() == 1);
    ASSERT(!isParsingFragment());
    ASSERT(!isWaitingForScripts());
    ASSERT(!isStopped());
#if !ENABLE(OILPAN)
    // ASSERT that this object is both attached to the Document and protected.
    ASSERT(refCount() >= 2);
#endif
    ASSERT(shouldUseThreading());
    ASSERT(!m_tokenizer);
    ASSERT(!m_token);
    ASSERT(!m_lastChunkBeforeScript);

    OwnPtr<ParsedChunk> chunk(popChunk);
    OwnPtr<CompactHTMLTokenStream> tokens = chunk->tokens.release();
    size_t elementTokenCount = 0;

    HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::startedChunkWithCheckpoint, AllowCrossThreadAccess(m_backgroundParser), chunk->inputCheckpoint));

    for (const auto& xssInfo : chunk->xssInfos) {
        m_textPosition = xssInfo->m_textPosition;
        m_xssAuditorDelegate.didBlockScript(*xssInfo);
        if (isStopped())
            break;
    }

    for (Vector<CompactHTMLToken>::const_iterator it = tokens->begin(); it != tokens->end(); ++it) {
        ASSERT(!isWaitingForScripts());

        if (!chunk->startingScript && (it->type() == HTMLToken::StartTag || it->type() == HTMLToken::EndTag))
            elementTokenCount++;

        if (document()->frame() && document()->frame()->navigationScheduler().locationChangePending()) {

            // To match main-thread parser behavior (which never checks locationChangePending on the EOF path)
            // we peek to see if this chunk has an EOF and process it anyway.
            if (tokens->last().type() == HTMLToken::EndOfFile) {
                ASSERT(m_speculations.isEmpty()); // There should never be any chunks after the EOF.
                prepareToStopParsing();
            }
            break;
        }

        m_textPosition = it->textPosition();

        constructTreeFromCompactHTMLToken(*it);

        if (isStopped())
            break;

        if (!m_queuedPreloads.isEmpty() && document()->documentElement())
            m_preloader->takeAndPreload(m_queuedPreloads);

        if (isWaitingForScripts()) {
            ASSERT(it + 1 == tokens->end()); // The </script> is assumed to be the last token of this bunch.
            runScriptsForPausedTreeBuilder();
            validateSpeculations(chunk.release());
            break;
        }

        if (it->type() == HTMLToken::EndOfFile) {
            ASSERT(it + 1 == tokens->end()); // The EOF is assumed to be the last token of this bunch.
            ASSERT(m_speculations.isEmpty()); // There should never be any chunks after the EOF.
            prepareToStopParsing();
            break;
        }

        ASSERT(!m_tokenizer);
        ASSERT(!m_token);
    }

    // Make sure all required pending text nodes are emitted before returning.
    // This leaves "script", "style" and "svg" nodes text nodes intact.
    if (!isStopped())
        m_treeBuilder->flush(FlushIfAtTextLimit);

    m_isParsingAtLineNumber = false;

    return elementTokenCount;
}

void HTMLDocumentParser::pumpPendingSpeculations()
{
#if !ENABLE(OILPAN)
    // ASSERT that this object is both attached to the Document and protected.
    ASSERT(refCount() >= 2);
#endif
    // If this assert fails, you need to call validateSpeculations to make sure
    // m_tokenizer and m_token don't have state that invalidates m_speculations.
    ASSERT(!m_tokenizer);
    ASSERT(!m_token);
    ASSERT(!m_lastChunkBeforeScript);
    ASSERT(!isWaitingForScripts());
    ASSERT(!isStopped());
    ASSERT(!isScheduledForResume());
    ASSERT(!inPumpSession());

    // FIXME: Here should never be reached when there is a blocking script,
    // but it happens in unknown scenarios. See https://crbug.com/440901
    if (isWaitingForScripts()) {
        m_parserScheduler->scheduleForResume();
        return;
    }

    // Do not allow pumping speculations in nested event loops.
    if (m_pumpSpeculationsSessionNestingLevel) {
        m_parserScheduler->scheduleForResume();
        return;
    }

    // FIXME: Pass in current input length.
    TRACE_EVENT_BEGIN1("devtools.timeline", "ParseHTML", "beginData", InspectorParseHtmlEvent::beginData(document(), lineNumber().zeroBasedInt()));

    SpeculationsPumpSession session(m_pumpSpeculationsSessionNestingLevel, contextForParsingSession());
    while (!m_speculations.isEmpty()) {
        ASSERT(!isScheduledForResume());
        size_t elementTokenCount = processParsedChunkFromBackgroundParser(m_speculations.takeFirst());
        session.addedElementTokens(elementTokenCount);

        // Always check isParsing first as m_document may be null.
        // Surprisingly, isScheduledForResume() may be set here as a result of
        // processParsedChunkFromBackgroundParser running arbitrary javascript
        // which invokes nested event loops. (e.g. inspector breakpoints)
        if (!isParsing() || isWaitingForScripts() || isScheduledForResume())
            break;

        if (m_speculations.isEmpty() || m_parserScheduler->yieldIfNeeded(session, m_speculations.first()->startingScript))
            break;
    }

    TRACE_EVENT_END1("devtools.timeline", "ParseHTML", "endData", InspectorParseHtmlEvent::endData(lineNumber().zeroBasedInt() - 1));
    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "UpdateCounters", TRACE_EVENT_SCOPE_THREAD, "data", InspectorUpdateCountersEvent::data());
}

void HTMLDocumentParser::forcePlaintextForTextDocument()
{
    if (shouldUseThreading()) {
        // This method is called before any data is appended, so we have to start
        // the background parser ourselves.
        if (!m_haveBackgroundParser)
            startBackgroundParser();

        HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::forcePlaintextForTextDocument, AllowCrossThreadAccess(m_backgroundParser)));
    } else
        m_tokenizer->setState(HTMLTokenizer::PLAINTEXTState);
}

Document* HTMLDocumentParser::contextForParsingSession()
{
    // The parsing session should interact with the document only when parsing
    // non-fragments. Otherwise, we might delay the load event mistakenly.
    if (isParsingFragment())
        return nullptr;
    return document();
}

void HTMLDocumentParser::pumpTokenizer()
{
    ASSERT(!isStopped());
#if !ENABLE(OILPAN)
    // ASSERT that this object is both attached to the Document and protected.
    ASSERT(refCount() >= 2);
#endif
    ASSERT(m_tokenizer);
    ASSERT(m_token);

    PumpSession session(m_pumpSessionNestingLevel, contextForParsingSession());

    // We tell the InspectorInstrumentation about every pump, even if we
    // end up pumping nothing.  It can filter out empty pumps itself.
    // FIXME: m_input.current().length() is only accurate if we
    // end up parsing the whole buffer in this pump.  We should pass how
    // much we parsed as part of didWriteHTML instead of willWriteHTML.
    TRACE_EVENT_BEGIN1("devtools.timeline", "ParseHTML", "beginData", InspectorParseHtmlEvent::beginData(document(), m_input.current().currentLine().zeroBasedInt()));

    m_xssAuditor.init(document(), &m_xssAuditorDelegate);

    while (canTakeNextToken()) {
        if (!isParsingFragment())
            m_sourceTracker.start(m_input.current(), m_tokenizer.get(), token());

        if (!m_tokenizer->nextToken(m_input.current(), token()))
            break;

        if (!isParsingFragment()) {
            m_sourceTracker.end(m_input.current(), m_tokenizer.get(), token());

            // We do not XSS filter innerHTML, which means we (intentionally) fail
            // http/tests/security/xssAuditor/dom-write-innerHTML.html
            if (OwnPtr<XSSInfo> xssInfo = m_xssAuditor.filterToken(FilterTokenRequest(token(), m_sourceTracker, m_tokenizer->shouldAllowCDATA())))
                m_xssAuditorDelegate.didBlockScript(*xssInfo);
        }

        constructTreeFromHTMLToken();
        ASSERT(isStopped() || token().isUninitialized());
    }

#if !ENABLE(OILPAN)
    // Ensure we haven't been totally deref'ed after pumping. Any caller of this
    // function should be holding a RefPtr to this to ensure we weren't deleted.
    ASSERT(refCount() >= 1);
#endif

    if (isStopped())
        return;

    // There should only be PendingText left since the tree-builder always flushes
    // the task queue before returning. In case that ever changes, crash.
    m_treeBuilder->flush(FlushAlways);
    RELEASE_ASSERT(!isStopped());

    if (isWaitingForScripts()) {
        ASSERT(m_tokenizer->state() == HTMLTokenizer::DataState);

        ASSERT(m_preloader);
        // TODO(kouhei): m_preloader should be always available for synchronous parsing case,
        // adding paranoia if for speculative crash fix for crbug.com/465478
        if (m_preloader) {
            if (!m_preloadScanner) {
                m_preloadScanner = adoptPtr(new HTMLPreloadScanner(m_options,
                    document()->url(),
                    CachedDocumentParameters::create(document())));
                m_preloadScanner->appendToEnd(m_input.current());
            }
            m_preloadScanner->scan(m_preloader.get(), document()->baseElementURL());
        }
    }

    TRACE_EVENT_END1("devtools.timeline", "ParseHTML", "endData", InspectorParseHtmlEvent::endData(m_input.current().currentLine().zeroBasedInt() - 1));
}

void HTMLDocumentParser::constructTreeFromHTMLToken()
{
    AtomicHTMLToken atomicToken(token());

    // We clear the m_token in case constructTreeFromAtomicToken
    // synchronously re-enters the parser. We don't clear the token immedately
    // for Character tokens because the AtomicHTMLToken avoids copying the
    // characters by keeping a pointer to the underlying buffer in the
    // HTMLToken. Fortunately, Character tokens can't cause us to re-enter
    // the parser.
    //
    // FIXME: Stop clearing the m_token once we start running the parser off
    // the main thread or once we stop allowing synchronous JavaScript
    // execution from parseAttribute.
    if (token().type() != HTMLToken::Character)
        token().clear();

    m_treeBuilder->constructTree(&atomicToken);

    // FIXME: constructTree may synchronously cause Document to be detached.
    if (!m_token)
        return;

    if (!token().isUninitialized()) {
        ASSERT(token().type() == HTMLToken::Character);
        token().clear();
    }
}

void HTMLDocumentParser::constructTreeFromCompactHTMLToken(const CompactHTMLToken& compactToken)
{
    AtomicHTMLToken token(compactToken);
    m_treeBuilder->constructTree(&token);
}

bool HTMLDocumentParser::hasInsertionPoint()
{
    // FIXME: The wasCreatedByScript() branch here might not be fully correct.
    //        Our model of the EOF character differs slightly from the one in
    //        the spec because our treatment is uniform between network-sourced
    //        and script-sourced input streams whereas the spec treats them
    //        differently.
    return m_input.hasInsertionPoint() || (wasCreatedByScript() && !m_input.haveSeenEndOfFile());
}

void HTMLDocumentParser::insert(const SegmentedString& source)
{
    if (isStopped())
        return;

    TRACE_EVENT1("blink", "HTMLDocumentParser::insert", "source_length", source.length());

    // pumpTokenizer can cause this parser to be detached from the Document,
    // but we need to ensure it isn't deleted yet.
    RefPtrWillBeRawPtr<HTMLDocumentParser> protect(this);

    if (!m_tokenizer) {
        ASSERT(!inPumpSession());
        ASSERT(m_haveBackgroundParser || wasCreatedByScript());
        m_token = adoptPtr(new HTMLToken);
        m_tokenizer = HTMLTokenizer::create(m_options);
    }

    SegmentedString excludedLineNumberSource(source);
    excludedLineNumberSource.setExcludeLineNumbers();
    m_input.insertAtCurrentInsertionPoint(excludedLineNumberSource);
    pumpTokenizerIfPossible();

    if (isWaitingForScripts()) {
        // Check the document.write() output with a separate preload scanner as
        // the main scanner can't deal with insertions.
        if (!m_insertionPreloadScanner) {
            m_insertionPreloadScanner = adoptPtr(new HTMLPreloadScanner(m_options,
                document()->url(),
                CachedDocumentParameters::create(document())));
        }

        m_insertionPreloadScanner->appendToEnd(source);
        m_insertionPreloadScanner->scan(m_preloader.get(), document()->baseElementURL());
    }

    endIfDelayed();
}

void HTMLDocumentParser::startBackgroundParser()
{
    ASSERT(!isStopped());
    ASSERT(shouldUseThreading());
    ASSERT(!m_haveBackgroundParser);
    ASSERT(document());
    m_haveBackgroundParser = true;

    RefPtr<WeakReference<BackgroundHTMLParser>> reference = WeakReference<BackgroundHTMLParser>::createUnbound();
    m_backgroundParser = WeakPtr<BackgroundHTMLParser>(reference);

    // FIXME(oysteine): Disabled due to crbug.com/398076 until a full fix can be implemented.
    if (RuntimeEnabledFeatures::threadedParserDataReceiverEnabled()) {
        if (DocumentLoader* loader = document()->loader())
            loader->attachThreadedDataReceiver(ParserDataReceiver::create(m_backgroundParser, document()->contextDocument().get()));
    }

    OwnPtr<BackgroundHTMLParser::Configuration> config = adoptPtr(new BackgroundHTMLParser::Configuration);
    config->options = m_options;
    config->parser = m_weakFactory.createWeakPtr();
    config->xssAuditor = adoptPtr(new XSSAuditor);
    config->xssAuditor->init(document(), &m_xssAuditorDelegate);
    config->preloadScanner = adoptPtr(new TokenPreloadScanner(document()->url().copy(), CachedDocumentParameters::create(document())));
    config->decoder = takeDecoder();
    if (document()->settings()) {
        if (document()->settings()->backgroundHtmlParserOutstandingTokenLimit())
            config->outstandingTokenLimit = document()->settings()->backgroundHtmlParserOutstandingTokenLimit();
        if (document()->settings()->backgroundHtmlParserPendingTokenLimit())
            config->pendingTokenLimit = document()->settings()->backgroundHtmlParserPendingTokenLimit();
    }

    ASSERT(config->xssAuditor->isSafeToSendToAnotherThread());
    ASSERT(config->preloadScanner->isSafeToSendToAnotherThread());
    HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::start, reference.release(), config.release(),
        AllowCrossThreadAccess(Platform::current()->currentThread()->scheduler())));
}

void HTMLDocumentParser::stopBackgroundParser()
{
    ASSERT(shouldUseThreading());
    ASSERT(m_haveBackgroundParser);
    m_haveBackgroundParser = false;

    HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::stop, AllowCrossThreadAccess(m_backgroundParser)));
    m_weakFactory.revokeAll();
}

void HTMLDocumentParser::append(const String& inputSource)
{
    if (isStopped())
        return;

    // We should never reach this point if we're using a parser thread,
    // as appendBytes() will directly ship the data to the thread.
    ASSERT(!shouldUseThreading());

    // pumpTokenizer can cause this parser to be detached from the Document,
    // but we need to ensure it isn't deleted yet.
    RefPtrWillBeRawPtr<HTMLDocumentParser> protect(this);
    TRACE_EVENT1(TRACE_DISABLED_BY_DEFAULT("blink.debug"), "HTMLDocumentParser::append", "size", inputSource.length());
    const SegmentedString source(inputSource);

    if (m_preloadScanner) {
        if (m_input.current().isEmpty() && !isWaitingForScripts()) {
            // We have parsed until the end of the current input and so are now moving ahead of the preload scanner.
            // Clear the scanner so we know to scan starting from the current input point if we block again.
            m_preloadScanner.clear();
        } else {
            m_preloadScanner->appendToEnd(source);
            if (isWaitingForScripts())
                m_preloadScanner->scan(m_preloader.get(), document()->baseElementURL());
        }
    }

    m_input.appendToEnd(source);

    if (inPumpSession()) {
        // We've gotten data off the network in a nested write.
        // We don't want to consume any more of the input stream now.  Do
        // not worry.  We'll consume this data in a less-nested write().
        return;
    }

    pumpTokenizerIfPossible();

    endIfDelayed();
}

void HTMLDocumentParser::end()
{
    ASSERT(!isDetached());
    ASSERT(!isScheduledForResume());

    if (m_haveBackgroundParser)
        stopBackgroundParser();

    // Informs the the rest of WebCore that parsing is really finished (and deletes this).
    m_treeBuilder->finished();

    DocumentParser::stopParsing();
}

void HTMLDocumentParser::attemptToRunDeferredScriptsAndEnd()
{
    ASSERT(isStopping());
    // FIXME: It may not be correct to disable this for the background parser.
    // That means hasInsertionPoint() may not be correct in some cases.
    ASSERT(!hasInsertionPoint() || m_haveBackgroundParser);
    if (m_scriptRunner && !m_scriptRunner->executeScriptsWaitingForParsing())
        return;
    end();
}

void HTMLDocumentParser::attemptToEnd()
{
    // finish() indicates we will not receive any more data. If we are waiting on
    // an external script to load, we can't finish parsing quite yet.

    if (shouldDelayEnd()) {
        m_endWasDelayed = true;
        return;
    }
    prepareToStopParsing();
}

void HTMLDocumentParser::endIfDelayed()
{
    // If we've already been detached, don't bother ending.
    if (isDetached())
        return;

    if (!m_endWasDelayed || shouldDelayEnd())
        return;

    m_endWasDelayed = false;
    prepareToStopParsing();
}

void HTMLDocumentParser::finish()
{
    // FIXME: We should ASSERT(!m_parserStopped) here, since it does not
    // makes sense to call any methods on DocumentParser once it's been stopped.
    // However, FrameLoader::stop calls DocumentParser::finish unconditionally.

    // flush may ending up executing arbitrary script, and possibly detach the parser.
    RefPtrWillBeRawPtr<HTMLDocumentParser> protect(this);
    flush();
    if (isDetached())
        return;

    // Empty documents never got an append() call, and thus have never started
    // a background parser. In those cases, we ignore shouldUseThreading()
    // and fall through to the non-threading case.
    if (m_haveBackgroundParser) {
        if (!m_input.haveSeenEndOfFile())
            m_input.closeWithoutMarkingEndOfFile();
        HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::finish, AllowCrossThreadAccess(m_backgroundParser)));
        return;
    }

    if (!m_tokenizer) {
        ASSERT(!m_token);
        // We're finishing before receiving any data. Rather than booting up
        // the background parser just to spin it down, we finish parsing
        // synchronously.
        m_token = adoptPtr(new HTMLToken);
        m_tokenizer = HTMLTokenizer::create(m_options);
    }

    // We're not going to get any more data off the network, so we tell the
    // input stream we've reached the end of file. finish() can be called more
    // than once, if the first time does not call end().
    if (!m_input.haveSeenEndOfFile())
        m_input.markEndOfFile();

    attemptToEnd();
}

bool HTMLDocumentParser::isExecutingScript() const
{
    if (!m_scriptRunner)
        return false;
    return m_scriptRunner->isExecutingScript();
}

bool HTMLDocumentParser::isParsingAtLineNumber() const
{
    return m_isParsingAtLineNumber && ScriptableDocumentParser::isParsingAtLineNumber();
}

OrdinalNumber HTMLDocumentParser::lineNumber() const
{
    if (m_haveBackgroundParser)
        return m_textPosition.m_line;

    return m_input.current().currentLine();
}

TextPosition HTMLDocumentParser::textPosition() const
{
    if (m_haveBackgroundParser)
        return m_textPosition;

    const SegmentedString& currentString = m_input.current();
    OrdinalNumber line = currentString.currentLine();
    OrdinalNumber column = currentString.currentColumn();

    return TextPosition(line, column);
}

bool HTMLDocumentParser::isWaitingForScripts() const
{
    // When the TreeBuilder encounters a </script> tag, it returns to the HTMLDocumentParser
    // where the script is transfered from the treebuilder to the script runner.
    // The script runner will hold the script until its loaded and run. During
    // any of this time, we want to count ourselves as "waiting for a script" and thus
    // run the preload scanner, as well as delay completion of parsing.
    bool treeBuilderHasBlockingScript = m_treeBuilder->hasParserBlockingScript();
    bool scriptRunnerHasBlockingScript = m_scriptRunner && m_scriptRunner->hasParserBlockingScript();
    // Since the parser is paused while a script runner has a blocking script, it should
    // never be possible to end up with both objects holding a blocking script.
    ASSERT(!(treeBuilderHasBlockingScript && scriptRunnerHasBlockingScript));
    // If either object has a blocking script, the parser should be paused.
    return treeBuilderHasBlockingScript || scriptRunnerHasBlockingScript;
}

void HTMLDocumentParser::resumeParsingAfterScriptExecution()
{
    ASSERT(!isExecutingScript());
    ASSERT(!isWaitingForScripts());

    if (m_haveBackgroundParser) {
        validateSpeculations(m_lastChunkBeforeScript.release());
        ASSERT(!m_lastChunkBeforeScript);
        // processParsedChunkFromBackgroundParser can cause this parser to be detached from the Document,
        // but we need to ensure it isn't deleted yet.
        RefPtrWillBeRawPtr<HTMLDocumentParser> protect(this);
        pumpPendingSpeculations();
        return;
    }

    m_insertionPreloadScanner.clear();
    pumpTokenizerIfPossible();
    endIfDelayed();
}

void HTMLDocumentParser::appendCurrentInputStreamToPreloadScannerAndScan()
{
    ASSERT(m_preloadScanner);
    m_preloadScanner->appendToEnd(m_input.current());
    m_preloadScanner->scan(m_preloader.get(), document()->baseElementURL());
}

void HTMLDocumentParser::notifyScriptLoaded(Resource* cachedResource)
{
    // pumpTokenizer can cause this parser to be detached from the Document,
    // but we need to ensure it isn't deleted yet.
    RefPtrWillBeRawPtr<HTMLDocumentParser> protect(this);

    ASSERT(m_scriptRunner);
    ASSERT(!isExecutingScript());

    if (isStopped()) {
        return;
    }

    if (isStopping()) {
        attemptToRunDeferredScriptsAndEnd();
        return;
    }

    m_scriptRunner->executeScriptsWaitingForLoad(cachedResource);
    if (!isWaitingForScripts())
        resumeParsingAfterScriptExecution();
}

void HTMLDocumentParser::executeScriptsWaitingForResources()
{
    // Document only calls this when the Document owns the DocumentParser
    // so this will not be called in the DocumentFragment case.
    ASSERT(m_scriptRunner);
    // Ignore calls unless we have a script blocking the parser waiting on a
    // stylesheet load.  Otherwise we are currently parsing and this
    // is a re-entrant call from encountering a </ style> tag.
    if (!m_scriptRunner->hasScriptsWaitingForResources())
        return;

    // pumpTokenizer can cause this parser to be detached from the Document,
    // but we need to ensure it isn't deleted yet.
    RefPtrWillBeRawPtr<HTMLDocumentParser> protect(this);
    m_scriptRunner->executeScriptsWaitingForResources();
    if (!isWaitingForScripts())
        resumeParsingAfterScriptExecution();
}

void HTMLDocumentParser::parseDocumentFragment(const String& source, DocumentFragment* fragment, Element* contextElement, ParserContentPolicy parserContentPolicy)
{
    RefPtrWillBeRawPtr<HTMLDocumentParser> parser = HTMLDocumentParser::create(fragment, contextElement, parserContentPolicy);
    parser->append(source);
    parser->finish();
    ASSERT(!parser->processingData()); // Make sure we're done. <rdar://problem/3963151>
    parser->detach(); // Allows ~DocumentParser to assert it was detached before destruction.
}

void HTMLDocumentParser::suspendScheduledTasks()
{
    ASSERT(!m_tasksWereSuspended);
    m_tasksWereSuspended = true;
    if (m_parserScheduler)
        m_parserScheduler->suspend();
}

void HTMLDocumentParser::resumeScheduledTasks()
{
    ASSERT(m_tasksWereSuspended);
    m_tasksWereSuspended = false;
    if (m_parserScheduler)
        m_parserScheduler->resume();
}

void HTMLDocumentParser::appendBytes(const char* data, size_t length)
{
    if (!length || isStopped())
        return;

    if (shouldUseThreading()) {
        if (!m_haveBackgroundParser)
            startBackgroundParser();

        OwnPtr<Vector<char>> buffer = adoptPtr(new Vector<char>(length));
        memcpy(buffer->data(), data, length);
        TRACE_EVENT1(TRACE_DISABLED_BY_DEFAULT("blink.debug"), "HTMLDocumentParser::appendBytes", "size", (unsigned)length);

        HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::appendRawBytesFromMainThread, AllowCrossThreadAccess(m_backgroundParser), buffer.release()));
        return;
    }

    DecodedDataDocumentParser::appendBytes(data, length);
}

void HTMLDocumentParser::flush()
{
    // If we've got no decoder, we never received any data.
    if (isDetached() || needsDecoder())
        return;

    if (m_haveBackgroundParser)
        HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::flush, AllowCrossThreadAccess(m_backgroundParser)));
    else
        DecodedDataDocumentParser::flush();
}

void HTMLDocumentParser::setDecoder(PassOwnPtr<TextResourceDecoder> decoder)
{
    ASSERT(decoder);
    DecodedDataDocumentParser::setDecoder(decoder);

    if (m_haveBackgroundParser)
        HTMLParserThread::shared()->postTask(threadSafeBind(&BackgroundHTMLParser::setDecoder, AllowCrossThreadAccess(m_backgroundParser), takeDecoder()));
}

}
