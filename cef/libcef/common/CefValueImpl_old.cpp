
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

#include "cef/include/internal/cef_types.h"
#include "cef/include/capi/cef_values_capi.h"
#include "libcef/common/CommonBase.h"
#include "libcef/common/CefValueImpl.h"

namespace cef {

BinaryValueImpl::BinaryValueImpl()
{
    m_valueBase.m_type = VTYPE_BINARY;
    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_baseClass.is_valid = is_valid;
    m_baseClass.is_owned = is_owned;
    m_baseClass.copy = copy;
    m_baseClass.get_size = get_size;
    m_baseClass.get_data = get_data;
    
    CefCppBase<cef_binary_value_t, BinaryValueImpl>::init(this);
    m_valueBase.m_basePtr = &m_baseClass.base;
}

cef_binary_value_t* BinaryValueImpl::copy(cef_binary_value_t* s)
{
    CEF_CHECK_ARGS_CAST(BinaryValueImpl, nullptr);
    BinaryValueImpl* copyInst = new BinaryValueImpl();
    copyInst->m_data = self->m_data;

    ValueBaseImpl::copy(&self->m_valueBase, &copyInst->m_valueBase);

    copyInst->ref();
    return copyInst->cast();
}

size_t BinaryValueImpl::get_size(cef_binary_value_t* s)
{
    CEF_CHECK_ARGS_CAST(BinaryValueImpl, 0);
    return self->m_data.size();
}

size_t BinaryValueImpl::get_data(cef_binary_value_t* s, void* buffer, size_t buffer_size, size_t data_offset)
{
    ASSERT(buffer);
    ASSERT(buffer_size > (size_t)0);

    CEF_CHECK_ARGS_CAST(BinaryValueImpl, 0);
    if (!buffer || buffer_size == 0)
        return 0;

    size_t size = self->m_data.size();
    ASSERT(data_offset < size);
    if (data_offset >= size)
        return 0;

    size = std::min(buffer_size, size - data_offset);
    const char* data = (const char*)self->m_data.data();
    memcpy(buffer, data + data_offset, size);
    return size;
}

//////////////////////////////////////////////////////////////////////////

DictionaryValueImpl::DictionaryValueImpl()
{
    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_valueBase.m_type = VTYPE_DICTIONARY;
    m_baseClass.is_valid = is_valid;
    m_baseClass.is_owned = is_owned;
    m_baseClass.copy = copy;
    m_baseClass.get_size = get_size;
    m_baseClass.clear = clear;
    m_baseClass.has_key = has_key;
    m_baseClass.get_keys = get_keys;
    m_baseClass.remove = remove;
    m_baseClass.get_type = get_type;
    m_baseClass.get_bool = get_bool;
    m_baseClass.get_int = get_int;
    m_baseClass.get_double = get_double;
    

    CefCppBase<cef_dictionary_value_t, DictionaryValueImpl>::init(this);
    m_valueBase.m_basePtr = &m_baseClass.base;
}

cef_dictionary_value_t* DictionaryValueImpl::copy(cef_dictionary_value_t* s, int exclude_empty_children)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, nullptr);

    DictionaryValueImpl* retVal = new DictionaryValueImpl();

//     for (Dictionary::const_iterator it = self->m_data.begin(); it != self->m_data.end(); ++it) {
//         ValueBaseImpl* othar = ValueBaseImpl::copyByCast(it->value, exclude_empty_children);
//         retVal->m_data.set(it->key, othar);
//     }

    retVal->ref();
    return retVal->cast();
}

size_t DictionaryValueImpl::get_size(cef_dictionary_value_t* s)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, 0);
    return self->m_data.size();
}

int DictionaryValueImpl::clear(cef_dictionary_value_t* s)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, 0);
    self->m_data.clear();
    return true;
}

int DictionaryValueImpl::has_key(cef_dictionary_value_t* s, const cef_string_t* key)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, 0);
//     Dictionary::const_iterator it = self->m_data.find(String(key->str, key->length));
//     return it != self->m_data.end();
    return false;
}

int DictionaryValueImpl::remove(cef_dictionary_value_t* s, const cef_string_t* key)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, 0);
    //self->m_data.remove(String(key->str, key->length));
    return true;
}

