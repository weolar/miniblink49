/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef HTMLDialogElement_h
#define HTMLDialogElement_h

#include "core/html/HTMLElement.h"

namespace blink {

class Document;
class ExceptionState;
class QualifiedName;

class HTMLDialogElement final : public HTMLElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(HTMLDialogElement);

    void close(const String& returnValue, ExceptionState&);
    void closeDialog(const String& returnValue = String());
    void show();
    void showModal(ExceptionState&);
    void removedFrom(ContainerNode*) override;

    // NotCentered means do not center the dialog. Centered means the dialog has
    // been centered and centeredPosition() is set. NeedsCentering means attempt
    // to center on the next layout, then set to Centered or NotCentered.
    enum CenteringMode { NotCentered, Centered, NeedsCentering };
    CenteringMode centeringMode() const { return m_centeringMode; }
    LayoutUnit centeredPosition() const
    {
        ASSERT(m_centeringMode == Centered);
        return m_centeredPosition;
    }
    void setCentered(LayoutUnit centeredPosition);
    void setNotCentered();

    String returnValue() const { return m_returnValue; }
    void setReturnValue(const String& returnValue) { m_returnValue = returnValue; }

private:
    explicit HTMLDialogElement(Document&);

    bool isPresentationAttribute(const QualifiedName&) const override;
    void defaultEventHandler(Event*) override;

    void forceLayoutForCentering();

    CenteringMode m_centeringMode;
    LayoutUnit m_centeredPosition;
    String m_returnValue;
};

} // namespace blink

#endif // HTMLDialogElement_h
