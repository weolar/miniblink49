/*
 * Copyright (C) 2010, 2013 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/Language.h"

#include "public/platform/Platform.h"
#include "wtf/text/WTFString.h"

namespace blink {

static String canonicalizeLanguageIdentifier(const String& languageCode)
{
    String copiedCode = languageCode;
    // Platform::defaultLocale() might provide a language code with '_'.
    copiedCode.replace('_', '-');
    return copiedCode;
}

static const AtomicString& platformLanguage()
{
    DEFINE_STATIC_LOCAL(AtomicString, computedDefaultLanguage, ());
    if (computedDefaultLanguage.isEmpty()) {
        computedDefaultLanguage = AtomicString(canonicalizeLanguageIdentifier(Platform::current()->defaultLocale()));
        ASSERT(!computedDefaultLanguage.isEmpty());
    }
    return computedDefaultLanguage;
}

static Vector<AtomicString>& preferredLanguagesOverride()
{
    DEFINE_STATIC_LOCAL(Vector<AtomicString>, override, ());
    return override;
}

void overrideUserPreferredLanguages(const Vector<AtomicString>& override)
{
    Vector<AtomicString>& canonicalized = preferredLanguagesOverride();
    canonicalized.resize(0);
    canonicalized.reserveCapacity(override.size());
    for (const auto& lang : override)
        canonicalized.append(canonicalizeLanguageIdentifier(lang));
}

AtomicString defaultLanguage()
{
    Vector<AtomicString>& override = preferredLanguagesOverride();
    if (!override.isEmpty())
        return override[0];
    return platformLanguage();
}

Vector<AtomicString> userPreferredLanguages()
{
    Vector<AtomicString>& override = preferredLanguagesOverride();
    if (!override.isEmpty())
        return override;

    Vector<AtomicString> languages;
    languages.reserveInitialCapacity(1);
    languages.append(platformLanguage());
    return languages;
}

size_t indexOfBestMatchingLanguageInList(const AtomicString& language, const Vector<AtomicString>& languageList)
{
    AtomicString languageWithoutLocaleMatch;
    AtomicString languageMatchButNotLocale;
    size_t languageWithoutLocaleMatchIndex = 0;
    size_t languageMatchButNotLocaleMatchIndex = 0;
    bool canMatchLanguageOnly = (language.length() == 2 || (language.length() >= 3 && language[2] == '-'));

    for (size_t i = 0; i < languageList.size(); ++i) {
        String canonicalizedLanguageFromList = canonicalizeLanguageIdentifier(languageList[i]);

        if (language == canonicalizedLanguageFromList)
            return i;

        if (canMatchLanguageOnly && canonicalizedLanguageFromList.length() >= 2) {
            if (language[0] == canonicalizedLanguageFromList[0] && language[1] == canonicalizedLanguageFromList[1]) {
                if (!languageWithoutLocaleMatch.length() && canonicalizedLanguageFromList.length() == 2) {
                    languageWithoutLocaleMatch = languageList[i];
                    languageWithoutLocaleMatchIndex = i;
                }
                if (!languageMatchButNotLocale.length() && canonicalizedLanguageFromList.length() >= 3) {
                    languageMatchButNotLocale = languageList[i];
                    languageMatchButNotLocaleMatchIndex = i;
                }
            }
        }
    }

    // If we have both a language-only match and a languge-but-not-locale match, return the
    // languge-only match as is considered a "better" match. For example, if the list
    // provided has both "en-GB" and "en" and the user prefers "en-US" we will return "en".
    if (languageWithoutLocaleMatch.length())
        return languageWithoutLocaleMatchIndex;

    if (languageMatchButNotLocale.length())
        return languageMatchButNotLocaleMatchIndex;

    return languageList.size();
}

}
