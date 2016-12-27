#ifndef functionalvc6_h
#define functionalvc6_h

namespace std {

template<typename>
class function;

template<typename>
class functionWrapBase;

template<typename R, typename... Args>
class functionWrapBase<R(Args...)> {
public:
    virtual ~functionWrapBase() { }
    virtual R operator()(Args... args) = 0;
protected:
    functionWrapBase() = default;
};

template<typename T, typename R, typename... Args>
class functionWrap : public functionWrapBase<R(Args...)> {
public:
    functionWrap(const T& obj)
        : m_obj(&obj) {}

    virtual R operator()(Args... args) {
        return (*m_obj)(args...);
    }

    const T* m_obj;
};

template<typename R, typename... Args>
class function<R(Args...)> {
public:
    template<typename T>
    function(const T& obj) {
        m_wrap = new functionWrap<T, R, Args...>(obj);
    }

    R operator()(Args... args) const {
        return (*m_wrap)(args...);
    }

    ~function() {
        delete m_wrap;
    }

    functionWrapBase<R(Args...)>* m_wrap;
};

}

#endif // functionalvc6_h