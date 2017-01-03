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
    functionWrapBase(void* parent)
        : m_parent(parent) {}

    virtual ~functionWrapBase() {
    }

    virtual R call(Args... args) = 0;

    void* getParent() const {
        return m_parent;
    }

protected:
    functionWrapBase() = delete;
    void* m_parent;
};

template<typename T, typename R, typename... Args>
class functionWrap : public functionWrapBase<R(Args...)> {
public:
    functionWrap(const T& obj, void* parent)
        : functionWrapBase(parent)
        , m_obj(&obj) {}

    virtual ~functionWrap() override {
    }

    virtual R call(Args... args) override {
        return (*m_obj)(args...);
    }

private:
    const T* m_obj;
};

template<typename R, typename... Args>
class function<R(Args...)> {
public:
    template<typename T>
    function(const T& obj) {
        m_wrap = new functionWrap<T, R, Args...>(obj, this);
    }

    template<typename T>
    function(const T&& obj) {
        m_wrap = new functionWrap<T, R, Args...>(obj, this);
    }

    function() = delete;

    template<typename T>
    function(const function<R(Args...)>& other) {
        m_wrap = new functionWrap<T, R, Args...>(other.m_wrap->obj, this);
    }

    template<typename T>
    function(function<R(Args...)>&& other) {
        m_wrap = new functionWrap<T, R, Args...>(other.m_wrap->obj, this);
    }

    template<typename T>
    function(const function<T>&& other) {
        m_wrap = new functionWrap<T, R, Args...>(other.m_wrap->obj, this);
    }

    R operator()(Args... args) const {
        return m_wrap->call(args...);
    }

    ~function() {
        if (m_wrap->getParent() == this)
            delete m_wrap;
    }

    functionWrapBase<R(Args...)>* m_wrap;
};

}

#endif // functionalvc6_h