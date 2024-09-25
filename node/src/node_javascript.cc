#include "node.h"
#include "node_natives.h"
#include "v8.h"
#include "env.h"
#include "env-inl.h"
#include <vector>

void testReadFile(const wchar_t* path, std::vector<char>* buffer)
{
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        DebugBreak();
        return;
    }

    DWORD fileSizeHigh;
    const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

    DWORD numberOfBytesRead = 0;
    buffer->resize(bufferSize);
    BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
    ::CloseHandle(hFile);
    b = b;
}

bool testSaveFile(const wchar_t* url, const char* buffer, unsigned int size)
{
    HANDLE hFile = CreateFileW(url, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE != hFile) {
        DWORD dwSize = 0;
        WriteFile(hFile, buffer, size, &dwSize, NULL);
        CloseHandle(hFile);
        return TRUE;
    }
    return FALSE;
}

namespace node {

using v8::HandleScope;
using v8::Local;
using v8::NewStringType;
using v8::Object;
using v8::String;

Local<String> MainSource(Environment* env)
{
//     std::vector<char> buffer;
//     testReadFile(L"G:\\mycode\\mb\\node\\lib\\internal\\bootstrap_node.js", &buffer);
//     return String::NewFromUtf8(env->isolate(), (buffer.data()), NewStringType::kNormal, buffer.size()).ToLocalChecked();

    return String::NewFromUtf8(
        env->isolate(),
        reinterpret_cast<const char*>(internal_bootstrap_node_native),
        NewStringType::kNormal,
        sizeof(internal_bootstrap_node_native))
        .ToLocalChecked();
}

void DefineJavaScript(Environment* env, Local<Object> target)
{
    HandleScope scope(env->isolate());

    for (auto native : natives) {
        if (native.source == internal_bootstrap_node_native)
            continue;        

        Local<String> name = String::NewFromUtf8(env->isolate(), native.name);
        if (false/*nullptr != strstr(native.name, "events")*/) {
            std::vector<char> buffer;
            testReadFile(L"G:\\mycode\\mb\\node\\lib\\events.js", &buffer);

            Local<String> source = String::NewFromUtf8(
                env->isolate(), reinterpret_cast<const char*>(buffer.data()),
                NewStringType::kNormal, buffer.size()).ToLocalChecked();
            target->Set(name, source);
        } else {
            Local<String> source = String::NewFromUtf8(
                env->isolate(), reinterpret_cast<const char*>(native.source),
                NewStringType::kNormal, native.source_len).ToLocalChecked();
            target->Set(name, source);
        }
    }
}

} // namespace node
