/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Rob Buis (rwlbuis@gmail.com)
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/html/HTMLLinkElement.h"

#include "bindings/core/v8/ScriptEventListener.h"
#include "bindings/core/v8/V8DOMActivityLogger.h"
#include "core/HTMLNames.h"
#include "core/css/MediaList.h"
#include "core/css/MediaQueryEvaluator.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/Attribute.h"
#include "core/dom/Document.h"
#include "core/dom/StyleEngine.h"
#include "core/events/Event.h"
#include "core/events/EventSender.h"
#include "core/fetch/CSSStyleSheetResource.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/SubresourceIntegrity.h"
#include "core/frame/UseCounter.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/LinkDefaultPresentation.h"
#include "core/html/LinkManifest.h"
#include "core/html/imports/LinkImport.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/style/StyleInheritedData.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/StdLibExtras.h"

namespace blink {

using namespace HTMLNames;

template <typename CharacterType>
static void parseSizes(const CharacterType* value, unsigned length, Vector<IntSize>& iconSizes)
{
    enum State {
        ParseStart,
        ParseWidth,
        ParseHeight
    };
    int width = 0;
    unsigned start = 0;
    unsigned i = 0;
    State state = ParseStart;
    bool invalid = false;
    for (; i < length; ++i) {
        if (state == ParseWidth) {
            if (value[i] == 'x' || value[i] == 'X') {
                if (i == start) {
                    invalid = true;
                    break;
                }
                width = charactersToInt(value + start, i - start);
                start = i + 1;
                state = ParseHeight;
            } else if (value[i] < '0' || value[i] > '9') {
                invalid = true;
                break;
            }
        } else if (state == ParseHeight) {
            if (value[i] == ' ') {
                if (i == start) {
                    invalid = true;
                    break;
                }
                int height = charactersToInt(value + start, i - start);
                iconSizes.append(IntSize(width, height));
                start = i + 1;
                state = ParseStart;
            } else if (value[i] < '0' || value[i] > '9') {
                invalid = true;
                break;
            }
        } else if (state == ParseStart) {
            if (value[i] >= '0' && value[i] <= '9') {
                start = i;
                state = ParseWidth;
            } else if (value[i] != ' ') {
                invalid = true;
                break;
            }
        }
    }
    if (invalid || state == ParseWidth || (state == ParseHeight && start == i)) {
        iconSizes.clear();
        return;
    }
    if (state == ParseHeight && i > start) {
        int height = charactersToInt(value + start, i - start);
        iconSizes.append(IntSize(width, height));
    }
}

static LinkEventSender& linkLoadEventSender()
{
    DEFINE_STATIC_LOCAL(LinkEventSender, sharedLoadEventSender, (EventTypeNames::load));
    return sharedLoadEventSender;
}

void HTMLLinkElement::parseSizesAttribute(const AtomicString& value, Vector<IntSize>& iconSizes)
{
    ASSERT(iconSizes.isEmpty());
    if (value.isEmpty())
        return;
    if (value.is8Bit())
        parseSizes(value.characters8(), value.length(), iconSizes);
    else
        parseSizes(value.characters16(), value.length(), iconSizes);
}

inline HTMLLinkElement::HTMLLinkElement(Document& document, bool createdByParser)
    : HTMLElement(linkTag, document)
    , m_linkLoader(this)
    , m_sizes(DOMSettableTokenList::create())
    , m_createdByParser(createdByParser)
    , m_isInShadowTree(false)
{
}

PassRefPtrWillBeRawPtr<HTMLLinkElement> HTMLLinkElement::create(Document& document, bool createdByParser)
{
    return adoptRefWillBeNoop(new HTMLLinkElement(document, createdByParser));
}

HTMLLinkElement::~HTMLLinkElement()
{
#if !ENABLE(OILPAN)
    m_link.clear();

    if (inDocument())
        document().styleEngine().removeStyleSheetCandidateNode(this);
#endif

    linkLoadEventSender().cancelEvent(this);
}

void HTMLLinkElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == relAttr) {
        m_relAttribute = LinkRelAttribute(value);
        process();
    } else if (name == hrefAttr) {
        process();
    } else if (name == typeAttr) {
        m_type = value;
        process();
    } else if (name == asAttr) {
        m_as = value;
        process();
    } else if (name == sizesAttr) {
        m_sizes->setValue(value);
        parseSizesAttribute(value, m_iconSizes);
        process();
    } else if (name == mediaAttr) {
        m_media = value.lower();
        process();
    } else if (name == disabledAttr) {
        UseCounter::count(document(), UseCounter::HTMLLinkElementDisabled);
        if (LinkStyle* link = linkStyle())
            link->setDisabledState(!value.isNull());
    } else {
        if (name == titleAttr) {
            if (LinkStyle* link = linkStyle())
                link->setSheetTitle(value);
        }

        HTMLElement::parseAttribute(name, value);
    }
}

