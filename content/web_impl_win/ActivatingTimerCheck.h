#ifndef ActivatingTimerCheck_h
#define ActivatingTimerCheck_h

class ActivatingTimerCheck {
public:
    ActivatingTimerCheck()
    {
        InitializeCriticalSection(&m_ctivatingLoaderMutex);
    }

    void add(content::WebTimerBase* timer)
    {
        EnterCriticalSection(&m_ctivatingLoaderMutex);
        bool find = false;
        for (size_t i = 0; i < m_activatingTimers.size(); ++i) {
            if (m_activatingTimers[i] != timer)
                continue;
            find = true;
        }
        if (!find)
            m_activatingTimers.append(timer);
        LeaveCriticalSection(&m_ctivatingLoaderMutex);
    }

    void remove(content::WebTimerBase* timer)
    {
        EnterCriticalSection(&m_ctivatingLoaderMutex);
        for (size_t i = 0; i < m_activatingTimers.size(); ++i) {
            if (m_activatingTimers[i] != timer)
                continue;
            m_activatingTimers.remove(i);
            break;
        }
        LeaveCriticalSection(&m_ctivatingLoaderMutex);
    }

    bool isActivating(content::WebTimerBase* timer)
    {
        EnterCriticalSection(&m_ctivatingLoaderMutex);
        bool isActivating = m_activatingTimers.contains(timer);
        LeaveCriticalSection(&m_ctivatingLoaderMutex);
        return isActivating;
    }

    void testPrint()
    {
#ifdef _DEBUG
        auto it = m_activatingTimers.begin();
        for (; it != m_activatingTimers.end(); ++it) {
            WTF::String outstr = String::format("ActivatingTimerCheck::testPrint: %p\n", *it);
            OutputDebugStringW(outstr.charactersWithNullTermination().data());
        }
#endif
    }

    size_t count()
    {
        EnterCriticalSection(&m_ctivatingLoaderMutex);
        size_t size = m_activatingTimers.size();
        LeaveCriticalSection(&m_ctivatingLoaderMutex);
        return size;
    }

private:
    WTF::Vector<content::WebTimerBase*> m_activatingTimers;
    CRITICAL_SECTION m_ctivatingLoaderMutex;
};

#endif // ActivatingTimerCheck_h