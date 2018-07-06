// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebMimeRegistryImpl_h
#define WebMimeRegistryImpl_h

#include <string>

#include "base/compiler_specific.h"
#include "third_party/WebKit/public/platform/WebMimeRegistry.h"
#include "third_party/WebKit/Source/wtf/HashSet.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"

namespace content {

class WebMimeRegistryImpl :
    NON_EXPORTED_BASE(public blink::WebMimeRegistry) {
public:
    WebMimeRegistryImpl();
    virtual ~WebMimeRegistryImpl();

    // WebMimeRegistry methods:
    virtual blink::WebMimeRegistry::SupportsType supportsMIMEType(
        const blink::WebString&) OVERRIDE;

    virtual blink::WebMimeRegistry::SupportsType supportsImageMIMEType(
        const blink::WebString&) OVERRIDE;

    virtual blink::WebMimeRegistry::SupportsType supportsJavaScriptMIMEType(
        const blink::WebString&) OVERRIDE;

    virtual blink::WebMimeRegistry::SupportsType supportsMediaMIMEType(
        const blink::WebString&,
        const blink::WebString&,
        const blink::WebString&) OVERRIDE;

    virtual bool supportsMediaSourceMIMEType(const blink::WebString&,
        const blink::WebString&) OVERRIDE;

    virtual blink::WebMimeRegistry::SupportsType supportsNonImageMIMEType(
        const blink::WebString&) OVERRIDE;

    virtual blink::WebString mimeTypeForExtension(const blink::WebString&) OVERRIDE;

    virtual blink::WebString wellKnownMimeTypeForExtension(
        const blink::WebString&) OVERRIDE;

    virtual blink::WebString mimeTypeFromFile(const blink::WebString&) OVERRIDE;

    virtual blink::WebMimeRegistry::SupportsType supportsImagePrefixedMIMEType(const blink::WebString& mimeType) OVERRIDE;

    blink::WebString extensionForMimeType(const blink::WebString& mime);
    Vector<blink::WebString> extensionsForMimeType(const blink::WebString& mime);

private:
    void ensureMimeTypeMap();

    WTF::HashSet<WTF::String>* m_supportedMIMETypes;
    WTF::HashSet<WTF::String>* m_supportedImageResourceMIMETypes;
    WTF::HashSet<WTF::String>* m_supportedJavaScriptMIMETypes;
    WTF::HashSet<String>* m_supportedNonImageMIMETypes;
    WTF::HashMap<WTF::String, WTF::String>* m_mimetypeMap;
};

}  // namespace content

#endif  // WebMimeRegistryImpl_h
