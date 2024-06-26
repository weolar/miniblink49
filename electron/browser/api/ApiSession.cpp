// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "browser/api/ApiSession.h"

#include "browser/api/ApiWebrequest.h"
#include "browser/api/ApiDownloadItem.h"
#include "browser/api/ApiWebContents.h"
#include "browser/api/ProtocolInterface.h"
#include "node/nodeblink.h"
#include "common/ThreadCall.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "common/FileUtil.h"
#include "common/StringUtil.h"
#include "common/IdLiveDetect.h"
#include "common/ThreadCall.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"
#include "gin/dictionary.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"

#include <vector>
#include <shlwapi.h>

namespace atom {

ApiSession::ApiSession(v8::Isolate* isolate, v8::Local<v8::Object> wrapper)
{
    m_webRequest = nullptr;
    m_protocol = nullptr;
    gin::Wrappable<ApiSession>::InitWith(isolate, wrapper);
}

ApiSession::~ApiSession()
{
    if (m_webRequest)
        delete m_webRequest;
    if (m_protocol)
        delete m_protocol;
}

void ApiSession::init(v8::Isolate* isolate, v8::Local<v8::Object> target)
{
    const char* className = "Session";
    v8::Local<v8::FunctionTemplate> funTempl = v8::FunctionTemplate::New(isolate, newFunction);

    funTempl->SetClassName(v8::String::NewFromUtf8(isolate, className));
    gin::ObjectTemplateBuilder builder(isolate, funTempl->InstanceTemplate());
    //builder.SetMethod("_getWebContents", &ApiSession::_getWebContentsApi);
    builder.SetProperty("webRequest", &ApiSession::webRequestApi);
    builder.SetProperty("protocol", &ApiSession::protocolApi);
    builder.SetMethod("setDownloadPath", &ApiSession::setDownloadPathApi);

    v8::Local<v8::Function> fun = funTempl->GetFunction();
    gin::Dictionary sessionClass(isolate, fun);
    sessionClass.SetMethod("fromPartition", &ApiSession::fromPartitionApi);

    constructor.Reset(isolate, fun);
    target->Set(v8::String::NewFromUtf8(isolate, className), fun);
}

void ApiSession::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (args.IsConstructCall()) {
        new ApiSession(isolate, args.This());
        args.GetReturnValue().Set(args.This());
        return;
    }
}

static std::string createSessionDirname(const std::string& name, std::wstring* fullpathW)
{
    std::vector<WCHAR> buffer;
    buffer.resize(MAX_PATH + 1);
    memset(buffer.data(), 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(NULL, buffer.data(), MAX_PATH);
    ::PathRemoveFileSpecW(buffer.data());

    unsigned int hash = StringUtil::hashString(name.c_str());

    wchar_t temp[20] = { 0 };
    wsprintf(temp, L"%x", hash);
    ::PathAppendW(buffer.data(), StringUtil::UTF8ToUTF16(ApiSession::kDefaultDir).c_str());
    if (name == ApiSession::kDefaultSessionName)
        ::PathAppendW(buffer.data(), StringUtil::UTF8ToUTF16(name).c_str());
    else
        ::PathAppendW(buffer.data(), temp);

    *fullpathW = buffer.data();
    return StringUtil::UTF16ToUTF8(*fullpathW);
}

ApiSession* ApiSession::create(v8::Isolate* isolate, const std::string& name)
{
    if (name.empty())
        DebugBreak();

    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { gin::ConvertToV8(isolate, name.c_str()) };
    v8::Local<v8::Function> constructorFunction = v8::Local<v8::Function>::New(isolate, constructor);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::MaybeLocal<v8::Object> obj = constructorFunction->NewInstance(context, argc, argv);
    v8::Local<v8::Object> objV8 = obj.ToLocalChecked();

    ApiSession* self = (ApiSession*)WrappableBase::GetNativePtr(objV8, &kWrapperInfo);
    self->m_liveSelf.Reset(isolate, objV8);
    self->m_name = name;

    std::wstring fullpathW;
    self->m_path = createSessionDirname(name, &fullpathW); // rootdir/minieleses/11223344/ 这种形式的目录

    if (!FileUtil::isDirExist(fullpathW)) {
        if (!::CreateDirectory(fullpathW.c_str(), NULL)) {
            MessageBoxW(0, L"创建session目录失败，请把本程序安装到有权限的目录", L"失败", 0);
            return false;
        }
    }

    return self;
}

void ApiSession::webRequestApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (!m_webRequest)
        m_webRequest = ApiWebRequest::create(args.GetIsolate());

    v8::Local<v8::Object> v = m_webRequest->GetWrapper(args.GetIsolate());
    args.GetReturnValue().Set(v);
}