cef_value_type_t DictionaryValueImpl::get_type(cef_dictionary_value_t* s, const cef_string_t* key)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, VTYPE_INVALID);
//     Dictionary::const_iterator it = self->m_data.find(String(key->str, key->length));
//     if (it != self->m_data.end() && nullptr != it->value)
//         return it->value->m_type;
    return VTYPE_INVALID;
}

int DictionaryValueImpl::get_bool(cef_dictionary_value_t* s, const cef_string_t* key)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, 0);
//     Dictionary::const_iterator it = self->m_data.find(String(key->str, key->length));
//     if (it != self->m_data.end() && nullptr != it->value && VTYPE_BOOL == it->value->m_type) {
//         FundamentalValueImpl* valueBase = (FundamentalValueImpl*)it->value;
//         return valueBase->m_booleanValue;
//     }
    return 0;
}

int DictionaryValueImpl::get_int(cef_dictionary_value_t* s, const cef_string_t* key)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, 0);
//     Dictionary::const_iterator it = self->m_data.find(String(key->str, key->length));
//     if (it != self->m_data.end() && nullptr != it->value && VTYPE_INT == it->value->m_type) {
//         FundamentalValueImpl* valueBase = (FundamentalValueImpl*)it->value;
//         return valueBase->m_integerValue;
//     }
    return 0;
}

double DictionaryValueImpl::get_double(cef_dictionary_value_t* s, const cef_string_t* key)
{
    CEF_CHECK_ARGS_CAST(DictionaryValueImpl, 0);
//     Dictionary::const_iterator it = self->m_data.find(String(key->str, key->length));
//     if (it != self->m_data.end() && nullptr != it->value && VTYPE_DOUBLE == it->value->m_type) {
//         FundamentalValueImpl* valueBase = (FundamentalValueImpl*)it->value;
//         return valueBase->m_doubleValue;
//     }
    return 0;
}

//////////////////////////////////////////////////////////////////////////

ListValueImpl::ListValueImpl()
{
    memset(&m_baseClass, 0, sizeof(m_baseClass));
    m_valueBase.m_type = VTYPE_LIST;
    m_baseClass.is_valid = is_valid;
    m_baseClass.is_owned = is_owned;
    m_baseClass.is_read_only = is_read_only;
    m_baseClass.copy = copy;
    m_baseClass.set_size = set_size;
    m_baseClass.get_size = get_size;
    m_baseClass.clear = clear;
    m_baseClass.remove = remove;
    m_baseClass.get_type = get_type;
    m_baseClass.get_bool = get_bool;
    m_baseClass.get_int = get_int;
    m_baseClass.get_double = get_double;
    m_baseClass.get_string = get_string;
    m_baseClass.get_binary = get_binary;
    m_baseClass.get_dictionary = get_dictionary;
    m_baseClass.get_list = get_list;
    m_baseClass.set_null = set_null;
    m_baseClass.set_bool = set_bool;
    m_baseClass.set_int = set_int;
    m_baseClass.set_double = set_double;
    m_baseClass.set_string = set_string;
    m_baseClass.set_binary = set_binary;
    m_baseClass.set_dictionary = set_dictionary;
    m_baseClass.set_list = set_list;
    
    CefCppBase<cef_list_value_t, ListValueImpl>::init(this);
    m_valueBase.m_basePtr = &m_baseClass.base;
}

int ListValueImpl::set_size(cef_list_value_t* s, size_t size)
{
    CEF_CHECK_ARGS_CAST(ListValueImpl, false);
    size_t currentSize = self->m_data.size();
    
    if (size < currentSize) {
        // Clean up any values above the requested size.
        for (size_t i = size; i < currentSize; ++i)
            self->m_data[i]->m_basePtr->release(self->m_data[i]->m_basePtr);
        self->m_data.remove(size, currentSize - size);
    } else if (size > 0) {
        // Expand the list size.
        for (size_t i = currentSize; i < size; ++i)
            self->m_data.append(&(new FundamentalValueImpl())->m_valueBase);
    }
    return true;
}

