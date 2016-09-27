// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "HTMLNames.h"

#include "wtf/StaticConstructors.h"
#include "wtf/StdLibExtras.h"

namespace blink {
namespace HTMLNames {

using namespace blink;

DEFINE_GLOBAL(AtomicString, xhtmlNamespaceURI)

// Tags

void* TagStorage[HTMLTagsCount * ((sizeof(HTMLQualifiedName) + sizeof(void *) - 1) / sizeof(void *))];
const HTMLQualifiedName& aTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[0];
const HTMLQualifiedName& abbrTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[1];
const HTMLQualifiedName& acronymTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[2];
const HTMLQualifiedName& addressTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[3];
const HTMLQualifiedName& appletTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[4];
const HTMLQualifiedName& areaTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[5];
const HTMLQualifiedName& articleTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[6];
const HTMLQualifiedName& asideTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[7];
const HTMLQualifiedName& audioTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[8];
const HTMLQualifiedName& bTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[9];
const HTMLQualifiedName& baseTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[10];
const HTMLQualifiedName& basefontTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[11];
const HTMLQualifiedName& bdiTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[12];
const HTMLQualifiedName& bdoTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[13];
const HTMLQualifiedName& bgsoundTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[14];
const HTMLQualifiedName& bigTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[15];
const HTMLQualifiedName& blockquoteTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[16];
const HTMLQualifiedName& bodyTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[17];
const HTMLQualifiedName& brTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[18];
const HTMLQualifiedName& buttonTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[19];
const HTMLQualifiedName& canvasTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[20];
const HTMLQualifiedName& captionTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[21];
const HTMLQualifiedName& centerTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[22];
const HTMLQualifiedName& citeTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[23];
const HTMLQualifiedName& codeTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[24];
const HTMLQualifiedName& colTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[25];
const HTMLQualifiedName& colgroupTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[26];
const HTMLQualifiedName& commandTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[27];
const HTMLQualifiedName& contentTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[28];
const HTMLQualifiedName& datalistTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[29];
const HTMLQualifiedName& ddTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[30];
const HTMLQualifiedName& delTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[31];
const HTMLQualifiedName& detailsTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[32];
const HTMLQualifiedName& dfnTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[33];
const HTMLQualifiedName& dialogTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[34];
const HTMLQualifiedName& dirTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[35];
const HTMLQualifiedName& divTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[36];
const HTMLQualifiedName& dlTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[37];
const HTMLQualifiedName& dtTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[38];
const HTMLQualifiedName& emTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[39];
const HTMLQualifiedName& embedTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[40];
const HTMLQualifiedName& fieldsetTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[41];
const HTMLQualifiedName& figcaptionTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[42];
const HTMLQualifiedName& figureTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[43];
const HTMLQualifiedName& fontTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[44];
const HTMLQualifiedName& footerTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[45];
const HTMLQualifiedName& formTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[46];
const HTMLQualifiedName& frameTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[47];
const HTMLQualifiedName& framesetTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[48];
const HTMLQualifiedName& h1Tag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[49];
const HTMLQualifiedName& h2Tag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[50];
const HTMLQualifiedName& h3Tag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[51];
const HTMLQualifiedName& h4Tag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[52];
const HTMLQualifiedName& h5Tag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[53];
const HTMLQualifiedName& h6Tag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[54];
const HTMLQualifiedName& headTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[55];
const HTMLQualifiedName& headerTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[56];
const HTMLQualifiedName& hgroupTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[57];
const HTMLQualifiedName& hrTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[58];
const HTMLQualifiedName& htmlTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[59];
const HTMLQualifiedName& iTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[60];
const HTMLQualifiedName& iframeTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[61];
const HTMLQualifiedName& imageTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[62];
const HTMLQualifiedName& imgTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[63];
const HTMLQualifiedName& inputTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[64];
const HTMLQualifiedName& insTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[65];
const HTMLQualifiedName& kbdTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[66];
const HTMLQualifiedName& keygenTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[67];
const HTMLQualifiedName& labelTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[68];
const HTMLQualifiedName& layerTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[69];
const HTMLQualifiedName& legendTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[70];
const HTMLQualifiedName& liTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[71];
const HTMLQualifiedName& linkTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[72];
const HTMLQualifiedName& listingTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[73];
const HTMLQualifiedName& mainTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[74];
const HTMLQualifiedName& mapTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[75];
const HTMLQualifiedName& markTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[76];
const HTMLQualifiedName& marqueeTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[77];
const HTMLQualifiedName& menuTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[78];
const HTMLQualifiedName& menuitemTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[79];
const HTMLQualifiedName& metaTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[80];
const HTMLQualifiedName& meterTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[81];
const HTMLQualifiedName& navTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[82];
const HTMLQualifiedName& nobrTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[83];
const HTMLQualifiedName& noembedTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[84];
const HTMLQualifiedName& noframesTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[85];
const HTMLQualifiedName& nolayerTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[86];
const HTMLQualifiedName& noscriptTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[87];
const HTMLQualifiedName& objectTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[88];
const HTMLQualifiedName& olTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[89];
const HTMLQualifiedName& optgroupTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[90];
const HTMLQualifiedName& optionTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[91];
const HTMLQualifiedName& outputTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[92];
const HTMLQualifiedName& pTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[93];
const HTMLQualifiedName& paramTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[94];
const HTMLQualifiedName& pictureTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[95];
const HTMLQualifiedName& plaintextTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[96];
const HTMLQualifiedName& preTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[97];
const HTMLQualifiedName& progressTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[98];
const HTMLQualifiedName& qTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[99];
const HTMLQualifiedName& rbTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[100];
const HTMLQualifiedName& rpTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[101];
const HTMLQualifiedName& rtTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[102];
const HTMLQualifiedName& rtcTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[103];
const HTMLQualifiedName& rubyTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[104];
const HTMLQualifiedName& sTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[105];
const HTMLQualifiedName& sampTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[106];
const HTMLQualifiedName& scriptTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[107];
const HTMLQualifiedName& sectionTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[108];
const HTMLQualifiedName& selectTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[109];
const HTMLQualifiedName& shadowTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[110];
const HTMLQualifiedName& smallTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[111];
const HTMLQualifiedName& sourceTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[112];
const HTMLQualifiedName& spanTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[113];
const HTMLQualifiedName& strikeTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[114];
const HTMLQualifiedName& strongTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[115];
const HTMLQualifiedName& styleTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[116];
const HTMLQualifiedName& subTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[117];
const HTMLQualifiedName& summaryTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[118];
const HTMLQualifiedName& supTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[119];
const HTMLQualifiedName& tableTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[120];
const HTMLQualifiedName& tbodyTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[121];
const HTMLQualifiedName& tdTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[122];
const HTMLQualifiedName& templateTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[123];
const HTMLQualifiedName& textareaTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[124];
const HTMLQualifiedName& tfootTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[125];
const HTMLQualifiedName& thTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[126];
const HTMLQualifiedName& theadTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[127];
const HTMLQualifiedName& titleTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[128];
const HTMLQualifiedName& trTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[129];
const HTMLQualifiedName& trackTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[130];
const HTMLQualifiedName& ttTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[131];
const HTMLQualifiedName& uTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[132];
const HTMLQualifiedName& ulTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[133];
const HTMLQualifiedName& varTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[134];
const HTMLQualifiedName& videoTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[135];
const HTMLQualifiedName& wbrTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[136];
const HTMLQualifiedName& xmpTag = reinterpret_cast<HTMLQualifiedName*>(&TagStorage)[137];


PassOwnPtr<const HTMLQualifiedName*[]> getHTMLTags()
{
    OwnPtr<const HTMLQualifiedName*[]> tags = adoptArrayPtr(new const HTMLQualifiedName*[HTMLTagsCount]);
    for (size_t i = 0; i < HTMLTagsCount; i++)
        tags[i] = reinterpret_cast<HTMLQualifiedName*>(&TagStorage) + i;
    return tags.release();
}

// Attributes

void* AttrStorage[HTMLAttrsCount * ((sizeof(QualifiedName) + sizeof(void *) - 1) / sizeof(void *))];

const QualifiedName& abbrAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[0];
const QualifiedName& acceptAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[1];
const QualifiedName& accept_charsetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[2];
const QualifiedName& accesskeyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[3];
const QualifiedName& actionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[4];
const QualifiedName& alignAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[5];
const QualifiedName& alinkAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[6];
const QualifiedName& allowfullscreenAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[7];
const QualifiedName& altAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[8];
const QualifiedName& archiveAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[9];
const QualifiedName& aria_activedescendantAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[10];
const QualifiedName& aria_atomicAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[11];
const QualifiedName& aria_autocompleteAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[12];
const QualifiedName& aria_busyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[13];
const QualifiedName& aria_checkedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[14];
const QualifiedName& aria_controlsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[15];
const QualifiedName& aria_describedbyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[16];
const QualifiedName& aria_disabledAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[17];
const QualifiedName& aria_dropeffectAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[18];
const QualifiedName& aria_expandedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[19];
const QualifiedName& aria_flowtoAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[20];
const QualifiedName& aria_grabbedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[21];
const QualifiedName& aria_haspopupAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[22];
const QualifiedName& aria_helpAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[23];
const QualifiedName& aria_hiddenAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[24];
const QualifiedName& aria_invalidAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[25];
const QualifiedName& aria_labelAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[26];
const QualifiedName& aria_labeledbyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[27];
const QualifiedName& aria_labelledbyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[28];
const QualifiedName& aria_levelAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[29];
const QualifiedName& aria_liveAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[30];
const QualifiedName& aria_multilineAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[31];
const QualifiedName& aria_multiselectableAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[32];
const QualifiedName& aria_orientationAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[33];
const QualifiedName& aria_ownsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[34];
const QualifiedName& aria_posinsetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[35];
const QualifiedName& aria_pressedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[36];
const QualifiedName& aria_readonlyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[37];
const QualifiedName& aria_relevantAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[38];
const QualifiedName& aria_requiredAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[39];
const QualifiedName& aria_selectedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[40];
const QualifiedName& aria_setsizeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[41];
const QualifiedName& aria_sortAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[42];
const QualifiedName& aria_valuemaxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[43];
const QualifiedName& aria_valueminAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[44];
const QualifiedName& aria_valuenowAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[45];
const QualifiedName& aria_valuetextAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[46];
const QualifiedName& asAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[47];
const QualifiedName& asyncAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[48];
const QualifiedName& autocapitalizeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[49];
const QualifiedName& autocompleteAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[50];
const QualifiedName& autofocusAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[51];
const QualifiedName& autoplayAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[52];
const QualifiedName& axisAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[53];
const QualifiedName& backgroundAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[54];
const QualifiedName& behaviorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[55];
const QualifiedName& bgcolorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[56];
const QualifiedName& borderAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[57];
const QualifiedName& bordercolorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[58];
const QualifiedName& captureAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[59];
const QualifiedName& cellpaddingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[60];
const QualifiedName& cellspacingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[61];
const QualifiedName& challengeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[62];
const QualifiedName& charAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[63];
const QualifiedName& charoffAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[64];
const QualifiedName& charsetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[65];
const QualifiedName& checkedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[66];
const QualifiedName& citeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[67];
const QualifiedName& classAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[68];
const QualifiedName& classidAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[69];
const QualifiedName& clearAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[70];
const QualifiedName& codeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[71];
const QualifiedName& codebaseAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[72];
const QualifiedName& codetypeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[73];
const QualifiedName& colorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[74];
const QualifiedName& colsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[75];
const QualifiedName& colspanAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[76];
const QualifiedName& compactAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[77];
const QualifiedName& contentAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[78];
const QualifiedName& contenteditableAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[79];
const QualifiedName& contextmenuAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[80];
const QualifiedName& controlsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[81];
const QualifiedName& coordsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[82];
const QualifiedName& crossoriginAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[83];
const QualifiedName& dataAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[84];
const QualifiedName& datetimeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[85];
const QualifiedName& declareAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[86];
const QualifiedName& defaultAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[87];
const QualifiedName& deferAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[88];
const QualifiedName& dirAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[89];
const QualifiedName& directionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[90];
const QualifiedName& dirnameAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[91];
const QualifiedName& disabledAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[92];
const QualifiedName& downloadAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[93];
const QualifiedName& draggableAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[94];
const QualifiedName& enctypeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[95];
const QualifiedName& endAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[96];
const QualifiedName& eventAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[97];
const QualifiedName& faceAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[98];
const QualifiedName& forAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[99];
const QualifiedName& formAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[100];
const QualifiedName& formactionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[101];
const QualifiedName& formenctypeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[102];
const QualifiedName& formmethodAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[103];
const QualifiedName& formnovalidateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[104];
const QualifiedName& formtargetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[105];
const QualifiedName& frameAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[106];
const QualifiedName& frameborderAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[107];
const QualifiedName& headersAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[108];
const QualifiedName& heightAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[109];
const QualifiedName& hiddenAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[110];
const QualifiedName& highAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[111];
const QualifiedName& hrefAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[112];
const QualifiedName& hreflangAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[113];
const QualifiedName& hspaceAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[114];
const QualifiedName& http_equivAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[115];
const QualifiedName& iconAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[116];
const QualifiedName& idAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[117];
const QualifiedName& incrementalAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[118];
const QualifiedName& inputmodeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[119];
const QualifiedName& integrityAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[120];
const QualifiedName& isAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[121];
const QualifiedName& ismapAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[122];
const QualifiedName& keytypeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[123];
const QualifiedName& kindAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[124];
const QualifiedName& labelAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[125];
const QualifiedName& langAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[126];
const QualifiedName& languageAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[127];
const QualifiedName& leftmarginAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[128];
const QualifiedName& linkAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[129];
const QualifiedName& listAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[130];
const QualifiedName& longdescAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[131];
const QualifiedName& loopAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[132];
const QualifiedName& lowAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[133];
const QualifiedName& lowsrcAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[134];
const QualifiedName& manifestAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[135];
const QualifiedName& marginheightAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[136];
const QualifiedName& marginwidthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[137];
const QualifiedName& maxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[138];
const QualifiedName& maxlengthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[139];
const QualifiedName& mayscriptAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[140];
const QualifiedName& mediaAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[141];
const QualifiedName& mediagroupAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[142];
const QualifiedName& methodAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[143];
const QualifiedName& minAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[144];
const QualifiedName& minlengthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[145];
const QualifiedName& multipleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[146];
const QualifiedName& mutedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[147];
const QualifiedName& nameAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[148];
const QualifiedName& nohrefAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[149];
const QualifiedName& nonceAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[150];
const QualifiedName& noresizeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[151];
const QualifiedName& noshadeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[152];
const QualifiedName& novalidateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[153];
const QualifiedName& nowrapAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[154];
const QualifiedName& objectAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[155];
const QualifiedName& onabortAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[156];
const QualifiedName& onanimationendAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[157];
const QualifiedName& onanimationiterationAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[158];
const QualifiedName& onanimationstartAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[159];
const QualifiedName& onautocompleteAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[160];
const QualifiedName& onautocompleteerrorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[161];
const QualifiedName& onbeforecopyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[162];
const QualifiedName& onbeforecutAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[163];
const QualifiedName& onbeforepasteAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[164];
const QualifiedName& onbeforeunloadAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[165];
const QualifiedName& onblurAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[166];
const QualifiedName& oncancelAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[167];
const QualifiedName& oncanplayAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[168];
const QualifiedName& oncanplaythroughAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[169];
const QualifiedName& onchangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[170];
const QualifiedName& onclickAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[171];
const QualifiedName& oncloseAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[172];
const QualifiedName& oncontextmenuAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[173];
const QualifiedName& oncopyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[174];
const QualifiedName& oncuechangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[175];
const QualifiedName& oncutAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[176];
const QualifiedName& ondblclickAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[177];
const QualifiedName& ondragAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[178];
const QualifiedName& ondragendAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[179];
const QualifiedName& ondragenterAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[180];
const QualifiedName& ondragleaveAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[181];
const QualifiedName& ondragoverAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[182];
const QualifiedName& ondragstartAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[183];
const QualifiedName& ondropAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[184];
const QualifiedName& ondurationchangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[185];
const QualifiedName& onemptiedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[186];
const QualifiedName& onendedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[187];
const QualifiedName& onerrorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[188];
const QualifiedName& onfocusAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[189];
const QualifiedName& onfocusinAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[190];
const QualifiedName& onfocusoutAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[191];
const QualifiedName& onhashchangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[192];
const QualifiedName& oninputAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[193];
const QualifiedName& oninvalidAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[194];
const QualifiedName& onkeydownAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[195];
const QualifiedName& onkeypressAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[196];
const QualifiedName& onkeyupAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[197];
const QualifiedName& onlanguagechangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[198];
const QualifiedName& onloadAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[199];
const QualifiedName& onloadeddataAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[200];
const QualifiedName& onloadedmetadataAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[201];
const QualifiedName& onloadstartAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[202];
const QualifiedName& onmessageAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[203];
const QualifiedName& onmousedownAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[204];
const QualifiedName& onmouseenterAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[205];
const QualifiedName& onmouseleaveAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[206];
const QualifiedName& onmousemoveAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[207];
const QualifiedName& onmouseoutAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[208];
const QualifiedName& onmouseoverAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[209];
const QualifiedName& onmouseupAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[210];
const QualifiedName& onmousewheelAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[211];
const QualifiedName& onofflineAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[212];
const QualifiedName& ononlineAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[213];
const QualifiedName& onorientationchangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[214];
const QualifiedName& onpagehideAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[215];
const QualifiedName& onpageshowAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[216];
const QualifiedName& onpasteAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[217];
const QualifiedName& onpauseAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[218];
const QualifiedName& onplayAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[219];
const QualifiedName& onplayingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[220];
const QualifiedName& onpopstateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[221];
const QualifiedName& onprogressAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[222];
const QualifiedName& onratechangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[223];
const QualifiedName& onresetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[224];
const QualifiedName& onresizeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[225];
const QualifiedName& onscrollAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[226];
const QualifiedName& onsearchAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[227];
const QualifiedName& onseekedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[228];
const QualifiedName& onseekingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[229];
const QualifiedName& onselectAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[230];
const QualifiedName& onselectionchangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[231];
const QualifiedName& onselectstartAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[232];
const QualifiedName& onshowAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[233];
const QualifiedName& onstalledAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[234];
const QualifiedName& onstorageAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[235];
const QualifiedName& onsubmitAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[236];
const QualifiedName& onsuspendAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[237];
const QualifiedName& ontimeupdateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[238];
const QualifiedName& ontoggleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[239];
const QualifiedName& ontouchcancelAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[240];
const QualifiedName& ontouchendAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[241];
const QualifiedName& ontouchmoveAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[242];
const QualifiedName& ontouchstartAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[243];
const QualifiedName& ontransitionendAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[244];
const QualifiedName& onunloadAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[245];
const QualifiedName& onvolumechangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[246];
const QualifiedName& onwaitingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[247];
const QualifiedName& onwebkitanimationendAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[248];
const QualifiedName& onwebkitanimationiterationAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[249];
const QualifiedName& onwebkitanimationstartAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[250];
const QualifiedName& onwebkitfullscreenchangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[251];
const QualifiedName& onwebkitfullscreenerrorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[252];
const QualifiedName& onwebkittransitionendAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[253];
const QualifiedName& onwheelAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[254];
const QualifiedName& openAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[255];
const QualifiedName& optimumAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[256];
const QualifiedName& patternAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[257];
const QualifiedName& pingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[258];
const QualifiedName& placeholderAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[259];
const QualifiedName& posterAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[260];
const QualifiedName& preloadAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[261];
const QualifiedName& pseudoAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[262];
const QualifiedName& radiogroupAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[263];
const QualifiedName& readonlyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[264];
const QualifiedName& relAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[265];
const QualifiedName& requiredAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[266];
const QualifiedName& resultsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[267];
const QualifiedName& revAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[268];
const QualifiedName& reversedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[269];
const QualifiedName& roleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[270];
const QualifiedName& rowsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[271];
const QualifiedName& rowspanAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[272];
const QualifiedName& rulesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[273];
const QualifiedName& sandboxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[274];
const QualifiedName& schemeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[275];
const QualifiedName& scopeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[276];
const QualifiedName& scrollamountAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[277];
const QualifiedName& scrolldelayAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[278];
const QualifiedName& scrollingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[279];
const QualifiedName& selectAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[280];
const QualifiedName& selectedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[281];
const QualifiedName& shapeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[282];
const QualifiedName& sizeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[283];
const QualifiedName& sizesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[284];
const QualifiedName& spanAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[285];
const QualifiedName& spellcheckAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[286];
const QualifiedName& srcAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[287];
const QualifiedName& srcdocAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[288];
const QualifiedName& srclangAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[289];
const QualifiedName& srcsetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[290];
const QualifiedName& standbyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[291];
const QualifiedName& startAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[292];
const QualifiedName& stepAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[293];
const QualifiedName& styleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[294];
const QualifiedName& summaryAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[295];
const QualifiedName& tabindexAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[296];
const QualifiedName& targetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[297];
const QualifiedName& textAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[298];
const QualifiedName& titleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[299];
const QualifiedName& topmarginAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[300];
const QualifiedName& translateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[301];
const QualifiedName& truespeedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[302];
const QualifiedName& typeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[303];
const QualifiedName& usemapAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[304];
const QualifiedName& valignAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[305];
const QualifiedName& valueAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[306];
const QualifiedName& valuetypeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[307];
const QualifiedName& versionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[308];
const QualifiedName& vlinkAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[309];
const QualifiedName& vspaceAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[310];
const QualifiedName& webkitdirectoryAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[311];
const QualifiedName& webkitdropzoneAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[312];
const QualifiedName& widthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[313];
const QualifiedName& wrapAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[314];



void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
        unsigned char isTag;
        unsigned char isAttr;
    };

