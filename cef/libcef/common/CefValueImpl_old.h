namespace cef {

class ValueBaseImpl {
public:
    // Specifies how the value will be used.
    enum ValueMode {
        // A reference to a value managed by an existing controller. These values
        // can be safely detached but ownership should not be transferred (make a
        // copy of the value instead).
        kReference,

        // The value has its own controller and will be deleted on destruction.
        // These values can only be detached to another controller otherwise any
        // references will not be properly managed.
        kOwnerWillDelete,

        // The value has its own controller and will not be deleted on destruction.
        // This should only be used for global values or scope-limited values that
        // will be explicitly detached.
        kOwnerNoDelete,
    };

    static void copy(ValueBaseImpl* self, ValueBaseImpl* othar)
    {
        othar->m_mode = self->m_mode;
        othar->m_isReadOnly = self->m_isReadOnly;
        othar->m_type = self->m_type;
    }

    static ValueBaseImpl* copyByCast(ValueBaseImpl* self, int exclude_empty_children);

    ValueMode m_mode;
    bool m_isReadOnly;
    cef_value_type_t m_type;
    cef_base_t* m_basePtr;

};

struct cef_base_wrap_t {
    cef_base_t base;
};

class FundamentalValueImpl : public CefCppBase < cef_base_wrap_t, FundamentalValueImpl > {
public:
    void willDeleted() { DebugBreak(); return; }

    FundamentalValueImpl(int value)
    {
        memset(&m_baseClass, 0, sizeof(m_baseClass));
        m_valueBase.m_type = VTYPE_INT;
        m_integerValue = value;
        CefCppBase<cef_base_wrap_t, FundamentalValueImpl>::init(this);
        m_valueBase.m_basePtr = &m_baseClass.base;
    }

    FundamentalValueImpl(double value)
    {
        memset(&m_baseClass, 0, sizeof(m_baseClass));
        m_valueBase.m_type = VTYPE_DOUBLE;
        m_doubleValue = value;
        CefCppBase<cef_base_wrap_t, FundamentalValueImpl>::init(this);
        m_valueBase.m_basePtr = &m_baseClass.base;
    }

    FundamentalValueImpl(bool value)
    {
        memset(&m_baseClass, 0, sizeof(m_baseClass));
        m_valueBase.m_type = VTYPE_BOOL;
        m_booleanValue = value;
        CefCppBase<cef_base_wrap_t, FundamentalValueImpl>::init(this);
        m_valueBase.m_basePtr = &m_baseClass.base;
    }

    FundamentalValueImpl()
    {
        memset(&m_baseClass, 0, sizeof(m_baseClass));
        m_valueBase.m_type = VTYPE_NULL;
        CefCppBase<cef_base_wrap_t, FundamentalValueImpl>::init(this);
        m_valueBase.m_basePtr = &m_baseClass.base;
    }

    ValueBaseImpl m_valueBase;

    union {
        bool m_booleanValue;
        int m_integerValue;
        double m_doubleValue;
    };
};

class StringValueImpl : public CefCppBase < cef_base_wrap_t, StringValueImpl > {
public:
    void willDeleted() { DebugBreak(); return; }

    StringValueImpl(const String& value)
    {
        memset(&m_baseClass, 0, sizeof(m_baseClass));
        m_valueBase.m_type = VTYPE_STRING;
        m_string = value;
        CefCppBase<cef_base_wrap_t, StringValueImpl>::init(this);
        m_valueBase.m_basePtr = &m_baseClass.base;
    }

    ValueBaseImpl m_valueBase;

    String m_string;
};

class BinaryValueImpl : public CefCppBase < cef_binary_value_t, BinaryValueImpl > {
public:
    BinaryValueImpl();
    void willDeleted() { DebugBreak(); return; }

    static int CEF_CALLBACK is_valid(cef_binary_value_t* self) { return true; }
    static int CEF_CALLBACK is_owned(cef_binary_value_t* s)
    {
        CEF_CHECK_ARGS_CAST(BinaryValueImpl, false);
        return self->m_valueBase.m_mode != ValueBaseImpl::kOwnerWillDelete;
    }

    static cef_binary_value_t* CEF_CALLBACK copy(cef_binary_value_t* self);

    static size_t CEF_CALLBACK get_size(cef_binary_value_t* self);

    static size_t CEF_CALLBACK get_data(cef_binary_value_t* self, void* buffer, size_t buffer_size, size_t data_offset);

    ValueBaseImpl m_valueBase;
    Vector<unsigned char> m_data;
};

class DictionaryValueImpl : public CefCppBase < cef_dictionary_value_t, DictionaryValueImpl > {
public:
    DictionaryValueImpl();
    void willDeleted() { DebugBreak(); return; }

    //typedef HashMap<String, ValueBaseImpl*> Dictionary;
    typedef HashMap<String, void*> Dictionary;
    typedef Dictionary::iterator iterator;
    typedef Dictionary::const_iterator const_iterator;

    static int CEF_CALLBACK is_valid(cef_dictionary_value_t* self) { return true; }
    static int CEF_CALLBACK is_owned(cef_dictionary_value_t* s)
    {
        CEF_CHECK_ARGS_CAST(BinaryValueImpl, false);
        return self->m_valueBase.m_mode != ValueBaseImpl::kOwnerWillDelete;
    }
    static int CEF_CALLBACK is_read_only(cef_dictionary_value_t* s)
    {
        CEF_CHECK_ARGS_CAST(DictionaryValueImpl, 0);
        return self->m_valueBase.m_isReadOnly;
    }
    static cef_dictionary_value_t* CEF_CALLBACK copy(cef_dictionary_value_t* self, int exclude_empty_children);

    static size_t CEF_CALLBACK get_size(cef_dictionary_value_t* self);
    static int CEF_CALLBACK clear(cef_dictionary_value_t* self);

    static int CEF_CALLBACK has_key(cef_dictionary_value_t* self, const cef_string_t* key);
    static int CEF_CALLBACK get_keys(cef_dictionary_value_t* self, cef_string_list_t keys) { DebugBreak(); return 0; }
    static int CEF_CALLBACK remove(cef_dictionary_value_t* self, const cef_string_t* key);
    static cef_value_type_t CEF_CALLBACK get_type(cef_dictionary_value_t* self, const cef_string_t* key);
    static int CEF_CALLBACK get_bool(cef_dictionary_value_t* self, const cef_string_t* key);
    static int CEF_CALLBACK get_int(cef_dictionary_value_t* self, const cef_string_t* key);
    static double CEF_CALLBACK get_double(cef_dictionary_value_t* self, const cef_string_t* key);

    ValueBaseImpl m_valueBase;
    Dictionary m_data;

};

class ListValueImpl : public CefCppBase < cef_list_value_t, ListValueImpl > {
public:
    ListValueImpl();
    void willDeleted() { return; }

    typedef Vector<ValueBaseImpl*> List;
    typedef List::iterator iterator;
    typedef List::const_iterator const_iterator;

    static int CEF_CALLBACK is_valid(cef_list_value_t* self) { return true; }
    static int CEF_CALLBACK is_owned(cef_list_value_t* s)
    {
        CEF_CHECK_ARGS_CAST(BinaryValueImpl, false);
        return self->m_valueBase.m_mode != ValueBaseImpl::kOwnerWillDelete;
    }
    static int CEF_CALLBACK is_read_only(cef_list_value_t* s)
    {
        CEF_CHECK_ARGS_CAST(ListValueImpl, 0);
        return self->m_valueBase.m_isReadOnly;
    }
    static cef_list_value_t* CEF_CALLBACK copy(cef_list_value_t* self) { DebugBreak(); return 0; }

    static int CEF_CALLBACK set_size(cef_list_value_t* self, size_t size);
    static size_t CEF_CALLBACK get_size(cef_list_value_t* self) { DebugBreak(); return 0; }
    static int CEF_CALLBACK clear(cef_list_value_t* self) { DebugBreak(); return 0; }

    static int CEF_CALLBACK remove(cef_list_value_t* self, int index) { DebugBreak(); return 0; }
    static cef_value_type_t CEF_CALLBACK get_type(cef_list_value_t* self, int index) { DebugBreak(); return VTYPE_INVALID; }
    static int CEF_CALLBACK get_bool(cef_list_value_t* self, int index) { DebugBreak(); return 0; }
    static int CEF_CALLBACK get_int(cef_list_value_t* self, int index) { DebugBreak(); return 0; }
    static double CEF_CALLBACK get_double(cef_list_value_t* self, int index) { DebugBreak(); return 0; }
    static cef_string_userfree_t CEF_CALLBACK get_string(cef_list_value_t* self, int index) { DebugBreak(); return 0; }
    static cef_binary_value_t* CEF_CALLBACK get_binary(cef_list_value_t* self, int index) { DebugBreak(); return 0; }
    static cef_dictionary_value_t* CEF_CALLBACK get_dictionary(cef_list_value_t* self, int index) { DebugBreak(); return 0; }
    static cef_list_value_t* CEF_CALLBACK get_list(cef_list_value_t* self, int index) { DebugBreak(); return 0; }
    static int CEF_CALLBACK set_null(cef_list_value_t* self, int index);
    static int CEF_CALLBACK set_bool(cef_list_value_t* self, int index, int value);
    static int CEF_CALLBACK set_int(cef_list_value_t* self, int index, int value);
    static int CEF_CALLBACK set_double(cef_list_value_t* self, int index, double value) { DebugBreak(); return 0; }
    static int CEF_CALLBACK set_string(cef_list_value_t* self, int index, const cef_string_t* value);
    static int CEF_CALLBACK set_binary(cef_list_value_t* self, int index, cef_binary_value_t* value) { DebugBreak(); return 0; }
    static int CEF_CALLBACK set_dictionary(cef_list_value_t* self, int index, cef_dictionary_value_t* value) { DebugBreak(); return 0; }
    static int CEF_CALLBACK set_list(cef_list_value_t* self, int index, cef_list_value_t* value) { DebugBreak(); return 0; }

    ValueBaseImpl m_valueBase;
    List m_data;

};

} // cef