bool HTMLLinkElement::shouldLoadLink()
{
    return inDocument();
}

bool HTMLLinkElement::loadLink(const String& type, const String& as, const KURL& url)
{
    return m_linkLoader.loadLink(m_relAttribute, fastGetAttribute(HTMLNames::crossoriginAttr), type, as, url, document());
}

LinkResource* HTMLLinkElement::linkResourceToProcess()
{
    bool visible = inDocument() && !m_isInShadowTree;
    if (!visible) {
        ASSERT(!linkStyle() || !linkStyle()->hasSheet());
        return nullptr;
    }

    if (!m_link) {
        if (m_relAttribute.isImport()) {
            m_link = LinkImport::create(this);
        } else if (m_relAttribute.isManifest()) {
            m_link = LinkManifest::create(this);
        } else if (m_relAttribute.isDefaultPresentation()) {
            m_link = LinkDefaultPresentation::create(this);
        } else {
            OwnPtrWillBeRawPtr<LinkStyle> link = LinkStyle::create(this);
            if (fastHasAttribute(disabledAttr)) {
                UseCounter::count(document(), UseCounter::HTMLLinkElementDisabled);
                link->setDisabledState(true);
            }
            m_link = link.release();
        }
    }

    return m_link.get();
}

LinkStyle* HTMLLinkElement::linkStyle() const
{
    if (!m_link || m_link->type() != LinkResource::Style)
        return nullptr;
    return static_cast<LinkStyle*>(m_link.get());
}

LinkImport* HTMLLinkElement::linkImport() const
{
    if (!m_link || m_link->type() != LinkResource::Import)
        return nullptr;
    return static_cast<LinkImport*>(m_link.get());
}

Document* HTMLLinkElement::import() const
{
    if (LinkImport* link = linkImport())
        return link->importedDocument();
    return nullptr;
}

void HTMLLinkElement::process()
{
    if (LinkResource* link = linkResourceToProcess())
        link->process();
}

Node::InsertionNotificationRequest HTMLLinkElement::insertedInto(ContainerNode* insertionPoint)
{
    if (insertionPoint->inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("link");
            argv.append(fastGetAttribute(relAttr));
            argv.append(fastGetAttribute(hrefAttr));
            activityLogger->logEvent("blinkAddElement", argv.size(), argv.data());
        }
    }
    HTMLElement::insertedInto(insertionPoint);
    if (!insertionPoint->inDocument())
        return InsertionDone;

    m_isInShadowTree = isInShadowTree();
    if (m_isInShadowTree) {
        String message = "HTML element <link> is ignored in shadow tree.";
        document().addConsoleMessage(ConsoleMessage::create(JSMessageSource, WarningMessageLevel, message));
        return InsertionDone;
    }

    document().styleEngine().addStyleSheetCandidateNode(this, m_createdByParser);

    process();

    if (m_link)
        m_link->ownerInserted();

    return InsertionDone;
}

