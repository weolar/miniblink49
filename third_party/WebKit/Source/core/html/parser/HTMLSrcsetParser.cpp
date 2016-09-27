/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/parser/HTMLSrcsetParser.h"

#include "core/dom/Document.h"
#include "core/fetch/MemoryCache.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/UseCounter.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/inspector/ConsoleMessage.h"
#include "platform/ParsingUtilities.h"

namespace blink {

static bool compareByDensity(const ImageCandidate& first, const ImageCandidate& second)
{
    return first.density() < second.density();
}

enum DescriptorTokenizerState {
    TokenStart,
    InParenthesis,
    AfterToken,
};

struct DescriptorToken {
    unsigned start;
    unsigned length;

    DescriptorToken(unsigned start, unsigned length)
        : start(start)
        , length(length)
    {
    }

    unsigned lastIndex()
    {
        return start + length - 1;
    }

    template<typename CharType>
    int toInt(const CharType* attribute, bool& isValid)
    {
        unsigned position = 0;
        // Make sure the integer is a valid non-negative integer
        // https://html.spec.whatwg.org/multipage/infrastructure.html#valid-non-negative-integer
        unsigned lengthExcludingDescriptor = length - 1;
        while (position < lengthExcludingDescriptor) {
            if (!isASCIIDigit(*(attribute + start + position))) {
                isValid = false;
                return 0;
            }
            ++position;
        }
        return charactersToIntStrict(attribute + start, lengthExcludingDescriptor, &isValid);
    }

