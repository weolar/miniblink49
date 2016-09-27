/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PagePopupController_h
#define PagePopupController_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"

namespace blink {

class Document;
class PagePopup;
class PagePopupClient;

class PagePopupController final : public RefCountedWillBeGarbageCollected<PagePopupController>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<PagePopupController> create(PagePopup&, PagePopupClient*);
    void setValueAndClosePopup(int numValue, const String& stringValue);
    void setValue(const String&);
    void closePopup();
    void selectFontsFromOwnerDocument(Document* targetDocument);
    String localizeNumberString(const String&);
    String formatMonth(int year, int zeroBaseMonth);
    String formatShortMonth(int year, int zeroBaseMonth);
    String formatWeek(int year, int weekNumber, const String& localizedStartDate);
    void clearPagePopupClient();
    void histogramEnumeration(const String& name, int sample, int boundaryValue);
    void setWindowRect(int x, int y, int width, int height);

    DEFINE_INLINE_TRACE() { }

private:
    PagePopupController(PagePopup&, PagePopupClient*);

    PagePopup& m_popup;
    PagePopupClient* m_popupClient;
};

} // namespace blink

#endif // PagePopupController_h
