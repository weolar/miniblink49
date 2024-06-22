// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "HTMLElementFactory.h"

#include "HTMLNames.h"
#include "core/html/HTMLAnchorElement.h"
#include "core/html/HTMLAppletElement.h"
#include "core/html/HTMLAreaElement.h"
#include "core/html/HTMLAudioElement.h"
#include "core/html/HTMLBDIElement.h"
#include "core/html/HTMLBRElement.h"
#include "core/html/HTMLBaseElement.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLButtonElement.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLContentElement.h"
#include "core/html/HTMLDListElement.h"
#include "core/html/HTMLDataListElement.h"
#include "core/html/HTMLDetailsElement.h"
#include "core/html/HTMLDialogElement.h"
#include "core/html/HTMLDirectoryElement.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLEmbedElement.h"
#include "core/html/HTMLFieldSetElement.h"
#include "core/html/HTMLFontElement.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLFrameElement.h"
#include "core/html/HTMLFrameSetElement.h"
#include "core/html/HTMLHRElement.h"
#include "core/html/HTMLHeadElement.h"
#include "core/html/HTMLHeadingElement.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLKeygenElement.h"
#include "core/html/HTMLLIElement.h"
#include "core/html/HTMLLabelElement.h"
#include "core/html/HTMLLegendElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/html/HTMLMapElement.h"
#include "core/html/HTMLMarqueeElement.h"
#include "core/html/HTMLMenuElement.h"
#include "core/html/HTMLMenuItemElement.h"
#include "core/html/HTMLMetaElement.h"
#include "core/html/HTMLMeterElement.h"
#include "core/html/HTMLModElement.h"
#include "core/html/HTMLNoEmbedElement.h"
#include "core/html/HTMLNoScriptElement.h"
#include "core/html/HTMLOListElement.h"
#include "core/html/HTMLObjectElement.h"
#include "core/html/HTMLOptGroupElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLOutputElement.h"
#include "core/html/HTMLParagraphElement.h"
#include "core/html/HTMLParamElement.h"
#include "core/html/HTMLPictureElement.h"
#include "core/html/HTMLPreElement.h"
#include "core/html/HTMLProgressElement.h"
#include "core/html/HTMLQuoteElement.h"
#include "core/html/HTMLRTElement.h"
#include "core/html/HTMLRubyElement.h"
#include "core/html/HTMLScriptElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/HTMLShadowElement.h"
#include "core/html/HTMLSourceElement.h"
#include "core/html/HTMLSpanElement.h"
#include "core/html/HTMLStyleElement.h"
#include "core/html/HTMLSummaryElement.h"
#include "core/html/HTMLTableCaptionElement.h"
#include "core/html/HTMLTableCellElement.h"
#include "core/html/HTMLTableColElement.h"
#include "core/html/HTMLTableElement.h"
#include "core/html/HTMLTableRowElement.h"
#include "core/html/HTMLTableSectionElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/html/HTMLTitleElement.h"
#include "core/html/HTMLTrackElement.h"
#include "core/html/HTMLUListElement.h"
#include "core/html/HTMLUnknownElement.h"
#include "core/html/HTMLVideoElement.h"
#include "core/html/HTMLWBRElement.h"
#include "core/html/HTMLUnknownElement.h"
#include "core/dom/custom/CustomElement.h"
#include "core/dom/custom/CustomElementRegistrationContext.h"
#include "core/dom/Document.h"
#include "core/frame/Settings.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/HashMap.h"

