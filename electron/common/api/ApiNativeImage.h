
#ifndef common_api_ApiNativeImageExport_h
#define common_api_ApiNativeImageExport_h

#include "common/api/EventEmitter.h"
#include "v8.h"

namespace Gdiplus {
class Bitmap;
}

namespace atom {

class NativeImage : public mate::EventEmitter<NativeImage> {
public:
    NativeImage(v8::Isolate* isolate, v8::Local<v8::Object> wrapper);
    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target);
    std::vector<unsigned char>* encodeToBuffer(const CLSID* clsid);
    v8::Local<v8::Object> toPNGAPI(const base::DictionaryValue& args);
    v8::Local<v8::Object> toJpeg(const base::DictionaryValue& args);
    v8::Local<v8::Object> toBitmap(const base::DictionaryValue& args);

    static v8::Local<v8::Object> NativeImage::createEmpty(v8::Isolate* isolate);
    static void createEmptyApi(const v8::FunctionCallbackInfo<v8::Value> info);
    static void createFromPathApi(const v8::FunctionCallbackInfo<v8::Value> info);
    static void createFromBufferApi(const v8::FunctionCallbackInfo<v8::Value> info);
    static v8::Local<v8::Object> createFromBITMAPINFO(v8::Isolate* isolate, const BITMAPINFO* gdiBitmapInfo, void* gdiBitmapData);
    static v8::Local<v8::Object> createNativeImageFromBuffer(v8::Isolate* isolate, const unsigned char* data, size_t size);

    static NativeImage* GetSelf(v8::Local<v8::Object> handle);
    HICON getIcon();
    HBITMAP getBitmap();

    int getWidth() const;
    int getHeight() const;

public:
    void createFromBufferImpl(const unsigned char* data, size_t size);
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;

private:
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    Gdiplus::Bitmap* m_gdipBitmap;
};

}

#endif // common_api_ApiNativeImageExport_h