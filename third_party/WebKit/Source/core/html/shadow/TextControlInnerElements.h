/*
 * Copyright (C) 2006, 2008, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TextControlInnerElements_h
#define TextControlInnerElements_h

#include "core/html/HTMLDivElement.h"
#include "wtf/Forward.h"

namespace blink {

class TextControlInnerContainer final : public HTMLDivElement {
public:
    static PassRefPtrWillBeRawPtr<TextControlInnerContainer> create(Document&);

protected:
    explicit TextControlInnerContainer(Document&);
    LayoutObject* createLayoutObject(const ComputedStyle&) override;
};

class EditingViewPortElement final : public HTMLDivElement {
public:
    static PassRefPtrWillBeRawPtr<EditingViewPortElement> create(Document&);

protected:
    explicit EditingViewPortElement(Document&);
    PassRefPtr<ComputedStyle> customStyleForLayoutObject() override;

private:
    bool supportsFocus() const override { return false; }
};

class TextControlInnerEditorElement final : public HTMLDivElement {
public:
    static PassRefPtrWillBeRawPtr<TextControlInnerEditorElement> create(Document&);

    void defaultEventHandler(Event*) override;

private:
    explicit TextControlInnerEditorElement(Document&);
    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    PassRefPtr<ComputedStyle> customStyleForLayoutObject() override;
    bool supportsFocus() const override { return false; }
};

class SearchFieldDecorationElement final : public HTMLDivElement {
public:
    static PassRefPtrWillBeRawPtr<SearchFieldDecorationElement> create(Document&);

    void defaultEventHandler(Event*) override;
    bool willRespondToMouseClickEvents() override;

private:
    explicit SearchFieldDecorationElement(Document&);
    const AtomicString& shadowPseudoId() const override;
    bool supportsFocus() const override { return false; }
};

class SearchFieldCancelButtonElement final : public HTMLDivElement {
public:
    static PassRefPtrWillBeRawPtr<SearchFieldCancelButtonElement> create(Document&);

    void defaultEventHandler(Event*) override;
    bool willRespondToMouseClickEvents() override;

private:
    explicit SearchFieldCancelButtonElement(Document&);
    void detach(const AttachContext& = AttachContext()) override;
    bool supportsFocus() const override { return false; }

    bool m_capturing;
};

} // namespace

#endif
