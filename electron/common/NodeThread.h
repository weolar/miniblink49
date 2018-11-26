#ifndef atom_NodeThread_h
#define atom_NodeThread_h

#define NODE_ARCH "ia32"
#define NODE_PLATFORM "win32"
#define NODE_WANT_INTERNALS 1
#define HAVE_OPENSSL 1
#define HAVE_ETW 1
#define HAVE_PERFCTR 1
#define V8_INSPECTOR_USE_STL 1
#define NODE_USE_V8_PLATFORM 0
#define USING_V8_SHARED 0
#define CARES_BUILDING_LIBRARY 0

#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"

namespace node {
class Environment;
}

namespace atom {

struct NodeNative {
    const char* name;
    const char* source;
    size_t sourceLen;
};

typedef struct _NodeArgc NodeArgc;
typedef void(*NodeInitCallBack)(NodeArgc*);
class NodeBindings;

typedef struct _NodeArgc {
//     char** argv;
//     int argc;
    v8::Platform* v8platform;
    uv_loop_t *childLoop;
    uv_async_t async;
    uv_thread_t thread;
    bool initType;
    HANDLE initEvent;
    node::Environment* childEnv;
//     NodeInitCallBack preInitcall;
//     NodeInitCallBack initcall;
    NodeBindings* m_nodeBinding;
} NodeArgc;

NodeArgc* runNodeThread();
node::Environment* nodeGetEnvironment(NodeArgc*);

} // atom

#endif // atom_NodeThread_h