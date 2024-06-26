// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "browser/api/ApiDownloadItem.h"
#include "common/api/EventEmitter.h"
#include "common/IdLiveDetect.h"
#include "common/StringUtil.h"
#include "common/NodeRegisterHelp.h"
#include "common/ThreadCall.h"
#include "gin/object_template_builder.h"
#include "mbvip/core/mb.h"
#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include <vector>
#include <map>

namespace atom {

ApiDownloadItem::ApiDownloadItem(v8::Isolate* isolate, v8::Local<v8::Object> wrapper)
{
    m_id = IdLiveDetect::get()->constructed(this);
    m_isPaused = false;
    m_state = kProgressing;
    gin::Wrappable<ApiDownloadItem>::InitWith(isolate, wrapper);
}

ApiDownloadItem::~ApiDownloadItem()
{
    IdLiveDetect::get()->deconstructed(m_id);
}

ApiDownloadItem* ApiDownloadItem::create(v8::Isolate* isolate)
{
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { v8::Null(isolate) };
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, constructor);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    v8::Local<v8::Object> objV8 = constructorFunction->NewInstance(context, argc, argv).ToLocalChecked(); // call into ApiDownloadItem::ApiDownloadItem
    ApiDownloadItem* self = (ApiDownloadItem*)WrappableBase::GetNativePtr(objV8, &kWrapperInfo);
    self->m_liveSelf.Reset(isolate, objV8);
    return self;
}

void ApiDownloadItem::init(v8::Isolate* isolate, v8::Local<v8::Object> target)
{
    const char* className = "DownloadItem";
    v8::Local<v8::FunctionTemplate> funTempl = v8::FunctionTemplate::New(isolate, newFunction);

    funTempl->SetClassName(v8::String::NewFromUtf8(isolate, className));
    gin::ObjectTemplateBuilder builder(isolate, funTempl->InstanceTemplate());
    builder.SetMethod("setSavePath", &ApiDownloadItem::setSavePathApi);
    builder.SetMethod("getSavePath", &ApiDownloadItem::getSavePathApi);
    builder.SetMethod("setSaveDialogOptions", &ApiDownloadItem::setSaveDialogOptionsApi);
    builder.SetMethod("getSaveDialogOptions", &ApiDownloadItem::getSaveDialogOptionsApi);
    builder.SetMethod("pause", &ApiDownloadItem::pauseApi);
    builder.SetMethod("isPaused", &ApiDownloadItem::isPausedApi);
    builder.SetMethod("resume", &ApiDownloadItem::resumeApi);
    builder.SetMethod("canResume", &ApiDownloadItem::canResumeApi);
    builder.SetMethod("cancels", &ApiDownloadItem::cancelsApi);
    builder.SetMethod("getURL", &ApiDownloadItem::getURLApi);
    builder.SetMethod("getMimeType", &ApiDownloadItem::getMimeTypeApi);
    builder.SetMethod("hasUserGesture", &ApiDownloadItem::hasUserGestureApi);
    builder.SetMethod("getFilename", &ApiDownloadItem::getFilenameApi);
    builder.SetMethod("getTotalBytes", &ApiDownloadItem::getTotalBytesApi);
    builder.SetMethod("getReceivedBytes", &ApiDownloadItem::getReceivedBytesApi);
    builder.SetMethod("getContentDisposition", &ApiDownloadItem::getContentDispositionApi);
    builder.SetMethod("getState", &ApiDownloadItem::getStateApi);
    builder.SetMethod("getURLChain", &ApiDownloadItem::getURLChainApi);
    builder.SetMethod("getLastModifiedTime", &ApiDownloadItem::getLastModifiedTimeApi);
    builder.SetMethod("getETag", &ApiDownloadItem::getETagApi);
    builder.SetMethod("getStartTime", &ApiDownloadItem::getStartTimeApi);

    v8::Local<v8::Function> fun = funTempl->GetFunction();
    constructor.Reset(isolate, fun);
    target->Set(v8::String::NewFromUtf8(isolate, className), fun);
}

void ApiDownloadItem::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (args.IsConstructCall()) {
        new ApiDownloadItem(isolate, args.This());
        args.GetReturnValue().Set(args.This());
        return;
    }
}

void ApiDownloadItem::setSavePathApi(const std::string path)
{
    m_savePath = StringUtil::normalizePath(path);
}

std::string ApiDownloadItem::getSavePathApi() const
{
    return m_savePath;
}

void ApiDownloadItem::setSaveDialogOptionsApi(const v8::FunctionCallbackInfo<v8::Value>& args/*options*/)
{

}

void ApiDownloadItem::getSaveDialogOptionsApi(const v8::FunctionCallbackInfo<v8::Value>& args) const
{

}

void ApiDownloadItem::pauseApi()
{
    m_isPaused = true;
}

bool ApiDownloadItem::isPausedApi() const
{
    return m_isPaused;
}

