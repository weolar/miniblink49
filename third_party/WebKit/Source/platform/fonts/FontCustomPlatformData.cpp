/*
 * Copyright (C) 2007 Apple Computer, Inc.
 * Copyright (c) 2007, 2008, 2009, Google Inc. All rights reserved.
 * Copyright (C) 2010 Company 100, Inc.
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
#include "platform/fonts/FontCustomPlatformData.h"

#include "platform/LayoutTestSupport.h"
#include "platform/SharedBuffer.h"
#include "platform/fonts/FontCache.h"
#include "platform/fonts/FontPlatformData.h"
#include "platform/fonts/opentype/OpenTypeSanitizer.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

FontCustomPlatformData::FontCustomPlatformData(PassRefPtr<SkTypeface> typeface)
    : m_typeface(typeface) { }

FontCustomPlatformData::~FontCustomPlatformData()
{
}

FontPlatformData FontCustomPlatformData::fontPlatformData(float size, bool bold, bool italic, FontOrientation orientation)
{
    ASSERT(m_typeface);
#if OS(WIN)
    if (!FontCache::useDirectWrite()) {
        // FIXME: Skia currently renders synthetic bold and italics with
        // hinting and without linear metrics on the windows GDI backend
        // while the DirectWrite backend does the right thing. Using
        // CreateFromName and specifying the bold/italics style allows
        // for proper rendering of synthetic style. Once Skia has been
        // updated this workaround will no longer be needed.
        // http://crbug.com/332958
        bool syntheticBold = bold && !m_typeface->isBold();
        bool syntheticItalic = italic && !m_typeface->isItalic();
        if (syntheticBold || syntheticItalic) {
            SkString name;
            m_typeface->getFamilyName(&name);

            int style = SkTypeface::kNormal;
            if (syntheticBold)
                style |= SkTypeface::kBold;
            if (syntheticItalic)
                style |= SkTypeface::kItalic;

            RefPtr<SkTypeface> typeface = adoptRef(FontCache::fontCache()->fontManager()->legacyCreateTypeface(name.c_str(), static_cast<SkTypeface::Style>(style)));
            syntheticBold = false;
            syntheticItalic = false;
            return FontPlatformData(typeface.release(), "", size, syntheticBold, syntheticItalic, orientation);
        }
    }
#endif
    return FontPlatformData(m_typeface.get(), "", size, bold && !m_typeface->isBold(), italic && !m_typeface->isItalic(), orientation);
}

PassOwnPtr<FontCustomPlatformData> FontCustomPlatformData::create(SharedBuffer* buffer, String& otsParseMessage)
{
    ASSERT_ARG(buffer, buffer);
#ifndef MINIBLINK_NOT_OTS
    OpenTypeSanitizer sanitizer(buffer);
    RefPtr<SharedBuffer> transcodeBuffer = sanitizer.sanitize();

    if (!transcodeBuffer) {
        otsParseMessage = sanitizer.getErrorString();
        return nullptr; // validation failed.
    }
    buffer = transcodeBuffer.get();

    SkMemoryStream* stream = new SkMemoryStream(buffer->getAsSkData().get());
#if OS(WIN)
    RefPtr<SkTypeface> typeface = adoptRef(FontCache::fontCache()->fontManager()->createFromStream(stream));
#else
    RefPtr<SkTypeface> typeface = adoptRef(SkTypeface::CreateFromStream(stream));
#endif
    if (!typeface)
        return nullptr;

    return adoptPtr(new FontCustomPlatformData(typeface.release()));
#endif // MINIBLINK_NOT_IMPLEMENTED
    OutputDebugStringW(L"FontCustomPlatformData not Implemented\n");
	return nullptr;
}

bool FontCustomPlatformData::supportsFormat(const String& format)
{
    return equalIgnoringCase(format, "truetype") || equalIgnoringCase(format, "opentype") 
#ifndef MINIBLINK_NOT_OTS
        || OpenTypeSanitizer::supportsFormat(format)
#endif // MINIBLINK_NOT_IMPLEMENTED
        ;
}

} // namespace blink
