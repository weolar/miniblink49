
#ifndef gin_v8_task_runner_h
#define gin_v8_task_runner_h

namespace gin {

class V8TaskToWebThreadTask : public blink::WebThread::Task {
public:
    V8TaskToWebThreadTask(v8::Task* task)
        : m_task(task)
    {
    }

    virtual ~V8TaskToWebThreadTask() override { delete m_task; }
    virtual void run() override { m_task->Run(); }

private:
    v8::Task* m_task;
};

class V8IdleTaskToWebThreadTask : public blink::WebThread::Task {
public:
    V8IdleTaskToWebThreadTask(v8::IdleTask* task)
        : m_task(task)
    {
    }
    virtual ~V8IdleTaskToWebThreadTask() override { delete m_task; }
    virtual void run() override
    {
        m_task->Run(0);
    }
private:
    v8::IdleTask* m_task;
};

#if V8_MAJOR_VERSION >= 7
class V8ForegroundTaskRunner : public v8::TaskRunner {
public:
    V8ForegroundTaskRunner(blink::WebThread* thread)
    {
        m_thread = thread;
    }

    ~V8ForegroundTaskRunner() override
    {

    }

    // v8::Platform implementation.
    void PostTask(std::unique_ptr<v8::Task> task) override
    {
        return m_thread->postTask(FROM_HERE, new V8TaskToWebThreadTask(task.release()));
    }

    void PostNonNestableTask(std::unique_ptr<v8::Task> task) override
    {
        return m_thread->postTask(FROM_HERE, new V8TaskToWebThreadTask(task.release()));
    }

    void PostDelayedTask(std::unique_ptr<v8::Task> task, double delay_in_seconds) override
    {
        return m_thread->postDelayedTask(FROM_HERE, new V8TaskToWebThreadTask(task.release()), (long long)(delay_in_seconds * 1000));
    }

    void PostIdleTask(std::unique_ptr<v8::IdleTask> task) override
    {
        return m_thread->postTask(FROM_HERE, new V8IdleTaskToWebThreadTask(task.release()));
    }

    bool NonNestableTasksEnabled() const override
    {
        return true;
    }

    bool IdleTasksEnabled() override
    {
        return false;
    }

private:
    blink::WebThread* m_thread;
};
#endif // V8_MAJOR_VERSION

} // gin

#endif // gin_v8_task_runner_h