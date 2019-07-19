// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"
#include "node/src/node_buffer.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/ApiNativeImage.h"
#include "common/asar/AsarUtil.h"
#include "common/InitGdiPlus.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"
#include "gin/dictionary.h"
#include "base/strings/string_util.h"

#undef min
#undef max
using std::max;
using std::min;

#include <Unknwn.h>
#include <gdiplus.h>
#include <objidl.h>
#include <vector>

namespace atom {
    
NativeImage::NativeImage(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
    m_gdipBitmap = nullptr;
    gin::Wrappable<NativeImage>::InitWith(isolate, wrapper);
}

void NativeImage::init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
    v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

    prototype->SetClassName(v8::String::NewFromUtf8(isolate, "NativeImage"));
    gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
    builder.SetMethod("toPNG", &NativeImage::toPNGAPI);

    constructor.Reset(isolate, prototype->GetFunction());
    target->Set(v8::String::NewFromUtf8(isolate, "NativeImage"), prototype->GetFunction());

    gin::Dictionary nativeImageClass(isolate, prototype->GetFunction());
    nativeImageClass.SetMethod("createEmpty", &NativeImage::createEmptyApi);
    nativeImageClass.SetMethod("createFromPath", &NativeImage::createFromPathApi);
}

std::vector<unsigned char>* NativeImage::encodeToBuffer(const CLSID* clsid) {
    IStream* pIStream = nullptr;

    std::vector<unsigned char>* output = nullptr;
    bool ok = false;
    HRESULT hr = ::CreateStreamOnHGlobal(NULL, true, &pIStream);
    if (S_OK != hr)
        return nullptr;
    Gdiplus::Status status = m_gdipBitmap->Save(pIStream, clsid, NULL);

    LARGE_INTEGER liTemp = { 0 };
    pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
    DWORD dwSize = 0;
    STATSTG stats = { 0 };
    pIStream->Stat(&stats, 0);

    do {
        if (0 == stats.cbSize.QuadPart || stats.cbSize.QuadPart > 2024 * 2024)
            break;

        dwSize = (DWORD)stats.cbSize.QuadPart;
        output = new std::vector<unsigned char>();
        output->resize(dwSize);

        ULONG readSize = 0;
        hr = pIStream->Read(output->data(), dwSize, &readSize);
        ok = (S_OK != hr);
    } while (false);

    if (!ok && output) {
        delete output;
        output = nullptr;
    }

    if (pIStream)
        pIStream->Release();

    return output;
}

v8::Local<v8::Object> NativeImage::toPNGAPI(const base::DictionaryValue& args) {
    double scaleFactor = 1.0;
    args.GetDouble("scaleFactor", &scaleFactor);

    std::vector<unsigned char>* output = encodeToBuffer(&s_pngClsid);
    if (!output)
        return v8::Local<v8::Object>();

    const char* data = reinterpret_cast<const char*>(output->data());
    size_t size = output->size();
    v8::Local<v8::Object> result = node::Buffer::Copy(isolate(), data, size).ToLocalChecked();

    delete output;

    return result;
}

v8::Local<v8::Object> NativeImage::toJpeg(const base::DictionaryValue& args) {
    int quality = 100;
    args.GetInteger("quality", &quality);

    std::vector<unsigned char>* output = encodeToBuffer(&s_jpgClsid);
    if (!output)
        return v8::Local<v8::Object>();

    const char* data = reinterpret_cast<const char*>(output->data());
    size_t size = output->size();
    v8::Local<v8::Object> result = node::Buffer::Copy(isolate(), data, size).ToLocalChecked();
    delete output;

    return result;
}

v8::Local<v8::Object> NativeImage::toBitmap(const base::DictionaryValue& args) {
    int quality = 100;
    args.GetInteger("quality", &quality);

    UINT w = m_gdipBitmap->GetWidth();
    UINT h = m_gdipBitmap->GetHeight();

    Gdiplus::Rect rect(0, 0, w, h);
    Gdiplus::BitmapData lockedBitmapData;
    m_gdipBitmap->LockBits(
#if _DEBUG
        &
#endif
        rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &lockedBitmapData);

    v8::Local<v8::Object> result;
    const char* data = reinterpret_cast<const char*>(lockedBitmapData.Scan0);
    if (!data)
        return result;

    int stride = lockedBitmapData.Stride;
    size_t size = w * stride / 4 + h;
    result = node::Buffer::Copy(isolate(), data, size).ToLocalChecked();

    return result;
}
 
v8::Local<v8::Object> NativeImage::createEmpty(v8::Isolate* isolate) {
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, constructor);
    v8::Local<v8::Object> obj = constructorFunction->NewInstance();

    NativeImage* self = (NativeImage*)WrappableBase::GetNativePtr(obj, &kWrapperInfo);
    return obj;
}

void NativeImage::createEmptyApi(const v8::FunctionCallbackInfo<v8::Value> info) {
    info.GetReturnValue().Set(createEmpty(info.GetIsolate()));
}

