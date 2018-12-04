#ifndef net_LambdaTask_h
#define net_LambdaTask_h

#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include <wtf/MainThread.h>
#include <functional>

namespace net {

class LambdaTask : public blink::WebThread::Task {
public:
    LambdaTask(std::function<void()>&& func)
        : m_func(func) {}
    virtual ~LambdaTask() override {}

    virtual void run() override { m_func(); }

    static void asyncCall(std::function<void()>&& func)
    {
        blink::Platform::current()->currentThread()->postTask(FROM_HERE, new LambdaTask(std::move(func)));
    }

private:
    std::function<void()> m_func;
};

}

#endif // net_LambdaTask_h