void HTMLLinkElement::removedFrom(ContainerNode* insertionPoint)
{
    HTMLElement::removedFrom(insertionPoint);
    if (!insertionPoint->inDocument())
        return;

    m_linkLoader.released();

    if (m_isInShadowTree) {
        ASSERT(!linkStyle() || !linkStyle()->hasSheet());
        return;
    }
    document().styleEngine().removeStyleSheetCandidateNode(this);

    RefPtrWillBeRawPtr<StyleSheet> removedSheet = sheet();

    if (m_link)
        m_link->ownerRemoved();

    document().removedStyleSheet(removedSheet.get());
}

void HTMLLinkElement::finishParsingChildren()
{
    m_createdByParser = false;
    HTMLElement::finishParsingChildren();
}

bool HTMLLinkElement::styleSheetIsLoading() const
{
    return linkStyle() && linkStyle()->styleSheetIsLoading();
}

void HTMLLinkElement::linkLoaded()
{
    dispatchEvent(Event::create(EventTypeNames::load));
}

void HTMLLinkElement::linkLoadingErrored()
{
    dispatchEvent(Event::create(EventTypeNames::error));
}

void HTMLLinkElement::didStartLinkPrerender()
{
    dispatchEvent(Event::create(EventTypeNames::webkitprerenderstart));
}

void HTMLLinkElement::didStopLinkPrerender()
{
    dispatchEvent(Event::create(EventTypeNames::webkitprerenderstop));
}

void HTMLLinkElement::didSendLoadForLinkPrerender()
{
    dispatchEvent(Event::create(EventTypeNames::webkitprerenderload));
}

void HTMLLinkElement::didSendDOMContentLoadedForLinkPrerender()
{
    dispatchEvent(Event::create(EventTypeNames::webkitprerenderdomcontentloaded));
}

bool HTMLLinkElement::sheetLoaded()
{
    ASSERT(linkStyle());
    return linkStyle()->sheetLoaded();
}

void HTMLLinkElement::notifyLoadedSheetAndAllCriticalSubresources(LoadedSheetErrorStatus errorStatus)
{
    ASSERT(linkStyle());
    linkStyle()->notifyLoadedSheetAndAllCriticalSubresources(errorStatus);
}

void HTMLLinkElement::dispatchPendingLoadEvents()
{
    linkLoadEventSender().dispatchPendingEvents();
}

void HTMLLinkElement::dispatchPendingEvent(LinkEventSender* eventSender)
{
    ASSERT_UNUSED(eventSender, eventSender == &linkLoadEventSender());
    ASSERT(m_link);
    if (m_link->hasLoaded())
        linkLoaded();
    else
        linkLoadingErrored();
}

void HTMLLinkElement::scheduleEvent()
{
    linkLoadEventSender().dispatchEventSoon(this);
}

void HTMLLinkElement::startLoadingDynamicSheet()
{
    ASSERT(linkStyle());
    linkStyle()->startLoadingDynamicSheet();
}

bool HTMLLinkElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name().localName() == hrefAttr || HTMLElement::isURLAttribute(attribute);
}

bool HTMLLinkElement::hasLegalLinkAttribute(const QualifiedName& name) const
{
    return name == hrefAttr || HTMLElement::hasLegalLinkAttribute(name);
}

const QualifiedName& HTMLLinkElement::subResourceAttributeName() const
{
    // If the link element is not css, ignore it.
    if (equalIgnoringCase(getAttribute(typeAttr), "text/css")) {
        // FIXME: Add support for extracting links of sub-resources which
        // are inside style-sheet such as @import, @font-face, url(), etc.
        return hrefAttr;
    }
    return HTMLElement::subResourceAttributeName();
}

KURL HTMLLinkElement::href() const
{
    return document().completeURL(getAttribute(hrefAttr));
}

const AtomicString& HTMLLinkElement::rel() const
{
    return getAttribute(relAttr);
}

const AtomicString& HTMLLinkElement::type() const
{
    return getAttribute(typeAttr);
}

bool HTMLLinkElement::async() const
{
    return fastHasAttribute(HTMLNames::asyncAttr);
}

