#ifndef atom_NodeBinding_h
#define atom_NodeBinding_h

#include "v8.h"
#include <list>

namespace node {
class Environment;
}

namespace gin {
class Dictionary;
}

typedef struct uv_loop_s uv_loop_t;
typedef struct uv_async_s uv_async_t;
typedef struct uv_async_s uv_async_t;

namespace atom {

class NodeBindings {
public:
    NodeBindings(bool isBrowser);
    ~NodeBindings();

    void setUvLoop(uv_loop_t* uvLoop)
    {
        m_uvLoop = uvLoop;
    }

    static void initNodeEnv();

    void bindFunction(gin::Dictionary* dict);

    node::Environment* createEnvironment(v8::Local<v8::Context> context);
    void loadEnvironment();
    void activateUVLoop(v8::Isolate* isoloate);

private:
    static void onCallNextTick(uv_async_t* handle);
    bool m_isBrowser;
    uv_loop_t* m_uvLoop;
    node::Environment* m_env;
    uv_async_t* m_callNextTickAsync;
    std::list<node::Environment*> m_pendingNextTicks;
};

} // atom

#endif // atom_NodeBinding_h