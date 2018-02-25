// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_WEBCLIPBOARD_IMPL_H_
#define CONTENT_RENDERER_WEBCLIPBOARD_IMPL_H_

#include "base/compiler_specific.h"
#include "third_party/WebKit/public/platform/WebClipboard.h"

namespace blink {
class IntSize;
}

namespace content {

// This type designates which clipboard the action should be applied to.
// Only platforms that use the X Window System support the selection buffer.
// Drag type is only supported on Mac OS X.
enum ClipboardType {
    CLIPBOARD_TYPE_COPY_PASTE,
    CLIPBOARD_TYPE_SELECTION,
    CLIPBOARD_TYPE_DRAG,
    CLIPBOARD_TYPE_LAST = CLIPBOARD_TYPE_DRAG
};
    
class WebClipboardImpl : public blink::WebClipboard {
 public:
     explicit WebClipboardImpl();

     virtual ~WebClipboardImpl();

     // WebClipboard methods:
     uint64 sequenceNumber(blink::WebClipboard::Buffer buffer) override;
     bool isFormatAvailable(blink::WebClipboard::Format format, Buffer buffer) override;
     blink::WebVector<blink::WebString> readAvailableTypes(
         Buffer buffer,
         bool* contains_filenames) override;

     blink::WebString readPlainText(blink::WebClipboard::Buffer buffer) override;
     blink::WebString readHTML(blink::WebClipboard::Buffer buffer,
         blink::WebURL* source_url,
         unsigned* fragment_start,
         unsigned* fragment_end) override;
     blink::WebData readImage(blink::WebClipboard::Buffer buffer) override;
     blink::WebString readCustomData(blink::WebClipboard::Buffer buffer,
         const blink::WebString& type) override;

     void writePlainText(const blink::WebString& plain_text) override;
     void writeHTML(const blink::WebString& htmlText, const blink::WebURL& sourceUrl, const blink::WebString& plainText, bool writeSmartPaste) override;
     void writeImage(const blink::WebImage& image,
         const blink::WebURL& source_url,
         const blink::WebString& title) override;
     void writeDataObject(const blink::WebDragData& data) override;

private:
    void clearClipboard();
    void writeHTMLInternal(const blink::WebString& htmlText, const blink::WebURL& sourceUrl, const blink::WebString& plainText, bool writeSmartPaste);
    void writeBookmark(const String& titleData, const String& urlData);
    void writeBitmapFromHandle(HBITMAP source_hbitmap, const blink::IntSize& size);
    bool writeBitmap(const SkBitmap& bitmap);
    bool convertBufferType(Buffer buffer, ClipboardType* result);
    HWND getClipboardWindow();
    void readAvailableTypes(ClipboardType type, Vector<blink::WebString>* types, bool* containsFilenames) const;
    void writeToClipboard(unsigned int format, HANDLE handle);
    void writeText(String string);

    HWND m_clipboardOwner;
};

}  // namespace content

#endif  // CONTENT_RENDERER_WEBCLIPBOARD_IMPL_H_
