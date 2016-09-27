/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2005 Alexey Proskuryakov.
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
#include "core/editing/iterators/CharacterIterator.h"

#include "core/dom/Document.h"
#include "core/editing/iterators/SimplifiedBackwardsTextIterator.h"
#include "platform/fonts/Character.h"
#include "platform/text/TextBoundaries.h"
#include "platform/text/TextBreakIteratorInternalICU.h"
#include "platform/text/UnicodeUtilities.h"
#include "wtf/text/CharacterNames.h"
//#include <unicode/usearch.h>

namespace blink {

// Buffer that knows how to compare with a search target.
// Keeps enough of the previous text to be able to search in the future, but no more.
// Non-breaking spaces are always equal to normal spaces.
// Case folding is also done if the CaseInsensitive option is specified.
// Matches are further filtered if the AtWordStarts option is specified, although some
// matches inside a word are permitted if TreatMedialCapitalAsWordStart is specified as well.
class SearchBuffer {
    WTF_MAKE_NONCOPYABLE(SearchBuffer);
public:
    SearchBuffer(const String& target, FindOptions);
    ~SearchBuffer();

    // Returns number of characters appended; guaranteed to be in the range [1, length].
    template<typename CharType>
    void append(const CharType*, size_t length);
    size_t numberOfCharactersJustAppended() const { return m_numberOfCharactersJustAppended; }

    bool needsMoreContext() const;
    void prependContext(const UChar*, size_t length);
    void reachedBreak();

    // Result is the size in characters of what was found.
    // And <startOffset> is the number of characters back to the start of what was found.
    size_t search(size_t& startOffset);
    bool atBreak() const;

private:
    bool isBadMatch(const UChar*, size_t length) const;
    bool isWordStartMatch(size_t start, size_t length) const;

    Vector<UChar> m_target;
    FindOptions m_options;

    Vector<UChar> m_buffer;
    size_t m_overlap;
    size_t m_prefixLength;
    size_t m_numberOfCharactersJustAppended;
    bool m_atBreak;
    bool m_needsMoreContext;