void ApiSession::protocolApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (!m_protocol)
        m_protocol = ProtocolInterface::inst();

    v8::Local<v8::Object> v = m_protocol->getWrapper(args.GetIsolate());
    args.GetReturnValue().Set(v);
}

void ApiSession::setDownloadPathApi(const std::string& path)
{
    m_downloadPath = StringUtil::normalizePath(path); // 不包括文件名
}

mbDownloadOpt ApiSession::onDownloadCallback(
    WebContents* webContents,
    mbWebView webView,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    mbNetJob job,
    mbNetJobDataBind* dataBind)
{
    ApiDownloadItem* item = nullptr;
    ApiDownloadItem** itemPtr = &item;
    mbDownloadBind bind = { 0 };
    bind.recvCallback = ApiDownloadItem::staticOnNetJobDataRecvCallback;
    bind.finishCallback = ApiDownloadItem::staticOnNetJobDataFinishCallback;
    bind.saveNameCallback = ApiDownloadItem::staticOnPopupDialogSaveNameCallback;

    ApiSession* self = this;
    int id = webContents->getIdApi();

    ThreadCall::callUiThreadSync([self, id, webContents, &bind, itemPtr] {
        if (IdLiveDetect::get()->isLive(id)) {
            *itemPtr = ApiDownloadItem::create(v8::Isolate::GetCurrent());
            self->mate::EventEmitter<ApiSession>::emit("will-download", *itemPtr, webContents);
        }
    });

    bind.param = item;
    item->m_url = url;
    item->m_mime = mime;
    item->m_recvSize = 0;
    item->m_disposition = disposition;
    item->m_allSize = expectedContentLength;

    if (item->getSavePath().empty()) {
        mbDialogOptions dialogOptions;
        dialogOptions.magic = 'mbdo';
        dialogOptions.defaultPath = m_downloadPath.c_str();

        return mbPopupDialogAndDownload(webView, &dialogOptions, expectedContentLength, url, mime, disposition, job, dataBind, &bind);
    } else {
        mbDownloadOptions downloadOptions;
        downloadOptions.magic = 'mbdo';
        downloadOptions.saveAsPathAndName = FALSE; // saveAsPathAndName表示强行把文件名+路径设置为为item->getSavePath，这里和electron规范不符合
        return mbDownloadByPath(webView, &downloadOptions, StringUtil::UTF8ToUTF16(item->getSavePath()).c_str(), expectedContentLength, url, mime, disposition, job, dataBind, &bind); // TODO
    }
}

static bool addHeaderFromValue(v8::Isolate* isolate, std::map<std::string, std::string>* newHttpHead, const std::string& key, const v8::Local<v8::Value>& localVal)
{
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::String> localStrVal;
    if (!localVal->ToString(context).ToLocal(&localStrVal))
        return false;
    
    std::string value;
    gin::ConvertFromV8(isolate, localStrVal, &value);
    //mbNetSetHTTPHeaderField(job, StringUtil::UTF8ToUTF16(key).c_str(), StringUtil::UTF8ToUTF16(value).c_str(), FALSE);
    newHttpHead->insert(std::make_pair(key, value));

    return true;
}

static mbSlist* cloneMbSlist(const mbSlist* old)
{
    if (!old)
        return nullptr;
    mbSlist* newSlist = new mbSlist();
    mbSlist* head = newSlist;

    while (old) {
        newSlist->data = _strdup(old->data);
        old = old->next;
        if (old) {
            newSlist->next = new mbSlist();
            newSlist = newSlist->next;
        }
    }
    return head;
}

static const void freeMbSlist(mbSlist* old)
{
    if (!old)
        return;

    while (old) {
        if (old->data)
            free(old->data);

        mbSlist* newSlist = old->next;
        delete old;
        old = newSlist;
    }
}

static const void mbSlistToDic(const mbSlist* slist, gin::Dictionary* dic)
{
    if (!slist)
        return;

    std::string key;
    std::string val;
    while (slist) {
        std::string str = slist->data;
        size_t pos = str.find(':');
        if (pos != std::string::npos) {
            key = str.substr(0, pos);
            val = str.substr(pos + 2);
            dic->Set(key, val);
        }       
        slist = slist->next;
    }
}

struct BeforeSendHeadersCallbackInfo {
    BeforeSendHeadersCallbackInfo(mbNetJob jobPtr)
    {
        job = jobPtr;
        isAsnyc = false;
        isCalled = false;
        isCancel = false;
        //needContinue = false;
    }
    mbNetJob job;

