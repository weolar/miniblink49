/*
 * Copyright (c) 2013, Google Inc. All rights reserved.
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

#include "config.h"
#include "core/html/LinkRelAttribute.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/text/CString.h"
#include <gtest/gtest.h>

namespace blink {

class LinkRelAttributeTest : public testing::Test {
protected:
    void SetUp() override
    {
        m_touchIconLoadingEnabled = RuntimeEnabledFeatures::touchIconLoadingEnabled();
        m_presentationEnabled = RuntimeEnabledFeatures::presentationEnabled();
    }

    void TearDown() override
    {
        RuntimeEnabledFeatures::setTouchIconLoadingEnabled(m_touchIconLoadingEnabled);
        RuntimeEnabledFeatures::setPresentationEnabled(m_presentationEnabled);
    }

private:
    bool m_touchIconLoadingEnabled;
    bool m_presentationEnabled;
};

static inline void testLinkRelAttribute(String value, bool isStyleSheet, IconType iconType, bool isAlternate, bool isDNSPrefetch, bool isLinkSubresource, bool isLinkPrerender, bool isImport = false, bool isPreconnect = false)
{
    LinkRelAttribute linkRelAttribute(value);
    ASSERT_EQ(isStyleSheet, linkRelAttribute.isStyleSheet()) << value.utf8().data();
    ASSERT_EQ(iconType, linkRelAttribute.iconType()) << value.utf8().data();
    ASSERT_EQ(isAlternate, linkRelAttribute.isAlternate()) << value.utf8().data();
    ASSERT_EQ(isDNSPrefetch, linkRelAttribute.isDNSPrefetch()) << value.utf8().data();
    ASSERT_EQ(isLinkSubresource, linkRelAttribute.isLinkSubresource()) << value.utf8().data();
    ASSERT_EQ(isLinkPrerender, linkRelAttribute.isLinkPrerender()) << value.utf8().data();
    ASSERT_EQ(isImport, linkRelAttribute.isImport()) << value.utf8().data();
    ASSERT_EQ(isPreconnect, linkRelAttribute.isPreconnect()) << value.utf8().data();
}

TEST_F(LinkRelAttributeTest, Constructor)
{
    RuntimeEnabledFeatures::setTouchIconLoadingEnabled(false);

    testLinkRelAttribute("stylesheet", true, InvalidIcon, false, false, false, false);
    testLinkRelAttribute("sTyLeShEeT", true, InvalidIcon, false, false, false, false);

    testLinkRelAttribute("icon", false, Favicon, false, false, false, false);
    testLinkRelAttribute("iCoN", false, Favicon, false, false, false, false);
    testLinkRelAttribute("shortcut icon", false, Favicon, false, false, false, false);
    testLinkRelAttribute("sHoRtCuT iCoN", false, Favicon, false, false, false, false);

    testLinkRelAttribute("dns-prefetch", false, InvalidIcon, false, true, false, false);
    testLinkRelAttribute("dNs-pReFeTcH", false, InvalidIcon, false, true, false, false);

    testLinkRelAttribute("apple-touch-icon", false, InvalidIcon, false, false, false, false);
    testLinkRelAttribute("aPpLe-tOuCh-IcOn", false, InvalidIcon, false, false, false, false);
    testLinkRelAttribute("apple-touch-icon-precomposed", false, InvalidIcon, false, false, false, false);
    testLinkRelAttribute("aPpLe-tOuCh-IcOn-pReCoMpOsEd", false, InvalidIcon, false, false, false, false);

    testLinkRelAttribute("alternate stylesheet", true, InvalidIcon, true, false, false, false);
    testLinkRelAttribute("stylesheet alternate", true, InvalidIcon, true, false, false, false);
    testLinkRelAttribute("aLtErNaTe sTyLeShEeT", true, InvalidIcon, true, false, false, false);
    testLinkRelAttribute("sTyLeShEeT aLtErNaTe", true, InvalidIcon, true, false, false, false);

    testLinkRelAttribute("stylesheet icon prerender aLtErNaTe", true, Favicon, true, false, false, true);
    testLinkRelAttribute("alternate subresource", false, InvalidIcon, true, false, true, false);
    testLinkRelAttribute("alternate icon stylesheet", true, Favicon, true, false, false, false);

    testLinkRelAttribute("import", false, InvalidIcon, false, false, false, false, true);
    // "import" is mutually exclusive and "stylesheet" wins when they conflict.
    testLinkRelAttribute("stylesheet import", true, InvalidIcon, false, false, false, false, false);

    testLinkRelAttribute("preconnect", false, InvalidIcon, false, false, false, false, false, true);
    testLinkRelAttribute("pReCoNnEcT", false, InvalidIcon, false, false, false, false, false, true);
}

TEST_F(LinkRelAttributeTest, ConstructorTouchIconLoadingEnabled)
{
    RuntimeEnabledFeatures::setTouchIconLoadingEnabled(true);

    testLinkRelAttribute("stylesheet", true, InvalidIcon, false, false, false, false);
    testLinkRelAttribute("sTyLeShEeT", true, InvalidIcon, false, false, false, false);

    testLinkRelAttribute("icon", false, Favicon, false, false, false, false);
    testLinkRelAttribute("iCoN", false, Favicon, false, false, false, false);
    testLinkRelAttribute("shortcut icon", false, Favicon, false, false, false, false);
    testLinkRelAttribute("sHoRtCuT iCoN", false, Favicon, false, false, false, false);

    testLinkRelAttribute("dns-prefetch", false, InvalidIcon, false, true, false, false);
    testLinkRelAttribute("dNs-pReFeTcH", false, InvalidIcon, false, true, false, false);
    testLinkRelAttribute("alternate dNs-pReFeTcH", false, InvalidIcon, true, true, false, false);

    testLinkRelAttribute("apple-touch-icon", false, TouchIcon, false, false, false, false);
    testLinkRelAttribute("aPpLe-tOuCh-IcOn", false, TouchIcon, false, false, false, false);
    testLinkRelAttribute("apple-touch-icon-precomposed", false, TouchPrecomposedIcon, false, false, false, false);
    testLinkRelAttribute("aPpLe-tOuCh-IcOn-pReCoMpOsEd", false, TouchPrecomposedIcon, false, false, false, false);

    testLinkRelAttribute("alternate stylesheet", true, InvalidIcon, true, false, false, false);
    testLinkRelAttribute("stylesheet alternate", true, InvalidIcon, true, false, false, false);
    testLinkRelAttribute("aLtErNaTe sTyLeShEeT", true, InvalidIcon, true, false, false, false);
    testLinkRelAttribute("sTyLeShEeT aLtErNaTe", true, InvalidIcon, true, false, false, false);

    testLinkRelAttribute("stylesheet icon prerender aLtErNaTe", true, Favicon, true, false, false, true);
    testLinkRelAttribute("alternate subresource", false, InvalidIcon, true, false, true, false);
    testLinkRelAttribute("alternate icon stylesheet", true, Favicon, true, false, false, false);

    testLinkRelAttribute("import", false, InvalidIcon, false, false, false, false, true);
    testLinkRelAttribute("alternate import", false, InvalidIcon, true, false, false, false, true);
    testLinkRelAttribute("stylesheet import", true, InvalidIcon, false, false, false, false, false);

    testLinkRelAttribute("preconnect", false, InvalidIcon, false, false, false, false, false, true);
    testLinkRelAttribute("pReCoNnEcT", false, InvalidIcon, false, false, false, false, false, true);
}

TEST_F(LinkRelAttributeTest, ConstructorDefaultPresentation)
{
    RuntimeEnabledFeatures::setPresentationEnabled(false);
    EXPECT_FALSE(LinkRelAttribute("default-presentation").isDefaultPresentation());

    RuntimeEnabledFeatures::setPresentationEnabled(true);
    EXPECT_TRUE(LinkRelAttribute("default-presentation").isDefaultPresentation());
}

} // namespace blink
