/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DocumentTiming_h
#define DocumentTiming_h

namespace blink {

class DocumentTiming {
public:
    DocumentTiming();

    void setDomLoading(double);
    void setDomInteractive(double);
    void setDomContentLoadedEventStart(double);
    void setDomContentLoadedEventEnd(double);
    void setDomComplete(double);
    void setFirstLayout(double);

    double domLoading() const { return m_domLoading; }
    double domInteractive() const { return m_domInteractive; }
    double domContentLoadedEventStart() const { return m_domContentLoadedEventStart; }
    double domContentLoadedEventEnd() const { return m_domContentLoadedEventEnd; }
    double domComplete() const { return m_domComplete; }
    double firstLayout() const { return m_firstLayout; }

private:
    double m_domLoading;
    double m_domInteractive;
    double m_domContentLoadedEventStart;
    double m_domContentLoadedEventEnd;
    double m_domComplete;
    double m_firstLayout;
};

}

#endif