int ListValueImpl::set_int(cef_list_value_t* s, int index, int value)
{
    CEF_CHECK_ARGS_CAST(ListValueImpl, false);
    size_t currentSize = self->m_data.size();
    if (index < 0 || (size_t)index >= currentSize)
        return false;
    self->m_data[index] = &(new FundamentalValueImpl(value))->m_valueBase;
    return true;
}

int ListValueImpl::set_bool(cef_list_value_t* s, int index, int value)
{
    CEF_CHECK_ARGS_CAST(ListValueImpl, false);
    size_t currentSize = self->m_data.size();
    if (index < 0 || (size_t)index >= currentSize)
        return false;
    self->m_data[index] = &(new FundamentalValueImpl((bool)value))->m_valueBase;
    return true;
}

int ListValueImpl::set_null(cef_list_value_t* s, int index)
{
    CEF_CHECK_ARGS_CAST(ListValueImpl, false);
    size_t currentSize = self->m_data.size();
    if (index < 0 || (size_t)index >= currentSize)
        return false;
    self->m_data[index] = &(new FundamentalValueImpl())->m_valueBase;
    return true;
}

int ListValueImpl::set_string(cef_list_value_t* s, int index, const cef_string_t* value)
{
    CEF_CHECK_ARGS_CAST(ListValueImpl, false);
    size_t currentSize = self->m_data.size();
    if (!value || index < 0 || (size_t)index >= currentSize)
        return false;
    self->m_data.insert(index, &(new StringValueImpl(String(value->str, value->length)))->m_valueBase);
    return true;
}
//////////////////////////////////////////////////////////////////////////

static ValueBaseImpl* copyByCast(ValueBaseImpl* self, int exclude_empty_children)
{
    ValueBaseImpl* othar = nullptr;
    FundamentalValueImpl* fundamentalValue = nullptr;
    cef_binary_value_t* binaryValue = nullptr;
    switch (self->m_type) {
    case VTYPE_BOOL:
        fundamentalValue = new FundamentalValueImpl(((FundamentalValueImpl*)self)->m_booleanValue);
        fundamentalValue->ref();
        othar = &fundamentalValue->m_valueBase;
    case VTYPE_DOUBLE:
        fundamentalValue = new FundamentalValueImpl(((FundamentalValueImpl*)self)->m_doubleValue);
        fundamentalValue->ref();
        othar = &fundamentalValue->m_valueBase;
    case VTYPE_INT:
        fundamentalValue = new FundamentalValueImpl(((FundamentalValueImpl*)self)->m_integerValue);
        fundamentalValue->ref();
        othar = &fundamentalValue->m_valueBase;
        break;
    case VTYPE_STRING:
        //othar = (BinaryValueImpl*)BinaryValueImpl::copy(cef_dictionary_value_t* self, exclude_empty_children);
        break;
    case VTYPE_BINARY:
//         binaryValue = BinaryValueImpl::copy(cef_dictionary_value_t* self, exclude_empty_children);
//         othar = &((BinaryValueImpl*)binaryValue)->m_valueBase;
        break;
    case VTYPE_DICTIONARY:
//         othar = (ValueBaseImpl*)DictionaryValueImpl::copy(cef_dictionary_value_t* self, exclude_empty_children);
//         othar = &((DictionaryValueImpl*)binaryValue)->m_valueBase;
        break;
    case VTYPE_LIST:
        //othar = (BinaryValueImpl*)BinaryValueImpl::copy(cef_dictionary_value_t* self, exclude_empty_children);
        break;
    }

    return othar;
}

//////////////////////////////////////////////////////////////////////////

} // cef

CEF_EXPORT cef_binary_value_t* cef_binary_value_create(const void* data, size_t data_size)
{
    cef::BinaryValueImpl* self = new cef::BinaryValueImpl();
    self->ref();
    self->m_data.resize(data_size);
    memcpy(self->m_data.data(), data, data_size);

    return self->cast();
}

CEF_EXPORT cef_list_value_t* cef_list_value_create()
{
    cef::ListValueImpl* self = new cef::ListValueImpl();
    self->ref();
    return self->cast();
}

CEF_EXPORT cef_dictionary_value_t* cef_dictionary_value_create()
{
    cef::DictionaryValueImpl* self = new cef::DictionaryValueImpl();
    self->ref();
    return self->cast();
}

//////////////////////////////////////////////////////////////////////////