IconType HTMLLinkElement::iconType() const
{
    return m_relAttribute.iconType();
}

const Vector<IntSize>& HTMLLinkElement::iconSizes() const
{
    return m_iconSizes;
}

DOMSettableTokenList* HTMLLinkElement::sizes() const
{
    return m_sizes.get();
}

DEFINE_TRACE(HTMLLinkElement)
{
    visitor->trace(m_link);
    visitor->trace(m_sizes);
    visitor->trace(m_linkLoader);
    HTMLElement::trace(visitor);
}

void HTMLLinkElement::attributeWillChange(const QualifiedName& name, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (name == hrefAttr && inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("link");
            argv.append(hrefAttr.toString());
            argv.append(oldValue);
            argv.append(newValue);
            activityLogger->logEvent("blinkSetAttribute", argv.size(), argv.data());
        }
    }
    HTMLElement::attributeWillChange(name, oldValue, newValue);
}

PassOwnPtrWillBeRawPtr<LinkStyle> LinkStyle::create(HTMLLinkElement* owner)
{
    return adoptPtrWillBeNoop(new LinkStyle(owner));
}

LinkStyle::LinkStyle(HTMLLinkElement* owner)
    : LinkResource(owner)
    , m_disabledState(Unset)
    , m_pendingSheetType(None)
    , m_loading(false)
    , m_firedLoad(false)
    , m_loadedSheet(false)
    , m_fetchFollowingCORS(false)
{
}

LinkStyle::~LinkStyle()
{
#if !ENABLE(OILPAN)
    if (m_sheet)
        m_sheet->clearOwnerNode();
#endif
}

Document& LinkStyle::document()
{
    return m_owner->document();
}

void LinkStyle::setCSSStyleSheet(const String& href, const KURL& baseURL, const String& charset, const CSSStyleSheetResource* cachedStyleSheet)
{
    if (!m_owner->inDocument()) {
        ASSERT(!m_sheet);
        return;
    }

    if (!cachedStyleSheet->errorOccurred() && !SubresourceIntegrity::CheckSubresourceIntegrity(*m_owner, cachedStyleSheet->sheetText(), KURL(baseURL, href), *cachedStyleSheet)) {
        m_loading = false;
        removePendingSheet();
        notifyLoadedSheetAndAllCriticalSubresources(Node::ErrorOccurredLoadingSubresource);
        return;
    }

    // Completing the sheet load may cause scripts to execute.
    RefPtrWillBeRawPtr<Node> protector(m_owner.get());

    CSSParserContext parserContext(m_owner->document(), 0, baseURL, charset);

    if (RefPtrWillBeRawPtr<StyleSheetContents> restoredSheet = const_cast<CSSStyleSheetResource*>(cachedStyleSheet)->restoreParsedStyleSheet(parserContext)) {
        ASSERT(restoredSheet->isCacheable());
        ASSERT(!restoredSheet->isLoading());

        if (m_sheet)
            clearSheet();
        m_sheet = CSSStyleSheet::create(restoredSheet, m_owner);
        m_sheet->setMediaQueries(MediaQuerySet::create(m_owner->media()));
        m_sheet->setTitle(m_owner->title());
        setCrossOriginStylesheetStatus(m_sheet.get());

        m_loading = false;
        restoredSheet->checkLoaded();
        return;
    }

    RefPtrWillBeRawPtr<StyleSheetContents> styleSheet = StyleSheetContents::create(href, parserContext);

    if (m_sheet)
        clearSheet();

    m_sheet = CSSStyleSheet::create(styleSheet, m_owner);
    m_sheet->setMediaQueries(MediaQuerySet::create(m_owner->media()));
    m_sheet->setTitle(m_owner->title());
    setCrossOriginStylesheetStatus(m_sheet.get());

    styleSheet->parseAuthorStyleSheet(cachedStyleSheet, m_owner->document().securityOrigin());

    m_loading = false;
    styleSheet->notifyLoadedSheet(cachedStyleSheet);
    styleSheet->checkLoaded();

    if (styleSheet->isCacheable())
        const_cast<CSSStyleSheetResource*>(cachedStyleSheet)->saveParsedStyleSheet(styleSheet);
}