    // Use placement new to initialize the globals.
    AtomicString xhtmlNS("http://www.w3.org/1999/xhtml", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&xhtmlNamespaceURI) AtomicString(xhtmlNS);
    static const NameEntry kNames[] = {
        { "a", 9778235, 1, 1, 0 },
        { "abbr", 650525, 4, 1, 1 },
        { "accept", 4839857, 6, 0, 1 },
        { "accept-charset", 5192676, 14, 0, 1 },
        { "accesskey", 15931860, 9, 0, 1 },
        { "acronym", 5486176, 7, 1, 0 },
        { "action", 14878034, 6, 0, 1 },
        { "address", 10008206, 7, 1, 0 },
        { "align", 10094397, 5, 0, 1 },
        { "alink", 2408650, 5, 0, 1 },
        { "allowfullscreen", 16375700, 15, 0, 1 },
        { "alt", 15335609, 3, 0, 1 },
        { "applet", 7823075, 6, 1, 0 },
        { "archive", 13839011, 7, 0, 1 },
        { "area", 7355486, 4, 1, 0 },
        { "aria-activedescendant", 13338511, 21, 0, 1 },
        { "aria-atomic", 12819054, 11, 0, 1 },
        { "aria-autocomplete", 1968559, 17, 0, 1 },
        { "aria-busy", 75863, 9, 0, 1 },
        { "aria-checked", 1517039, 12, 0, 1 },
        { "aria-controls", 5490937, 13, 0, 1 },
        { "aria-describedby", 5549123, 16, 0, 1 },
        { "aria-disabled", 1389455, 13, 0, 1 },
        { "aria-dropeffect", 6009141, 15, 0, 1 },
        { "aria-expanded", 6946219, 13, 0, 1 },
        { "aria-flowto", 12186959, 11, 0, 1 },
        { "aria-grabbed", 5310651, 12, 0, 1 },
        { "aria-haspopup", 7196359, 13, 0, 1 },
        { "aria-help", 12225580, 9, 0, 1 },
        { "aria-hidden", 7981896, 11, 0, 1 },
        { "aria-invalid", 3505375, 12, 0, 1 },
        { "aria-label", 561329, 10, 0, 1 },
        { "aria-labeledby", 5436988, 14, 0, 1 },
        { "aria-labelledby", 5412329, 15, 0, 1 },
        { "aria-level", 8886396, 10, 0, 1 },
        { "aria-live", 2064917, 9, 0, 1 },
        { "aria-multiline", 13431022, 14, 0, 1 },
        { "aria-multiselectable", 14347206, 20, 0, 1 },
        { "aria-orientation", 11685540, 16, 0, 1 },
        { "aria-owns", 15671106, 9, 0, 1 },
        { "aria-posinset", 14818088, 13, 0, 1 },
        { "aria-pressed", 8024627, 12, 0, 1 },
        { "aria-readonly", 1588657, 13, 0, 1 },
        { "aria-relevant", 16457375, 13, 0, 1 },
        { "aria-required", 10656135, 13, 0, 1 },
        { "aria-selected", 16608989, 13, 0, 1 },
        { "aria-setsize", 13328899, 12, 0, 1 },
        { "aria-sort", 15568337, 9, 0, 1 },
        { "aria-valuemax", 15306660, 13, 0, 1 },
        { "aria-valuemin", 2814091, 13, 0, 1 },
        { "aria-valuenow", 10189949, 13, 0, 1 },
        { "aria-valuetext", 14438832, 14, 0, 1 },
        { "article", 2968800, 7, 1, 0 },
        { "as", 8588719, 2, 0, 1 },
        { "aside", 10128566, 5, 1, 0 },
        { "async", 2556481, 5, 0, 1 },
        { "audio", 2129307, 5, 1, 0 },
        { "autocapitalize", 15613526, 14, 0, 1 },
        { "autocomplete", 14667434, 12, 0, 1 },
        { "autofocus", 16282013, 9, 0, 1 },
        { "autoplay", 12002525, 8, 0, 1 },
        { "axis", 14347904, 4, 0, 1 },
        { "b", 7170995, 1, 1, 0 },
        { "background", 8107606, 10, 0, 1 },
        { "base", 4376626, 4, 1, 0 },
        { "basefont", 5439393, 8, 1, 0 },
        { "bdi", 14809812, 3, 1, 0 },
        { "bdo", 9906743, 3, 1, 0 },
        { "behavior", 11804611, 8, 0, 1 },
        { "bgcolor", 11074228, 7, 0, 1 },
        { "bgsound", 9424179, 7, 1, 0 },
        { "big", 2330122, 3, 1, 0 },
        { "blockquote", 6047050, 10, 1, 0 },
        { "body", 4862692, 4, 1, 0 },
        { "border", 4389816, 6, 0, 1 },
        { "bordercolor", 12757423, 11, 0, 1 },
        { "br", 5574895, 2, 1, 0 },
        { "button", 679691, 6, 1, 0 },
        { "canvas", 10973550, 6, 1, 0 },
        { "caption", 2437650, 7, 1, 0 },
        { "capture", 9991654, 7, 0, 1 },
        { "cellpadding", 3763331, 11, 0, 1 },
        { "cellspacing", 7849745, 11, 0, 1 },
        { "center", 909285, 6, 1, 0 },
        { "challenge", 12755365, 9, 0, 1 },
        { "char", 12296599, 4, 0, 1 },
        { "charoff", 5577617, 7, 0, 1 },
        { "charset", 14273970, 7, 0, 1 },
        { "checked", 10213523, 7, 0, 1 },
        { "cite", 3704129, 4, 1, 1 },
        { "class", 1023741, 5, 0, 1 },
        { "classid", 1789995, 7, 0, 1 },
        { "clear", 12832795, 5, 0, 1 },
        { "code", 11588001, 4, 1, 1 },
        { "codebase", 3375722, 8, 0, 1 },
        { "codetype", 7482920, 8, 0, 1 },
        { "col", 12850806, 3, 1, 0 },
        { "colgroup", 3733719, 8, 1, 0 },
        { "color", 2734929, 5, 0, 1 },
        { "cols", 4921975, 4, 0, 1 },
        { "colspan", 9715319, 7, 0, 1 },
        { "command", 2099501, 7, 1, 0 },
        { "compact", 369367, 7, 0, 1 },
        { "content", 6894765, 7, 1, 1 },
        { "contenteditable", 1577657, 15, 0, 1 },
        { "contextmenu", 14578063, 11, 0, 1 },
        { "controls", 3696819, 8, 0, 1 },
        { "coords", 14288829, 6, 0, 1 },
        { "crossorigin", 15142862, 11, 0, 1 },
        { "data", 5273875, 4, 0, 1 },
        { "datalist", 7058973, 8, 1, 0 },
        { "datetime", 12167257, 8, 0, 1 },
        { "dd", 16480447, 2, 1, 0 },
        { "declare", 13440114, 7, 0, 1 },
        { "default", 6674383, 7, 0, 1 },
        { "defer", 13832204, 5, 0, 1 },
        { "del", 2501459, 3, 1, 0 },
        { "details", 4653180, 7, 1, 0 },
        { "dfn", 5168569, 3, 1, 0 },
        { "dialog", 1232842, 6, 1, 0 },
        { "dir", 5122803, 3, 1, 1 },
        { "direction", 16092904, 9, 0, 1 },
        { "dirname", 9899348, 7, 0, 1 },
        { "disabled", 12237545, 8, 0, 1 },
        { "div", 9762591, 3, 1, 0 },
        { "dl", 5117854, 2, 1, 0 },
        { "download", 1050544, 8, 0, 1 },
        { "draggable", 9281278, 9, 0, 1 },
        { "dt", 8177126, 2, 1, 0 },
        { "em", 2724661, 2, 1, 0 },
        { "embed", 855371, 5, 1, 0 },
        { "enctype", 11388071, 7, 0, 1 },
        { "end", 1590106, 3, 0, 1 },
        { "event", 4205160, 5, 0, 1 },
        { "face", 12427611, 4, 0, 1 },
        { "fieldset", 37097, 8, 1, 0 },
        { "figcaption", 13388641, 10, 1, 0 },
        { "figure", 13776479, 6, 1, 0 },
        { "font", 4588695, 4, 1, 0 },
        { "footer", 13359933, 6, 1, 0 },
        { "for", 4145292, 3, 0, 1 },
        { "form", 10060301, 4, 1, 1 },
        { "formaction", 12667227, 10, 0, 1 },
        { "formenctype", 6334991, 11, 0, 1 },
        { "formmethod", 7653832, 10, 0, 1 },
        { "formnovalidate", 5722173, 14, 0, 1 },
        { "formtarget", 15594141, 10, 0, 1 },
        { "frame", 3408403, 5, 1, 1 },
        { "frameborder", 6035536, 11, 0, 1 },
        { "frameset", 11229107, 8, 1, 0 },
        { "h1", 3953979, 2, 1, 0 },
        { "h2", 9378819, 2, 1, 0 },
        { "h3", 1803164, 2, 1, 0 },
        { "h4", 15343268, 2, 1, 0 },
        { "h5", 8065295, 2, 1, 0 },
        { "h6", 9566891, 2, 1, 0 },
        { "head", 11457121, 4, 1, 0 },
        { "header", 5896178, 6, 1, 0 },
        { "headers", 2627673, 7, 0, 1 },
        { "height", 6697833, 6, 0, 1 },
        { "hgroup", 8927907, 6, 1, 0 },
        { "hidden", 12930326, 6, 0, 1 },
        { "high", 11748844, 4, 0, 1 },
        { "hr", 7182703, 2, 1, 0 },
        { "href", 5797448, 4, 0, 1 },
        { "hreflang", 12582042, 8, 0, 1 },
        { "hspace", 4182216, 6, 0, 1 },
        { "html", 7123128, 4, 1, 0 },
        { "http-equiv", 12415310, 10, 0, 1 },
        { "i", 16149244, 1, 1, 0 },
        { "icon", 12131262, 4, 0, 1 },
        { "id", 6145549, 2, 0, 1 },
        { "iframe", 16643553, 6, 1, 0 },
        { "image", 10287573, 5, 1, 0 },
        { "img", 11398162, 3, 1, 0 },
        { "incremental", 9277578, 11, 0, 1 },
        { "input", 10365436, 5, 1, 0 },
        { "inputmode", 2781876, 9, 0, 1 },
        { "ins", 939786, 3, 1, 0 },
        { "integrity", 8960270, 9, 0, 1 },
        { "is", 15617733, 2, 0, 1 },
        { "ismap", 6887213, 5, 0, 1 },
        { "kbd", 8554610, 3, 1, 0 },
        { "keygen", 5603105, 6, 1, 0 },
        { "keytype", 13164152, 7, 0, 1 },
        { "kind", 7022868, 4, 0, 1 },
        { "label", 15092473, 5, 1, 1 },
        { "lang", 3702417, 4, 0, 1 },
        { "language", 4109061, 8, 0, 1 },
        { "layer", 16475212, 5, 1, 0 },
        { "leftmargin", 10359506, 10, 0, 1 },
        { "legend", 16417202, 6, 1, 0 },
        { "li", 11097939, 2, 1, 0 },
        { "link", 7010491, 4, 1, 1 },
        { "list", 15078159, 4, 0, 1 },
        { "listing", 15674586, 7, 1, 0 },
        { "longdesc", 2363416, 8, 0, 1 },
        { "loop", 12741482, 4, 0, 1 },
        { "low", 9523344, 3, 0, 1 },
        { "lowsrc", 8943146, 6, 0, 1 },
        { "main", 13501767, 4, 1, 0 },
        { "manifest", 3853375, 8, 0, 1 },
        { "map", 16519384, 3, 1, 0 },
        { "marginheight", 14117911, 12, 0, 1 },
        { "marginwidth", 5458369, 11, 0, 1 },
        { "mark", 16508213, 4, 1, 0 },
        { "marquee", 1416619, 7, 1, 0 },
        { "max", 3449166, 3, 0, 1 },
        { "maxlength", 5985164, 9, 0, 1 },
        { "mayscript", 9718095, 9, 0, 1 },
        { "media", 13905581, 5, 0, 1 },
        { "mediagroup", 13605962, 10, 0, 1 },
        { "menu", 13428288, 4, 1, 0 },
        { "menuitem", 3687436, 8, 1, 0 },
        { "meta", 14067019, 4, 1, 0 },
        { "meter", 5815470, 5, 1, 0 },
        { "method", 14679268, 6, 0, 1 },
        { "min", 2196278, 3, 0, 1 },
        { "minlength", 4633736, 9, 0, 1 },
        { "multiple", 6185486, 8, 0, 1 },
        { "muted", 11680767, 5, 0, 1 },
        { "name", 8774809, 4, 0, 1 },
        { "nav", 15269619, 3, 1, 0 },
        { "nobr", 419763, 4, 1, 0 },
        { "noembed", 11476379, 7, 1, 0 },
        { "noframes", 13751064, 8, 1, 0 },
        { "nohref", 2672291, 6, 0, 1 },
        { "nolayer", 10430133, 7, 1, 0 },
        { "nonce", 12597381, 5, 0, 1 },
        { "noresize", 11098542, 8, 0, 1 },
        { "noscript", 10168152, 8, 1, 0 },
        { "noshade", 12172860, 7, 0, 1 },
        { "novalidate", 5146510, 10, 0, 1 },
        { "nowrap", 15804466, 6, 0, 1 },
        { "object", 6907086, 6, 1, 1 },
        { "ol", 6672076, 2, 1, 0 },
        { "onabort", 9017557, 7, 0, 1 },
        { "onanimationend", 15950082, 14, 0, 1 },
        { "onanimationiteration", 6837014, 20, 0, 1 },
        { "onanimationstart", 3893294, 16, 0, 1 },
        { "onautocomplete", 6917578, 14, 0, 1 },
        { "onautocompleteerror", 8480614, 19, 0, 1 },
        { "onbeforecopy", 10445229, 12, 0, 1 },
        { "onbeforecut", 7604779, 11, 0, 1 },
        { "onbeforepaste", 10932396, 13, 0, 1 },
        { "onbeforeunload", 10408067, 14, 0, 1 },
        { "onblur", 10264279, 6, 0, 1 },
        { "oncancel", 1433968, 8, 0, 1 },
        { "oncanplay", 11894364, 9, 0, 1 },
        { "oncanplaythrough", 9632525, 16, 0, 1 },
        { "onchange", 1588214, 8, 0, 1 },
        { "onclick", 668060, 7, 0, 1 },
        { "onclose", 76675, 7, 0, 1 },
        { "oncontextmenu", 10957436, 13, 0, 1 },
        { "oncopy", 3498124, 6, 0, 1 },
        { "oncuechange", 8858494, 11, 0, 1 },
        { "oncut", 530518, 5, 0, 1 },
        { "ondblclick", 9484479, 10, 0, 1 },
        { "ondrag", 7755023, 6, 0, 1 },
        { "ondragend", 12737756, 9, 0, 1 },
        { "ondragenter", 7625423, 11, 0, 1 },
        { "ondragleave", 2163828, 11, 0, 1 },
        { "ondragover", 7621126, 10, 0, 1 },
        { "ondragstart", 7563418, 11, 0, 1 },
        { "ondrop", 5167850, 6, 0, 1 },
        { "ondurationchange", 3875418, 16, 0, 1 },
        { "onemptied", 7216733, 9, 0, 1 },
        { "onended", 4168741, 7, 0, 1 },
        { "onerror", 2658286, 7, 0, 1 },
        { "onfocus", 11484753, 7, 0, 1 },
        { "onfocusin", 2970475, 9, 0, 1 },
        { "onfocusout", 4459422, 10, 0, 1 },
        { "onhashchange", 897163, 12, 0, 1 },
        { "oninput", 7945347, 7, 0, 1 },
        { "oninvalid", 15155464, 9, 0, 1 },
        { "onkeydown", 917683, 9, 0, 1 },
        { "onkeypress", 10628960, 10, 0, 1 },
        { "onkeyup", 7098679, 7, 0, 1 },
        { "onlanguagechange", 1694325, 16, 0, 1 },
        { "onload", 1245164, 6, 0, 1 },
        { "onloadeddata", 6667087, 12, 0, 1 },
        { "onloadedmetadata", 11435152, 16, 0, 1 },
        { "onloadstart", 11661007, 11, 0, 1 },
        { "onmessage", 15511252, 9, 0, 1 },
        { "onmousedown", 10902312, 11, 0, 1 },
        { "onmouseenter", 12011136, 12, 0, 1 },
        { "onmouseleave", 9750696, 12, 0, 1 },
        { "onmousemove", 13128910, 11, 0, 1 },
        { "onmouseout", 3672358, 10, 0, 1 },
        { "onmouseover", 5456314, 11, 0, 1 },
        { "onmouseup", 8932826, 9, 0, 1 },
        { "onmousewheel", 8230218, 12, 0, 1 },
        { "onoffline", 8780424, 9, 0, 1 },
        { "ononline", 6361718, 8, 0, 1 },
        { "onorientationchange", 4305565, 19, 0, 1 },
        { "onpagehide", 11684100, 10, 0, 1 },
        { "onpageshow", 2047001, 10, 0, 1 },
        { "onpaste", 8282984, 7, 0, 1 },
        { "onpause", 9216789, 7, 0, 1 },
        { "onplay", 4469333, 6, 0, 1 },
        { "onplaying", 6622660, 9, 0, 1 },
        { "onpopstate", 5473424, 10, 0, 1 },
        { "onprogress", 11307581, 10, 0, 1 },
        { "onratechange", 8724969, 12, 0, 1 },
        { "onreset", 871509, 7, 0, 1 },
        { "onresize", 6999701, 8, 0, 1 },
        { "onscroll", 11456051, 8, 0, 1 },
        { "onsearch", 13779639, 8, 0, 1 },
        { "onseeked", 15270585, 8, 0, 1 },
        { "onseeking", 1397795, 9, 0, 1 },
        { "onselect", 10837779, 8, 0, 1 },
        { "onselectionchange", 2738107, 17, 0, 1 },
        { "onselectstart", 4928924, 13, 0, 1 },
        { "onshow", 15330334, 6, 0, 1 },
        { "onstalled", 366130, 9, 0, 1 },
        { "onstorage", 3472742, 9, 0, 1 },
        { "onsubmit", 10895898, 8, 0, 1 },
        { "onsuspend", 3306534, 9, 0, 1 },
        { "ontimeupdate", 15260240, 12, 0, 1 },
        { "ontoggle", 14882770, 8, 0, 1 },
        { "ontouchcancel", 11977340, 13, 0, 1 },
        { "ontouchend", 14649050, 10, 0, 1 },
        { "ontouchmove", 3062696, 11, 0, 1 },
        { "ontouchstart", 15650339, 12, 0, 1 },
        { "ontransitionend", 14486006, 15, 0, 1 },
        { "onunload", 265474, 8, 0, 1 },
        { "onvolumechange", 1227173, 14, 0, 1 },
        { "onwaiting", 14930497, 9, 0, 1 },
        { "onwebkitanimationend", 4247475, 20, 0, 1 },
        { "onwebkitanimationiteration", 2350168, 26, 0, 1 },
        { "onwebkitanimationstart", 1438902, 22, 0, 1 },
        { "onwebkitfullscreenchange", 4583393, 24, 0, 1 },
        { "onwebkitfullscreenerror", 16488541, 23, 0, 1 },
        { "onwebkittransitionend", 3568476, 21, 0, 1 },
        { "onwheel", 3280514, 7, 0, 1 },
        { "open", 13703631, 4, 0, 1 },
        { "optgroup", 10180907, 8, 1, 0 },
        { "optimum", 3350287, 7, 0, 1 },
        { "option", 2998548, 6, 1, 0 },
        { "output", 15319621, 6, 1, 0 },
        { "p", 587733, 1, 1, 0 },
        { "param", 15210019, 5, 1, 0 },
        { "pattern", 16283150, 7, 0, 1 },
        { "picture", 7136372, 7, 1, 0 },
        { "ping", 9193756, 4, 0, 1 },
        { "placeholder", 6132349, 11, 0, 1 },
        { "plaintext", 2551274, 9, 1, 0 },
        { "poster", 2570443, 6, 0, 1 },
        { "pre", 16061734, 3, 1, 0 },
        { "preload", 11887554, 7, 0, 1 },
        { "progress", 11461517, 8, 1, 0 },
        { "pseudo", 6923620, 6, 0, 1 },
        { "q", 12607837, 1, 1, 0 },
        { "radiogroup", 3065371, 10, 0, 1 },
        { "rb", 4749743, 2, 1, 0 },
        { "readonly", 4471832, 8, 0, 1 },
        { "rel", 8963158, 3, 0, 1 },
        { "required", 4894573, 8, 0, 1 },
        { "results", 972036, 7, 0, 1 },
        { "rev", 12217053, 3, 0, 1 },
        { "reversed", 15723650, 8, 0, 1 },
        { "role", 16084934, 4, 0, 1 },
        { "rows", 11770835, 4, 0, 1 },
        { "rowspan", 13630419, 7, 0, 1 },
        { "rp", 4502666, 2, 1, 0 },
        { "rt", 8494892, 2, 1, 0 },
        { "rtc", 4350543, 3, 1, 0 },
        { "ruby", 12793227, 4, 1, 0 },
        { "rules", 8604373, 5, 0, 1 },
        { "s", 11249530, 1, 1, 0 },
        { "samp", 3617274, 4, 1, 0 },
        { "sandbox", 3448190, 7, 0, 1 },
        { "scheme", 3952063, 6, 0, 1 },
        { "scope", 5624816, 5, 0, 1 },
        { "script", 7137273, 6, 1, 0 },
        { "scrollamount", 8309213, 12, 0, 1 },
        { "scrolldelay", 15755012, 11, 0, 1 },
        { "scrolling", 4321742, 9, 0, 1 },
        { "section", 8056235, 7, 1, 0 },
        { "select", 210571, 6, 1, 1 },
        { "selected", 15762609, 8, 0, 1 },
        { "shadow", 1972219, 6, 1, 0 },
        { "shape", 15210140, 5, 0, 1 },
        { "size", 12715024, 4, 0, 1 },
        { "sizes", 6795010, 5, 0, 1 },
        { "small", 14362904, 5, 1, 0 },
        { "source", 341674, 6, 1, 0 },
        { "span", 11168892, 4, 1, 1 },
        { "spellcheck", 4108424, 10, 0, 1 },
        { "src", 11517827, 3, 0, 1 },
        { "srcdoc", 15297056, 6, 0, 1 },
        { "srclang", 13402710, 7, 0, 1 },
        { "srcset", 6236095, 6, 0, 1 },
        { "standby", 8394231, 7, 0, 1 },
        { "start", 1021290, 5, 0, 1 },
        { "step", 5337026, 4, 0, 1 },
        { "strike", 15072495, 6, 1, 0 },
        { "strong", 13282129, 6, 1, 0 },
        { "style", 10993676, 5, 1, 1 },
        { "sub", 15782866, 3, 1, 0 },
        { "summary", 1148260, 7, 1, 1 },
        { "sup", 8741418, 3, 1, 0 },
        { "tabindex", 10415591, 8, 0, 1 },
        { "table", 8181317, 5, 1, 0 },
        { "target", 1752822, 6, 0, 1 },
        { "tbody", 14028375, 5, 1, 0 },
        { "td", 10309076, 2, 1, 0 },
        { "template", 12693000, 8, 1, 0 },
        { "text", 2784654, 4, 0, 1 },
        { "textarea", 10510497, 8, 1, 0 },
        { "tfoot", 13009441, 5, 1, 0 },
        { "th", 3915297, 2, 1, 0 },
        { "thead", 13349519, 5, 1, 0 },
        { "title", 2337488, 5, 1, 1 },
        { "topmargin", 10843382, 9, 0, 1 },
        { "tr", 16624306, 2, 1, 0 },
        { "track", 10988706, 5, 1, 0 },
        { "translate", 9982484, 9, 0, 1 },
        { "truespeed", 11732619, 9, 0, 1 },
        { "tt", 9170852, 2, 1, 0 },
        { "type", 1916283, 4, 0, 1 },
        { "u", 13317144, 1, 1, 0 },
        { "ul", 12324505, 2, 1, 0 },
        { "usemap", 16643238, 6, 0, 1 },
        { "valign", 8345445, 6, 0, 1 },
        { "value", 10871467, 5, 0, 1 },
        { "valuetype", 4808367, 9, 0, 1 },
        { "var", 9573901, 3, 1, 0 },
        { "version", 8128447, 7, 0, 1 },
        { "video", 16765639, 5, 1, 0 },
        { "vlink", 12743618, 5, 0, 1 },
        { "vspace", 9862276, 6, 0, 1 },
        { "wbr", 8290101, 3, 1, 0 },
        { "webkitdirectory", 1344740, 15, 0, 1 },
        { "webkitdropzone", 5315147, 14, 0, 1 },
        { "width", 12902275, 5, 0, 1 },
        { "wrap", 7601899, 4, 0, 1 },
        { "xmp", 8764488, 3, 1, 0 },
    };

    size_t tag_i = 0;
    size_t attr_i = 0;
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        if (kNames[i].isTag) {
            void* address = reinterpret_cast<HTMLQualifiedName*>(&TagStorage) + tag_i;
            QualifiedName::createStatic(address, stringImpl, xhtmlNS);
            tag_i++;
        }

        if (!kNames[i].isAttr)
            continue;
        void* address = reinterpret_cast<QualifiedName*>(&AttrStorage) + attr_i;
        QualifiedName::createStatic(address, stringImpl);
        attr_i++;
    }
    ASSERT(tag_i == HTMLTagsCount);
    ASSERT(attr_i == HTMLAttrsCount);
}

} // HTML
} // namespace blink
