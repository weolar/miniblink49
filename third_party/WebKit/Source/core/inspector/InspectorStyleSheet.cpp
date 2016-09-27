/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/inspector/InspectorStyleSheet.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptRegexp.h"
#include "core/CSSPropertyNames.h"
#include "core/css/CSSImportRule.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/css/CSSMediaRule.h"
#include "core/css/CSSRuleList.h"
#include "core/css/CSSStyleRule.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/CSSSupportsRule.h"
#include "core/css/StylePropertySet.h"
#include "core/css/StyleRule.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/parser/CSSParser.h"
#include "core/css/parser/CSSParserObserver.h"
#include "core/dom/DOMNodeIds.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/html/HTMLStyleElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/inspector/ContentSearchUtils.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InspectorCSSAgent.h"
#include "core/inspector/InspectorResourceAgent.h"
#include "core/svg/SVGStyleElement.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/TextPosition.h"

using blink::TypeBuilder::Array;
using blink::RuleSourceDataList;
using blink::CSSRuleSourceData;
using blink::CSSStyleSheet;

namespace {

using namespace blink;

static CSSParserContext parserContextForDocument(Document *document)
{
    return document ? CSSParserContext(*document, 0) : strictCSSParserContext();
}

class StyleSheetHandler final : public CSSParserObserver {
public:
    StyleSheetHandler(const String& parsedText, Document* document, RuleSourceDataList* result)
        : m_parsedText(parsedText)
        , m_document(document)
        , m_result(result)
        , m_mediaQueryExpValueRangeStart(UINT_MAX)
    {
        ASSERT(m_result);
    }

private:
    virtual void startRuleHeader(StyleRule::Type, unsigned) override;
    virtual void endRuleHeader(unsigned) override;
    virtual void observeSelector(unsigned startOffset, unsigned endOffset) override;
    virtual void startRuleBody(unsigned) override;
    virtual void endRuleBody(unsigned) override;
    virtual void observeProperty(unsigned startOffset, unsigned endOffset, bool isImportant, bool isParsed) override;
    virtual void observeComment(unsigned startOffset, unsigned endOffset) override;
    virtual void startMediaQueryExp(unsigned offset) override;
    virtual void endMediaQueryExp(unsigned offset) override;
    virtual void startMediaQuery() override;
    virtual void endMediaQuery() override;

    void addNewRuleToSourceTree(PassRefPtrWillBeRawPtr<CSSRuleSourceData>);
    PassRefPtrWillBeRawPtr<CSSRuleSourceData> popRuleData();
    template <typename CharacterType> inline void setRuleHeaderEnd(const CharacterType*, unsigned);
    void fixUnparsedPropertyRanges(CSSRuleSourceData*);

