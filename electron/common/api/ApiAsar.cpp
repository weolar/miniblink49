// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"
#include "common/NodeRegisterHelp.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"
#include "gin/dictionary.h"
#include "common/asar/Archive.h"
#include "common/asar/AsarUtil.h"
#include "common/asar/AsarJs.h"
#include "common/asar/AsarInitJs.h"

#include <vector>

namespace atom {

class V8Archive : public gin::Wrappable<V8Archive> {
public:
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (!args.IsConstructCall()) {
            args.GetReturnValue().Set(v8::False(isolate));
            return;
        }

        new V8Archive(isolate, args.This()/*, std::move(archive)*/);
        args.GetReturnValue().Set(args.This());
    }

    bool init(const std::string& pathString) {
        base::FilePath path = base::FilePath::FromUTF8Unsafe(base::StringPiece(pathString));
        std::unique_ptr<asar::Archive> archive(new asar::Archive(path));
        if (!archive->Init())
            return false;

        m_archive = std::move(archive);
        return true;
    }

    static void buildPrototype(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);
        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Archive"));

        gin::ObjectTemplateBuilder(isolate, prototype->InstanceTemplate())
            .SetMethod("init", &V8Archive::init)
            .SetMethod("path", &V8Archive::getPath)
            .SetMethod("getFileInfo", &V8Archive::getFileInfo)
            .SetMethod("stat", &V8Archive::stat)
            .SetMethod("readdir", &V8Archive::readdir)
            .SetMethod("realpath", &V8Archive::realpath)
            .SetMethod("copyFileOut", &V8Archive::copyFileOut)
            .SetMethod("getFd", &V8Archive::getFD)
            .SetMethod("destroy", &V8Archive::destroy);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Archive"), prototype->GetFunction());
    }

protected:
    V8Archive(v8::Isolate* isolate, v8::Local<v8::Object> wrapper/*, std::unique_ptr<asar::Archive> archive*/)
        : m_archive(nullptr /*std::move(archive)*/) {
        gin::Wrappable<V8Archive>::InitWith(isolate, wrapper);
    }

    // Returns the path of the file.
    std::string getPath() {
        return m_archive->path().AsUTF8Unsafe();
    }

    // Reads the offset and size of file.
    v8::Local<v8::Value> getFileInfo(const std::string & pathString) {
        base::FilePath path = base::FilePath::FromUTF8Unsafe(base::StringPiece(pathString));
        asar::Archive::FileInfo info;
        if (!m_archive || !m_archive->GetFileInfo(path, &info))
            return v8::False(isolate());
        gin::Dictionary dict(isolate(), v8::Object::New(isolate()));
        dict.Set("size", info.size);
        dict.Set("unpacked", info.unpacked);
        dict.Set("offset", info.offset);
        return dict.GetHandle();
    }

    // Returns a fake result of fs.stat(path).
    v8::Local<v8::Value> stat(const std::string& pathString) {
        base::FilePath path = base::FilePath::FromUTF8Unsafe(base::StringPiece(pathString));
        asar::Archive::Stats stats;
        if (!m_archive || !m_archive->Stat(path, &stats))
            return v8::False(isolate());
        gin::Dictionary dict(isolate(), v8::Object::New(isolate()));
        dict.Set("size", stats.size);
        dict.Set("offset", stats.offset);
        dict.Set("isFile", stats.is_file);
        dict.Set("isDirectory", stats.is_directory);
        dict.Set("isLink", stats.is_link);
        return dict.GetHandle();
    }

    // Returns all files under a directory.
    v8::Local<v8::Value> readdir(const std::string & pathString) {
        base::FilePath path = base::FilePath::FromUTF8Unsafe(base::StringPiece(pathString));
        std::vector<base::FilePath> files;
        if (!m_archive || !m_archive->Readdir(path, &files))
            return v8::False(isolate());

        v8::Local<v8::Array> v8Arr = v8::Array::New(isolate(), files.size());
        for (size_t i = 0; i < files.size(); ++i) {
            const base::FilePath& pathIt = files[i];
            std::string pathItString = pathIt.AsUTF8Unsafe();
            v8Arr->Set(i, v8::String::NewFromUtf8(isolate(), pathItString.c_str(), v8::NewStringType::kNormal, pathItString.size()).ToLocalChecked());
        }
        return v8Arr;
    }

    // Returns the path of file with symbol link resolved.
    v8::Local<v8::Value> realpath(const std::string & pathString) {
        base::FilePath path = base::FilePath::FromUTF8Unsafe(base::StringPiece(pathString));
        base::FilePath realpath;
        if (!m_archive || !m_archive->Realpath(path, &realpath))
            return v8::False(isolate());

        std::string realpathString = realpath.AsUTF8Unsafe();
        return v8::String::NewFromUtf8(isolate(), realpathString.c_str(), v8::NewStringType::kNormal, realpathString.size()).ToLocalChecked();
    }

    // Copy the file out into a temporary file and returns the new path.
    v8::Local<v8::Value> copyFileOut(const std::string & pathString) {
        base::FilePath path = base::FilePath::FromUTF8Unsafe(base::StringPiece(pathString));
        base::FilePath newPath;
        if (!m_archive || !m_archive->CopyFileOut(path, &newPath))
            return v8::False(isolate());
        std::string newPathString = newPath.AsUTF8Unsafe();
        return v8::String::NewFromUtf8(isolate(), newPathString.c_str(), v8::NewStringType::kNormal, newPathString.size()).ToLocalChecked();
    }

    // Return the file descriptor.
    int getFD() const {
        if (!m_archive)
            return -1;
        return m_archive->GetFD();
    }

    // Free the resources used by archive.
    void destroy() {
        m_archive.reset();
    }

