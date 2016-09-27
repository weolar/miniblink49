/*
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
#include "core/html/HTMLDimension.h"

#include "wtf/MathExtras.h"
#include "wtf/text/WTFString.h"

namespace blink {

template <typename CharacterType>
static HTMLDimension parseDimension(const CharacterType* characters, size_t lastParsedIndex, size_t endOfCurrentToken)
{
    HTMLDimension::HTMLDimensionType type = HTMLDimension::Absolute;
    double value = 0.;

    // HTML5's split removes leading and trailing spaces so we need to skip the leading spaces here.
    while (lastParsedIndex < endOfCurrentToken && isASCIISpace((characters[lastParsedIndex])))
        ++lastParsedIndex;

    // This is Step 5.5. in the algorithm. Going to the last step would make the code less readable.
    if (lastParsedIndex >= endOfCurrentToken)
        return HTMLDimension(value, HTMLDimension::Relative);

    size_t position = lastParsedIndex;
    while (position < endOfCurrentToken && isASCIIDigit(characters[position]))
        ++position;

    if (position > lastParsedIndex) {
        bool ok = false;
        unsigned integerValue = charactersToUIntStrict(characters + lastParsedIndex, position - lastParsedIndex, &ok);
        if (!ok)
            return HTMLDimension(0., HTMLDimension::Relative);
        value += integerValue;

        if (position < endOfCurrentToken && characters[position] == '.') {
            ++position;
            Vector<CharacterType> fractionNumbers;
            while (position < endOfCurrentToken && (isASCIIDigit(characters[position]) || isASCIISpace(characters[position]))) {
                if (isASCIIDigit(characters[position]))
                    fractionNumbers.append(characters[position]);
                ++position;
            }

            if (fractionNumbers.size()) {
                double fractionValue = charactersToUIntStrict(fractionNumbers.data(), fractionNumbers.size(), &ok);
                if (!ok)
                    return HTMLDimension(0., HTMLDimension::Relative);

                value += fractionValue / pow(10., static_cast<double>(fractionNumbers.size()));
            }
        }
    }

    while (position < endOfCurrentToken && isASCIISpace(characters[position]))
        ++position;

    if (position < endOfCurrentToken) {
        if (characters[position] == '*')
            type = HTMLDimension::Relative;
        else if (characters[position] == '%')
            type = HTMLDimension::Percentage;
    }

    return HTMLDimension(value, type);
}

static HTMLDimension parseDimension(const String& rawToken, size_t lastParsedIndex, size_t endOfCurrentToken)
{
    if (rawToken.is8Bit())
        return parseDimension<LChar>(rawToken.characters8(), lastParsedIndex, endOfCurrentToken);
    return parseDimension<UChar>(rawToken.characters16(), lastParsedIndex, endOfCurrentToken);
}

// This implements the "rules for parsing a list of dimensions" per HTML5.
// http://www.whatwg.org/specs/web-apps/current-work/multipage/common-microsyntaxes.html#rules-for-parsing-a-list-of-dimensions
Vector<HTMLDimension> parseListOfDimensions(const String& input)
{
    static const char comma = ',';

    // Step 2. Remove the last character if it's a comma.
    String trimmedString = input;
    if (trimmedString.endsWith(comma))
        trimmedString.truncate(trimmedString.length() - 1);

    // HTML5's split doesn't return a token for an empty string so
    // we need to match them here.
    if (trimmedString.isEmpty())
        return Vector<HTMLDimension>();

    // Step 3. To avoid String copies, we just look for commas instead of splitting.
    Vector<HTMLDimension> parsedDimensions;
    size_t lastParsedIndex = 0;
    while (true) {
        size_t nextComma = trimmedString.find(comma, lastParsedIndex);
        if (nextComma == kNotFound)
            break;

        parsedDimensions.append(parseDimension(trimmedString, lastParsedIndex, nextComma));
        lastParsedIndex = nextComma + 1;
    }

    parsedDimensions.append(parseDimension(trimmedString, lastParsedIndex, trimmedString.length()));
    return parsedDimensions;
}

} // namespace blink