    const String& m_parsedText;
    RawPtrWillBeMember<Document> m_document;
    RuleSourceDataList* m_result;
    RuleSourceDataList m_currentRuleDataStack;
    RefPtrWillBeMember<CSSRuleSourceData> m_currentRuleData;
    RefPtrWillBeMember<CSSMediaQuerySourceData> m_currentMediaQueryData;
    unsigned m_mediaQueryExpValueRangeStart;
};

void StyleSheetHandler::startRuleHeader(StyleRule::Type type, unsigned offset)
{
    // Pop off data for a previous invalid rule.
    if (m_currentRuleData)
        m_currentRuleDataStack.removeLast();

    RefPtrWillBeRawPtr<CSSRuleSourceData> data = CSSRuleSourceData::create(type);
    data->ruleHeaderRange.start = offset;
    m_currentRuleData = data;
    m_currentRuleDataStack.append(data.release());
}

template <typename CharacterType>
inline void StyleSheetHandler::setRuleHeaderEnd(const CharacterType* dataStart, unsigned listEndOffset)
{
    while (listEndOffset > 1) {
        if (isHTMLSpace<CharacterType>(*(dataStart + listEndOffset - 1)))
            --listEndOffset;
        else
            break;
    }

    m_currentRuleDataStack.last()->ruleHeaderRange.end = listEndOffset;
    if (!m_currentRuleDataStack.last()->selectorRanges.isEmpty())
        m_currentRuleDataStack.last()->selectorRanges.last().end = listEndOffset;
}

void StyleSheetHandler::endRuleHeader(unsigned offset)
{
    ASSERT(!m_currentRuleDataStack.isEmpty());

    if (m_parsedText.is8Bit())
        setRuleHeaderEnd<LChar>(m_parsedText.characters8(), offset);
    else
        setRuleHeaderEnd<UChar>(m_parsedText.characters16(), offset);
}

void StyleSheetHandler::observeSelector(unsigned startOffset, unsigned endOffset)
{
    ASSERT(m_currentRuleDataStack.size());
    m_currentRuleDataStack.last()->selectorRanges.append(SourceRange(startOffset, endOffset));
}

void StyleSheetHandler::startRuleBody(unsigned offset)
{
    m_currentRuleData.clear();
    ASSERT(!m_currentRuleDataStack.isEmpty());
    if (m_parsedText[offset] == '{')
        ++offset; // Skip the rule body opening brace.
    m_currentRuleDataStack.last()->ruleBodyRange.start = offset;
}

void StyleSheetHandler::endRuleBody(unsigned offset)
{
    ASSERT(!m_currentRuleDataStack.isEmpty());
    m_currentRuleDataStack.last()->ruleBodyRange.end = offset;
    RefPtrWillBeRawPtr<CSSRuleSourceData> rule = popRuleData();

    fixUnparsedPropertyRanges(rule.get());
    addNewRuleToSourceTree(rule.release());
}

void StyleSheetHandler::addNewRuleToSourceTree(PassRefPtrWillBeRawPtr<CSSRuleSourceData> rule)
{
    if (m_currentRuleDataStack.isEmpty())
        m_result->append(rule);
    else
        m_currentRuleDataStack.last()->childRules.append(rule);
}

PassRefPtrWillBeRawPtr<CSSRuleSourceData> StyleSheetHandler::popRuleData()
{
    ASSERT(!m_currentRuleDataStack.isEmpty());
    m_currentRuleData.clear();
    RefPtrWillBeRawPtr<CSSRuleSourceData> data = m_currentRuleDataStack.last();
    m_currentRuleDataStack.removeLast();
    return data.release();
}

template <typename CharacterType>
static inline void fixUnparsedProperties(const CharacterType* characters, CSSRuleSourceData* ruleData)
{
    WillBeHeapVector<CSSPropertySourceData>& propertyData = ruleData->styleSourceData->propertyData;
    unsigned size = propertyData.size();
    if (!size)
        return;

    CSSPropertySourceData* nextData = &(propertyData.at(0));
    for (unsigned i = 0; i < size; ++i) {
        CSSPropertySourceData* currentData = nextData;
        nextData = i < size - 1 ? &(propertyData.at(i + 1)) : nullptr;

        if (currentData->parsedOk)
            continue;
        if (currentData->range.end > 0 && characters[currentData->range.end - 1] == ';')
            continue;

        unsigned propertyEnd;
        if (!nextData)
            propertyEnd = ruleData->ruleBodyRange.end - 1;
        else
            propertyEnd = nextData->range.start - 1;

        while (isHTMLSpace<CharacterType>(characters[propertyEnd]))
            --propertyEnd;

        // propertyEnd points at the last property text character.
        unsigned newPropertyEnd = propertyEnd + 1; // Exclusive of the last property text character.
        if (currentData->range.end != newPropertyEnd) {
            currentData->range.end = newPropertyEnd;
            unsigned valueStart = currentData->range.start + currentData->name.length();
            while (valueStart < propertyEnd && characters[valueStart] != ':')
                ++valueStart;
            if (valueStart < propertyEnd)
                ++valueStart; // Shift past the ':'.
            while (valueStart < propertyEnd && isHTMLSpace<CharacterType>(characters[valueStart]))
                ++valueStart;
            // Need to exclude the trailing ';' from the property value.
            currentData->value = String(characters + valueStart, propertyEnd - valueStart + (characters[propertyEnd] == ';' ? 0 : 1));
        }
    }
}

void StyleSheetHandler::fixUnparsedPropertyRanges(CSSRuleSourceData* ruleData)
{
    if (!ruleData->styleSourceData)
        return;

    if (m_parsedText.is8Bit()) {
        fixUnparsedProperties<LChar>(m_parsedText.characters8(), ruleData);
        return;
    }

    fixUnparsedProperties<UChar>(m_parsedText.characters16(), ruleData);
}

void StyleSheetHandler::observeProperty(unsigned startOffset, unsigned endOffset, bool isImportant, bool isParsed)
{
    if (m_currentRuleDataStack.isEmpty() || !m_currentRuleDataStack.last()->styleSourceData)
        return;

    ASSERT(endOffset <= m_parsedText.length());
    if (endOffset < m_parsedText.length() && m_parsedText[endOffset] == ';') // Include semicolon into the property text.
        ++endOffset;

    ASSERT(startOffset < endOffset);
    String propertyString = m_parsedText.substring(startOffset, endOffset - startOffset).stripWhiteSpace();
    if (propertyString.endsWith(';'))
        propertyString = propertyString.left(propertyString.length() - 1);
    size_t colonIndex = propertyString.find(':');
    ASSERT(colonIndex != kNotFound);

    String name = propertyString.left(colonIndex).stripWhiteSpace();
    String value = propertyString.substring(colonIndex + 1, propertyString.length()).stripWhiteSpace();
    m_currentRuleDataStack.last()->styleSourceData->propertyData.append(
        CSSPropertySourceData(name, value, isImportant, false, isParsed, SourceRange(startOffset, endOffset)));
}

void StyleSheetHandler::observeComment(unsigned startOffset, unsigned endOffset)
{
    ASSERT(endOffset <= m_parsedText.length());

    if (m_currentRuleDataStack.isEmpty() || !m_currentRuleDataStack.last()->ruleHeaderRange.end || !m_currentRuleDataStack.last()->styleSourceData)
        return;

    // The lexer is not inside a property AND it is scanning a declaration-aware rule body.
    String commentText = m_parsedText.substring(startOffset, endOffset - startOffset);

    ASSERT(commentText.startsWith("/*"));
    commentText = commentText.substring(2);

    // Require well-formed comments.
    if (!commentText.endsWith("*/"))
        return;
    commentText = commentText.substring(0, commentText.length() - 2).stripWhiteSpace();
    if (commentText.isEmpty())
        return;

    // FIXME: Use the actual rule type rather than STYLE_RULE?
    RuleSourceDataList sourceData;

    StyleSheetHandler handler(commentText, m_document, &sourceData);
    CSSParser::parseDeclarationListForInspector(parserContextForDocument(m_document), commentText, handler);
    WillBeHeapVector<CSSPropertySourceData>& commentPropertyData = sourceData.first()->styleSourceData->propertyData;
    if (commentPropertyData.size() != 1)
        return;
    CSSPropertySourceData& propertyData = commentPropertyData.at(0);
    if (propertyData.range.length() != commentText.length())
        return;

    m_currentRuleDataStack.last()->styleSourceData->propertyData.append(
        CSSPropertySourceData(propertyData.name, propertyData.value, false, true, true, SourceRange(startOffset, endOffset)));
}

void StyleSheetHandler::startMediaQueryExp(unsigned offset)
{
    ASSERT(m_currentMediaQueryData);
    m_mediaQueryExpValueRangeStart = offset;
}

void StyleSheetHandler::endMediaQueryExp(unsigned offset)
{
    ASSERT(m_currentMediaQueryData);
    ASSERT(offset >= m_mediaQueryExpValueRangeStart);
    ASSERT(offset <= m_parsedText.length());
    while (offset > m_mediaQueryExpValueRangeStart && isSpaceOrNewline(m_parsedText[offset - 1]))
        --offset;
    while (offset > m_mediaQueryExpValueRangeStart && isSpaceOrNewline(m_parsedText[m_mediaQueryExpValueRangeStart]))
        ++m_mediaQueryExpValueRangeStart;
    m_currentMediaQueryData->expData.append(CSSMediaQueryExpSourceData(SourceRange(m_mediaQueryExpValueRangeStart, offset)));
}

void StyleSheetHandler::startMediaQuery()
{
    ASSERT(m_currentRuleDataStack.size() && m_currentRuleDataStack.last()->mediaSourceData);
    RefPtrWillBeRawPtr<CSSMediaQuerySourceData> data = CSSMediaQuerySourceData::create();
    m_currentMediaQueryData = data;
    m_currentRuleDataStack.last()->mediaSourceData->queryData.append(data);
}

void StyleSheetHandler::endMediaQuery()
{
    m_currentMediaQueryData.clear();
}

bool verifyRuleText(Document* document, const String& ruleText)
{
    DEFINE_STATIC_LOCAL(String, bogusPropertyName, ("-webkit-boguz-propertee"));
    RefPtrWillBeRawPtr<StyleSheetContents> styleSheet = StyleSheetContents::create(strictCSSParserContext());
    RuleSourceDataList sourceData;
    String text = ruleText + " div { " + bogusPropertyName + ": none; }";
    StyleSheetHandler handler(text, document, &sourceData);
    CSSParser::parseSheetForInspector(parserContextForDocument(document), styleSheet.get(), text, handler);
    unsigned ruleCount = sourceData.size();

    // Exactly two rules should be parsed.
    if (ruleCount != 2)
        return false;

    // Added rule must be style rule.
    if (!sourceData.at(0)->styleSourceData)
        return false;

    WillBeHeapVector<CSSPropertySourceData>& propertyData = sourceData.at(1)->styleSourceData->propertyData;
    unsigned propertyCount = propertyData.size();

    // Exactly one property should be in rule.
    if (propertyCount != 1)
        return false;

    // Check for the property name.
    if (propertyData.at(0).name != bogusPropertyName)
        return false;

    return true;
}

bool verifyStyleText(Document* document, const String& text)
{
    return verifyRuleText(document, "div {" + text + "}");
}

bool verifySelectorText(Document* document, const String& selectorText)
{
    DEFINE_STATIC_LOCAL(String, bogusPropertyName, ("-webkit-boguz-propertee"));
    RefPtrWillBeRawPtr<StyleSheetContents> styleSheet = StyleSheetContents::create(strictCSSParserContext());
    RuleSourceDataList sourceData;
    String text = selectorText + " { " + bogusPropertyName + ": none; }";
    StyleSheetHandler handler(text, document, &sourceData);
    CSSParser::parseSheetForInspector(parserContextForDocument(document), styleSheet.get(), text, handler);

    // Exactly one rule should be parsed.
    unsigned ruleCount = sourceData.size();
    if (ruleCount != 1 || sourceData.at(0)->type != StyleRule::Style)
        return false;

    // Exactly one property should be in style rule.
    WillBeHeapVector<CSSPropertySourceData>& propertyData = sourceData.at(0)->styleSourceData->propertyData;
    unsigned propertyCount = propertyData.size();
    if (propertyCount != 1)
        return false;

    // Check for the property name.
    if (propertyData.at(0).name != bogusPropertyName)
        return false;

    return true;
}

bool verifyMediaText(Document* document, const String& mediaText)
{
    DEFINE_STATIC_LOCAL(String, bogusPropertyName, ("-webkit-boguz-propertee"));
    RefPtrWillBeRawPtr<StyleSheetContents> styleSheet = StyleSheetContents::create(strictCSSParserContext());
    RuleSourceDataList sourceData;
    String text = "@media " + mediaText + " { div { " + bogusPropertyName + ": none; } }";
    StyleSheetHandler handler(text, document, &sourceData);
    CSSParser::parseSheetForInspector(parserContextForDocument(document), styleSheet.get(), text, handler);

    // Exactly one media rule should be parsed.
    unsigned ruleCount = sourceData.size();
    if (ruleCount != 1 || sourceData.at(0)->type != StyleRule::Media)
        return false;

    // Media rule should have exactly one style rule child.
    RuleSourceDataList& childSourceData = sourceData.at(0)->childRules;
    ruleCount = childSourceData.size();
    if (ruleCount != 1 || !childSourceData.at(0)->styleSourceData)
        return false;

    // Exactly one property should be in style rule.
    WillBeHeapVector<CSSPropertySourceData>& propertyData = childSourceData.at(0)->styleSourceData->propertyData;
    unsigned propertyCount = propertyData.size();
    if (propertyCount != 1)
        return false;

    // Check for the property name.
    if (propertyData.at(0).name != bogusPropertyName)
        return false;

    return true;
}

void flattenSourceData(RuleSourceDataList* dataList, RuleSourceDataList* result)
{
    for (size_t i = 0; i < dataList->size(); ++i) {
        RefPtrWillBeMember<CSSRuleSourceData>& data = dataList->at(i);

        // The result->append()'ed types should be exactly the same as in collectFlatRules().
        switch (data->type) {
        case StyleRule::Style:
        case StyleRule::Import:
        case StyleRule::Page:
        case StyleRule::FontFace:
        case StyleRule::Viewport:
        case StyleRule::Keyframes:
            result->append(data);
            break;
        case StyleRule::Media:
        case StyleRule::Supports:
            result->append(data);
            flattenSourceData(&data->childRules, result);
            break;
        default:
            break;
        }
    }
}

PassRefPtrWillBeRawPtr<CSSRuleList> asCSSRuleList(CSSRule* rule)
{
    if (!rule)
        return nullptr;

    if (rule->type() == CSSRule::MEDIA_RULE)
        return toCSSMediaRule(rule)->cssRules();

    if (rule->type() == CSSRule::SUPPORTS_RULE)
        return toCSSSupportsRule(rule)->cssRules();

    return nullptr;
}

template <typename RuleList>
void collectFlatRules(RuleList ruleList, CSSRuleVector* result)
{
    if (!ruleList)
        return;

    for (unsigned i = 0, size = ruleList->length(); i < size; ++i) {
        CSSRule* rule = ruleList->item(i);

        // The result->append()'ed types should be exactly the same as in flattenSourceData().
        switch (rule->type()) {
        case CSSRule::STYLE_RULE:
        case CSSRule::IMPORT_RULE:
        case CSSRule::CHARSET_RULE:
        case CSSRule::PAGE_RULE:
        case CSSRule::FONT_FACE_RULE:
        case CSSRule::VIEWPORT_RULE:
        case CSSRule::KEYFRAMES_RULE:
            result->append(rule);
            break;
        case CSSRule::MEDIA_RULE:
        case CSSRule::SUPPORTS_RULE:
            result->append(rule);
            collectFlatRules(asCSSRuleList(rule), result);
            break;
        default:
            break;
        }
    }
}

typedef HashMap<unsigned, unsigned, WTF::IntHash<unsigned>, WTF::UnsignedWithZeroKeyHashTraits<unsigned>> IndexMap;

void diff(const Vector<String>& listA, const Vector<String>& listB, IndexMap* aToB, IndexMap* bToA)
{
    // Cut of common prefix.
    size_t startOffset = 0;
    while (startOffset < listA.size() && startOffset < listB.size()) {
        if (listA.at(startOffset) != listB.at(startOffset))
            break;
        aToB->set(startOffset, startOffset);
        bToA->set(startOffset, startOffset);
        ++startOffset;
    }

    // Cut of common suffix.
    size_t endOffset = 0;
    while (endOffset < listA.size() - startOffset && endOffset < listB.size() - startOffset) {
        size_t indexA = listA.size() - endOffset - 1;
        size_t indexB = listB.size() - endOffset - 1;
        if (listA.at(indexA) != listB.at(indexB))
            break;
        aToB->set(indexA, indexB);
        bToA->set(indexB, indexA);
        ++endOffset;
    }

    int n = listA.size() - startOffset - endOffset;
    int m = listB.size() - startOffset - endOffset;

    // If we mapped either of arrays, we have no more work to do.
    if (n == 0 || m == 0)
        return;

    int** diff = new int*[n];
    int** backtrack = new int*[n];
    for (int i = 0; i < n; ++i) {
        diff[i] = new int[m];
        backtrack[i] = new int[m];
    }

    // Compute longest common subsequence of two cssom models.
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int max = 0;
            int track = 0;

            if (i > 0 && diff[i - 1][j] > max) {
                max = diff[i - 1][j];
                track = 1;
            }

            if (j > 0 && diff[i][j - 1] > max) {
                max = diff[i][j - 1];
                track = 2;
            }

            if (listA.at(i + startOffset) == listB.at(j + startOffset)) {
                int value = i > 0 && j > 0 ? diff[i - 1][j - 1] + 1 : 1;
                if (value > max) {
                    max = value;
                    track = 3;
                }
            }

            diff[i][j] = max;
            backtrack[i][j] = track;
        }
    }

    // Backtrack and add missing mapping.
    int i = n - 1, j = m - 1;
    while (i >= 0 && j >= 0 && backtrack[i][j]) {
        switch (backtrack[i][j]) {
        case 1:
            i -= 1;
            break;
        case 2:
            j -= 1;
            break;
        case 3:
            aToB->set(i + startOffset, j + startOffset);
            bToA->set(j + startOffset, i + startOffset);
            i -= 1;
            j -= 1;
            break;
        default:
            ASSERT_NOT_REACHED();
        }
    }

    for (int i = 0; i < n; ++i) {
        delete [] diff[i];
        delete [] backtrack[i];
    }
    delete [] diff;
    delete [] backtrack;
}

