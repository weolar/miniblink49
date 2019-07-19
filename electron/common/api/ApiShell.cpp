// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <string>

#include "common/PlatformUtil.h"
#include "gin/dictionary.h"
#include "gin/arguments.h"

//#if defined(OS_WIN)
#include "base/win/scoped_com_initializer.h"
#include "base/win/shortcut.h"
#include "base/files/file_path.h"
#include "base/strings/string_util.h"
#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"
#include "common/NodeRegisterHelp.h"

namespace gin {

template<>
struct Converter<base::win::ShortcutOperation> {
    static bool FromV8(v8::Isolate* isolate, v8::Handle<v8::Value> val,
        base::win::ShortcutOperation* out) {
        std::string operation;
        if (!ConvertFromV8(isolate, val, &operation))
            return false;
        if (operation.empty() || operation == "create")
            *out = base::win::SHORTCUT_CREATE_ALWAYS;
        else if (operation == "update")
            *out = base::win::SHORTCUT_UPDATE_EXISTING;
        else if (operation == "replace")
            *out = base::win::SHORTCUT_REPLACE_EXISTING;
        else
            return false;
        return true;
    }
};

}  // namespace gin
//#endif

namespace {

void openExternal(const v8::FunctionCallbackInfo<v8::Value>& info) {
    gin::Arguments args(info);

    if (args.Length() != 1 && args.Length() != 2) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }

    std::string url;
    if (!args.GetNext(&url)) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }
    std::wstring urlW(base::UTF8ToWide(url));

    bool activate = true;

    gin::Dictionary options = gin::Dictionary::CreateEmpty(info.GetIsolate());
    if (args.GetNext(&options)) {
        options.Get("activate", &activate);
    }
    
    bool b = platform_util::openExternal(urlW, activate);
    info.GetReturnValue().Set(b);
}

//#if defined(OS_WIN)
void writeShortcutLink(const v8::FunctionCallbackInfo<v8::Value>& info) {
    std::string shortcutPathStr;
    gin::Arguments args(info);

    if (args.Length() != 2) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }

    if (!args.GetNext(&shortcutPathStr)) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }

    base::win::ShortcutOperation operation = base::win::SHORTCUT_CREATE_ALWAYS;
    args.GetNext(&operation);
    gin::Dictionary options = gin::Dictionary::CreateEmpty(args.isolate());
    if (!args.GetNext(&options)) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }

    base::win::ShortcutProperties properties;
    std::string pathStr;
    std::string strA;
    int index;

    if (options.Get("target", &pathStr))
        properties.set_target(base::StringToFilePath(pathStr));
    if (options.Get("cwd", &pathStr))
        properties.set_working_dir(base::StringToFilePath(pathStr));
    if (options.Get("args", &strA))
        properties.set_arguments(base::UTF8ToWide(strA));
    if (options.Get("description", &strA))
        properties.set_description(base::UTF8ToWide(strA));
    if (options.Get("icon", &pathStr) && options.Get("iconIndex", &index))
        properties.set_icon(base::StringToFilePath(pathStr), index);
    if (options.Get("appUserModelId", &strA))
        properties.set_app_id(base::UTF8ToWide(strA));

    base::win::ScopedCOMInitializer com_initializer;
    base::win::CreateOrUpdateShortcutLink(base::StringToFilePath(shortcutPathStr), properties, operation);
}

void readShortcutLink(const v8::FunctionCallbackInfo<v8::Value>& info) {
    gin::Arguments args(info);
    std::string pathStr;
    if (args.Length() != 2) {
        args.ThrowError();
        info.GetReturnValue().Set(v8::Null(args.isolate()));
        return;
    }
    if (!args.GetNext(&pathStr)) {
        args.ThrowError();
        info.GetReturnValue().Set(v8::Null(args.isolate()));
        return;
    }

    using base::win::ShortcutProperties;
    gin::Dictionary options = gin::Dictionary::CreateEmpty(args.isolate());
    base::win::ScopedCOMInitializer com_initializer;
    base::win::ShortcutProperties properties;
    if (!base::win::ResolveShortcutProperties(base::StringToFilePath(pathStr), ShortcutProperties::PROPERTIES_ALL, &properties)) {
        args.ThrowTypeError("Failed to read shortcut link");
        info.GetReturnValue().Set(v8::Null(args.isolate()));
        return;
    }
    options.Set("target", properties.target.AsUTF8Unsafe());
    options.Set("cwd", properties.working_dir.AsUTF8Unsafe());
    options.Set("args", base::UTF16ToASCII(properties.arguments));
    options.Set("description", base::UTF16ToASCII(properties.description));
    options.Set("icon", properties.icon.AsUTF8Unsafe());
    options.Set("iconIndex", properties.icon_index);
    options.Set("appUserModelId", base::UTF16ToASCII(properties.app_id));
    info.GetReturnValue().Set(gin::Converter<gin::Dictionary>::ToV8(args.isolate(), options));
}

void showItemInFolder(const v8::FunctionCallbackInfo<v8::Value>& info) {
    std::string fullPathStr;
    gin::Arguments args(info);

    if (args.Length() != 1) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }
    if (!args.GetNext(&fullPathStr)) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }

    platform_util::showItemInFolder(base::StringToFilePath(fullPathStr));
}

void openItem(const v8::FunctionCallbackInfo<v8::Value>& info) {
    std::string fullPathStr;
    gin::Arguments args(info);

    if (args.Length() != 1) {
        args.ThrowError();
        return;
    }
    if (!args.GetNext(&fullPathStr)) {
        args.ThrowError();
        return;
    }

    platform_util::openItem(base::StringToFilePath(fullPathStr));
}

void moveItemToTrash(const v8::FunctionCallbackInfo<v8::Value>& info) {
    std::string fullPathStr;
    gin::Arguments args(info);

    if (args.Length() != 1) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }
    if (!args.GetNext(&fullPathStr)) {
        args.ThrowError();
        info.GetReturnValue().Set(false);
        return;
    }

    bool b = platform_util::moveItemToTrash(base::StringToFilePath(fullPathStr));
    info.GetReturnValue().Set(b);
}

void beep(const v8::FunctionCallbackInfo<v8::Value>& info) {
    platform_util::beep();
}
//#endif

void initializeShellApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv) {
    v8::Isolate* isolate = context->GetIsolate();
    v8::Local<v8::Object> obj = v8::Object::New(isolate);
    gin::Dictionary dict(context->GetIsolate(), obj);
    dict.SetMethod("showItemInFolder", &showItemInFolder);
    dict.SetMethod("openItem", &openItem);
    dict.SetMethod("openExternal", &openExternal);
    dict.SetMethod("moveItemToTrash", &moveItemToTrash);
    dict.SetMethod("beep", &beep);
    //#if defined(OS_WIN)
    dict.SetMethod("writeShortcutLink", &writeShortcutLink);
    dict.SetMethod("readShortcutLink", &readShortcutLink);
    //#endif

    exports->Set(v8::String::NewFromUtf8(isolate, "Shell"), obj);
}

}  // namespace

static const char CommonShellNative[] = "console.log('CommonShellNative');;";
static NodeNative nativeCommonShellNative{ "Shell", CommonShellNative, sizeof(CommonShellNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_common_shell, initializeShellApi, &nativeCommonShellNative)
