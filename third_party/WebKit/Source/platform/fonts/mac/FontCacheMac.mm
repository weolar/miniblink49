/*
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Nicholas Shanks <webkit@nickshanks.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "config.h"
#import "platform/fonts/FontCache.h"

#import <AppKit/AppKit.h>
#include "platform/LayoutTestSupport.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/fonts/FontDescription.h"
#include "platform/fonts/FontFaceCreationParams.h"
#include  "platform/fonts/FontPlatformData.h"
#include "platform/fonts/SimpleFontData.h"
#include "platform/fonts/mac/FontFamilyMatcherMac.h"
#include "public/platform/Platform.h"
#include "public/platform/WebTraceLocation.h"
#include <wtf/Functional.h>
#include <wtf/MainThread.h>
#include <wtf/StdLibExtras.h>

// Forward declare Mac SPIs.
// Request for public API: rdar://13803570
@interface NSFont (WebKitSPI)
+ (NSFont*)findFontLike:(NSFont*)font forString:(NSString*)string withRange:(NSRange)range inLanguage:(id)useNil;
+ (NSFont*)findFontLike:(NSFont*)font forCharacter:(UniChar)uc inLanguage:(id)useNil;
@end

namespace blink {

static void invalidateFontCache()
{
    if (!isMainThread()) {
        Platform::current()->mainThread()->postTask(FROM_HERE, bind(&invalidateFontCache));
        return;
    }
    FontCache::fontCache()->invalidate();
}

static void fontCacheRegisteredFontsChangedNotificationCallback(CFNotificationCenterRef, void* observer, CFStringRef name, const void *, CFDictionaryRef)
{
    ASSERT_UNUSED(observer, observer == FontCache::fontCache());
    ASSERT_UNUSED(name, CFEqual(name, kCTFontManagerRegisteredFontsChangedNotification));
    invalidateFontCache();
}

static bool useHinting()
{
    // Enable hinting when subpixel font scaling is disabled or
    // when running the set of standard non-subpixel layout tests,
    // otherwise use subpixel glyph positioning.
    return (LayoutTestSupport::isRunningLayoutTest() && !LayoutTestSupport::isFontAntialiasingEnabledForTest());
}

void FontCache::platformInit()
{
    CFNotificationCenterAddObserver(CFNotificationCenterGetLocalCenter(), this, fontCacheRegisteredFontsChangedNotificationCallback, kCTFontManagerRegisteredFontsChangedNotification, 0, CFNotificationSuspensionBehaviorDeliverImmediately);
}

static int toAppKitFontWeight(FontWeight fontWeight)
{
    static int appKitFontWeights[] = {
        2,  // FontWeight100
        3,  // FontWeight200
        4,  // FontWeight300
        5,  // FontWeight400
        6,  // FontWeight500
        8,  // FontWeight600
        9,  // FontWeight700
        10, // FontWeight800
        12, // FontWeight900
    };
    return appKitFontWeights[fontWeight];
}

static inline bool isAppKitFontWeightBold(NSInteger appKitFontWeight)
{
    return appKitFontWeight >= 7;
}

PassRefPtr<SimpleFontData> FontCache::fallbackFontForCharacter(const FontDescription& fontDescription, UChar32 character, const SimpleFontData* fontDataToSubstitute)
{
    // FIXME: We should fix getFallbackFamily to take a UChar32
    // and remove this split-to-UChar16 code.
    UChar codeUnits[2];
    int codeUnitsLength;
    if (character <= 0xFFFF) {
        codeUnits[0] = character;
        codeUnitsLength = 1;
    } else {
        codeUnits[0] = U16_LEAD(character);
        codeUnits[1] = U16_TRAIL(character);
        codeUnitsLength = 2;
    }

    const FontPlatformData& platformData = fontDataToSubstitute->platformData();
    NSFont* nsFont = toNSFont(platformData.ctFont());

    NSString *string = [[NSString alloc] initWithCharactersNoCopy:codeUnits length:codeUnitsLength freeWhenDone:NO];
    NSFont *substituteFont = [NSFont findFontLike:nsFont forString:string withRange:NSMakeRange(0, codeUnitsLength) inLanguage:nil];
    [string release];

    // FIXME: Remove this SPI usage: http://crbug.com/255122
    if (!substituteFont && codeUnitsLength == 1)
        substituteFont = [NSFont findFontLike:nsFont forCharacter:codeUnits[0] inLanguage:nil];
    if (!substituteFont)
        return nullptr;

    // Use the family name from the AppKit-supplied substitute font, requesting the
    // traits, weight, and size we want. One way this does better than the original
    // AppKit request is that it takes synthetic bold and oblique into account.
    // But it does create the possibility that we could end up with a font that
    // doesn't actually cover the characters we need.

    NSFontManager *fontManager = [NSFontManager sharedFontManager];

    NSFontTraitMask traits;
    NSInteger weight;
    CGFloat size;

    if (nsFont) {
        traits = [fontManager traitsOfFont:nsFont];
        if (platformData.m_syntheticBold)
            traits |= NSBoldFontMask;
        if (platformData.m_syntheticItalic)
            traits |= NSFontItalicTrait;
        weight = [fontManager weightOfFont:nsFont];
        size = [nsFont pointSize];
    } else {
        // For custom fonts nsFont is nil.
        traits = fontDescription.style() ? NSFontItalicTrait : 0;
        weight = toAppKitFontWeight(fontDescription.weight());
        size = fontDescription.computedPixelSize();
    }

    NSFontTraitMask substituteFontTraits = [fontManager traitsOfFont:substituteFont];
    NSInteger substituteFontWeight = [fontManager weightOfFont:substituteFont];

    if (traits != substituteFontTraits || weight != substituteFontWeight || !nsFont) {
        if (NSFont *bestVariation = [fontManager fontWithFamily:[substituteFont familyName] traits:traits weight:weight size:size]) {
            if ((!nsFont || [fontManager traitsOfFont:bestVariation] != substituteFontTraits || [fontManager weightOfFont:bestVariation] != substituteFontWeight)
                && [[bestVariation coveredCharacterSet] longCharacterIsMember:character])
                substituteFont = bestVariation;
        }
    }

    substituteFont = useHinting() ? [substituteFont screenFont] : [substituteFont printerFont];

    substituteFontTraits = [fontManager traitsOfFont:substituteFont];
    substituteFontWeight = [fontManager weightOfFont:substituteFont];

    FontPlatformData alternateFont(substituteFont, platformData.size(),
        isAppKitFontWeightBold(weight) && !isAppKitFontWeightBold(substituteFontWeight),
        (traits & NSFontItalicTrait) && !(substituteFontTraits & NSFontItalicTrait),
        platformData.orientation());

    return fontDataFromFontPlatformData(&alternateFont, DoNotRetain);
}

PassRefPtr<SimpleFontData> FontCache::getLastResortFallbackFont(const FontDescription& fontDescription, ShouldRetain shouldRetain)
{
    DEFINE_STATIC_LOCAL(AtomicString, timesStr, ("Times", AtomicString::ConstructFromLiteral));

    // FIXME: Would be even better to somehow get the user's default font here.  For now we'll pick
    // the default that the user would get without changing any prefs.
    RefPtr<SimpleFontData> simpleFontData = getFontData(fontDescription, timesStr, false, shouldRetain);
    if (simpleFontData)
        return simpleFontData.release();

    // The Times fallback will almost always work, but in the highly unusual case where
    // the user doesn't have it, we fall back on Lucida Grande because that's
    // guaranteed to be there, according to Nathan Taylor. This is good enough
    // to avoid a crash at least.
    DEFINE_STATIC_LOCAL(AtomicString, lucidaGrandeStr, ("Lucida Grande", AtomicString::ConstructFromLiteral));
    return getFontData(fontDescription, lucidaGrandeStr, false, shouldRetain);
}

FontPlatformData* FontCache::createFontPlatformData(const FontDescription& fontDescription, const FontFaceCreationParams& creationParams, float fontSize)
{
    NSFontTraitMask traits = fontDescription.style() ? NSFontItalicTrait : 0;
    NSInteger weight = toAppKitFontWeight(fontDescription.weight());
    float size = fontSize;

    NSFont *nsFont = MatchNSFontFamily(creationParams.family(),traits, weight, size);
    if (!nsFont)
        return 0;

    NSFontManager *fontManager = [NSFontManager sharedFontManager];
    NSFontTraitMask actualTraits = 0;
    if (fontDescription.style())
        actualTraits = [fontManager traitsOfFont:nsFont];
    NSInteger actualWeight = [fontManager weightOfFont:nsFont];

    NSFont *platformFont = useHinting() ? [nsFont screenFont] : [nsFont printerFont];
    bool syntheticBold = (isAppKitFontWeightBold(weight) && !isAppKitFontWeightBold(actualWeight)) || fontDescription.isSyntheticBold();
    bool syntheticItalic = ((traits & NSFontItalicTrait) && !(actualTraits & NSFontItalicTrait)) || fontDescription.isSyntheticItalic();

    // FontPlatformData::typeface() is null in the case of Chromium out-of-process font loading failing.
    // Out-of-process loading occurs for registered fonts stored in non-system locations.
    // When loading fails, we do not want to use the returned FontPlatformData since it will not have
    // a valid SkTypeface.
    OwnPtr<FontPlatformData> platformData = adoptPtr(new FontPlatformData(platformFont, size, syntheticBold, syntheticItalic, fontDescription.orientation()));
    if (!platformData->typeface()) {
        return nullptr;
    }
    return platformData.leakPtr();
}

} // namespace blink
