/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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

#include "config.h"
#include "core/testing/InternalSettings.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/css/PointerProperties.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/Settings.h"
#include "core/page/Page.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/Supplementable.h"
#include "platform/text/LocaleToScriptMapping.h"

#define InternalSettingsGuardForSettingsReturn(returnValue) \
    if (!settings()) { \
        exceptionState.throwDOMException(InvalidAccessError, "The settings object cannot be obtained."); \
        return returnValue; \
    }

#define InternalSettingsGuardForSettings()  \
    if (!settings()) { \
        exceptionState.throwDOMException(InvalidAccessError, "The settings object cannot be obtained."); \
        return; \
    }

#define InternalSettingsGuardForPage() \
    if (!page()) { \
        exceptionState.throwDOMException(InvalidAccessError, "The page object cannot be obtained."); \
        return; \
    }

namespace blink {

InternalSettings::Backup::Backup(Settings* settings)
    : m_originalAuthorShadowDOMForAnyElementEnabled(RuntimeEnabledFeatures::authorShadowDOMForAnyElementEnabled())
    , m_originalCSP(RuntimeEnabledFeatures::experimentalContentSecurityPolicyFeaturesEnabled())
    , m_originalOverlayScrollbarsEnabled(RuntimeEnabledFeatures::overlayScrollbarsEnabled())
    , m_originalEditingBehavior(settings->editingBehaviorType())
    , m_originalTextAutosizingEnabled(settings->textAutosizingEnabled())
    , m_originalTextAutosizingWindowSizeOverride(settings->textAutosizingWindowSizeOverride())
    , m_originalAccessibilityFontScaleFactor(settings->accessibilityFontScaleFactor())
    , m_originalMediaTypeOverride(settings->mediaTypeOverride())
    , m_originalDisplayModeOverride(settings->displayModeOverride())
    , m_originalMockScrollbarsEnabled(settings->mockScrollbarsEnabled())
    , m_originalMockGestureTapHighlightsEnabled(settings->mockGestureTapHighlightsEnabled())
    , m_langAttributeAwareFormControlUIEnabled(RuntimeEnabledFeatures::langAttributeAwareFormControlUIEnabled())
    , m_imagesEnabled(settings->imagesEnabled())
    , m_defaultVideoPosterURL(settings->defaultVideoPosterURL())
    , m_originalLayerSquashingEnabled(settings->layerSquashingEnabled())
    , m_originalImageColorProfilesEnabled(RuntimeEnabledFeatures::imageColorProfilesEnabled())
    , m_originalImageAnimationPolicy(settings->imageAnimationPolicy())
    , m_originalScrollTopLeftInteropEnabled(RuntimeEnabledFeatures::scrollTopLeftInteropEnabled())
{
}

void InternalSettings::Backup::restoreTo(Settings* settings)
{
    RuntimeEnabledFeatures::setAuthorShadowDOMForAnyElementEnabled(m_originalAuthorShadowDOMForAnyElementEnabled);
    RuntimeEnabledFeatures::setExperimentalContentSecurityPolicyFeaturesEnabled(m_originalCSP);
    RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(m_originalOverlayScrollbarsEnabled);
    settings->setEditingBehaviorType(m_originalEditingBehavior);
    settings->setTextAutosizingEnabled(m_originalTextAutosizingEnabled);
    settings->setTextAutosizingWindowSizeOverride(m_originalTextAutosizingWindowSizeOverride);
    settings->setAccessibilityFontScaleFactor(m_originalAccessibilityFontScaleFactor);
    settings->setMediaTypeOverride(m_originalMediaTypeOverride);
    settings->setDisplayModeOverride(m_originalDisplayModeOverride);
    settings->setMockScrollbarsEnabled(m_originalMockScrollbarsEnabled);
    settings->setMockGestureTapHighlightsEnabled(m_originalMockGestureTapHighlightsEnabled);
    RuntimeEnabledFeatures::setLangAttributeAwareFormControlUIEnabled(m_langAttributeAwareFormControlUIEnabled);
    settings->setImagesEnabled(m_imagesEnabled);
    settings->setDefaultVideoPosterURL(m_defaultVideoPosterURL);
    settings->genericFontFamilySettings().reset();
    RuntimeEnabledFeatures::setImageColorProfilesEnabled(m_originalImageColorProfilesEnabled);
    settings->setImageAnimationPolicy(m_originalImageAnimationPolicy);
    RuntimeEnabledFeatures::setScrollTopLeftInteropEnabled(m_originalScrollTopLeftInteropEnabled);
}

#if ENABLE(OILPAN)
InternalSettings* InternalSettings::from(Page& page)
{
    if (!HeapSupplement<Page>::from(page, supplementName()))
        HeapSupplement<Page>::provideTo(page, supplementName(), new InternalSettings(page));
    return static_cast<InternalSettings*>(HeapSupplement<Page>::from(page, supplementName()));
}
#else
// We can't use RefCountedSupplement because that would try to make InternalSettings RefCounted
// and InternalSettings is already RefCounted via its base class, InternalSettingsGenerated.
// Instead, we manually make InternalSettings supplement Page.
class InternalSettingsWrapper : public Supplement<Page> {
public:
    explicit InternalSettingsWrapper(Page& page)
        : m_internalSettings(InternalSettings::create(page)) { }
    virtual ~InternalSettingsWrapper() { m_internalSettings->hostDestroyed(); }
#if ENABLE(ASSERT)
    virtual bool isRefCountedWrapper() const override { return true; }
#endif
    InternalSettings* internalSettings() const { return m_internalSettings.get(); }

private:
    RefPtr<InternalSettings> m_internalSettings;
};

InternalSettings* InternalSettings::from(Page& page)
{
    if (!Supplement<Page>::from(page, supplementName()))
        Supplement<Page>::provideTo(page, supplementName(), adoptPtr(new InternalSettingsWrapper(page)));
    return static_cast<InternalSettingsWrapper*>(Supplement<Page>::from(page, supplementName()))->internalSettings();
}
#endif

const char* InternalSettings::supplementName()
{
    return "InternalSettings";
}

InternalSettings::~InternalSettings()
{
}

InternalSettings::InternalSettings(Page& page)
    : InternalSettingsGenerated(&page)
    , m_page(&page)
    , m_backup(&page.settings())
{
}

void InternalSettings::resetToConsistentState()
{
    m_backup.restoreTo(settings());
    m_backup = Backup(settings());
    m_backup.m_originalTextAutosizingEnabled = settings()->textAutosizingEnabled();

    InternalSettingsGenerated::resetToConsistentState();
}

Settings* InternalSettings::settings() const
{
    if (!page())
        return 0;
    return &page()->settings();
}

void InternalSettings::setMockScrollbarsEnabled(bool enabled, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setMockScrollbarsEnabled(enabled);
}

void InternalSettings::setMockGestureTapHighlightsEnabled(bool enabled, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setMockGestureTapHighlightsEnabled(enabled);
}

void InternalSettings::setAuthorShadowDOMForAnyElementEnabled(bool isEnabled)
{
    RuntimeEnabledFeatures::setAuthorShadowDOMForAnyElementEnabled(isEnabled);
}

void InternalSettings::setExperimentalContentSecurityPolicyFeaturesEnabled(bool enabled)
{
    RuntimeEnabledFeatures::setExperimentalContentSecurityPolicyFeaturesEnabled(enabled);
}

void InternalSettings::setImageColorProfilesEnabled(bool enabled)
{
    RuntimeEnabledFeatures::setImageColorProfilesEnabled(enabled);
}

void InternalSettings::setOverlayScrollbarsEnabled(bool enabled)
{
    RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(enabled);
}

void InternalSettings::setViewportEnabled(bool enabled, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setViewportEnabled(enabled);
}

void InternalSettings::setViewportMetaEnabled(bool enabled, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setViewportMetaEnabled(enabled);
}

void InternalSettings::setStandardFontFamily(const AtomicString& family, const String& script, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    UScriptCode code = scriptNameToCode(script);
    if (code == USCRIPT_INVALID_CODE)
        return;
    if (settings()->genericFontFamilySettings().updateStandard(family, code))
        settings()->notifyGenericFontFamilyChange();
}

void InternalSettings::setSerifFontFamily(const AtomicString& family, const String& script, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    UScriptCode code = scriptNameToCode(script);
    if (code == USCRIPT_INVALID_CODE)
        return;
    if (settings()->genericFontFamilySettings().updateSerif(family, code))
        settings()->notifyGenericFontFamilyChange();
}

void InternalSettings::setSansSerifFontFamily(const AtomicString& family, const String& script, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    UScriptCode code = scriptNameToCode(script);
    if (code == USCRIPT_INVALID_CODE)
        return;
    if (settings()->genericFontFamilySettings().updateSansSerif(family, code))
        settings()->notifyGenericFontFamilyChange();
}

void InternalSettings::setFixedFontFamily(const AtomicString& family, const String& script, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    UScriptCode code = scriptNameToCode(script);
    if (code == USCRIPT_INVALID_CODE)
        return;
    if (settings()->genericFontFamilySettings().updateFixed(family, code))
        settings()->notifyGenericFontFamilyChange();
}

void InternalSettings::setCursiveFontFamily(const AtomicString& family, const String& script, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    UScriptCode code = scriptNameToCode(script);
    if (code == USCRIPT_INVALID_CODE)
        return;
    if (settings()->genericFontFamilySettings().updateCursive(family, code))
        settings()->notifyGenericFontFamilyChange();
}

void InternalSettings::setFantasyFontFamily(const AtomicString& family, const String& script, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    UScriptCode code = scriptNameToCode(script);
    if (code == USCRIPT_INVALID_CODE)
        return;
    if (settings()->genericFontFamilySettings().updateFantasy(family, code))
        settings()->notifyGenericFontFamilyChange();
}

void InternalSettings::setPictographFontFamily(const AtomicString& family, const String& script, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    UScriptCode code = scriptNameToCode(script);
    if (code == USCRIPT_INVALID_CODE)
        return;
    if (settings()->genericFontFamilySettings().updatePictograph(family, code))
        settings()->notifyGenericFontFamilyChange();
}

void InternalSettings::setTextAutosizingEnabled(bool enabled, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setTextAutosizingEnabled(enabled);
}

void InternalSettings::setTextAutosizingWindowSizeOverride(int width, int height, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setTextAutosizingWindowSizeOverride(IntSize(width, height));
}

void InternalSettings::setTextTrackKindUserPreference(const String& preference, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    String token = preference.stripWhiteSpace();
    TextTrackKindUserPreference userPreference = TextTrackKindUserPreference::Default;
    if (token == "default")
        userPreference = TextTrackKindUserPreference::Default;
    else if (token == "captions")
        userPreference = TextTrackKindUserPreference::Captions;
    else if (token == "subtitles")
        userPreference = TextTrackKindUserPreference::Subtitles;
    else
        exceptionState.throwDOMException(SyntaxError, "The user preference for text track kind " + preference + ")' is invalid.");

    settings()->setTextTrackKindUserPreference(userPreference);
}

void InternalSettings::setMediaTypeOverride(const String& mediaType, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setMediaTypeOverride(mediaType);
}

void InternalSettings::setAccessibilityFontScaleFactor(float fontScaleFactor, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setAccessibilityFontScaleFactor(fontScaleFactor);
}

void InternalSettings::setEditingBehavior(const String& editingBehavior, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    if (equalIgnoringCase(editingBehavior, "win"))
        settings()->setEditingBehaviorType(EditingWindowsBehavior);
    else if (equalIgnoringCase(editingBehavior, "mac"))
        settings()->setEditingBehaviorType(EditingMacBehavior);
    else if (equalIgnoringCase(editingBehavior, "unix"))
        settings()->setEditingBehaviorType(EditingUnixBehavior);
    else if (equalIgnoringCase(editingBehavior, "android"))
        settings()->setEditingBehaviorType(EditingAndroidBehavior);
    else
        exceptionState.throwDOMException(SyntaxError, "The editing behavior type provided ('" + editingBehavior + "') is invalid.");
}

void InternalSettings::setLangAttributeAwareFormControlUIEnabled(bool enabled)
{
    RuntimeEnabledFeatures::setLangAttributeAwareFormControlUIEnabled(enabled);
}

void InternalSettings::setImagesEnabled(bool enabled, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setImagesEnabled(enabled);
}

void InternalSettings::setDefaultVideoPosterURL(const String& url, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setDefaultVideoPosterURL(url);
}

DEFINE_TRACE(InternalSettings)
{
    visitor->trace(m_page);
    InternalSettingsGenerated::trace(visitor);
#if ENABLE(OILPAN)
    HeapSupplement<Page>::trace(visitor);
#endif
}

void InternalSettings::setAvailablePointerTypes(const String& pointers, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();

    // Allow setting multiple pointer types by passing comma seperated list
    // ("coarse,fine").
    Vector<String> tokens;
    pointers.split(",", false, tokens);

    int pointerTypes = 0;
    for (size_t i = 0; i < tokens.size(); ++i) {
        String token = tokens[i].stripWhiteSpace();

        if (token == "coarse")
            pointerTypes |= PointerTypeCoarse;
        else if (token == "fine")
            pointerTypes |= PointerTypeFine;
        else if (token == "none")
            pointerTypes |= PointerTypeNone;
        else
            exceptionState.throwDOMException(SyntaxError, "The pointer type token ('" + token + ")' is invalid.");
    }

    settings()->setAvailablePointerTypes(pointerTypes);
}

void InternalSettings::setDisplayModeOverride(const String& displayMode, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    String token = displayMode.stripWhiteSpace();

    WebDisplayMode mode = WebDisplayModeBrowser;
    if (token == "browser")
        mode = WebDisplayModeBrowser;
    else if (token == "minimal-ui")
        mode = WebDisplayModeMinimalUi;
    else if (token == "standalone")
        mode = WebDisplayModeStandalone;
    else if (token == "fullscreen")
        mode = WebDisplayModeFullscreen;
    else
        exceptionState.throwDOMException(SyntaxError, "The display-mode token ('" + token + ")' is invalid.");

    settings()->setDisplayModeOverride(mode);
}

void InternalSettings::setPrimaryPointerType(const String& pointer, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    String token = pointer.stripWhiteSpace();

    PointerType type = PointerTypeNone;
    if (token == "coarse")
        type = PointerTypeCoarse;
    else if (token == "fine")
        type = PointerTypeFine;
    else if (token == "none")
        type = PointerTypeNone;
    else
        exceptionState.throwDOMException(SyntaxError, "The pointer type token ('" + token + ")' is invalid.");

    settings()->setPrimaryPointerType(type);
}

void InternalSettings::setAvailableHoverTypes(const String& types, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();

    // Allow setting multiple hover types by passing comma seperated list
    // ("on-demand,none").
    Vector<String> tokens;
    types.split(",", false, tokens);

    int hoverTypes = 0;
    for (size_t i = 0; i < tokens.size(); ++i) {
        String token = tokens[i].stripWhiteSpace();

        if (token == "none")
            hoverTypes |= HoverTypeNone;
        else if (token == "on-demand")
            hoverTypes |= HoverTypeOnDemand;
        else if (token == "hover")
            hoverTypes |= HoverTypeHover;
        else
            exceptionState.throwDOMException(SyntaxError, "The hover type token ('" + token + ")' is invalid.");
    }

    settings()->setAvailableHoverTypes(hoverTypes);
}

void InternalSettings::setPrimaryHoverType(const String& type, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    String token = type.stripWhiteSpace();

    HoverType hoverType = HoverTypeNone;
    if (token == "none")
        hoverType = HoverTypeNone;
    else if (token == "on-demand")
        hoverType = HoverTypeOnDemand;
    else if (token == "hover")
        hoverType = HoverTypeHover;
    else
        exceptionState.throwDOMException(SyntaxError, "The hover type token ('" + token + ")' is invalid.");

    settings()->setPrimaryHoverType(hoverType);
}

void InternalSettings::setImageAnimationPolicy(const String& policy, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    if (equalIgnoringCase(policy, "allowed"))
        settings()->setImageAnimationPolicy(ImageAnimationPolicyAllowed);
    else if (equalIgnoringCase(policy, "once"))
        settings()->setImageAnimationPolicy(ImageAnimationPolicyAnimateOnce);
    else if (equalIgnoringCase(policy, "none"))
        settings()->setImageAnimationPolicy(ImageAnimationPolicyNoAnimation);
    else
        exceptionState.throwDOMException(SyntaxError, "The image animation policy provided ('" + policy + "') is invalid.");
}

void InternalSettings::setScrollTopLeftInteropEnabled(bool enabled)
{
    RuntimeEnabledFeatures::setScrollTopLeftInteropEnabled(enabled);
}

void InternalSettings::setLinkHeaderEnabled(bool enabled)
{
    RuntimeEnabledFeatures::setLinkHeaderEnabled(enabled);
}

void InternalSettings::setDnsPrefetchLogging(bool enabled, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setLogDnsPrefetchAndPreconnect(enabled);
}

void InternalSettings::setPreloadLogging(bool enabled, ExceptionState& exceptionState)
{
    InternalSettingsGuardForSettings();
    settings()->setLogPreload(enabled);
}

}