private:
    std::unique_ptr<asar::Archive> m_archive;

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
    DISALLOW_COPY_AND_ASSIGN(V8Archive);
};

v8::Persistent<v8::Function> V8Archive::constructor;
gin::WrapperInfo V8Archive::kWrapperInfo = { gin::kEmbedderNativeGin };

#define FAKE_ASAR 0

void initAsarSupport(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Value> process = info[0];
    v8::Local<v8::Value> require = info[1];
    // Evaluate asar_init.coffee.
    std::string buffer;

#if FAKE_ASAR
    asar::ReadFileToString(L"E:\\mycode\\miniblink49\\trunk\\electron\\lib\\common\\asar_init.js", &buffer);
    const char* asarInitNative = &buffer.at(0);
    size_t asarInitNativeLength = buffer.size();
#else
    const char* asarInitNative = atom::AsarInitJs;
    size_t asarInitNativeLength = 690;
#endif
    v8::Local<v8::Script> asar_init = v8::Script::Compile(v8::String::NewFromUtf8(
        isolate,
        asarInitNative,
        v8::String::kNormalString,
        asarInitNativeLength));
    v8::Local<v8::Value> result = asar_init->Run();
    if (!result->IsFunction())
        return;

    v8::Function* resultFunc = v8::Function::Cast(*result);

#if FAKE_ASAR
    asar::ReadFileToString(L"E:\\mycode\\miniblink49\\trunk\\electron\\lib\\common\\asar.js", &buffer);
    v8::Local<v8::String> asarNativeV8 = v8::String::NewFromUtf8(isolate, &buffer.at(0), v8::String::kNormalString, buffer.size());
#else
    v8::Local<v8::String> asarNativeV8 = v8::String::NewFromUtf8(isolate, AsarJs, v8::String::kNormalString, AsarJsLength);
#endif
    v8::Local<v8::Value> vals[] = { process, require, asarNativeV8 };

    // Initialize asar support.
    v8::MaybeLocal<v8::Value> ret = resultFunc->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 3, vals);
}

void initializeAsarApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv) {
    v8::Isolate* isolate = context->GetIsolate();
    gin::Dictionary dict(isolate, exports);
    //dict.SetMethod("createArchive", &V8Archive::newFunction);
    dict.SetMethod("initAsarSupport", &initAsarSupport);

    V8Archive::buildPrototype(isolate, exports);
}

}  // atom namespace

static const char CommonAsarNative[] = "console.log('CommonAsarNative');;";
static NodeNative nativeCommonAsarNative{ "Asar", CommonAsarNative, sizeof(CommonAsarNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_common_asar, atom::initializeAsarApi, &nativeCommonAsarNative)