namespace blink {

using namespace HTMLNames;

typedef PassRefPtrWillBeRawPtr<HTMLElement> (*ConstructorFunction)(
    Document&,
    HTMLFormElement*,
    bool createdByParser);

typedef HashMap<AtomicString, ConstructorFunction> FunctionMap;

static FunctionMap* g_constructors = 0;

static PassRefPtrWillBeRawPtr<HTMLElement> abbrConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(abbrTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> acronymConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(acronymTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> addressConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(addressTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> articleConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(articleTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> asideConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(asideTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> bConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(bTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> basefontConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(basefontTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> bdoConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(bdoTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> bigConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(bigTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> centerConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(centerTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> citeConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(citeTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> codeConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(codeTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> commandConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(commandTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> ddConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(ddTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> dfnConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(dfnTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> dtConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(dtTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> emConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(emTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> figcaptionConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(figcaptionTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> figureConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(figureTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> footerConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(footerTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> headerConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(headerTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> hgroupConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(hgroupTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> iConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(iTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> kbdConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(kbdTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> layerConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(layerTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> mainConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(mainTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> markConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(markTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> navConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(navTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> nobrConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(nobrTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> noframesConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(noframesTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> nolayerConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(nolayerTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> plaintextConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(plaintextTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> rpConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(rpTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> sConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(sTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> sampConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(sampTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> sectionConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(sectionTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> smallConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(smallTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> strikeConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(strikeTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> strongConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(strongTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> subConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(subTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> supConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(supTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> ttConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(ttTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> uConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(uTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> varConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLElement::create(varTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> aConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLAnchorElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> areaConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLAreaElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> audioConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    if (!RuntimeEnabledFeatures::mediaEnabled())
        return HTMLUnknownElement::create(audioTag, document);
    return HTMLAudioElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> brConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLBRElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> baseConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLBaseElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> bodyConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLBodyElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> canvasConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLCanvasElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> contentConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLContentElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> dlConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLDListElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> datalistConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLDataListElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> detailsConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLDetailsElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> dialogConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLDialogElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> dirConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLDirectoryElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> divConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLDivElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> fontConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLFontElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> formConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLFormElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> frameConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLFrameElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> framesetConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLFrameSetElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> hrConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHRElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> headConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHeadElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> h1Constructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHeadingElement::create(h1Tag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> h2Constructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHeadingElement::create(h2Tag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> h3Constructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHeadingElement::create(h3Tag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> h4Constructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHeadingElement::create(h4Tag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> h5Constructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHeadingElement::create(h5Tag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> h6Constructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHeadingElement::create(h6Tag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> htmlConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLHtmlElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> iframeConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLIFrameElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> liConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLLIElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> legendConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLLegendElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> mapConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLMapElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> marqueeConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLMarqueeElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> menuConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLMenuElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> menuitemConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    if (!RuntimeEnabledFeatures::contextMenuEnabled())
        return HTMLUnknownElement::create(menuitemTag, document);
    return HTMLMenuItemElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> metaConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLMetaElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> meterConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLMeterElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> delConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLModElement::create(delTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> insConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLModElement::create(insTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> olConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLOListElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> optgroupConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLOptGroupElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> optionConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLOptionElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> pConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLParagraphElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> paramConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLParamElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> pictureConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLPictureElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> preConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLPreElement::create(preTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> listingConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLPreElement::create(listingTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> xmpConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLPreElement::create(xmpTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> progressConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLProgressElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> blockquoteConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLQuoteElement::create(blockquoteTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> qConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLQuoteElement::create(qTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> shadowConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLShadowElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> sourceConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    if (!RuntimeEnabledFeatures::mediaEnabled())
        return HTMLUnknownElement::create(sourceTag, document);
    return HTMLSourceElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> spanConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLSpanElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> captionConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableCaptionElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> tdConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableCellElement::create(tdTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> thConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableCellElement::create(thTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> colConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableColElement::create(colTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> colgroupConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableColElement::create(colgroupTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> tableConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> trConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableRowElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> tbodyConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableSectionElement::create(tbodyTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> tfootConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableSectionElement::create(tfootTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> theadConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTableSectionElement::create(theadTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> templateConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTemplateElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> titleConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTitleElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> trackConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTrackElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> ulConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLUListElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> bgsoundConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLUnknownElement::create(bgsoundTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> imageConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLUnknownElement::create(imageTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> rbConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLUnknownElement::create(rbTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> rtcConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLUnknownElement::create(rtcTag, document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> videoConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    if (!RuntimeEnabledFeatures::mediaEnabled())
        return HTMLUnknownElement::create(videoTag, document);
    return HTMLVideoElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> buttonConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLButtonElement::create(document, formElement);
}
static PassRefPtrWillBeRawPtr<HTMLElement> fieldsetConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLFieldSetElement::create(document, formElement);
}
static PassRefPtrWillBeRawPtr<HTMLElement> keygenConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLKeygenElement::create(document, formElement);
}
static PassRefPtrWillBeRawPtr<HTMLElement> labelConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLLabelElement::create(document, formElement);
}
static PassRefPtrWillBeRawPtr<HTMLElement> outputConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLOutputElement::create(document, formElement);
}
static PassRefPtrWillBeRawPtr<HTMLElement> selectConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLSelectElement::create(document, formElement);
}
static PassRefPtrWillBeRawPtr<HTMLElement> textareaConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLTextAreaElement::create(document, formElement);
}
static PassRefPtrWillBeRawPtr<HTMLElement> appletConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLAppletElement::create(document, createdByParser);
}
static PassRefPtrWillBeRawPtr<HTMLElement> embedConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLEmbedElement::create(document, createdByParser);
}
static PassRefPtrWillBeRawPtr<HTMLElement> linkConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLLinkElement::create(document, createdByParser);
}
static PassRefPtrWillBeRawPtr<HTMLElement> scriptConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLScriptElement::create(document, createdByParser);
}
static PassRefPtrWillBeRawPtr<HTMLElement> styleConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLStyleElement::create(document, createdByParser);
}
static PassRefPtrWillBeRawPtr<HTMLElement> imgConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLImageElement::create(document, formElement, createdByParser);
}
static PassRefPtrWillBeRawPtr<HTMLElement> inputConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLInputElement::create(document, formElement, createdByParser);
}
static PassRefPtrWillBeRawPtr<HTMLElement> objectConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLObjectElement::create(document, formElement, createdByParser);
}
static PassRefPtrWillBeRawPtr<HTMLElement> bdiConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLBDIElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> noembedConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLNoEmbedElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> noscriptConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLNoScriptElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> rtConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLRTElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> rubyConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLRubyElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> summaryConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLSummaryElement::create(document);
}
static PassRefPtrWillBeRawPtr<HTMLElement> wbrConstructor(
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    return HTMLWBRElement::create(document);
}

struct CreateHTMLFunctionMapData {
  const QualifiedName& tag;
  ConstructorFunction func;
};

static void createHTMLFunctionMap()
{
    ASSERT(!g_constructors);
    g_constructors = new FunctionMap;
    // Empty array initializer lists are illegal [dcl.init.aggr] and will not
    // compile in MSVC. If tags list is empty, add check to skip this.
    static const CreateHTMLFunctionMapData data[] = {
        { abbrTag, abbrConstructor },
        { acronymTag, acronymConstructor },
        { addressTag, addressConstructor },
        { articleTag, articleConstructor },
        { asideTag, asideConstructor },
        { bTag, bConstructor },
        { basefontTag, basefontConstructor },
        { bdoTag, bdoConstructor },
        { bigTag, bigConstructor },
        { centerTag, centerConstructor },
        { citeTag, citeConstructor },
        { codeTag, codeConstructor },
        { commandTag, commandConstructor },
        { ddTag, ddConstructor },
        { dfnTag, dfnConstructor },
        { dtTag, dtConstructor },
        { emTag, emConstructor },
        { figcaptionTag, figcaptionConstructor },
        { figureTag, figureConstructor },
        { footerTag, footerConstructor },
        { headerTag, headerConstructor },
        { hgroupTag, hgroupConstructor },
        { iTag, iConstructor },
        { kbdTag, kbdConstructor },
        { layerTag, layerConstructor },
        { mainTag, mainConstructor },
        { markTag, markConstructor },
        { navTag, navConstructor },
        { nobrTag, nobrConstructor },
        { noframesTag, noframesConstructor },
        { nolayerTag, nolayerConstructor },
        { plaintextTag, plaintextConstructor },
        { rpTag, rpConstructor },
        { sTag, sConstructor },
        { sampTag, sampConstructor },
        { sectionTag, sectionConstructor },
        { smallTag, smallConstructor },
        { strikeTag, strikeConstructor },
        { strongTag, strongConstructor },
        { subTag, subConstructor },
        { supTag, supConstructor },
        { ttTag, ttConstructor },
        { uTag, uConstructor },
        { varTag, varConstructor },
        { aTag, aConstructor },
        { areaTag, areaConstructor },
        { audioTag, audioConstructor },
        { brTag, brConstructor },
        { baseTag, baseConstructor },
        { bodyTag, bodyConstructor },
        { canvasTag, canvasConstructor },
        { contentTag, contentConstructor },
        { dlTag, dlConstructor },
        { datalistTag, datalistConstructor },
        { detailsTag, detailsConstructor },
        { dialogTag, dialogConstructor },
        { dirTag, dirConstructor },
        { divTag, divConstructor },
        { fontTag, fontConstructor },
        { formTag, formConstructor },
        { frameTag, frameConstructor },
        { framesetTag, framesetConstructor },
        { hrTag, hrConstructor },
        { headTag, headConstructor },
        { h1Tag, h1Constructor },
        { h2Tag, h2Constructor },
        { h3Tag, h3Constructor },
        { h4Tag, h4Constructor },
        { h5Tag, h5Constructor },
        { h6Tag, h6Constructor },
        { htmlTag, htmlConstructor },
        { iframeTag, iframeConstructor },
        { liTag, liConstructor },
        { legendTag, legendConstructor },
        { mapTag, mapConstructor },
        { marqueeTag, marqueeConstructor },
        { menuTag, menuConstructor },
        { menuitemTag, menuitemConstructor },
        { metaTag, metaConstructor },
        { meterTag, meterConstructor },
        { delTag, delConstructor },
        { insTag, insConstructor },
        { olTag, olConstructor },
        { optgroupTag, optgroupConstructor },
        { optionTag, optionConstructor },
        { pTag, pConstructor },
        { paramTag, paramConstructor },
        { pictureTag, pictureConstructor },
        { preTag, preConstructor },
        { listingTag, listingConstructor },
        { xmpTag, xmpConstructor },
        { progressTag, progressConstructor },
        { blockquoteTag, blockquoteConstructor },
        { qTag, qConstructor },
        { shadowTag, shadowConstructor },
        { sourceTag, sourceConstructor },
        { spanTag, spanConstructor },
        { captionTag, captionConstructor },
        { tdTag, tdConstructor },
        { thTag, thConstructor },
        { colTag, colConstructor },
        { colgroupTag, colgroupConstructor },
        { tableTag, tableConstructor },
        { trTag, trConstructor },
        { tbodyTag, tbodyConstructor },
        { tfootTag, tfootConstructor },
        { theadTag, theadConstructor },
        { templateTag, templateConstructor },
        { titleTag, titleConstructor },
        { trackTag, trackConstructor },
        { ulTag, ulConstructor },
        { bgsoundTag, bgsoundConstructor },
        { imageTag, imageConstructor },
        { rbTag, rbConstructor },
        { rtcTag, rtcConstructor },
        { videoTag, videoConstructor },
        { buttonTag, buttonConstructor },
        { fieldsetTag, fieldsetConstructor },
        { keygenTag, keygenConstructor },
        { labelTag, labelConstructor },
        { outputTag, outputConstructor },
        { selectTag, selectConstructor },
        { textareaTag, textareaConstructor },
        { appletTag, appletConstructor },
        { embedTag, embedConstructor },
        { linkTag, linkConstructor },
        { scriptTag, scriptConstructor },
        { styleTag, styleConstructor },
        { imgTag, imgConstructor },
        { inputTag, inputConstructor },
        { objectTag, objectConstructor },
        { bdiTag, bdiConstructor },
        { noembedTag, noembedConstructor },
        { noscriptTag, noscriptConstructor },
        { rtTag, rtConstructor },
        { rubyTag, rubyConstructor },
        { summaryTag, summaryConstructor },
        { wbrTag, wbrConstructor },
    };
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(data); i++)
        g_constructors->set(data[i].tag.localName(), data[i].func);
}

PassRefPtrWillBeRawPtr<HTMLElement> HTMLElementFactory::createHTMLElement(
    const AtomicString& localName,
    Document& document,
    HTMLFormElement* formElement,
    bool createdByParser)
{
    if (!g_constructors)
        createHTMLFunctionMap();
    if (ConstructorFunction function = g_constructors->get(localName))
        return function(document, formElement, createdByParser);

#if 1 // def MINIBLINK_NOT_IMPLEMENTED
    if (document.registrationContext() && CustomElement::isValidName(localName)) {
        RefPtrWillBeRawPtr<Element> element = document.registrationContext()->createCustomTagElement(document, QualifiedName(nullAtom, localName, xhtmlNamespaceURI));
        ASSERT_WITH_SECURITY_IMPLICATION(element->isHTMLElement());
        return static_pointer_cast<HTMLElement>(element.release());
    }
#endif // MINIBLINK_NOT_IMPLEMENTED

    return HTMLUnknownElement::create(QualifiedName(nullAtom, localName, xhtmlNamespaceURI), document);
}

} // namespace blink
