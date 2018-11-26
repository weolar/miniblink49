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
//#define USING_UV_SHARED 0
#define BUILDING_UV_SHARED 1
#define CARES_BUILDING_LIBRARY 0

#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"

struct NodeNative {
    const char* name;
    const char* source;
    size_t sourceLen;
};

extern "C" NODE_EXTERN void* nodeCreateDefaultPlatform();
extern "C" NODE_EXTERN void nodeDeleteNodeEnvironment(node::Environment* env);

typedef void* BlinkMicrotaskSuppressionHandle;
extern "C" NODE_EXTERN BlinkMicrotaskSuppressionHandle nodeBlinkMicrotaskSuppressionEnter(v8::Isolate* isolate);
extern "C" NODE_EXTERN void nodeBlinkMicrotaskSuppressionLeave(BlinkMicrotaskSuppressionHandle handle);

extern "C" NODE_EXTERN void* nodeBlinkAllocateUninitialized(size_t length);
extern "C" NODE_EXTERN void nodeBlinkFree(void* data, size_t length);

#endif //_NODEBLINK_H_