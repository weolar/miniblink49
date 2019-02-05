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

    virtual functionWrapBase<R(Args...)>* clone(void* parent) = 0;
    virtual functionWrapBase<R(Args...)>* moveTo(void* parent) = 0;

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
    functionWrap(const T&& obj, void* parent)
        : functionWrapBase<R(Args...)>(parent)
        , m_obj(new T(obj)) {}

    functionWrap(const functionWrap& other) = delete;
    functionWrap(const functionWrap&& other) = delete;

    virtual ~functionWrap() override {
        delete m_obj;
    }

    virtual functionWrapBase<R(Args...)>* clone(void* parent) override {
        return new functionWrap<T, R, Args...>(std::move(*m_obj), parent);
    }

    virtual functionWrapBase<R(Args...)>* moveTo(void* parent) override {
        auto ret = new functionWrap<T, R, Args...>(m_obj, parent);
        m_obj = nullptr;
        return ret;
    }

    virtual R call(Args... args) override {
        return (*m_obj)(args...);
    }

private:
    functionWrap(const T* obj, void* parent) 
        : functionWrapBase<R(Args...)>(parent)
        , m_obj(obj) { }

    const T* m_obj;
};

template<typename R, typename... Args>
class function<R(Args...)> {
public:
    function() = delete;

    template<typename T>
    function(const T& obj) = delete;

    explicit function(const function<R(Args...)>& other) {
        m_wrap = other.m_wrap->clone(this);
    }

    explicit function(function<R(Args...)>&& other) {
        m_wrap = other.m_wrap->moveTo(this);
    }
    
    template<typename T>
    function(const T&& obj) {
        m_wrap = new functionWrap<T, R, Args...>(forward<const T>(obj), this);
    }

    R operator()(Args... args) const {
        return m_wrap->call(args...);
    }

    ~function() {
        if (m_wrap->getParent() == this)
            delete m_wrap;
    }

private:
    functionWrapBase<R(Args...)>* m_wrap;
};

}

#endif // functionalvc6_h