    std::map<std::string, std::string> newHttpHead;
    bool isAsnyc;
    bool isCalled;
    bool isCancel;
    //bool needContinue;
};

static void beforeSendHeadersCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    BeforeSendHeadersCallbackInfo* info = (BeforeSendHeadersCallbackInfo*)(v8::External::Cast(*args.Data())->Value());
    info->isCalled = true;
    if (1 != args.Length())
        return;

    v8::Local<v8::Value> args0 = args[0];
    gin::Dictionary beforeSendResponse(isolate, args0.As<v8::Object>());

    bool cancel = false;
    do {
        if (beforeSendResponse.Get("cancel", &cancel) && cancel) {
            //mbNetCancelRequest(info->job);
            info->isCancel = true;
            break;
        }

        v8::Local<v8::Object> headers;
        if (!beforeSendResponse.Get("requestHeaders", &headers))
            break;

        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::Local<v8::Array> keys = headers->GetOwnPropertyNames(context).ToLocalChecked();
        for (uint32_t i = 0; i < keys->Length(); i++) {
            v8::Local<v8::Value> keyVal;
            if (!keys->Get(context, i).ToLocal(&keyVal))
                break;

            std::string key;
            gin::ConvertFromV8(isolate, keyVal, &key);

            v8::Local<v8::Value> localVal = headers->Get(context, keyVal).ToLocalChecked();
            if (localVal->IsArray()) {
                v8::Local<v8::Array> values = localVal.As<v8::Array>();
                for (uint32_t j = 0; j < values->Length(); j++) {
                    if (!addHeaderFromValue(isolate, &info->newHttpHead, key, values->Get(context, j).ToLocalChecked()))
                        break;
                }
            } else {
                if (!addHeaderFromValue(isolate, &info->newHttpHead, key, localVal))
                    break;
            }
        }
    } while (false);

    if (info->isAsnyc && !cancel) {
        std::map<std::string, std::string>::const_iterator it = info->newHttpHead.begin();
        for (; it != info->newHttpHead.end(); ++it) {
            mbNetSetHTTPHeaderField(info->job, StringUtil::UTF8ToUTF16(it->first).c_str(), StringUtil::UTF8ToUTF16(it->second).c_str(), FALSE);
        }

        mbNetContinueJob(info->job);
        //info->needContinue = true;
        delete info;
    }
}

void ApiSession::onLoadUrlBeginInBlinkThread(mbWebView webView, const char* url, mbNetJob job)
{
    if (!m_webRequest)
        return;

    if (!m_webRequest->m_beforeSendHeadersCb.IsEmpty())
        dispatchSendHeaders(webView, url, job, &m_webRequest->m_beforeSendHeadersCb);

    if (!m_webRequest->m_sendHeadersCb.IsEmpty())
        dispatchSendHeaders(webView, url, job, &m_webRequest->m_sendHeadersCb);
}

void ApiSession::dispatchSendHeaders(mbWebView webView, const char* url, mbNetJob job, v8::Persistent<v8::Value>* persistentCb)
{
    ApiSession* self = this;

    mbRequestType httpMethod = mbNetGetRequestMethod(job);
    std::string* referrer = new std::string(mbNetGetReferrer(job));
    std::string* urlStr = new std::string(url);

    mbSlist* httpHead = cloneMbSlist(mbNetGetRawHttpHeadInBlinkThread(job));
    BeforeSendHeadersCallbackInfo* info = new BeforeSendHeadersCallbackInfo(job);

    ThreadCall::callUiThreadSync([self, webView, persistentCb, urlStr, info, httpMethod, httpHead, referrer] {
        if (!self->m_webRequest || !(persistentCb->IsEmpty())) {
            info->isCalled = true;
            return;
        }

        v8::Local<v8::Value> cbVal = persistentCb->Get(v8::Isolate::GetCurrent());
        v8::Function* cb = v8::Function::Cast(*cbVal);

        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::EscapableHandleScope handleScope(isolate);
        v8::Local<v8::Context> context = cb->CreationContext();

        gin::Dictionary details = gin::Dictionary::CreateEmpty (isolate);
        details.Set("id", (int)(info->job));
        details.Set("method", httpMethod == kMbRequestTypeGet ? "GET" : (httpMethod == kMbRequestTypePost ? "POST" : "PUT"));
        details.Set("referrer", referrer->c_str());
        details.Set("url", urlStr->c_str());

        gin::Dictionary requestHeaders = gin::Dictionary::CreateEmpty (isolate);
        details.Set("requestHeaders", requestHeaders);
        mbSlistToDic(httpHead, &requestHeaders);
        freeMbSlist(httpHead);

        v8::Local<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New(isolate);

        v8::Local<v8::Value> data = v8::External::New(isolate, info);

        tmpl->SetCallHandler(beforeSendHeadersCallback, data);
        v8::Local<v8::Function> func = tmpl->GetFunction(context).FromMaybe(v8::Local<v8::Function>());
         
        v8::Local<v8::Value> argv[2] = { details.GetHandle(), func };
        v8::MaybeLocal<v8::Value> ret = cb->Call(context, v8::Null(isolate), 2, argv);

        delete referrer;
        delete urlStr;
    });

    if (!info->isCalled) {
        info->isAsnyc = true;
        mbNetHoldJobToAsynCommit(job);
    } else {
        if (info->isCancel)
            mbNetCancelRequest(job);
        delete info;
    }
}