void NativeImage::createFromPathApi(const v8::FunctionCallbackInfo<v8::Value> info/*v8::Isolate* isolate, const std::string& path*/) {
    std::string fileContents;
    std::string path;
    if (info.Length() == 1 && info[0]->IsString()) {
        v8::String::Utf8Value pathString(info[0]);
        path = *pathString;
    }

    if (0 == path.size() || !asar::ReadFileToString(base::UTF8ToWide(path).c_str(), &fileContents))
        return;

    const unsigned char* data = reinterpret_cast<const unsigned char*>(fileContents.data());
    size_t size = fileContents.size();

    v8::Local<v8::Object> obj = createNativeImageFromBuffer(info.GetIsolate(), data, size);
    info.GetReturnValue().Set(obj);
}

void NativeImage::createFromBufferApi(const v8::FunctionCallbackInfo<v8::Value> info) {
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Value> buffer;
    v8::Local<v8::Object> resultObj;
    if (info.Length() == 1) {
        buffer = info[0];
        if (!buffer->IsUint8Array()) {
            info.GetReturnValue().Set(resultObj);
            return;
        }
    }
    int width = 0;
    int height = 0;
    double scaleFactor = 1.0;
    if (info.Length() == 2 && info[1]->IsObject()) {
        gin::Dictionary options(isolate, info[1]->ToObject());
        options.GetBydefaultVal("width", width, &width);
        options.GetBydefaultVal("height", height, &height);
        options.GetBydefaultVal("scaleFactor", scaleFactor, &scaleFactor);
    }

    unsigned char* data = reinterpret_cast<unsigned char*>(node::Buffer::Data(buffer));
    size_t size = node::Buffer::Length(buffer);

    resultObj = createNativeImageFromBuffer(isolate, data, size);
    info.GetReturnValue().Set(resultObj);
}

v8::Local<v8::Object> NativeImage::createFromBITMAPINFO(v8::Isolate* isolate, const BITMAPINFO* gdiBitmapInfo, void* gdiBitmapData) {
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, NativeImage::constructor);
    v8::Local<v8::Object> resultObj = constructorFunction->NewInstance();
    NativeImage* self = (NativeImage*)WrappableBase::GetNativePtr(resultObj, &NativeImage::kWrapperInfo);
    self->m_gdipBitmap = Gdiplus::Bitmap::FromBITMAPINFO(gdiBitmapInfo, gdiBitmapData);

    return resultObj;
}

void NativeImage::createFromBufferImpl(const unsigned char* data, size_t size) {
    HGLOBAL memHandle = ::GlobalAlloc(GMEM_FIXED, size);
    BYTE* pMem = (BYTE*)::GlobalLock(memHandle);
    memcpy(pMem, data, size);
    ::GlobalUnlock(memHandle);

    IStream* istream = nullptr;
    ::CreateStreamOnHGlobal(memHandle, FALSE, &istream);
    m_gdipBitmap = Gdiplus::Bitmap::FromStream(istream);

    if (memHandle)
        ::GlobalFree(memHandle);

    if (istream)
        istream->Release();
}

v8::Local<v8::Object> NativeImage::createNativeImageFromBuffer(v8::Isolate* isolate, const unsigned char* data, size_t size) {
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, NativeImage::constructor);
    v8::Local<v8::Object> resultObj = constructorFunction->NewInstance();
    NativeImage* self = (NativeImage*)WrappableBase::GetNativePtr(resultObj, &NativeImage::kWrapperInfo);
    self->createFromBufferImpl(data, size);

    return resultObj;
}

void NativeImage::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    if (args.IsConstructCall()) {
        new NativeImage(isolate, args.This());
        args.GetReturnValue().Set(args.This());
        return;
    }
}

NativeImage* NativeImage::GetSelf(v8::Local<v8::Object> handle) {
    return (NativeImage*)WrappableBase::GetNativePtr(handle, &kWrapperInfo);
}

HICON NativeImage::getIcon() {
    HICON hIcon = NULL;
    if (m_gdipBitmap)
        m_gdipBitmap->GetHICON(&hIcon);
    return hIcon;
}

HBITMAP NativeImage::getBitmap() {
    HBITMAP hBitmap = NULL;
    Gdiplus::Color colorBackground = 0xff000000;
    if (m_gdipBitmap)
        m_gdipBitmap->GetHBITMAP(colorBackground, &hBitmap);
    return hBitmap;
}

int NativeImage::getWidth() const {
    if (m_gdipBitmap)
        return m_gdipBitmap->GetWidth();
    return 0;
}
int NativeImage::getHeight() const {
    if (m_gdipBitmap)
        return m_gdipBitmap->GetHeight();
    return 0;
}

v8::Persistent<v8::Function> NativeImage::constructor;
gin::WrapperInfo NativeImage::kWrapperInfo = { gin::kEmbedderNativeGin };

void initializeNativeImageApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv) {
    node::Environment* env = node::Environment::GetCurrent(context);
    NativeImage::init(env->isolate(), exports);
}

}  // atom namespace

static const char CommonNativeImageNative[] = "console.log('BrowserNativeImageNative');;";
static NodeNative nativeCommonNativeImageNative{ "NativeImage", CommonNativeImageNative, sizeof(CommonNativeImageNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_common_nativeImage, atom::initializeNativeImageApi, &nativeCommonNativeImageNative)
