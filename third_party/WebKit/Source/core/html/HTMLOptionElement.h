/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
 *
 */

#ifndef HTMLOptionElement_h
#define HTMLOptionElement_h

#include "core/CoreExport.h"
#include "core/html/HTMLElement.h"

namespace blink {

class ExceptionState;
class HTMLDataListElement;
class HTMLSelectElement;
class ComputedStyle;

class CORE_EXPORT HTMLOptionElement final : public HTMLElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<HTMLOptionElement> create(Document&);
    static PassRefPtrWillBeRawPtr<HTMLOptionElement> createForJSConstructor(Document&, const String& data, const AtomicString& value,
        bool defaultSelected, bool selected, ExceptionState&);

    String text() const;
    void setText(const String&, ExceptionState&);

    int index() const;

    String value() const;
    void setValue(const AtomicString&);

    bool selected() const;
    void setSelected(bool);

    HTMLDataListElement* ownerDataListElement() const;
    HTMLSelectElement* ownerSelectElement() const;

    String label() const;
    void setLabel(const AtomicString&);

    bool ownElementDisabled() const { return m_disabled; }

    bool isDisabledFormControl() const override;

    String textIndentedToRespectGroupLabel() const;

    void setSelectedState(bool);

    HTMLFormElement* form() const;
    bool spatialNavigationFocused() const;

    bool isDisplayNone() const;

    int listIndex() const;

private:
    explicit HTMLOptionElement(Document&);
    ~HTMLOptionElement();

    bool supportsFocus() const override;
    void attach(const AttachContext& = AttachContext()) override;
    void detach(const AttachContext& = AttachContext()) override;
    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void didNotifySubtreeInsertionsToDocument() override;
    void removedFrom(ContainerNode*) override;
    void accessKeyAction(bool) override;
    void childrenChanged(const ChildrenChange&) override;

    // <option> never has a layoutObject so we manually manage a cached style.
    void updateNonComputedStyle();
    ComputedStyle* nonLayoutObjectComputedStyle() const override;
    PassRefPtr<ComputedStyle> customStyleForLayoutObject() override;
    void didAddUserAgentShadowRoot(ShadowRoot&) override;

    String collectOptionInnerText() const;

    void updateLabel();

    bool m_disabled;
    bool m_isSelected;
    RefPtr<ComputedStyle> m_style;
};

} // namespace blink

#endif // HTMLOptionElement_h
