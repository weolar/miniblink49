#include "node.h"
#include "node_natives.h"
#include "v8.h"
#include "env.h"
#include "env-inl.h"
#include "bootstrap_node_js.h"

namespace content {
void readJsFile(const wchar_t* path, std::vector<char>* buffer);
}

namespace node {

using v8::HandleScope;
using v8::Local;
using v8::NewStringType;
using v8::Object;
using v8::String;

Local<String> MainSource(Environment* env) {
    return String::NewFromUtf8(
        env->isolate(),
        reinterpret_cast<const char*>(internal_bootstrap_node_native_hook),
        NewStringType::kNormal,
        15869).ToLocalChecked();

//     std::vector<char>* buffer = new std::vector<char>();
//     content::readJsFile(L"E:\\mycode\\miniblink49\\trunk\\node\\lib\\internal\\bootstrap_node.js", buffer);
// 
//     return String::NewFromUtf8(
//         env->isolate(),
//         reinterpret_cast<const char*>(&buffer->at(0)),
//         NewStringType::kNormal,
//         buffer->size()).ToLocalChecked();
}

void DefineJavaScript(Environment* env, Local<Object> target) {
    HandleScope scope(env->isolate());

    for (auto native : natives) {
        if (native.source != internal_bootstrap_node_native) {

//             if (0 == strcmp(native.name, "internal/child_process")) {
//                 std::vector<char>* buffer = new std::vector<char>();
//                 content::readJsFile(L"E:\\mycode\\miniblink49\\trunk\\node\\lib\\internal\\child_process.js", buffer);
//                 native.source = (const unsigned char*)&buffer->at(0);
//                 native.source_len = buffer->size();
//             }

            if (0 == strcmp(native.name, "url")) {
                std::vector<char>* buffer = new std::vector<char>();
                content::readJsFile(L"E:\\mycode\\miniblink49\\trunk\\node\\lib\\url.js", buffer);
                native.source = (const unsigned char*)&buffer->at(0);
                native.source_len = buffer->size();
            }

            if (0 == strcmp(native.name, "internal/url")) {
                std::vector<char>* buffer = new std::vector<char>();
                content::readJsFile(L"E:\\mycode\\miniblink49\\trunk\\node\\lib\\internal\\url.js", buffer);
                native.source = (const unsigned char*)&buffer->at(0);
                native.source_len = buffer->size();
            }

            if (0 == strcmp(native.name, "internal/querystring")) {
                std::vector<char>* buffer = new std::vector<char>();
                content::readJsFile(L"E:\\mycode\\miniblink49\\trunk\\node\\lib\\internal\\querystring.js", buffer);
                native.source = (const unsigned char*)&buffer->at(0);
                native.source_len = buffer->size();
            }

            Local<String> name = String::NewFromUtf8(env->isolate(), native.name);
            Local<String> source =
                String::NewFromUtf8(
                    env->isolate(), reinterpret_cast<const char*>(native.source),
                    NewStringType::kNormal, native.source_len).ToLocalChecked();
            target->Set(name, source);
        }
    }
}

}  // namespace node