void ApiDownloadItem::resumeApi()
{
    m_isPaused = false;
}

bool ApiDownloadItem::canResumeApi() const
{
    return true;
}

void ApiDownloadItem::cancelsApi()
{
    OutputDebugStringA("ApiDownloadItem::cancelsApi is not impl\n");
}

std::string ApiDownloadItem::getURLApi() const
{
    return m_url;
}

std::string ApiDownloadItem::getMimeTypeApi() const
{
    return m_mime;
}

bool ApiDownloadItem::hasUserGestureApi() const
{
    return true;
}

std::string ApiDownloadItem::getFilenameApi() const
{
    return "";
}

int ApiDownloadItem::getTotalBytesApi() const
{
    return m_allSize;
}

int ApiDownloadItem::getReceivedBytesApi() const
{
    return m_recvSize;
}

std::string ApiDownloadItem::getContentDispositionApi() const
{
    return m_disposition;
}

std::string ApiDownloadItem::getStateApi() const
{
    switch (m_state) {
    case kProgressing:
        return "progressing";
    case kCompleted:
        return "completed";
    case kCancelled:
        return "cancelled";
    }
    return "progressing";
}

std::vector<std::string> ApiDownloadItem::getURLChainApi() const
{
    std::vector<std::string> ret;
    ret.push_back(m_url);
    return ret;
}

std::string ApiDownloadItem::getLastModifiedTimeApi() const
{
    return "";
}

std::string ApiDownloadItem::getETagApi() const
{
    return "";
}

std::string ApiDownloadItem::getStartTimeApi() const
{
    return "";
}

void ApiDownloadItem::staticOnNetJobDataRecvCallback(void* ptr, mbNetJob job, const char* data, int length)
{
    ApiDownloadItem* item = (ApiDownloadItem*)ptr;
    item->m_recvSize += length;

    ThreadCall::callUiThreadSync([item] {
        item->mate::EventEmitter<ApiDownloadItem>::emit("updated", "progressing");
    });
}

static unsigned int __stdcall msgBoxThread(void* param)
{
    std::function<void(void)>* callback = (std::function<void(void)>*)param;
    (*callback)();
    delete callback;
    return 0;
}

void ApiDownloadItem::staticOnPopupDialogSaveNameCallback(void* ptr, const wchar_t* filePath)
{
    ApiDownloadItem* item = (ApiDownloadItem*)ptr;
    item->m_savePath = StringUtil::UTF16ToUTF8(filePath);
}

void ApiDownloadItem::staticOnNetJobDataFinishCallback(void* ptr, mbNetJob job, mbLoadingResult result)
{
    OutputDebugStringA("onNetJobDataFinishCallback\n");

    ApiDownloadItem* item = (ApiDownloadItem*)ptr;
//     std::string url = item->m_url;
// 
//     wchar_t temp[20] = { 0 };
//     wsprintf(temp, L"%d", item->m_recvSize);

//     std::wstring* title = new std::wstring(utf8ToUtf16(url));
//     *title += L" 下载完成：";
//     *title += temp;


    ThreadCall::callUiThreadSync([item, result] {
        if (result == MB_LOADING_SUCCEEDED)
            item->mate::EventEmitter<ApiDownloadItem>::emit("done", "completed", "completed");
        else
            item->mate::EventEmitter<ApiDownloadItem>::emit("done", "interrupted", "interrupted");
    });

    delete item;

//     std::function<void(void)>* callback = new std::function<void(void)>([title, result] {
//         LPCWSTR lpCaption = (result == MB_LOADING_SUCCEEDED ? L"下载成功" : L"下载失败");
//         ::MessageBoxW(nullptr, title->c_str(), lpCaption, MB_OK);
//         delete title;
//     });
// 
//     unsigned int threadId = 0; // 为了不卡blink线程，messagbox放到另外个线程弹出
//     HANDLE threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, msgBoxThread, callback, 0, &threadId));
//     ::CloseHandle(threadHandle);
}

gin::WrapperInfo ApiDownloadItem::kWrapperInfo = { gin::kEmbedderNativeGin };
v8::Persistent<v8::Function> ApiDownloadItem::constructor;

void initializeBrowserDownloadItemApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv)
{
    node::Environment* env = node::Environment::GetCurrent(context);
    ApiDownloadItem::init(env->isolate(), exports);
}

static const char BrowserDownloadItemName[] = "console.log('BrowserDownloadItemNative');;";
static NodeNative BrowserDownloadItemNative{ "DownloadItem", BrowserDownloadItemName, sizeof(BrowserDownloadItemName) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_downloaditem, initializeBrowserDownloadItemApi, &BrowserDownloadItemNative)

} // atom namespace

namespace gin {

v8::Local<v8::Value> ConvertToV8(v8::Isolate* isolate, const atom::ApiDownloadItem& item)
{
    atom::ApiDownloadItem* it = (atom::ApiDownloadItem*)&item;
    return it->GetWrapper(isolate);
}

}