bool LinkStyle::sheetLoaded()
{
    if (!styleSheetIsLoading()) {
        removePendingSheet();
        return true;
    }
    return false;
}

void LinkStyle::notifyLoadedSheetAndAllCriticalSubresources(Node::LoadedSheetErrorStatus errorStatus)
{
    if (m_firedLoad)
        return;
    m_loadedSheet = (errorStatus == Node::NoErrorLoadingSubresource);
    if (m_owner)
        m_owner->scheduleEvent();
    m_firedLoad = true;
}

void LinkStyle::startLoadingDynamicSheet()
{
    ASSERT(m_pendingSheetType < Blocking);
    addPendingSheet(Blocking);
}

void LinkStyle::clearSheet()
{
    ASSERT(m_sheet);
    ASSERT(m_sheet->ownerNode() == m_owner);
    m_sheet->clearOwnerNode();
    m_sheet = nullptr;
}

bool LinkStyle::styleSheetIsLoading() const
{
    if (m_loading)
        return true;
    if (!m_sheet)
        return false;
    return m_sheet->contents()->isLoading();
}

void LinkStyle::addPendingSheet(PendingSheetType type)
{
    if (type <= m_pendingSheetType)
        return;
    m_pendingSheetType = type;

    if (m_pendingSheetType == NonBlocking)
        return;
    m_owner->document().styleEngine().addPendingSheet();
}

void LinkStyle::removePendingSheet()
{
    PendingSheetType type = m_pendingSheetType;
    m_pendingSheetType = None;

    if (type == None)
        return;
    if (type == NonBlocking) {
        // Tell StyleEngine to re-compute styleSheets of this m_owner's treescope.
        m_owner->document().styleEngine().modifiedStyleSheetCandidateNode(m_owner);
        // Document::removePendingSheet() triggers the style selector recalc for blocking sheets.
        // FIXME: We don't have enough knowledge at this point to know if we're adding or removing a sheet
        // so we can't call addedStyleSheet() or removedStyleSheet().
        m_owner->document().styleResolverChanged();
        return;
    }

    m_owner->document().styleEngine().removePendingSheet(m_owner);
}

void LinkStyle::setDisabledState(bool disabled)
{
    LinkStyle::DisabledState oldDisabledState = m_disabledState;
    m_disabledState = disabled ? Disabled : EnabledViaScript;
    if (oldDisabledState != m_disabledState) {
        // If we change the disabled state while the sheet is still loading, then we have to
        // perform three checks:
        if (styleSheetIsLoading()) {
            // Check #1: The sheet becomes disabled while loading.
            if (m_disabledState == Disabled)
                removePendingSheet();

            // Check #2: An alternate sheet becomes enabled while it is still loading.
            if (m_owner->relAttribute().isAlternate() && m_disabledState == EnabledViaScript)
                addPendingSheet(Blocking);

            // Check #3: A main sheet becomes enabled while it was still loading and
            // after it was disabled via script. It takes really terrible code to make this
            // happen (a double toggle for no reason essentially). This happens on
            // virtualplastic.net, which manages to do about 12 enable/disables on only 3
            // sheets. :)
            if (!m_owner->relAttribute().isAlternate() && m_disabledState == EnabledViaScript && oldDisabledState == Disabled)
                addPendingSheet(Blocking);

            // If the sheet is already loading just bail.
            return;
        }

        if (m_sheet)
            m_sheet->setDisabled(disabled);

        // Load the sheet, since it's never been loaded before.
        if (!m_sheet && m_disabledState == EnabledViaScript) {
            if (m_owner->shouldProcessStyle())
                process();
        } else {
            // FIXME: We don't have enough knowledge here to know if we should call addedStyleSheet() or removedStyleSheet().
            m_owner->document().styleResolverChanged();
        }
    }
}

