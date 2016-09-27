/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
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

#ifndef CSSStyleSheet_h
#define CSSStyleSheet_h

#include "core/CoreExport.h"
#include "core/css/CSSRule.h"
#include "core/css/StyleSheet.h"
#include "platform/heap/Handle.h"
#include "wtf/Noncopyable.h"
#include "wtf/text/TextPosition.h"

namespace blink {

class CSSImportRule;
class CSSRule;
class CSSRuleList;
class CSSStyleSheet;
class Document;
class ExceptionState;
class MediaQuerySet;
class SecurityOrigin;
class StyleSheetContents;

enum StyleSheetUpdateType {
    PartialRuleUpdate,
    EntireStyleSheetUpdate
};

class CORE_EXPORT CSSStyleSheet final : public StyleSheet {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<CSSStyleSheet> create(PassRefPtrWillBeRawPtr<StyleSheetContents>, CSSImportRule* ownerRule = 0);
    static PassRefPtrWillBeRawPtr<CSSStyleSheet> create(PassRefPtrWillBeRawPtr<StyleSheetContents>, Node* ownerNode);
    static PassRefPtrWillBeRawPtr<CSSStyleSheet> createInline(Node*, const KURL&, const TextPosition& startPosition = TextPosition::minimumPosition(), const String& encoding = String());
    static PassRefPtrWillBeRawPtr<CSSStyleSheet> createInline(PassRefPtrWillBeRawPtr<StyleSheetContents>, Node* ownerNode, const TextPosition& startPosition = TextPosition::minimumPosition());

    virtual ~CSSStyleSheet();

    virtual CSSStyleSheet* parentStyleSheet() const override;
    virtual Node* ownerNode() const override { return m_ownerNode; }
    virtual MediaList* media() const override;
    virtual String href() const override;
    virtual String title() const override { return m_title; }
    virtual bool disabled() const override { return m_isDisabled; }
    virtual void setDisabled(bool) override;

    PassRefPtrWillBeRawPtr<CSSRuleList> cssRules();
    unsigned insertRule(const String& rule, unsigned index, ExceptionState&);
    unsigned insertRule(const String& rule, ExceptionState&); // Deprecated.
    void deleteRule(unsigned index, ExceptionState&);

    // IE Extensions
    PassRefPtrWillBeRawPtr<CSSRuleList> rules();
    int addRule(const String& selector, const String& style, int index, ExceptionState&);
    int addRule(const String& selector, const String& style, ExceptionState&);
    void removeRule(unsigned index, ExceptionState& exceptionState) { deleteRule(index, exceptionState); }

    // For CSSRuleList.
    unsigned length() const;
    CSSRule* item(unsigned index);

    virtual void clearOwnerNode() override;

    virtual CSSRule* ownerRule() const override { return m_ownerRule; }
    virtual KURL baseURL() const override;
    virtual bool isLoading() const override;

    void clearOwnerRule() { m_ownerRule = nullptr; }
    Document* ownerDocument() const;
    MediaQuerySet* mediaQueries() const { return m_mediaQueries.get(); }
    void setMediaQueries(PassRefPtrWillBeRawPtr<MediaQuerySet>);
    void setTitle(const String& title) { m_title = title; }
    // Set by LinkStyle iff CORS-enabled fetch of stylesheet succeeded from this origin.
    void setAllowRuleAccessFromOrigin(PassRefPtr<SecurityOrigin> allowedOrigin);

    class RuleMutationScope {
        WTF_MAKE_NONCOPYABLE(RuleMutationScope);
        STACK_ALLOCATED();
    public:
        explicit RuleMutationScope(CSSStyleSheet*);
        explicit RuleMutationScope(CSSRule*);
        ~RuleMutationScope();

    private:
        RawPtrWillBeMember<CSSStyleSheet> m_styleSheet;
    };

    void willMutateRules();
    void didMutateRules();
    void didMutate(StyleSheetUpdateType = PartialRuleUpdate);

    void clearChildRuleCSSOMWrappers();

    StyleSheetContents* contents() const { return m_contents.get(); }

    bool isInline() const { return m_isInlineStylesheet; }
    TextPosition startPositionInSource() const { return m_startPosition; }

    bool sheetLoaded();
    bool loadCompleted() const { return m_loadCompleted; }
    void startLoadingDynamicSheet();

    DECLARE_VIRTUAL_TRACE();

private:
    CSSStyleSheet(PassRefPtrWillBeRawPtr<StyleSheetContents>, CSSImportRule* ownerRule);
    CSSStyleSheet(PassRefPtrWillBeRawPtr<StyleSheetContents>, Node* ownerNode, bool isInlineStylesheet, const TextPosition& startPosition);

    virtual bool isCSSStyleSheet() const override { return true; }
    virtual String type() const override { return "text/css"; }

    void reattachChildRuleCSSOMWrappers();

    bool canAccessRules() const;

    void setLoadCompleted(bool);

    RefPtrWillBeMember<StyleSheetContents> m_contents;
    bool m_isInlineStylesheet;
    bool m_isDisabled;
    String m_title;
    RefPtrWillBeMember<MediaQuerySet> m_mediaQueries;

    RefPtr<SecurityOrigin> m_allowRuleAccessFromOrigin;

    RawPtrWillBeMember<Node> m_ownerNode;
    RawPtrWillBeMember<CSSRule> m_ownerRule;

    TextPosition m_startPosition;
    bool m_loadCompleted;
    mutable RefPtrWillBeMember<MediaList> m_mediaCSSOMWrapper;
    mutable WillBeHeapVector<RefPtrWillBeMember<CSSRule>> m_childRuleCSSOMWrappers;
    mutable OwnPtrWillBeMember<CSSRuleList> m_ruleListCSSOMWrapper;
};

inline CSSStyleSheet::RuleMutationScope::RuleMutationScope(CSSStyleSheet* sheet)
    : m_styleSheet(sheet)
{
    if (m_styleSheet)
        m_styleSheet->willMutateRules();
}

inline CSSStyleSheet::RuleMutationScope::RuleMutationScope(CSSRule* rule)
    : m_styleSheet(rule ? rule->parentStyleSheet() : 0)
{
    if (m_styleSheet)
        m_styleSheet->willMutateRules();
}

inline CSSStyleSheet::RuleMutationScope::~RuleMutationScope()
{
    if (m_styleSheet)
        m_styleSheet->didMutateRules();
}

DEFINE_TYPE_CASTS(CSSStyleSheet, StyleSheet, sheet, sheet->isCSSStyleSheet(), sheet.isCSSStyleSheet());

} // namespace blink

#endif
