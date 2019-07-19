// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "common/api/ApiNativeImage.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"
#include "gin/dictionary.h"
#include "base/strings/string_util.h"
#include <vector>

namespace atom {

class Clipboard : public mate::EventEmitter<Clipboard> {
public:
    Clipboard(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<Clipboard>::InitWith(isolate, wrapper);
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Clipboard"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("_readImage", &Clipboard::_readImageApi);
        builder.SetMethod("_readText", &Clipboard::_readTextApi);
        builder.SetMethod("_writeText", &Clipboard::_writeTextApi);
        builder.SetMethod("_writeImage", &Clipboard::_writeImageApi);
        builder.SetMethod("_clear", &Clipboard::_clearApi);
        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Clipboard"), prototype->GetFunction());
    }

    void _clearApi(const std::string& type) {
        ::EmptyClipboard();
    }

    void _writeImageApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        NativeImage* nativeImage = nullptr;
        if (args[0]->IsObject()) {
            v8::Local<v8::Object> handle = args[0]->ToObject();
            nativeImage = NativeImage::GetSelf(handle);
        }
        if (!nativeImage)
            return;

        std::string type;
        if (args[1]->IsString()) {
            v8::Local<v8::String> str = args[0]->ToString();
            v8::String::Utf8Value stringUtf8(str);
            if (0 != stringUtf8.length())
                type.assign(*stringUtf8);
        }

        HBITMAP sourceBitmap = nativeImage->getBitmap();
        int width = nativeImage->getWidth();
        int height = nativeImage->getHeight();
        if (!sourceBitmap || 0 == width || 0 == height)
            return;

        ::EmptyClipboard();

        HDC dc = ::GetDC(NULL);
        HDC compatibleDC = ::CreateCompatibleDC(NULL);
        HDC sourceDC = ::CreateCompatibleDC(NULL);

        // This is the HBITMAP we will eventually write to the clipboard
        HBITMAP hbitmap = ::CreateCompatibleBitmap(dc, width, height);
        if (!hbitmap) {
            // Failed to create the bitmap
            ::DeleteDC(compatibleDC);
            ::DeleteDC(sourceDC);
            ::ReleaseDC(NULL, dc);
            return;
        }

        HBITMAP oldBitmap = (HBITMAP)SelectObject(compatibleDC, hbitmap);
        HBITMAP oldSource = (HBITMAP)SelectObject(sourceDC, sourceBitmap);

        // Now we need to blend it into an HBITMAP we can place on the clipboard
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        ::GdiAlphaBlend(compatibleDC, 0, 0, width, height, sourceDC, 0, 0, width, height, bf);

        // Clean up all the handles we just opened
        ::SelectObject(compatibleDC, oldBitmap);
        ::SelectObject(sourceDC, oldSource);
        ::DeleteObject(oldBitmap);
        ::DeleteObject(oldSource);
        ::DeleteDC(compatibleDC);
        ::DeleteDC(sourceDC);
        ::ReleaseDC(NULL, dc);

        ::OpenClipboard(NULL);
        ::SetClipboardData(CF_BITMAP, hbitmap);
        ::CloseClipboard();
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            new Clipboard(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

    std::string _readTextApi(const std::string& type) {
        if (!::OpenClipboard(nullptr))
            return std::string();

        HANDLE data = ::GetClipboardData(CF_UNICODETEXT);
        if (!data) {
            ::CloseClipboard();
            return std::string();
        }

        LPCWSTR dataText = (LPCWSTR)::GlobalLock(data);
        std::wstring text(dataText, wcslen(dataText));
        ::GlobalUnlock(data);        

        ::CloseClipboard();
        return base::WideToUTF8(text);
    }

    void _writeTextApi(const std::string& text, const std::string& type) {
        if (0 == text.size())
            return;

        std::wstring strW(base::UTF8ToWide(text));
        HGLOBAL data = ::GlobalAlloc(GMEM_MOVEABLE, ((strW.size() + 1) * sizeof(wchar_t)));
        if (!data)
            return;

        wchar_t* rawData = static_cast<wchar_t*>(::GlobalLock(data));
        memcpy(rawData, &strW[0], strW.size() * sizeof(wchar_t));
        rawData[strW.size()] = L'\0';
        ::GlobalUnlock(data);

        ::EmptyClipboard();

        if (!::OpenClipboard(nullptr))
            return;
        ::SetClipboardData(CF_UNICODETEXT, data);
        ::CloseClipboard();
    }

    v8::Local<v8::Object> readImage(const std::string& type) {
        if (!::OpenClipboard(nullptr))
            return NativeImage::createEmpty(isolate());

        HANDLE hBitmap = ::GetClipboardData(CF_DIB);
        if (!hBitmap) {
            ::CloseClipboard();
            return NativeImage::createEmpty(isolate());
        }

        BITMAPINFO* bitmap = static_cast<BITMAPINFO*>(::GlobalLock(hBitmap));
        if (!bitmap) {
            ::CloseClipboard();
            return NativeImage::createEmpty(isolate());
        }
        int colorTableLength = 0;
        switch (bitmap->bmiHeader.biBitCount) {
        case 1:
        case 4:
        case 8:
            colorTableLength = bitmap->bmiHeader.biClrUsed ? bitmap->bmiHeader.biClrUsed : 1 << bitmap->bmiHeader.biBitCount;
            break;
        case 16:
        case 32:
            if (bitmap->bmiHeader.biCompression == BI_BITFIELDS)
                colorTableLength = 3;
            break;
        case 24:
            break;
        default:
            DebugBreak();
        }
        void* bitmapBits = reinterpret_cast<char*>(bitmap) + bitmap->bmiHeader.biSize + colorTableLength * sizeof(RGBQUAD);
        size_t size = bitmap->bmiHeader.biWidth * bitmap->bmiHeader.biHeight * 4;
        v8::Local<v8::Object> obj = NativeImage::createFromBITMAPINFO(isolate(), bitmap, bitmapBits);
        ::GlobalUnlock(hBitmap);
        ::CloseClipboard();
        return obj;
    }

    v8::Local<v8::Object> _readImageApi(const std::string& type) {
        return readImage(type);
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
};
v8::Persistent<v8::Function> Clipboard::constructor;
gin::WrapperInfo Clipboard::kWrapperInfo = { gin::kEmbedderNativeGin };

void initializeClipboardApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Clipboard::init(env->isolate(), exports);
}

}  // atom namespace

static const char CommonClipboardNative[] = "console.log('CommonClipboardNative');;";
static NodeNative nativeCommonClipboard{ "Clipboard", CommonClipboardNative, sizeof(CommonClipboardNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_common_clipboard, atom::initializeClipboardApi, &nativeCommonClipboard)