    bool m_targetRequiresKanaWorkaround;
    Vector<UChar> m_normalizedTarget;
    mutable Vector<UChar> m_normalizedMatch;
};

template <typename Strategy>
CharacterIteratorAlgorithm<Strategy>::CharacterIteratorAlgorithm(const PositionAlgorithm<Strategy>& start, const PositionAlgorithm<Strategy>& end, TextIteratorBehaviorFlags behavior)
    : m_offset(0)
    , m_runOffset(0)
    , m_atBreak(true)
    , m_textIterator(start, end, behavior)
{
    initialize();
}

template <typename Strategy>
CharacterIteratorAlgorithm<Strategy>::CharacterIteratorAlgorithm(const EphemeralRangeTemplate<Strategy>& range, TextIteratorBehaviorFlags behavior)
    : CharacterIterator(range.startPosition(), range.endPosition(), behavior)
{
}

template <typename Strategy>
void CharacterIteratorAlgorithm<Strategy>::initialize()
{
    while (!atEnd() && !m_textIterator.length())
        m_textIterator.advance();
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy> CharacterIteratorAlgorithm<Strategy>::range() const
{
    EphemeralRangeTemplate<Strategy> range(m_textIterator.range());
    if (m_textIterator.atEnd() || m_textIterator.length() <= 1)
        return range;
    PositionAlgorithm<Strategy> startPosition = range.startPosition().parentAnchoredEquivalent();
    PositionAlgorithm<Strategy> endPosition = range.endPosition().parentAnchoredEquivalent();
    Node* node = startPosition.containerNode();
    ASSERT(node == endPosition.containerNode());
    int offset = startPosition.offsetInContainerNode() + m_runOffset;
    return EphemeralRangeTemplate<Strategy>(PositionAlgorithm<Strategy>(node, offset), PositionAlgorithm<Strategy>(node, offset + 1));
}

template <typename Strategy>
Document* CharacterIteratorAlgorithm<Strategy>::ownerDocument() const
{
    return m_textIterator.ownerDocument();
}

template <typename Strategy>
Node* CharacterIteratorAlgorithm<Strategy>::currentContainer() const
{
    return m_textIterator.currentContainer();
}

template <typename Strategy>
int CharacterIteratorAlgorithm<Strategy>::startOffset() const
{
    if (!m_textIterator.atEnd()) {
        if (m_textIterator.length() > 1)
            return m_textIterator.startOffsetInCurrentContainer() + m_runOffset;
        ASSERT(!m_runOffset);
    }
    return m_textIterator.startOffsetInCurrentContainer();
}

template <typename Strategy>
int CharacterIteratorAlgorithm<Strategy>::endOffset() const
{
    if (!m_textIterator.atEnd()) {
        if (m_textIterator.length() > 1)
            return m_textIterator.startOffsetInCurrentContainer() + m_runOffset + 1;
        ASSERT(!m_runOffset);
    }
    return m_textIterator.endOffsetInCurrentContainer();
}

template <typename Strategy>
PositionAlgorithm<Strategy> CharacterIteratorAlgorithm<Strategy>::startPosition() const
{
    if (!m_textIterator.atEnd()) {
        if (m_textIterator.length() > 1) {
            Node* n = m_textIterator.currentContainer();
            int offset = m_textIterator.startOffsetInCurrentContainer() + m_runOffset;
            return PositionAlgorithm<Strategy>::createLegacyEditingPosition(n, offset);
        }
        ASSERT(!m_runOffset);
    }
    return m_textIterator.startPositionInCurrentContainer();
}

template <typename Strategy>
PositionAlgorithm<Strategy> CharacterIteratorAlgorithm<Strategy>::endPosition() const
{
    if (!m_textIterator.atEnd()) {
        if (m_textIterator.length() > 1) {
            Node* n = m_textIterator.currentContainer();
            int offset = m_textIterator.startOffsetInCurrentContainer() + m_runOffset;
            return PositionAlgorithm<Strategy>::createLegacyEditingPosition(n, offset + 1);
        }
        ASSERT(!m_runOffset);
    }
    return m_textIterator.endPositionInCurrentContainer();
}

template <typename Strategy>
void CharacterIteratorAlgorithm<Strategy>::advance(int count)
{
    if (count <= 0) {
        ASSERT(!count);
        return;
    }

    m_atBreak = false;

    // easy if there is enough left in the current m_textIterator run
    int remaining = m_textIterator.length() - m_runOffset;
    if (count < remaining) {
        m_runOffset += count;
        m_offset += count;
        return;
    }

    // exhaust the current m_textIterator run
    count -= remaining;
    m_offset += remaining;

    // move to a subsequent m_textIterator run
    for (m_textIterator.advance(); !atEnd(); m_textIterator.advance()) {
        int runLength = m_textIterator.length();
        if (!runLength) {
            m_atBreak = m_textIterator.breaksAtReplacedElement();
        } else {
            // see whether this is m_textIterator to use
            if (count < runLength) {
                m_runOffset = count;
                m_offset += count;
                return;
            }

            // exhaust this m_textIterator run
            count -= runLength;
            m_offset += runLength;
        }
    }

    // ran to the end of the m_textIterator... no more runs left
    m_atBreak = true;
    m_runOffset = 0;
}

template <typename Strategy>
EphemeralRangeTemplate<Strategy> CharacterIteratorAlgorithm<Strategy>::calculateCharacterSubrange(int offset, int length)
{
    advance(offset);
    const PositionAlgorithm<Strategy> startPos = startPosition();

    if (length > 1)
        advance(length - 1);
    return EphemeralRangeTemplate<Strategy>(startPos, endPosition());
}

static const size_t minimumSearchBufferSize = 8192;

#if ENABLE(ASSERT)
static bool searcherInUse;
#endif

#ifdef MINIBLINK_NOT_IMPLEMENTED

static UStringSearch* createSearcher()
{
    // Provide a non-empty pattern and non-empty text so usearch_open will not fail,
    // but it doesn't matter exactly what it is, since we don't perform any searches
    // without setting both the pattern and the text.
    UErrorCode status = U_ZERO_ERROR;
    String searchCollatorName = currentSearchLocaleID() + String("@collation=search");
    UStringSearch* searcher = usearch_open(&newlineCharacter, 1, &newlineCharacter, 1, searchCollatorName.utf8().data(), 0, &status);
    ASSERT(status == U_ZERO_ERROR || status == U_USING_FALLBACK_WARNING || status == U_USING_DEFAULT_WARNING);
    return searcher;
}

static UStringSearch* searcher()
{
    static UStringSearch* searcher = createSearcher();
    return searcher;
}

#endif // MINIBLINK_NOT_IMPLEMENTED

static inline void lockSearcher()
{
#if ENABLE(ASSERT)
    ASSERT(!searcherInUse);
    searcherInUse = true;
#endif
}

static inline void unlockSearcher()
{
#if ENABLE(ASSERT)
    ASSERT(searcherInUse);
    searcherInUse = false;
#endif
}

inline SearchBuffer::SearchBuffer(const String& target, FindOptions options)
    : m_options(options)
    , m_prefixLength(0)
    , m_numberOfCharactersJustAppended(0)
    , m_atBreak(true)
    , m_needsMoreContext(options & AtWordStarts)
    , m_targetRequiresKanaWorkaround(containsKanaLetters(target))
{
    ASSERT(!target.isEmpty());
    target.appendTo(m_target);

    // FIXME: We'd like to tailor the searcher to fold quote marks for us instead
    // of doing it in a separate replacement pass here, but ICU doesn't offer a way
    // to add tailoring on top of the locale-specific tailoring as of this writing.
    foldQuoteMarksAndSoftHyphens(m_target.data(), m_target.size());

    size_t targetLength = m_target.size();
    m_buffer.reserveInitialCapacity(std::max(targetLength * 8, minimumSearchBufferSize));
    m_overlap = m_buffer.capacity() / 4;

    if ((m_options & AtWordStarts) && targetLength) {
        UChar32 targetFirstCharacter;
        U16_GET(m_target.data(), 0, 0, targetLength, targetFirstCharacter);
        // Characters in the separator category never really occur at the beginning of a word,
        // so if the target begins with such a character, we just ignore the AtWordStart option.
        if (isSeparator(targetFirstCharacter)) {
            m_options &= ~AtWordStarts;
            m_needsMoreContext = false;
        }
    }

    // Grab the single global searcher.
    // If we ever have a reason to do more than once search buffer at once, we'll have
    // to move to multiple searchers.
    lockSearcher();

#ifdef MINIBLINK_NOT_IMPLEMENTED
    UStringSearch* searcher = blink::searcher();
    UCollator* collator = usearch_getCollator(searcher);

    UCollationStrength strength = m_options & CaseInsensitive ? UCOL_PRIMARY : UCOL_TERTIARY;
    if (ucol_getStrength(collator) != strength) {
        ucol_setStrength(collator, strength);
        usearch_reset(searcher);
    }

    UErrorCode status = U_ZERO_ERROR;
    usearch_setPattern(searcher, m_target.data(), targetLength, &status);
    ASSERT(status == U_ZERO_ERROR);
#endif // MINIBLINK_NOT_IMPLEMENTED
	notImplemented();

    // The kana workaround requires a normalized copy of the target string.
    if (m_targetRequiresKanaWorkaround)
        normalizeCharactersIntoNFCForm(m_target.data(), m_target.size(), m_normalizedTarget);
}

inline SearchBuffer::~SearchBuffer()
{
    // Leave the static object pointing to valid strings (pattern=targer,
    // text=buffer). Otheriwse, usearch_reset() will results in 'use-after-free'
    // error.
#ifdef MINIBLINK_NOT_IMPLEMENTED
    UErrorCode status = U_ZERO_ERROR;
    usearch_setPattern(blink::searcher(), &newlineCharacter, 1, &status);
    usearch_setText(blink::searcher(), &newlineCharacter, 1, &status);
    ASSERT(status == U_ZERO_ERROR);
#endif // MINIBLINK_NOT_IMPLEMENTED
	notImplemented();

    unlockSearcher();
}

template<typename CharType>
inline void SearchBuffer::append(const CharType* characters, size_t length)
{
    ASSERT(length);

    if (m_atBreak) {
        m_buffer.shrink(0);
        m_prefixLength = 0;
        m_atBreak = false;
    } else if (m_buffer.size() == m_buffer.capacity()) {
        memcpy(m_buffer.data(), m_buffer.data() + m_buffer.size() - m_overlap, m_overlap * sizeof(UChar));
        m_prefixLength -= std::min(m_prefixLength, m_buffer.size() - m_overlap);
        m_buffer.shrink(m_overlap);
    }

    size_t oldLength = m_buffer.size();
    size_t usableLength = std::min(m_buffer.capacity() - oldLength, length);
    ASSERT(usableLength);
    m_buffer.resize(oldLength + usableLength);
    UChar* destination = m_buffer.data() + oldLength;
    StringImpl::copyChars(destination, characters, usableLength);
    foldQuoteMarksAndSoftHyphens(destination, usableLength);
    m_numberOfCharactersJustAppended = usableLength;
}

inline bool SearchBuffer::needsMoreContext() const
{
    return m_needsMoreContext;
}

inline void SearchBuffer::prependContext(const UChar* characters, size_t length)
{
    ASSERT(m_needsMoreContext);
    ASSERT(m_prefixLength == m_buffer.size());

    if (!length)
        return;

    m_atBreak = false;

    size_t wordBoundaryContextStart = length;
    if (wordBoundaryContextStart) {
        U16_BACK_1(characters, 0, wordBoundaryContextStart);
        wordBoundaryContextStart = startOfLastWordBoundaryContext(characters, wordBoundaryContextStart);
    }

    size_t usableLength = std::min(m_buffer.capacity() - m_prefixLength, length - wordBoundaryContextStart);
    m_buffer.prepend(characters + length - usableLength, usableLength);
    m_prefixLength += usableLength;

    if (wordBoundaryContextStart || m_prefixLength == m_buffer.capacity())
        m_needsMoreContext = false;
}

inline bool SearchBuffer::atBreak() const
{
    return m_atBreak;
}

inline void SearchBuffer::reachedBreak()
{
    m_atBreak = true;
}

inline bool SearchBuffer::isBadMatch(const UChar* match, size_t matchLength) const
{
    // This function implements the kana workaround. If usearch treats
    // it as a match, but we do not want to, then it's a "bad match".
    if (!m_targetRequiresKanaWorkaround)
        return false;

    // Normalize into a match buffer. We reuse a single buffer rather than
    // creating a new one each time.
    normalizeCharactersIntoNFCForm(match, matchLength, m_normalizedMatch);

    return !checkOnlyKanaLettersInStrings(m_normalizedTarget.begin(), m_normalizedTarget.size(), m_normalizedMatch.begin(), m_normalizedMatch.size());
}

inline bool SearchBuffer::isWordStartMatch(size_t start, size_t length) const
{
    ASSERT(m_options & AtWordStarts);

    if (!start)
        return true;

    int size = m_buffer.size();
    int offset = start;
    UChar32 firstCharacter;
    U16_GET(m_buffer.data(), 0, offset, size, firstCharacter);

    if (m_options & TreatMedialCapitalAsWordStart) {
        UChar32 previousCharacter;
        U16_PREV(m_buffer.data(), 0, offset, previousCharacter);

        if (isSeparator(firstCharacter)) {
            // The start of a separator run is a word start (".org" in "webkit.org").
            if (!isSeparator(previousCharacter))
                return true;
        } else if (isASCIIUpper(firstCharacter)) {
            // The start of an uppercase run is a word start ("Kit" in "WebKit").
            if (!isASCIIUpper(previousCharacter))
                return true;
            // The last character of an uppercase run followed by a non-separator, non-digit
            // is a word start ("Request" in "XMLHTTPRequest").
            offset = start;
            U16_FWD_1(m_buffer.data(), offset, size);
            UChar32 nextCharacter = 0;
            if (offset < size)
                U16_GET(m_buffer.data(), 0, offset, size, nextCharacter);
            if (!isASCIIUpper(nextCharacter) && !isASCIIDigit(nextCharacter) && !isSeparator(nextCharacter))
                return true;
        } else if (isASCIIDigit(firstCharacter)) {
            // The start of a digit run is a word start ("2" in "WebKit2").
            if (!isASCIIDigit(previousCharacter))
                return true;
        } else if (isSeparator(previousCharacter) || isASCIIDigit(previousCharacter)) {
            // The start of a non-separator, non-uppercase, non-digit run is a word start,
            // except after an uppercase. ("org" in "webkit.org", but not "ore" in "WebCore").
            return true;
        }
    }

    // Chinese and Japanese lack word boundary marks, and there is no clear agreement on what constitutes
    // a word, so treat the position before any CJK character as a word start.
    if (Character::isCJKIdeographOrSymbol(firstCharacter))
        return true;

    size_t wordBreakSearchStart = start + length;
    while (wordBreakSearchStart > start)
        wordBreakSearchStart = findNextWordFromIndex(m_buffer.data(), m_buffer.size(), wordBreakSearchStart, false /* backwards */);
    if (wordBreakSearchStart != start)
        return false;
    if (m_options & WholeWord)
        return static_cast<int>(start + length) == findWordEndBoundary(m_buffer.data(), m_buffer.size(), wordBreakSearchStart);
    return true;
}

inline size_t SearchBuffer::search(size_t& start)
{
    size_t size = m_buffer.size();
    if (m_atBreak) {
        if (!size)
            return 0;
    } else {
        if (size != m_buffer.capacity())
            return 0;
    }
#ifdef MINIBLINK_NOT_IMPLEMENTED
    UStringSearch* searcher = blink::searcher();

    UErrorCode status = U_ZERO_ERROR;
    usearch_setText(searcher, m_buffer.data(), size, &status);
    ASSERT(status == U_ZERO_ERROR);

    usearch_setOffset(searcher, m_prefixLength, &status);
    ASSERT(status == U_ZERO_ERROR);

    int matchStart = usearch_next(searcher, &status);
    ASSERT(status == U_ZERO_ERROR);

nextMatch:
    if (!(matchStart >= 0 && static_cast<size_t>(matchStart) < size)) {
        ASSERT(matchStart == USEARCH_DONE);
        return 0;
    }

    // Matches that start in the overlap area are only tentative.
    // The same match may appear later, matching more characters,
    // possibly including a combining character that's not yet in the buffer.
    if (!m_atBreak && static_cast<size_t>(matchStart) >= size - m_overlap) {
        size_t overlap = m_overlap;
        if (m_options & AtWordStarts) {
            // Ensure that there is sufficient context before matchStart the next time around for
            // determining if it is at a word boundary.
            int wordBoundaryContextStart = matchStart;
            U16_BACK_1(m_buffer.data(), 0, wordBoundaryContextStart);
            wordBoundaryContextStart = startOfLastWordBoundaryContext(m_buffer.data(), wordBoundaryContextStart);
            overlap = std::min(size - 1, std::max(overlap, size - wordBoundaryContextStart));
        }
        memcpy(m_buffer.data(), m_buffer.data() + size - overlap, overlap * sizeof(UChar));
        m_prefixLength -= std::min(m_prefixLength, size - overlap);
        m_buffer.shrink(overlap);
        return 0;
    }

    size_t matchedLength = usearch_getMatchedLength(searcher);
    ASSERT_WITH_SECURITY_IMPLICATION(matchStart + matchedLength <= size);

    // If this match is "bad", move on to the next match.
    if (isBadMatch(m_buffer.data() + matchStart, matchedLength) || ((m_options & AtWordStarts) && !isWordStartMatch(matchStart, matchedLength))) {
        matchStart = usearch_next(searcher, &status);
        ASSERT(status == U_ZERO_ERROR);
        goto nextMatch;
    }

    size_t newSize = size - (matchStart + 1);
    memmove(m_buffer.data(), m_buffer.data() + matchStart + 1, newSize * sizeof(UChar));
    m_prefixLength -= std::min<size_t>(m_prefixLength, matchStart + 1);
    m_buffer.shrink(newSize);

    start = size - matchStart;
    return matchedLength;
#endif // MINIBLINK_NOT_IMPLEMENTED
	notImplemented();

	return 0;
}

// Check if there's any unpaird surrogate code point.
// Non-character code points are not checked.
static bool isValidUTF16(const String& s)
{
    if (s.is8Bit())
        return true;
    const UChar* ustr = s.characters16();
    size_t length = s.length();
    size_t position = 0;
    while (position < length) {
        UChar32 character;
        U16_NEXT(ustr, position, length, character);
        if (U_IS_SURROGATE(character))
            return false;
    }
    return true;
}

static size_t findPlainTextInternal(CharacterIterator& it, const String& target, FindOptions options, size_t& matchStart)
{
    matchStart = 0;
    size_t matchLength = 0;

    if (!isValidUTF16(target))
        return 0;

    SearchBuffer buffer(target, options);

    if (buffer.needsMoreContext()) {
        for (SimplifiedBackwardsTextIterator backwardsIterator(Position::firstPositionInNode(it.ownerDocument()), Position(it.currentContainer(), it.startOffset())); !backwardsIterator.atEnd(); backwardsIterator.advance()) {
            Vector<UChar, 1024> characters;
            backwardsIterator.prependTextTo(characters);
            buffer.prependContext(characters.data(), characters.size());
            if (!buffer.needsMoreContext())
                break;
        }
    }

    while (!it.atEnd()) {
        it.appendTextTo(buffer);
        it.advance(buffer.numberOfCharactersJustAppended());
tryAgain:
        size_t matchStartOffset;
        if (size_t newMatchLength = buffer.search(matchStartOffset)) {
            // Note that we found a match, and where we found it.
            size_t lastCharacterInBufferOffset = it.characterOffset();
            ASSERT(lastCharacterInBufferOffset >= matchStartOffset);
            matchStart = lastCharacterInBufferOffset - matchStartOffset;
            matchLength = newMatchLength;
            // If searching forward, stop on the first match.
            // If searching backward, don't stop, so we end up with the last match.
            if (!(options & Backwards))
                break;
            goto tryAgain;
        }
        if (it.atBreak() && !buffer.atBreak()) {
            buffer.reachedBreak();
            goto tryAgain;
        }
    }

    return matchLength;
}

static const TextIteratorBehaviorFlags iteratorFlagsForFindPlainText = TextIteratorEntersTextControls | TextIteratorEntersOpenShadowRoots | TextIteratorDoesNotBreakAtReplacedElement;

EphemeralRange findPlainText(const EphemeralRange& inputRange, const String& target, FindOptions options)
{
    // CharacterIterator requires layoutObjects to be up-to-date.
    if (!inputRange.startPosition().inDocument())
        return EphemeralRange();
    ASSERT(inputRange.startPosition().document() == inputRange.endPosition().document());

    // FIXME: Reduce the code duplication with above (but how?).
    size_t matchStart;
    size_t matchLength;
    {
        TextIteratorBehaviorFlags behavior = iteratorFlagsForFindPlainText;
        if (options & FindAPICall)
            behavior |= TextIteratorForWindowFind;
        CharacterIterator findIterator(inputRange, behavior);
        matchLength = findPlainTextInternal(findIterator, target, options, matchStart);
        if (!matchLength)
            return EphemeralRange(options & Backwards ? inputRange.startPosition() : inputRange.endPosition());
    }

    CharacterIterator computeRangeIterator(inputRange, iteratorFlagsForFindPlainText);
    return computeRangeIterator.calculateCharacterSubrange(matchStart, matchLength);
}

template class CORE_TEMPLATE_EXPORT CharacterIteratorAlgorithm<EditingStrategy>;

} // namespace blink
