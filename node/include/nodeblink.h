#ifndef _NODEBLINK_H_
#define _NODEBLINK_H_

#define NODE_ARCH "ia32"
#define NODE_PLATFORM "win32"
#define NODE_WANT_INTERNALS 1
#define HAVE_OPENSSL 1
#define HAVE_ETW 1
#define HAVE_PERFCTR 1
#define V8_INSPECTOR_USE_STL 1
#define NODE_USE_V8_PLATFORM 0
#define USING_V8_SHARED 0
#define USING_UV_SHARED 1
#define CARES_BUILDING_LIBRARY 0

#include "node.h"
#include "env.h"
#include "env-inl.h"
#include "uv.h"

struct NodeNative {
    const char* name;
    const char* source;
    size_t sourceLen;
};

namespace node {

typedef struct _NodeArgc NodeArgc;
typedef void(*NodeInitCallBack)(NodeArgc*);

typedef struct _NodeArgc {
    char** argv;
    int argc;
    v8::Platform* v8platform;
    uv_loop_t *childLoop;
    uv_async_t async;
    uv_thread_t thread;
    bool initType;
    HANDLE initEvent;
    Environment* childEnv;
    NodeInitCallBack initcall;
    NodeInitCallBack preInitcall;
    void *data;
} NodeArgc;

extern "C" NODE_EXTERN NodeArgc* nodeRunThread(int argc, const wchar_t *wargv[], NodeInitCallBack initcall, NodeInitCallBack preInitcall, void *data);
extern "C" NODE_EXTERN Environment* nodeGetEnvironment(NodeArgc*);

extern "C" NODE_EXTERN void* nodeCreateDefaultPlatform();

typedef void* BlinkMicrotaskSuppressionHandle;

extern "C" NODE_EXTERN BlinkMicrotaskSuppressionHandle blinkMicrotaskSuppressionEnter(v8::Isolate* isolate);
extern "C" NODE_EXTERN void blinkMicrotaskSuppressionLeave(BlinkMicrotaskSuppressionHandle handle);

} // node

#endif //_NODEBLINK_H_