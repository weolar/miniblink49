#ifndef CommonBase_h
#define CommonBase_h

template<class BaseType, class ChildType>
class CefCppBase {
public:
    BaseType m_baseClass;
    int m_ref;

    static void init(ChildType* self)
    {
        self->m_baseClass.base.size = sizeof(ChildType);
        self->m_baseClass.base.add_ref = add_ref;
        self->m_baseClass.base.release = release;
        self->m_baseClass.base.has_one_ref = hasOneRef;
        self->m_ref = 1;

#ifdef _DEBUG
        for (int i = 0; i < sizeof(BaseType) / sizeof(void*); ++i) {
            void** p = (void**)(&self->m_baseClass);
            if (nullptr == p[i])
                DebugBreak();
        }
#endif
    }

    static void CEF_CALLBACK add_ref(cef_base_t* s)
    {
        if (s->size != sizeof(ChildType)) {
            DebugBreak();
            return;
        }
        ChildType* self = (ChildType*)s;
        self->ref();
    }

    static int CEF_CALLBACK release(cef_base_t* s)
    {
        if (s->size != sizeof(ChildType)) {
            DebugBreak();
            return 0;
        }
        ChildType* self = (ChildType*)s;
        return self->deref();
    }

    CefCppBase* ref()
    {
        m_ref++;
        return this;
    }

    int deref()
    {
        m_ref--;
        if (0 == m_ref) {
            ((ChildType*)this)->willDeleted();
            delete this;
            return 0;
        }
        return m_ref;
    }

    BaseType* cast()
    {
        return &m_baseClass;
    }

    static int CEF_CALLBACK get_refct(cef_base_t* s)
    {
        if (s->size != sizeof(ChildType)) {
            DebugBreak();
            return 0;
        }
        ChildType* self = (ChildType*)s;
        return self->m_ref;
    }

    static int CEF_CALLBACK hasOneRef(cef_base_t* s)
    {
        if (s->size != sizeof(ChildType)) {
            DebugBreak();
            return 0;
        }

        ChildType* self = (ChildType*)s;
        return self->m_ref == 1;
    }
};

#define CEF_CHECK_ARGS_CAST(cppClass, retVal) \
    if (s->base.size != sizeof(cppClass)) { \
        DebugBreak(); \
        return retVal; \
    } \
    cppClass* self = (cppClass*)s;

#define CEF_CHECK_ARGS_CAST_NORET(cppClass) \
    if (s->base.size != sizeof(cppClass)) { \
        DebugBreak(); \
        return; \
    } \
    cppClass* self = (cppClass*)s;

#endif // CommonBase_h