void LinkStyle::setCrossOriginStylesheetStatus(CSSStyleSheet* sheet)
{
    if (m_fetchFollowingCORS && resource() && !resource()->errorOccurred()) {
        // Record the security origin the CORS access check succeeded at, if cross origin.
        // Only origins that are script accessible to it may access the stylesheet's rules.
        sheet->setAllowRuleAccessFromOrigin(m_owner->document().securityOrigin());
    }
    m_fetchFollowingCORS = false;
}

void LinkStyle::process()
{
    ASSERT(m_owner->shouldProcessStyle());
    String type = m_owner->typeValue().lower();
    String as = m_owner->asValue().lower();
    LinkRequestBuilder builder(m_owner);

    if (m_owner->relAttribute().iconType() != InvalidIcon && builder.url().isValid() && !builder.url().isEmpty()) {
        if (!m_owner->shouldLoadLink())
            return;
        if (!document().securityOrigin()->canDisplay(builder.url()))
            return;
        if (!document().contentSecurityPolicy()->allowImageFromSource(builder.url()))
            return;
        if (document().frame() && document().frame()->loader().client())
            document().frame()->loader().client()->dispatchDidChangeIcons(m_owner->relAttribute().iconType());
    }

    if (!m_owner->loadLink(type, as, builder.url()))
        return;

    if (m_disabledState != Disabled && m_owner->relAttribute().isStyleSheet() && shouldLoadResource() && builder.url().isValid()) {

        if (resource()) {
            removePendingSheet();
            clearResource();
        }

        if (!m_owner->shouldLoadLink())
            return;

        m_loading = true;

        bool mediaQueryMatches = true;
        LocalFrame* frame = loadingFrame();
        if (!m_owner->media().isEmpty() && frame && frame->document()) {
            RefPtr<ComputedStyle> documentStyle = StyleResolver::styleForDocument(*frame->document());
            RefPtrWillBeRawPtr<MediaQuerySet> media = MediaQuerySet::create(m_owner->media());
            MediaQueryEvaluator evaluator(frame);
            mediaQueryMatches = evaluator.eval(media.get());
        }

        // Don't hold up layout tree construction and script execution on stylesheets
        // that are not needed for the layout at the moment.
        bool blocking = mediaQueryMatches && !m_owner->isAlternate();
        addPendingSheet(blocking ? Blocking : NonBlocking);

        // Load stylesheets that are not needed for the layout immediately with low priority.
        FetchRequest request = builder.build(blocking);
        AtomicString crossOriginMode = m_owner->fastGetAttribute(HTMLNames::crossoriginAttr);
        if (!crossOriginMode.isNull()) {
            request.setCrossOriginAccessControl(document().securityOrigin(), crossOriginMode);
            setFetchFollowingCORS();
        }
        setResource(CSSStyleSheetResource::fetch(request, document().fetcher()));

        if (!resource()) {
            // The request may have been denied if (for example) the stylesheet is local and the document is remote, or if there was a Content Security Policy Failure.
            m_loading = false;
            removePendingSheet();
            notifyLoadedSheetAndAllCriticalSubresources(Node::ErrorOccurredLoadingSubresource);
        }
    } else if (m_sheet) {
        // we no longer contain a stylesheet, e.g. perhaps rel or type was changed
        RefPtrWillBeRawPtr<StyleSheet> removedSheet = m_sheet.get();
        clearSheet();
        document().removedStyleSheet(removedSheet.get());
    }
}

void LinkStyle::setSheetTitle(const String& title)
{
    if (m_sheet)
        m_sheet->setTitle(title);
}

void LinkStyle::ownerRemoved()
{
    if (m_sheet)
        clearSheet();

    if (styleSheetIsLoading())
        removePendingSheet();
}

DEFINE_TRACE(LinkStyle)
{
    visitor->trace(m_sheet);
    LinkResource::trace(visitor);
}

} // namespace blink
