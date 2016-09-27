/*
 * Copyright (c) 2008, 2009, Google Inc. All rights reserved.
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
#include "platform/MIMETypeRegistry.h"

#include "platform/plugins/PluginData.h"
#include "public/platform/Platform.h"
#include "public/platform/WebMimeRegistry.h"
#include "wtf/text/CString.h"

namespace blink {

String MIMETypeRegistry::getMIMETypeForExtension(const String &ext)
{
    return Platform::current()->mimeRegistry()->mimeTypeForExtension(ext);
}

String MIMETypeRegistry::getWellKnownMIMETypeForExtension(const String &ext)
{
    // This method must be thread safe and should not consult the OS/registry.
    return Platform::current()->mimeRegistry()->wellKnownMimeTypeForExtension(ext);
}

String MIMETypeRegistry::getMIMETypeForPath(const String& path)
{
    int pos = path.reverseFind('.');
    if (pos < 0)
        return "application/octet-stream";
    String extension = path.substring(pos + 1);
    String mimeType = getMIMETypeForExtension(extension);
    if (mimeType.isEmpty()) {
        // If there's no mimetype registered for the extension, check to see
        // if a plugin can handle the extension.
        mimeType = getPluginMimeTypeFromExtension(extension);
    }
    if (mimeType.isEmpty())
        return "application/octet-stream";
    return mimeType;
}

bool MIMETypeRegistry::isSupportedImageMIMEType(const String& mimeType)
{
    return Platform::current()->mimeRegistry()->supportsImageMIMEType(mimeType.lower())
        != WebMimeRegistry::IsNotSupported;
}

bool MIMETypeRegistry::isSupportedImageResourceMIMEType(const String& mimeType)
{
    return isSupportedImageMIMEType(mimeType);
}

bool MIMETypeRegistry::isSupportedImagePrefixedMIMEType(const String& mimeType)
{
    return Platform::current()->mimeRegistry()->supportsImagePrefixedMIMEType(mimeType.lower())
        != WebMimeRegistry::IsNotSupported;
}

bool MIMETypeRegistry::isSupportedImageMIMETypeForEncoding(const String& mimeType)
{
    if (equalIgnoringCase(mimeType, "image/jpeg") || equalIgnoringCase(mimeType, "image/png"))
        return true;
    if (equalIgnoringCase(mimeType, "image/webp"))
        return true;
    return false;
}

bool MIMETypeRegistry::isSupportedJavaScriptMIMEType(const String& mimeType)
{
    return Platform::current()->mimeRegistry()->supportsJavaScriptMIMEType(mimeType.lower())
        != WebMimeRegistry::IsNotSupported;
}

bool MIMETypeRegistry::isSupportedNonImageMIMEType(const String& mimeType)
{
    return Platform::current()->mimeRegistry()->supportsNonImageMIMEType(mimeType.lower())
        != WebMimeRegistry::IsNotSupported;
}

bool MIMETypeRegistry::isSupportedMediaSourceMIMEType(const String& mimeType, const String& codecs)
{
    return !mimeType.isEmpty()
        && Platform::current()->mimeRegistry()->supportsMediaSourceMIMEType(mimeType.lower(), codecs);
}

bool MIMETypeRegistry::isJavaAppletMIMEType(const String& mimeType)
{
    // Since this set is very limited and is likely to remain so we won't bother with the overhead
    // of using a hash set.
    // Any of the MIME types below may be followed by any number of specific versions of the JVM,
    // which is why we use startsWith()
    return mimeType.startsWith("application/x-java-applet", TextCaseInsensitive)
        || mimeType.startsWith("application/x-java-bean", TextCaseInsensitive)
        || mimeType.startsWith("application/x-java-vm", TextCaseInsensitive);
}

} // namespace blink