String canonicalCSSText(RefPtrWillBeRawPtr<CSSRule> rule)
{
    if (rule->type() != CSSRule::STYLE_RULE)
        return rule->cssText();
    RefPtrWillBeRawPtr<CSSStyleRule> styleRule = toCSSStyleRule(rule.get());

    Vector<String> propertyNames;
    CSSStyleDeclaration* style = styleRule->style();
    for (unsigned i = 0; i < style->length(); ++i)
        propertyNames.append(style->item(i));

    std::sort(propertyNames.begin(), propertyNames.end(), WTF::codePointCompareLessThan);

    StringBuilder builder;
    builder.append(styleRule->selectorText());
    builder.append("{");
    for (unsigned i = 0; i < propertyNames.size(); ++i) {
        String name = propertyNames.at(i);
        builder.append(" ");
        builder.append(name);
        builder.append(":");
        builder.append(style->getPropertyValue(name));
        if (!style->getPropertyPriority(name).isEmpty()) {
            builder.append(" ");
            builder.append(style->getPropertyPriority(name));
        }
        builder.append(";");
    }
    builder.append("}");

    return builder.toString();
}

} // namespace

namespace blink {

enum MediaListSource {
    MediaListSourceLinkedSheet,
    MediaListSourceInlineSheet,
    MediaListSourceMediaRule,
    MediaListSourceImportRule
};

static PassRefPtr<TypeBuilder::CSS::SourceRange> buildSourceRangeObject(const SourceRange& range, const LineEndings* lineEndings)
{
    if (!lineEndings)
        return nullptr;
    TextPosition start = TextPosition::fromOffsetAndLineEndings(range.start, *lineEndings);
    TextPosition end = TextPosition::fromOffsetAndLineEndings(range.end, *lineEndings);

    RefPtr<TypeBuilder::CSS::SourceRange> result = TypeBuilder::CSS::SourceRange::create()
        .setStartLine(start.m_line.zeroBasedInt())
        .setStartColumn(start.m_column.zeroBasedInt())
        .setEndLine(end.m_line.zeroBasedInt())
        .setEndColumn(end.m_column.zeroBasedInt());
    return result.release();
}

PassRefPtrWillBeRawPtr<InspectorStyle> InspectorStyle::create(PassRefPtrWillBeRawPtr<CSSStyleDeclaration> style, PassRefPtrWillBeRawPtr<CSSRuleSourceData> sourceData, InspectorStyleSheetBase* parentStyleSheet)
{
    return adoptRefWillBeNoop(new InspectorStyle(style, sourceData, parentStyleSheet));
}

InspectorStyle::InspectorStyle(PassRefPtrWillBeRawPtr<CSSStyleDeclaration> style, PassRefPtrWillBeRawPtr<CSSRuleSourceData> sourceData, InspectorStyleSheetBase* parentStyleSheet)
    : m_style(style)
    , m_sourceData(sourceData)
    , m_parentStyleSheet(parentStyleSheet)
{
    ASSERT(m_style);
}

PassRefPtr<TypeBuilder::CSS::CSSStyle> InspectorStyle::buildObjectForStyle()
{
    RefPtr<TypeBuilder::CSS::CSSStyle> result = styleWithProperties();
    if (m_sourceData) {
        if (m_parentStyleSheet && !m_parentStyleSheet->id().isEmpty())
            result->setStyleSheetId(m_parentStyleSheet->id());
        result->setRange(buildSourceRangeObject(m_sourceData->ruleBodyRange, m_parentStyleSheet->lineEndings()));
        String sheetText;
        bool success = m_parentStyleSheet->getText(&sheetText);
        if (success) {
            const SourceRange& bodyRange = m_sourceData->ruleBodyRange;
            result->setCssText(sheetText.substring(bodyRange.start, bodyRange.end - bodyRange.start));
        }
    }

    return result.release();
}

PassRefPtr<TypeBuilder::Array<TypeBuilder::CSS::CSSComputedStyleProperty>> InspectorStyle::buildArrayForComputedStyle()
{
    RefPtr<TypeBuilder::Array<TypeBuilder::CSS::CSSComputedStyleProperty>> result = TypeBuilder::Array<TypeBuilder::CSS::CSSComputedStyleProperty>::create();
    WillBeHeapVector<CSSPropertySourceData> properties;
    populateAllProperties(properties);

    for (auto& property : properties) {
        RefPtr<TypeBuilder::CSS::CSSComputedStyleProperty> entry = TypeBuilder::CSS::CSSComputedStyleProperty::create()
            .setName(property.name)
            .setValue(property.value);
        result->addItem(entry);
    }

    return result.release();
}

bool InspectorStyle::styleText(String* result)
{
    if (!m_sourceData)
        return false;

    return textForRange(m_sourceData->ruleBodyRange, result);
}

bool InspectorStyle::textForRange(const SourceRange& range, String* result)
{
    String styleSheetText;
    bool success = m_parentStyleSheet->getText(&styleSheetText);
    if (!success)
        return false;

    ASSERT(0 <= range.start);
    ASSERT(range.start <= range.end);
    ASSERT(range.end <= styleSheetText.length());
    *result = styleSheetText.substring(range.start, range.end - range.start);
    return true;
}

void InspectorStyle::populateAllProperties(WillBeHeapVector<CSSPropertySourceData>& result)
{
    HashSet<String> sourcePropertyNames;

    if (m_sourceData && m_sourceData->styleSourceData) {
        WillBeHeapVector<CSSPropertySourceData>& sourcePropertyData = m_sourceData->styleSourceData->propertyData;
        for (const auto& data : sourcePropertyData) {
            result.append(data);
            sourcePropertyNames.add(data.name.lower());
        }
    }

    for (int i = 0, size = m_style->length(); i < size; ++i) {
        String name = m_style->item(i);
        if (!sourcePropertyNames.add(name.lower()).isNewEntry)
            continue;

        String value = m_style->getPropertyValue(name);
        if (value.isEmpty())
            continue;
        result.append(CSSPropertySourceData(name, value, !m_style->getPropertyPriority(name).isEmpty(), false, true, SourceRange()));
    }
}

PassRefPtr<TypeBuilder::CSS::CSSStyle> InspectorStyle::styleWithProperties()
{
    RefPtr<Array<TypeBuilder::CSS::CSSProperty> > propertiesObject = Array<TypeBuilder::CSS::CSSProperty>::create();
    RefPtr<Array<TypeBuilder::CSS::ShorthandEntry> > shorthandEntries = Array<TypeBuilder::CSS::ShorthandEntry>::create();
    HashSet<String> foundShorthands;

    WillBeHeapVector<CSSPropertySourceData> properties;
    populateAllProperties(properties);

    for (auto& styleProperty : properties) {
        const CSSPropertySourceData& propertyEntry = styleProperty;
        const String& name = propertyEntry.name;

        RefPtr<TypeBuilder::CSS::CSSProperty> property = TypeBuilder::CSS::CSSProperty::create()
            .setName(name)
            .setValue(propertyEntry.value);
        propertiesObject->addItem(property);

        // Default "parsedOk" == true.
        if (!propertyEntry.parsedOk)
            property->setParsedOk(false);
        String text;
        if (styleProperty.range.length() && textForRange(styleProperty.range, &text))
            property->setText(text);
        if (propertyEntry.important)
            property->setImportant(true);
        if (styleProperty.range.length()) {
            property->setRange(buildSourceRangeObject(propertyEntry.range, m_parentStyleSheet ? m_parentStyleSheet->lineEndings() : nullptr));
            if (!propertyEntry.disabled) {
                property->setImplicit(false);
            }
            property->setDisabled(propertyEntry.disabled);
        } else if (!propertyEntry.disabled) {
            bool implicit = m_style->isPropertyImplicit(name);
            // Default "implicit" == false.
            if (implicit)
                property->setImplicit(true);

            String shorthand = m_style->getPropertyShorthand(name);
            if (!shorthand.isEmpty()) {
                if (foundShorthands.add(shorthand).isNewEntry) {
                    RefPtr<TypeBuilder::CSS::ShorthandEntry> entry = TypeBuilder::CSS::ShorthandEntry::create()
                        .setName(shorthand)
                        .setValue(shorthandValue(shorthand));
                    if (!m_style->getPropertyPriority(name).isEmpty())
                        entry->setImportant(true);
                    shorthandEntries->addItem(entry);
                }
            }
        }
    }

    RefPtr<TypeBuilder::CSS::CSSStyle> result = TypeBuilder::CSS::CSSStyle::create()
        .setCssProperties(propertiesObject)
        .setShorthandEntries(shorthandEntries);
    return result.release();
}

String InspectorStyle::shorthandValue(const String& shorthandProperty)
{
    StringBuilder builder;
    String value = m_style->getPropertyValue(shorthandProperty);
    if (value.isEmpty()) {
        for (unsigned i = 0; i < m_style->length(); ++i) {
            String individualProperty = m_style->item(i);
            if (m_style->getPropertyShorthand(individualProperty) != shorthandProperty)
                continue;
            if (m_style->isPropertyImplicit(individualProperty))
                continue;
            String individualValue = m_style->getPropertyValue(individualProperty);
            if (individualValue == "initial")
                continue;
            if (!builder.isEmpty())
                builder.append(' ');
            builder.append(individualValue);
        }
    } else {
        builder.append(value);
    }

    if (!m_style->getPropertyPriority(shorthandProperty).isEmpty())
        builder.append(" !important");

    return builder.toString();
}

DEFINE_TRACE(InspectorStyle)
{
    visitor->trace(m_sourceData);
    visitor->trace(m_style);
    visitor->trace(m_parentStyleSheet);
}

InspectorStyleSheetBase::InspectorStyleSheetBase(Listener* listener)
    : m_id(IdentifiersFactory::createIdentifier())
    , m_listener(listener)
    , m_lineEndings(adoptPtr(new LineEndings()))
{
}

void InspectorStyleSheetBase::onStyleSheetTextChanged()
{
    m_lineEndings = adoptPtr(new LineEndings());
    if (listener())
        listener()->styleSheetChanged(this);
}

PassRefPtr<TypeBuilder::CSS::CSSStyle> InspectorStyleSheetBase::buildObjectForStyle(CSSStyleDeclaration* style)
{
    return inspectorStyle(style)->buildObjectForStyle();
}

const LineEndings* InspectorStyleSheetBase::lineEndings()
{
    if (m_lineEndings->size() > 0)
        return m_lineEndings.get();
    String text;
    if (getText(&text))
        m_lineEndings = WTF::lineEndings(text);
    return m_lineEndings.get();
}

bool InspectorStyleSheetBase::lineNumberAndColumnToOffset(unsigned lineNumber, unsigned columnNumber, unsigned* offset)
{
    const LineEndings* endings = lineEndings();
    if (lineNumber >= endings->size())
        return false;
    unsigned charactersInLine = lineNumber > 0 ? endings->at(lineNumber) - endings->at(lineNumber - 1) - 1 : endings->at(0);
    if (columnNumber > charactersInLine)
        return false;
    TextPosition position(OrdinalNumber::fromZeroBasedInt(lineNumber), OrdinalNumber::fromZeroBasedInt(columnNumber));
    *offset = position.toOffset(*endings).zeroBasedInt();
    return true;
}

PassRefPtrWillBeRawPtr<InspectorStyleSheet> InspectorStyleSheet::create(InspectorResourceAgent* resourceAgent, PassRefPtrWillBeRawPtr<CSSStyleSheet> pageStyleSheet, TypeBuilder::CSS::StyleSheetOrigin::Enum origin, const String& documentURL, InspectorCSSAgent* cssAgent)
{
    return adoptRefWillBeNoop(new InspectorStyleSheet(resourceAgent, pageStyleSheet, origin, documentURL, cssAgent));
}

InspectorStyleSheet::InspectorStyleSheet(InspectorResourceAgent* resourceAgent, PassRefPtrWillBeRawPtr<CSSStyleSheet> pageStyleSheet, TypeBuilder::CSS::StyleSheetOrigin::Enum origin, const String& documentURL, InspectorCSSAgent* cssAgent)
    : InspectorStyleSheetBase(cssAgent)
    , m_cssAgent(cssAgent)
    , m_resourceAgent(resourceAgent)
    , m_pageStyleSheet(pageStyleSheet)
    , m_origin(origin)
    , m_documentURL(documentURL)
{
    String text;
    bool success = inlineStyleSheetText(&text);
    if (!success)
        success = resourceStyleSheetText(&text);
    if (success)
        innerSetText(text, false);
}

InspectorStyleSheet::~InspectorStyleSheet()
{
}

DEFINE_TRACE(InspectorStyleSheet)
{
    visitor->trace(m_cssAgent);
    visitor->trace(m_resourceAgent);
    visitor->trace(m_pageStyleSheet);
    visitor->trace(m_sourceData);
    visitor->trace(m_cssomFlatRules);
    visitor->trace(m_parsedFlatRules);
    InspectorStyleSheetBase::trace(visitor);
}

static String styleSheetURL(CSSStyleSheet* pageStyleSheet)
{
    if (pageStyleSheet && !pageStyleSheet->contents()->baseURL().isEmpty())
        return pageStyleSheet->contents()->baseURL().string();
    return emptyString();
}

String InspectorStyleSheet::finalURL()
{
    String url = styleSheetURL(m_pageStyleSheet.get());
    return url.isEmpty() ? m_documentURL : url;
}

bool InspectorStyleSheet::setText(const String& text, ExceptionState& exceptionState)
{
    innerSetText(text, true);

    if (listener())
        listener()->willReparseStyleSheet();

    {
        // Have a separate scope for clearRules() (bug 95324).
        CSSStyleSheet::RuleMutationScope mutationScope(m_pageStyleSheet.get());
        m_pageStyleSheet->contents()->clearRules();
        m_pageStyleSheet->clearChildRuleCSSOMWrappers();
    }
    {
        CSSStyleSheet::RuleMutationScope mutationScope(m_pageStyleSheet.get());
        m_pageStyleSheet->contents()->parseString(text);
    }

    if (listener())
        listener()->didReparseStyleSheet();
    onStyleSheetTextChanged();
    m_pageStyleSheet->ownerDocument()->styleResolverChanged(FullStyleUpdate);
    return true;
}

RefPtrWillBeRawPtr<CSSStyleRule> InspectorStyleSheet::setRuleSelector(const SourceRange& range, const String& text, SourceRange* newRange, String* oldText, ExceptionState& exceptionState)
{
    if (!verifySelectorText(m_pageStyleSheet->ownerDocument(), text)) {
        exceptionState.throwDOMException(SyntaxError, "Selector or media text is not valid.");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSRuleSourceData> sourceData = findRuleByHeaderRange(range);
    if (!sourceData || !sourceData->styleSourceData) {
        exceptionState.throwDOMException(NotFoundError, "Source range didn't match existing source range");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSRule> rule = ruleForSourceData(sourceData);
    if (!rule || !rule->parentStyleSheet() || rule->type() != CSSRule::STYLE_RULE) {
        exceptionState.throwDOMException(NotFoundError, "Source range didn't match existing style source range");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSStyleRule> styleRule = InspectorCSSAgent::asCSSStyleRule(rule.get());
    styleRule->setSelectorText(text);

    replaceText(sourceData->ruleHeaderRange, text, newRange, oldText);
    onStyleSheetTextChanged();

    return styleRule;
}

RefPtrWillBeRawPtr<CSSStyleRule> InspectorStyleSheet::setStyleText(const SourceRange& range, const String& text, SourceRange* newRange, String* oldText, ExceptionState& exceptionState)
{
    if (!verifyStyleText(m_pageStyleSheet->ownerDocument(), text)) {
        exceptionState.throwDOMException(SyntaxError, "Style text is not valid.");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSRuleSourceData> sourceData = findRuleByBodyRange(range);
    if (!sourceData || !sourceData->styleSourceData) {
        exceptionState.throwDOMException(NotFoundError, "Source range didn't match existing style source range");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSRule> rule = ruleForSourceData(sourceData);
    if (!rule || !rule->parentStyleSheet() || rule->type() != CSSRule::STYLE_RULE) {
        exceptionState.throwDOMException(NotFoundError, "Source range didn't match existing style source range");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSStyleRule> styleRule = InspectorCSSAgent::asCSSStyleRule(rule.get());
    styleRule->style()->setCSSText(text, exceptionState);

    replaceText(sourceData->ruleBodyRange, text, newRange, oldText);
    onStyleSheetTextChanged();

    return styleRule;
}

RefPtrWillBeRawPtr<CSSMediaRule> InspectorStyleSheet::setMediaRuleText(const SourceRange& range, const String& text, SourceRange* newRange, String* oldText, ExceptionState& exceptionState)
{
    if (!verifyMediaText(m_pageStyleSheet->ownerDocument(), text)) {
        exceptionState.throwDOMException(SyntaxError, "Selector or media text is not valid.");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSRuleSourceData> sourceData = findRuleByHeaderRange(range);
    if (!sourceData || !sourceData->mediaSourceData) {
        exceptionState.throwDOMException(NotFoundError, "Source range didn't match existing source range");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSRule> rule = ruleForSourceData(sourceData);
    if (!rule || !rule->parentStyleSheet() || rule->type() != CSSRule::MEDIA_RULE) {
        exceptionState.throwDOMException(NotFoundError, "Source range didn't match existing style source range");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSMediaRule>  mediaRule = InspectorCSSAgent::asCSSMediaRule(rule.get());
    mediaRule->media()->setMediaText(text);

    replaceText(sourceData->ruleHeaderRange, text, newRange, oldText);
    onStyleSheetTextChanged();

    return mediaRule;
}

RefPtrWillBeRawPtr<CSSRuleSourceData> InspectorStyleSheet::ruleSourceDataAfterSourceRange(const SourceRange& sourceRange)
{
    ASSERT(m_sourceData);
    unsigned index = 0;
    for (; index < m_sourceData->size(); ++index) {
        RefPtrWillBeRawPtr<CSSRuleSourceData> sd = m_sourceData->at(index);
        if (sd->ruleHeaderRange.start >= sourceRange.end)
            break;
    }
    return index < m_sourceData->size() ? m_sourceData->at(index) : nullptr;
}

CSSStyleRule* InspectorStyleSheet::insertCSSOMRuleInStyleSheet(CSSRule* insertBefore, const String& ruleText, ExceptionState& exceptionState)
{
    unsigned index = 0;
    for (; index < m_pageStyleSheet->length(); ++index) {
        CSSRule* rule = m_pageStyleSheet->item(index);
        if (rule == insertBefore)
            break;
    }

    m_pageStyleSheet->insertRule(ruleText, index, exceptionState);
    CSSRule* rule = m_pageStyleSheet->item(index);
    CSSStyleRule* styleRule = InspectorCSSAgent::asCSSStyleRule(rule);
    if (!styleRule) {
        m_pageStyleSheet->deleteRule(index, ASSERT_NO_EXCEPTION);
        exceptionState.throwDOMException(SyntaxError, "The rule '" + ruleText + "' could not be added in style sheet.");
        return nullptr;
    }
    return styleRule;
}

CSSStyleRule* InspectorStyleSheet::insertCSSOMRuleInMediaRule(CSSMediaRule* mediaRule, CSSRule* insertBefore, const String& ruleText, ExceptionState& exceptionState)
{
    unsigned index = 0;
    for (; index < mediaRule->length(); ++index) {
        CSSRule* rule = mediaRule->item(index);
        if (rule == insertBefore)
            break;
    }

    mediaRule->insertRule(ruleText, index, exceptionState);
    CSSRule* rule = mediaRule->item(index);
    CSSStyleRule* styleRule = InspectorCSSAgent::asCSSStyleRule(rule);
    if (!styleRule) {
        mediaRule->deleteRule(index, ASSERT_NO_EXCEPTION);
        exceptionState.throwDOMException(SyntaxError, "The rule '" + ruleText + "' could not be added in media rule.");
        return nullptr;
    }
    return styleRule;
}

CSSStyleRule* InspectorStyleSheet::insertCSSOMRuleBySourceRange(const SourceRange& sourceRange, const String& ruleText, ExceptionState& exceptionState)
{
    ASSERT(m_sourceData);

    RefPtrWillBeRawPtr<CSSRuleSourceData> containingRuleSourceData = nullptr;
    for (size_t i = 0; i < m_sourceData->size(); ++i) {
        RefPtrWillBeRawPtr<CSSRuleSourceData> ruleSourceData = m_sourceData->at(i);
        if (ruleSourceData->ruleHeaderRange.start < sourceRange.start && sourceRange.start < ruleSourceData->ruleBodyRange.start) {
            exceptionState.throwDOMException(NotFoundError, "Cannot insert rule inside rule selector.");
            return nullptr;
        }
        if (sourceRange.start < ruleSourceData->ruleBodyRange.start || ruleSourceData->ruleBodyRange.end < sourceRange.start)
            continue;
        if (!containingRuleSourceData || containingRuleSourceData->ruleBodyRange.length() > ruleSourceData->ruleBodyRange.length())
            containingRuleSourceData = ruleSourceData;
    }

    RefPtrWillBeRawPtr<CSSRuleSourceData> insertBefore = ruleSourceDataAfterSourceRange(sourceRange);
    RefPtrWillBeRawPtr<CSSRule> insertBeforeRule = ruleForSourceData(insertBefore);

    if (!containingRuleSourceData)
        return insertCSSOMRuleInStyleSheet(insertBeforeRule.get(), ruleText, exceptionState);

    RefPtrWillBeRawPtr<CSSRule> rule = ruleForSourceData(containingRuleSourceData);
    if (!rule || rule->type() != CSSRule::MEDIA_RULE) {
        exceptionState.throwDOMException(NotFoundError, "Cannot insert rule in non-media rule.");
        return nullptr;
    }

    return insertCSSOMRuleInMediaRule(toCSSMediaRule(rule.get()), insertBeforeRule.get(), ruleText, exceptionState);
}

RefPtrWillBeRawPtr<CSSStyleRule> InspectorStyleSheet::addRule(const String& ruleText, const SourceRange& location, SourceRange* addedRange, ExceptionState& exceptionState)
{
    if (location.start != location.end) {
        exceptionState.throwDOMException(NotFoundError, "Source range must be collapsed.");
        return nullptr;
    }

    if (!verifyRuleText(m_pageStyleSheet->ownerDocument(), ruleText)) {
        exceptionState.throwDOMException(SyntaxError, "Rule text is not valid.");
        return nullptr;
    }

    if (!m_sourceData) {
        exceptionState.throwDOMException(NotFoundError, "Style is read-only.");
        return nullptr;
    }

    RefPtrWillBeRawPtr<CSSStyleRule> styleRule = insertCSSOMRuleBySourceRange(location, ruleText, exceptionState);
    if (exceptionState.hadException())
        return nullptr;

    replaceText(location, ruleText, addedRange, nullptr);
    onStyleSheetTextChanged();
    return styleRule;
}

bool InspectorStyleSheet::deleteRule(const SourceRange& range, ExceptionState& exceptionState)
{
    if (!m_sourceData) {
        exceptionState.throwDOMException(NotFoundError, "Style is read-only.");
        return false;
    }

    // Find index of CSSRule that entirely belongs to the range.
    RefPtrWillBeRawPtr<CSSRuleSourceData> foundData = nullptr;

    for (size_t i = 0; i < m_sourceData->size(); ++i) {
        RefPtrWillBeRawPtr<CSSRuleSourceData> ruleSourceData = m_sourceData->at(i);
        unsigned ruleStart = ruleSourceData->ruleHeaderRange.start;
        unsigned ruleEnd = ruleSourceData->ruleBodyRange.end + 1;
        bool startBelongs = ruleStart >= range.start && ruleStart < range.end;
        bool endBelongs = ruleEnd > range.start && ruleEnd <= range.end;

        if (startBelongs != endBelongs)
            break;
        if (!startBelongs)
            continue;
        if (!foundData || foundData->ruleBodyRange.length() > ruleSourceData->ruleBodyRange.length())
            foundData = ruleSourceData;
    }
    RefPtrWillBeRawPtr<CSSRule> rule = ruleForSourceData(foundData);
    if (!rule) {
        exceptionState.throwDOMException(NotFoundError, "No style rule could be found in given range.");
        return false;
    }
    CSSStyleSheet* styleSheet = rule->parentStyleSheet();
    if (!styleSheet) {
        exceptionState.throwDOMException(NotFoundError, "No parent stylesheet could be found.");
        return false;
    }
    CSSRule* parentRule = rule->parentRule();
    if (parentRule) {
        if (parentRule->type() != CSSRule::MEDIA_RULE) {
            exceptionState.throwDOMException(NotFoundError, "Cannot remove rule from non-media rule.");
            return false;
        }
        CSSMediaRule* parentMediaRule = toCSSMediaRule(parentRule);
        size_t index = 0;
        while (index < parentMediaRule->length() && parentMediaRule->item(index) != rule)
            ++index;
        ASSERT(index < parentMediaRule->length());
        parentMediaRule->deleteRule(index, exceptionState);
    } else {
        size_t index = 0;
        while (index < styleSheet->length() && styleSheet->item(index) != rule)
            ++index;
        ASSERT(index < styleSheet->length());
        styleSheet->deleteRule(index, exceptionState);
    }
    // |rule| MAY NOT be addressed after this line!

    if (exceptionState.hadException())
        return false;

    replaceText(range, "", nullptr, nullptr);
    onStyleSheetTextChanged();
    return true;
}

void InspectorStyleSheet::replaceText(const SourceRange& range, const String& text, SourceRange* newRange, String* oldText)
{
    String sheetText = m_text;
    if (oldText)
        *oldText = sheetText.substring(range.start, range.length());
    sheetText.replace(range.start, range.length(), text);
    if (newRange)
        *newRange = SourceRange(range.start, range.start + text.length());
    innerSetText(sheetText, true);
}

void InspectorStyleSheet::innerSetText(const String& text, bool markAsLocallyModified)
{
    OwnPtrWillBeRawPtr<RuleSourceDataList> ruleTree = adoptPtrWillBeNoop(new RuleSourceDataList());
    RefPtrWillBeRawPtr<StyleSheetContents> styleSheet = StyleSheetContents::create(m_pageStyleSheet->contents()->parserContext());
    StyleSheetHandler handler(text, m_pageStyleSheet->ownerDocument(), ruleTree.get());
    CSSParser::parseSheetForInspector(m_pageStyleSheet->contents()->parserContext(), styleSheet.get(), text, handler);
    RefPtrWillBeRawPtr<CSSStyleSheet> sourceDataSheet = nullptr;
    if (toCSSImportRule(m_pageStyleSheet->ownerRule()))
        sourceDataSheet = CSSStyleSheet::create(styleSheet, toCSSImportRule(m_pageStyleSheet->ownerRule()));
    else
        sourceDataSheet = CSSStyleSheet::create(styleSheet, m_pageStyleSheet->ownerNode());

    m_parsedFlatRules.clear();
    collectFlatRules(sourceDataSheet.get(), &m_parsedFlatRules);

    m_sourceData = adoptPtrWillBeNoop(new RuleSourceDataList());
    flattenSourceData(ruleTree.get(), m_sourceData.get());
    m_text = text;

    if (markAsLocallyModified) {
        Element* element = ownerStyleElement();
        if (element)
            m_cssAgent->addEditedStyleElement(DOMNodeIds::idForNode(element), text);
        else
            m_cssAgent->addEditedStyleSheet(finalURL(), text);
    }
}

PassRefPtr<TypeBuilder::CSS::CSSStyleSheetHeader> InspectorStyleSheet::buildObjectForStyleSheetInfo()
{
    CSSStyleSheet* styleSheet = pageStyleSheet();
    if (!styleSheet)
        return nullptr;

    Document* document = styleSheet->ownerDocument();
    LocalFrame* frame = document ? document->frame() : nullptr;

    RefPtr<TypeBuilder::CSS::CSSStyleSheetHeader> result = TypeBuilder::CSS::CSSStyleSheetHeader::create()
        .setStyleSheetId(id())
        .setOrigin(m_origin)
        .setDisabled(styleSheet->disabled())
        .setSourceURL(url())
        .setTitle(styleSheet->title())
        .setFrameId(frame ? IdentifiersFactory::frameId(frame) : "")
        .setIsInline(styleSheet->isInline() && !startsAtZero())
        .setStartLine(styleSheet->startPositionInSource().m_line.zeroBasedInt())
        .setStartColumn(styleSheet->startPositionInSource().m_column.zeroBasedInt());

    if (hasSourceURL())
        result->setHasSourceURL(true);

    if (styleSheet->ownerNode())
        result->setOwnerNode(DOMNodeIds::idForNode(styleSheet->ownerNode()));

    String sourceMapURLValue = sourceMapURL();
    if (!sourceMapURLValue.isEmpty())
        result->setSourceMapURL(sourceMapURLValue);
    return result.release();
}

PassRefPtr<TypeBuilder::Array<TypeBuilder::CSS::Selector>> InspectorStyleSheet::selectorsFromSource(CSSRuleSourceData* sourceData, const String& sheetText)
{
    ScriptRegexp comment("/\\*[^]*?\\*/", TextCaseSensitive, MultilineEnabled);
    RefPtr<TypeBuilder::Array<TypeBuilder::CSS::Selector> > result = TypeBuilder::Array<TypeBuilder::CSS::Selector>::create();
    const SelectorRangeList& ranges = sourceData->selectorRanges;
    for (size_t i = 0, size = ranges.size(); i < size; ++i) {
        const SourceRange& range = ranges.at(i);
        String selector = sheetText.substring(range.start, range.length());

        // We don't want to see any comments in the selector components, only the meaningful parts.
        int matchLength;
        int offset = 0;
        while ((offset = comment.match(selector, offset, &matchLength)) >= 0)
            selector.replace(offset, matchLength, "");

        RefPtr<TypeBuilder::CSS::Selector> simpleSelector = TypeBuilder::CSS::Selector::create()
            .setValue(selector.stripWhiteSpace());
        simpleSelector->setRange(buildSourceRangeObject(range, lineEndings()));
        result->addItem(simpleSelector.release());
    }
    return result.release();
}

PassRefPtr<TypeBuilder::CSS::SelectorList> InspectorStyleSheet::buildObjectForSelectorList(CSSStyleRule* rule)
{
    RefPtrWillBeRawPtr<CSSRuleSourceData> sourceData = sourceDataForRule(rule);
    RefPtr<TypeBuilder::Array<TypeBuilder::CSS::Selector> > selectors;

    // This intentionally does not rely on the source data to avoid catching the trailing comments (before the declaration starting '{').
    String selectorText = rule->selectorText();

    if (sourceData) {
        selectors = selectorsFromSource(sourceData.get(), m_text);
    } else {
        selectors = TypeBuilder::Array<TypeBuilder::CSS::Selector>::create();
        const CSSSelectorList& selectorList = rule->styleRule()->selectorList();
        for (const CSSSelector* selector = selectorList.first(); selector; selector = CSSSelectorList::next(*selector))
            selectors->addItem(TypeBuilder::CSS::Selector::create().setValue(selector->selectorText()).release());
    }
    RefPtr<TypeBuilder::CSS::SelectorList> result = TypeBuilder::CSS::SelectorList::create()
        .setSelectors(selectors)
        .setText(selectorText)
        .release();
    return result.release();
}

static bool canBind(TypeBuilder::CSS::StyleSheetOrigin::Enum origin)
{
    return origin != TypeBuilder::CSS::StyleSheetOrigin::User_agent && origin != TypeBuilder::CSS::StyleSheetOrigin::Injected;
}

PassRefPtr<TypeBuilder::CSS::CSSRule> InspectorStyleSheet::buildObjectForRule(CSSStyleRule* rule, PassRefPtr<Array<TypeBuilder::CSS::CSSMedia> > mediaStack)
{
    CSSStyleSheet* styleSheet = pageStyleSheet();
    if (!styleSheet)
        return nullptr;

    RefPtr<TypeBuilder::CSS::CSSRule> result = TypeBuilder::CSS::CSSRule::create()
        .setSelectorList(buildObjectForSelectorList(rule))
        .setOrigin(m_origin)
        .setStyle(buildObjectForStyle(rule->style()));

    if (canBind(m_origin)) {
        if (!id().isEmpty())
            result->setStyleSheetId(id());
    }

    if (mediaStack)
        result->setMedia(mediaStack);

    return result.release();
}

bool InspectorStyleSheet::getText(String* result)
{
    if (m_sourceData) {
        *result = m_text;
        return true;
    }
    return false;
}

PassRefPtr<TypeBuilder::CSS::SourceRange> InspectorStyleSheet::ruleHeaderSourceRange(CSSRule* rule)
{
    if (!m_sourceData)
        return nullptr;
    RefPtrWillBeRawPtr<CSSRuleSourceData> sourceData = sourceDataForRule(rule);
    if (!sourceData)
        return nullptr;
    return buildSourceRangeObject(sourceData->ruleHeaderRange, lineEndings());
}

PassRefPtr<TypeBuilder::CSS::SourceRange> InspectorStyleSheet::mediaQueryExpValueSourceRange(CSSRule* rule, size_t mediaQueryIndex, size_t mediaQueryExpIndex)
{
    if (!m_sourceData)
        return nullptr;
    RefPtrWillBeRawPtr<CSSRuleSourceData> sourceData = sourceDataForRule(rule);
    if (!sourceData || !sourceData->mediaSourceData || mediaQueryIndex >= sourceData->mediaSourceData->queryData.size())
        return nullptr;
    RefPtrWillBeRawPtr<CSSMediaQuerySourceData> mediaQueryData = sourceData->mediaSourceData->queryData.at(mediaQueryIndex);
    if (mediaQueryExpIndex >= mediaQueryData->expData.size())
        return nullptr;
    return buildSourceRangeObject(mediaQueryData->expData.at(mediaQueryExpIndex).valueRange, lineEndings());
}

PassRefPtrWillBeRawPtr<InspectorStyle> InspectorStyleSheet::inspectorStyle(RefPtrWillBeRawPtr<CSSStyleDeclaration> style)
{
    return style ? InspectorStyle::create(style, sourceDataForRule(style->parentRule()), this) : nullptr;
}

String InspectorStyleSheet::sourceURL()
{
    if (!m_sourceURL.isNull())
        return m_sourceURL;
    if (m_origin != TypeBuilder::CSS::StyleSheetOrigin::Regular) {
        m_sourceURL = "";
        return m_sourceURL;
    }

    String styleSheetText;
    bool success = getText(&styleSheetText);
    if (success) {
        bool deprecated;
        String commentValue = ContentSearchUtils::findSourceURL(styleSheetText, ContentSearchUtils::CSSMagicComment, &deprecated);
        if (!commentValue.isEmpty()) {
            // FIXME: add deprecated console message here.
            m_sourceURL = commentValue;
            return commentValue;
        }
    }
    m_sourceURL = "";
    return m_sourceURL;
}

String InspectorStyleSheet::url()
{
    // "sourceURL" is present only for regular rules, otherwise "origin" should be used in the frontend.
    if (m_origin != TypeBuilder::CSS::StyleSheetOrigin::Regular)
        return String();

    CSSStyleSheet* styleSheet = pageStyleSheet();
    if (!styleSheet)
        return String();

    if (hasSourceURL())
        return sourceURL();

    if (styleSheet->isInline() && startsAtZero())
        return String();

    return finalURL();
}

bool InspectorStyleSheet::hasSourceURL()
{
    return !sourceURL().isEmpty();
}

bool InspectorStyleSheet::startsAtZero()
{
    CSSStyleSheet* styleSheet = pageStyleSheet();
    if (!styleSheet)
        return true;

    return styleSheet->startPositionInSource() == TextPosition::minimumPosition();
}

String InspectorStyleSheet::sourceMapURL()
{
    if (m_origin != TypeBuilder::CSS::StyleSheetOrigin::Regular)
        return String();

    String styleSheetText;
    bool success = getText(&styleSheetText);
    if (success) {
        bool deprecated;
        String commentValue = ContentSearchUtils::findSourceMapURL(styleSheetText, ContentSearchUtils::CSSMagicComment, &deprecated);
        if (!commentValue.isEmpty()) {
            // FIXME: add deprecated console message here.
            return commentValue;
        }
    }
    return m_pageStyleSheet->contents()->sourceMapURL();
}

RefPtrWillBeRawPtr<CSSRuleSourceData> InspectorStyleSheet::findRuleByHeaderRange(const SourceRange& sourceRange)
{
    if (!m_sourceData)
        return nullptr;

    for (size_t i = 0; i < m_sourceData->size(); ++i) {
        RefPtrWillBeRawPtr<CSSRuleSourceData> ruleSourceData = m_sourceData->at(i);
        if (ruleSourceData->ruleHeaderRange.start == sourceRange.start && ruleSourceData->ruleHeaderRange.end == sourceRange.end) {
            return ruleSourceData;
        }
    }
    return nullptr;
}

RefPtrWillBeRawPtr<CSSRuleSourceData> InspectorStyleSheet::findRuleByBodyRange(const SourceRange& sourceRange)
{
    if (!m_sourceData)
        return nullptr;

    for (size_t i = 0; i < m_sourceData->size(); ++i) {
        RefPtrWillBeRawPtr<CSSRuleSourceData> ruleSourceData = m_sourceData->at(i);
        if (ruleSourceData->ruleBodyRange.start == sourceRange.start && ruleSourceData->ruleBodyRange.end == sourceRange.end) {
            return ruleSourceData;
        }
    }
    return nullptr;
}

RefPtrWillBeRawPtr<CSSRule> InspectorStyleSheet::ruleForSourceData(RefPtrWillBeRawPtr<CSSRuleSourceData> sourceData)
{
    if (!m_sourceData || !sourceData)
        return nullptr;

    remapSourceDataToCSSOMIfNecessary();

    size_t index = m_sourceData->find(sourceData.get());
    if (index == kNotFound)
        return nullptr;
    IndexMap::iterator it = m_sourceDataToRule.find(index);
    if (it == m_sourceDataToRule.end())
        return nullptr;

    ASSERT(it->value < m_cssomFlatRules.size());

    // Check that CSSOM did not mutate this rule.
    RefPtrWillBeRawPtr<CSSRule> result = m_cssomFlatRules.at(it->value);
    if (canonicalCSSText(m_parsedFlatRules.at(index)) != canonicalCSSText(result))
        return nullptr;
    return result;
}

RefPtrWillBeRawPtr<CSSRuleSourceData> InspectorStyleSheet::sourceDataForRule(RefPtrWillBeRawPtr<CSSRule> rule)
{
    if (!m_sourceData || !rule)
        return nullptr;

    remapSourceDataToCSSOMIfNecessary();

    size_t index = m_cssomFlatRules.find(rule.get());
    if (index == kNotFound)
        return nullptr;
    IndexMap::iterator it = m_ruleToSourceData.find(index);
    if (it == m_ruleToSourceData.end())
        return nullptr;

    ASSERT(it->value < m_sourceData->size());

    // Check that CSSOM did not mutate this rule.
    RefPtrWillBeRawPtr<CSSRule> parsedRule = m_parsedFlatRules.at(it->value);
    if (canonicalCSSText(rule) != canonicalCSSText(parsedRule))
        return nullptr;

    return m_sourceData->at(it->value);
}

void InspectorStyleSheet::remapSourceDataToCSSOMIfNecessary()
{
    CSSRuleVector cssomRules;
    collectFlatRules(m_pageStyleSheet.get(), &cssomRules);

    if (cssomRules.size() != m_cssomFlatRules.size()) {
        mapSourceDataToCSSOM();
        return;
    }

    for (size_t i = 0; i < m_cssomFlatRules.size(); ++i) {
        if (m_cssomFlatRules.at(i) != cssomRules.at(i)) {
            mapSourceDataToCSSOM();
            return;
        }
    }
}

void InspectorStyleSheet::mapSourceDataToCSSOM()
{
    m_ruleToSourceData.clear();
    m_sourceDataToRule.clear();

    m_cssomFlatRules.clear();
    CSSRuleVector& cssomRules = m_cssomFlatRules;
    collectFlatRules(m_pageStyleSheet.get(), &cssomRules);

    if (!m_sourceData)
        return;

    CSSRuleVector& parsedRules = m_parsedFlatRules;

    Vector<String> cssomRulesText = Vector<String>();
    Vector<String> parsedRulesText = Vector<String>();
    for (size_t i = 0; i < cssomRules.size(); ++i)
        cssomRulesText.append(canonicalCSSText(cssomRules.at(i)));
    for (size_t j = 0; j < parsedRules.size(); ++j)
        parsedRulesText.append(canonicalCSSText(parsedRules.at(j)));

    diff(cssomRulesText, parsedRulesText, &m_ruleToSourceData, &m_sourceDataToRule);
}

const CSSRuleVector& InspectorStyleSheet::flatRules()
{
    remapSourceDataToCSSOMIfNecessary();
    return m_cssomFlatRules;
}

bool InspectorStyleSheet::resourceStyleSheetText(String* result)
{
    if (m_origin == TypeBuilder::CSS::StyleSheetOrigin::Injected || m_origin == TypeBuilder::CSS::StyleSheetOrigin::User_agent)
        return false;

    if (!m_pageStyleSheet->ownerDocument())
        return false;

    KURL url(ParsedURLString, m_pageStyleSheet->href());
    if (m_cssAgent->getEditedStyleSheet(url, result))
        return true;

    bool base64Encoded;
    bool success = m_resourceAgent->fetchResourceContent(m_pageStyleSheet->ownerDocument(), url, result, &base64Encoded);
    return success && !base64Encoded;
}

Element* InspectorStyleSheet::ownerStyleElement()
{
    Node* ownerNode = m_pageStyleSheet->ownerNode();
    if (!ownerNode || !ownerNode->isElementNode())
        return nullptr;
    Element* ownerElement = toElement(ownerNode);

    if (!isHTMLStyleElement(ownerElement) && !isSVGStyleElement(ownerElement))
        return nullptr;
    return ownerElement;
}

bool InspectorStyleSheet::inlineStyleSheetText(String* result)
{
    Element* ownerElement = ownerStyleElement();
    if (!ownerElement)
        return false;
    if (m_cssAgent->getEditedStyleElement(DOMNodeIds::idForNode(ownerElement), result))
        return true;
    *result = ownerElement->textContent();
    return true;
}

PassRefPtrWillBeRawPtr<InspectorStyleSheetForInlineStyle> InspectorStyleSheetForInlineStyle::create(PassRefPtrWillBeRawPtr<Element> element, Listener* listener)
{
    return adoptRefWillBeNoop(new InspectorStyleSheetForInlineStyle(element, listener));
}

InspectorStyleSheetForInlineStyle::InspectorStyleSheetForInlineStyle(PassRefPtrWillBeRawPtr<Element> element, Listener* listener)
    : InspectorStyleSheetBase(listener)
    , m_element(element)
{
    ASSERT(m_element);
}

void InspectorStyleSheetForInlineStyle::didModifyElementAttribute()
{
    m_inspectorStyle.clear();
}

bool InspectorStyleSheetForInlineStyle::setText(const String& text, ExceptionState& exceptionState)
{
    if (!verifyStyleText(&m_element->document(), text)) {
        exceptionState.throwDOMException(SyntaxError, "Style text is not valid.");
        return false;
    }

    {
        InspectorCSSAgent::InlineStyleOverrideScope overrideScope(m_element->ownerDocument());
        m_element->setAttribute("style", AtomicString(text), exceptionState);
    }
    if (!exceptionState.hadException())
        onStyleSheetTextChanged();
    return !exceptionState.hadException();
}

bool InspectorStyleSheetForInlineStyle::getText(String* result)
{
    *result = elementStyleText();
    return true;
}

PassRefPtrWillBeRawPtr<InspectorStyle> InspectorStyleSheetForInlineStyle::inspectorStyle(RefPtrWillBeRawPtr<CSSStyleDeclaration> style)
{
    if (!m_inspectorStyle)
        m_inspectorStyle = InspectorStyle::create(m_element->style(), ruleSourceData(), this);

    return m_inspectorStyle;
}

RefPtrWillBeRawPtr<CSSRuleSourceData> InspectorStyleSheetForInlineStyle::ruleSourceData()
{
    const String& text = elementStyleText();
    RefPtrWillBeRawPtr<CSSRuleSourceData> ruleSourceData = nullptr;
    if (text.isEmpty()) {
        ruleSourceData = CSSRuleSourceData::create(StyleRule::Style);
        ruleSourceData->ruleBodyRange.start = 0;
        ruleSourceData->ruleBodyRange.end = 0;
    } else {
        RuleSourceDataList ruleSourceDataResult;
        StyleSheetHandler handler(text, &m_element->document(), &ruleSourceDataResult);
        CSSParser::parseDeclarationListForInspector(parserContextForDocument(&m_element->document()), text, handler);
        ruleSourceData = ruleSourceDataResult.first().release();
    }
    return ruleSourceData;
}

CSSStyleDeclaration* InspectorStyleSheetForInlineStyle::inlineStyle()
{
    return m_element->style();
}

const String& InspectorStyleSheetForInlineStyle::elementStyleText()
{
    return m_element->getAttribute("style").string();
}

DEFINE_TRACE(InspectorStyleSheetForInlineStyle)
{
    visitor->trace(m_element);
    visitor->trace(m_inspectorStyle);
    InspectorStyleSheetBase::trace(visitor);
}

} // namespace blink