    template<typename CharType>
    float toFloat(const CharType* attribute, bool& isValid)
    {
        // Make sure the is a valid floating point number
        // https://html.spec.whatwg.org/multipage/infrastructure.html#valid-floating-point-number
        unsigned lengthExcludingDescriptor = length - 1;
        if (lengthExcludingDescriptor > 0 && *(attribute + start) == '+') {
            isValid = false;
            return 0;
        }
        return charactersToFloat(attribute + start, lengthExcludingDescriptor, &isValid);
    }
};

template<typename CharType>
static void appendDescriptorAndReset(const CharType* attributeStart, const CharType*& descriptorStart, const CharType* position, Vector<DescriptorToken>& descriptors)
{
    if (position > descriptorStart)
        descriptors.append(DescriptorToken(descriptorStart - attributeStart, position - descriptorStart));
    descriptorStart = 0;
}

// The following is called appendCharacter to match the spec's terminology.
template<typename CharType>
static void appendCharacter(const CharType* descriptorStart, const CharType* position)
{
    // Since we don't copy the tokens, this just set the point where the descriptor tokens start.
    if (!descriptorStart)
        descriptorStart = position;
}

template<typename CharType>
static bool isEOF(const CharType* position, const CharType* end)
{
    return position >= end;
}

template<typename CharType>
static void tokenizeDescriptors(const CharType* attributeStart,
    const CharType*& position,
    const CharType* attributeEnd,
    Vector<DescriptorToken>& descriptors)
{
    DescriptorTokenizerState state = TokenStart;
    const CharType* descriptorsStart = position;
    const CharType* currentDescriptorStart = descriptorsStart;
    while (true) {
        switch (state) {
        case TokenStart:
            if (isEOF(position, attributeEnd)) {
                appendDescriptorAndReset(attributeStart, currentDescriptorStart, attributeEnd, descriptors);
                return;
            }
            if (isComma(*position)) {
                appendDescriptorAndReset(attributeStart, currentDescriptorStart, position, descriptors);
                ++position;
                return;
            }
            if (isHTMLSpace(*position)) {
                appendDescriptorAndReset(attributeStart, currentDescriptorStart, position, descriptors);
                currentDescriptorStart = position + 1;
                state = AfterToken;
            } else if (*position == '(') {
                appendCharacter(currentDescriptorStart, position);
                state = InParenthesis;
            } else {
                appendCharacter(currentDescriptorStart, position);
            }
            break;
        case InParenthesis:
            if (isEOF(position, attributeEnd)) {
                appendDescriptorAndReset(attributeStart, currentDescriptorStart, attributeEnd, descriptors);
                return;
            }
            if (*position == ')') {
                appendCharacter(currentDescriptorStart, position);
                state = TokenStart;
            } else {
                appendCharacter(currentDescriptorStart, position);
            }
            break;
        case AfterToken:
            if (isEOF(position, attributeEnd))
                return;
            if (!isHTMLSpace(*position)) {
                state = TokenStart;
                currentDescriptorStart = position;
                --position;
            }
            break;
        }
        ++position;
    }
}

static void srcsetError(Document* document, String message)
{
    if (document && document->frame()) {
        StringBuilder errorMessage;
        errorMessage.append("Failed parsing 'srcset' attribute value since ");
        errorMessage.append(message);
        document->frame()->console().addMessage(ConsoleMessage::create(OtherMessageSource, ErrorMessageLevel, errorMessage.toString()));
    }
}

template<typename CharType>
static bool parseDescriptors(const CharType* attribute, Vector<DescriptorToken>& descriptors, DescriptorParsingResult& result, Document* document)
{
    for (DescriptorToken& descriptor : descriptors) {
        if (descriptor.length == 0)
            continue;
        CharType c = attribute[descriptor.lastIndex()];
        bool isValid = false;
        if (c == 'w') {
            if (result.hasDensity() || result.hasWidth()) {
                srcsetError(document, "it has multiple 'w' descriptors or a mix of 'x' and 'w' descriptors.");
                return false;
            }
            int resourceWidth = descriptor.toInt(attribute, isValid);
            if (!isValid || resourceWidth <= 0) {
                srcsetError(document, "its 'w' descriptor is invalid.");
                return false;
            }
            result.setResourceWidth(resourceWidth);
        } else if (c == 'h') {
            // This is here only for future compat purposes.
            // The value of the 'h' descriptor is not used.
            if (result.hasDensity() || result.hasHeight()) {
                srcsetError(document, "it has multiple 'h' descriptors or a mix of 'x' and 'h' descriptors.");
                return false;
            }
            int resourceHeight = descriptor.toInt(attribute, isValid);
            if (!isValid || resourceHeight <= 0) {
                srcsetError(document, "its 'h' descriptor is invalid.");
                return false;
            }
            result.setResourceHeight(resourceHeight);
        } else if (c == 'x') {
            if (result.hasDensity() || result.hasHeight() || result.hasWidth()) {
                srcsetError(document, "it has multiple 'x' descriptors or a mix of 'x' and 'w'/'h' descriptors.");
                return false;
            }
            float density = descriptor.toFloat(attribute, isValid);
            if (!isValid || density < 0) {
                srcsetError(document, "its 'x' descriptor is invalid.");
                return false;
            }
            result.setDensity(density);
        } else {
            srcsetError(document, "it has an unknown descriptor.");
            return false;
        }
    }
    bool res = !result.hasHeight() || result.hasWidth();
    if (!res)
        srcsetError(document, "it has an 'h' descriptor and no 'w' descriptor.");
    return res;
}

static bool parseDescriptors(const String& attribute, Vector<DescriptorToken>& descriptors, DescriptorParsingResult& result, Document* document)
{
    // FIXME: See if StringView can't be extended to replace DescriptorToken here.
    if (attribute.is8Bit()) {
        return parseDescriptors(attribute.characters8(), descriptors, result, document);
    }
    return parseDescriptors(attribute.characters16(), descriptors, result, document);
}

// http://picture.responsiveimages.org/#parse-srcset-attr
template<typename CharType>
static void parseImageCandidatesFromSrcsetAttribute(const String& attribute, const CharType* attributeStart, unsigned length, Vector<ImageCandidate>& imageCandidates, Document* document)
{
    const CharType* position = attributeStart;
    const CharType* attributeEnd = position + length;

    while (position < attributeEnd) {
        // 4. Splitting loop: Collect a sequence of characters that are space characters or U+002C COMMA characters.
        skipWhile<CharType, isHTMLSpaceOrComma<CharType>>(position, attributeEnd);
        if (position == attributeEnd) {
            // Contrary to spec language - descriptor parsing happens on each candidate, so when we reach the attributeEnd, we can exit.
            break;
        }
        const CharType* imageURLStart = position;

        // 6. Collect a sequence of characters that are not space characters, and let that be url.
        skipUntil<CharType, isHTMLSpace<CharType>>(position, attributeEnd);
        const CharType* imageURLEnd = position;

        DescriptorParsingResult result;

        // 8. If url ends with a U+002C COMMA character (,)
        if (isComma(*(position - 1))) {
            // Remove all trailing U+002C COMMA characters from url.
            imageURLEnd = position - 1;
            reverseSkipWhile<CharType, isComma>(imageURLEnd, imageURLStart);
            ++imageURLEnd;
            // If url is empty, then jump to the step labeled splitting loop.
            if (imageURLStart == imageURLEnd)
                continue;
        } else {
            skipWhile<CharType, isHTMLSpace<CharType>>(position, attributeEnd);
            Vector<DescriptorToken> descriptorTokens;
            tokenizeDescriptors(attributeStart, position, attributeEnd, descriptorTokens);
            // Contrary to spec language - descriptor parsing happens on each candidate.
            // This is a black-box equivalent, to avoid storing descriptor lists for each candidate.
            if (!parseDescriptors(attribute, descriptorTokens, result, document)) {
                if (document) {
                    UseCounter::count(document, UseCounter::SrcsetDroppedCandidate);
                    if (document->frame())
                        document->frame()->console().addMessage(ConsoleMessage::create(OtherMessageSource, ErrorMessageLevel, String("Dropped srcset candidate ") + String(imageURLStart, imageURLEnd - imageURLStart)));
                }
                continue;
            }
        }

        ASSERT(imageURLEnd > attributeStart);
        unsigned imageURLStartingPosition = imageURLStart - attributeStart;
        ASSERT(imageURLEnd > imageURLStart);
        unsigned imageURLLength = imageURLEnd - imageURLStart;
        imageCandidates.append(ImageCandidate(attribute, imageURLStartingPosition, imageURLLength, result, ImageCandidate::SrcsetOrigin));
        // 11. Return to the step labeled splitting loop.
    }
}

static void parseImageCandidatesFromSrcsetAttribute(const String& attribute, Vector<ImageCandidate>& imageCandidates, Document* document)
{
    if (attribute.isNull())
        return;

    if (attribute.is8Bit())
        parseImageCandidatesFromSrcsetAttribute<LChar>(attribute, attribute.characters8(), attribute.length(), imageCandidates, document);
    else
        parseImageCandidatesFromSrcsetAttribute<UChar>(attribute, attribute.characters16(), attribute.length(), imageCandidates, document);
}

static unsigned selectionLogic(Vector<ImageCandidate*>& imageCandidates, float deviceScaleFactor)
{
    unsigned i = 0;

    for (; i < imageCandidates.size() - 1; ++i) {
        unsigned next = i + 1;
        float nextDensity;
        float currentDensity;
        float geometricMean;

        nextDensity = imageCandidates[next]->density();
        if (nextDensity < deviceScaleFactor)
            continue;

        currentDensity = imageCandidates[i]->density();
        geometricMean = sqrt(currentDensity * nextDensity);
        if (((deviceScaleFactor <= 1.0) && (deviceScaleFactor > currentDensity)) || (deviceScaleFactor >= geometricMean))
            return next;
        break;
    }
    return i;
}

static unsigned avoidDownloadIfHigherDensityResourceIsInCache(Vector<ImageCandidate*>& imageCandidates, unsigned winner, Document* document)
{
    if (!document)
        return winner;
    for (unsigned i = imageCandidates.size() - 1; i > winner; --i) {
        KURL url = document->completeURL(stripLeadingAndTrailingHTMLSpaces(imageCandidates[i]->url()));
        if (memoryCache()->resourceForURL(url, document->fetcher()->getCacheIdentifier()))
            return i;
    }
    return winner;
}

static ImageCandidate pickBestImageCandidate(float deviceScaleFactor, float sourceSize, Vector<ImageCandidate>& imageCandidates, Document* document = nullptr)
{
    const float defaultDensityValue = 1.0;
    bool ignoreSrc = false;
    if (imageCandidates.isEmpty())
        return ImageCandidate();

    // http://picture.responsiveimages.org/#normalize-source-densities
    for (ImageCandidate& image : imageCandidates) {
        if (image.resourceWidth() > 0) {
            image.setDensity((float)image.resourceWidth() / sourceSize);
            ignoreSrc = true;
        } else if (image.density() < 0) {
            image.setDensity(defaultDensityValue);
        }
    }

    std::stable_sort(imageCandidates.begin(), imageCandidates.end(), compareByDensity);

    Vector<ImageCandidate*> deDupedImageCandidates;
    float prevDensity = -1.0;
    for (ImageCandidate& image : imageCandidates) {
        if (image.density() != prevDensity && (!ignoreSrc || !image.srcOrigin()))
            deDupedImageCandidates.append(&image);
        prevDensity = image.density();
    }
    unsigned winner = selectionLogic(deDupedImageCandidates, deviceScaleFactor);
    ASSERT(winner < deDupedImageCandidates.size());
    winner = avoidDownloadIfHigherDensityResourceIsInCache(deDupedImageCandidates, winner, document);

    float winningDensity = deDupedImageCandidates[winner]->density();
    // 16. If an entry b in candidates has the same associated ... pixel density as an earlier entry a in candidates,
    // then remove entry b
    while ((winner > 0) && (deDupedImageCandidates[winner - 1]->density() == winningDensity))
        --winner;

    return *deDupedImageCandidates[winner];
}

ImageCandidate bestFitSourceForSrcsetAttribute(float deviceScaleFactor, float sourceSize, const String& srcsetAttribute, Document* document)
{
    Vector<ImageCandidate> imageCandidates;

    parseImageCandidatesFromSrcsetAttribute(srcsetAttribute, imageCandidates, document);

    return pickBestImageCandidate(deviceScaleFactor, sourceSize, imageCandidates, document);
}

ImageCandidate bestFitSourceForImageAttributes(float deviceScaleFactor, float sourceSize, const String& srcAttribute, const String& srcsetAttribute, Document* document)
{
    if (srcsetAttribute.isNull()) {
        if (srcAttribute.isNull())
            return ImageCandidate();
        return ImageCandidate(srcAttribute, 0, srcAttribute.length(), DescriptorParsingResult(), ImageCandidate::SrcOrigin);
    }

    Vector<ImageCandidate> imageCandidates;

    parseImageCandidatesFromSrcsetAttribute(srcsetAttribute, imageCandidates, document);

    if (!srcAttribute.isEmpty())
        imageCandidates.append(ImageCandidate(srcAttribute, 0, srcAttribute.length(), DescriptorParsingResult(), ImageCandidate::SrcOrigin));

    return pickBestImageCandidate(deviceScaleFactor, sourceSize, imageCandidates, document);
}

String bestFitSourceForImageAttributes(float deviceScaleFactor, float sourceSize, const String& srcAttribute, ImageCandidate& srcsetImageCandidate)
{
    if (srcsetImageCandidate.isEmpty())
        return srcAttribute;

    Vector<ImageCandidate> imageCandidates;
    imageCandidates.append(srcsetImageCandidate);

    if (!srcAttribute.isEmpty())
        imageCandidates.append(ImageCandidate(srcAttribute, 0, srcAttribute.length(), DescriptorParsingResult(), ImageCandidate::SrcOrigin));

    return pickBestImageCandidate(deviceScaleFactor, sourceSize, imageCandidates).toString();
}

}