void ApiSession::fromPartitionApi(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (1 != args.Length() && !(args[0]->IsString()))
        return;

    std::string sessionName;
    if (!gin::ConvertFromV8(args.GetIsolate(), args[0], &sessionName))
        return;
    v8::Isolate* isolate = args.GetIsolate();

    ApiSession* self = SessionMgr::get()->findOrCreateSession(isolate, sessionName, false);
    if (!self)
        self = SessionMgr::get()->findOrCreateSession(isolate, "", true);
    args.GetReturnValue().Set(self->GetWrapper(isolate));
}

SessionMgr* SessionMgr::m_inst = nullptr;
const char* ApiSession::kDefaultSessionName = "default";
const char* ApiSession::kDefaultDir = "minieleses";

static bool createDir()
{
    std::vector<WCHAR> fullpath;
    fullpath.resize(MAX_PATH + 1);
    memset(fullpath.data(), 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(NULL, fullpath.data(), MAX_PATH);
    ::PathRemoveFileSpecW(fullpath.data());

    std::wstring name = fullpath.data();
    name += L"\\";
    name += StringUtil::UTF8ToUTF16(ApiSession::kDefaultDir);

    if (!FileUtil::isDirExist(name)) {
        if (!::CreateDirectory(name.c_str(), NULL)) {
            MessageBoxW(0, L"创建minieleses目录失败，请把本程序安装到有权限的目录", L"失败", 0);
            return false;
        }
    }

    name += L"\\";
    name += StringUtil::UTF8ToUTF16(ApiSession::kDefaultSessionName);
    if (!FileUtil::isDirExist(name)) {
        if (!::CreateDirectory(name.c_str(), NULL)) {
            MessageBoxW(0, L"创建default目录失败，请把本程序安装到有权限的目录", L"失败", 0);
            return false;
        }
    }
    return true;
}

SessionMgr::SessionMgr()
{
    ::InitializeCriticalSection(&m_lock); 
}

SessionMgr* SessionMgr::get()
{
    //  先创建rootdir/minielectronsession/default/目录
    if (!createDir())
        return nullptr;

    if (m_inst)
        return m_inst;
    m_inst = new SessionMgr();
    return m_inst;
}

ApiSession* SessionMgr::findOrCreateSession(v8::Isolate* isolate, const std::string& sessionName, bool createIfNotExist)
{
    ApiSession* ses = nullptr;

    ::EnterCriticalSection(&m_lock);
    std::string name = sessionName;
    if (name.empty())
        name = ApiSession::kDefaultSessionName;
    std::map<std::string, ApiSession *>::iterator it = m_map.find(name);
    if (it != m_map.end()) {
        ses = it->second;
        ::LeaveCriticalSection(&m_lock);
        return ses;
    } else if (!createIfNotExist) {
        ::LeaveCriticalSection(&m_lock);
        return nullptr;
    }

    ses = ApiSession::create(isolate, name);
    if (!ses)
        ExitProcess(-1);
    m_map.insert(std::make_pair(name, ses));
    ::LeaveCriticalSection(&m_lock);

    return ses;
}

gin::WrapperInfo ApiSession::kWrapperInfo = { gin::kEmbedderNativeGin };
v8::Persistent<v8::Function> ApiSession::constructor;

void initializeBrowserSessionApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv)
{
    node::Environment* env = node::Environment::GetCurrent(context);
    ApiSession::init(env->isolate(), exports);
}

static const char BrowserSessionName[] = "console.log('BrowserSessionNative');;";
static NodeNative BrowserSessionNative{ "Screen", BrowserSessionName, sizeof(BrowserSessionName) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_session, initializeBrowserSessionApi, &BrowserSessionNative)

} // atom namespace
