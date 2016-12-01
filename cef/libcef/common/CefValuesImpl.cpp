// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/common/CefValuesImpl.h"

#include <algorithm>
#include <vector>
#include "include/base/cef_logging.h"

// CefValueImpl implementation.

// static
CefRefPtr<CefValue> CefValue::Create() {
    return new CefValueImpl(base::Value::CreateNullValue()/*.release()*/);
}

// static
CefRefPtr<CefValue> CefValueImpl::GetOrCreateRefOrCopy(
    base::Value* value,
    void* parent_value,
    bool read_only,
    CefValueController* controller) {
    DCHECK(value);

    if (value->IsType(base::Value::TYPE_BINARY)) {
        base::BinaryValue* binary_value = static_cast<base::BinaryValue*>(value);
        return new CefValueImpl(CefBinaryValueImpl::GetOrCreateRef(
            binary_value, parent_value, controller));
    }

    if (value->IsType(base::Value::TYPE_DICTIONARY)) {
        base::DictionaryValue* dict_value =
            static_cast<base::DictionaryValue*>(value);
        return new CefValueImpl(CefDictionaryValueImpl::GetOrCreateRef(
            dict_value, parent_value, read_only, controller));
    }

    if (value->IsType(base::Value::TYPE_LIST)) {
        base::ListValue* list_value = static_cast<base::ListValue*>(value);
        return new CefValueImpl(CefListValueImpl::GetOrCreateRef(
            list_value, parent_value, read_only, controller));
    }

    return new CefValueImpl(value->DeepCopy());
}

CefValueImpl::CefValueImpl() {
}

CefValueImpl::CefValueImpl(base::Value* value) {
    SetValue(value);
}

CefValueImpl::CefValueImpl(CefRefPtr<CefBinaryValue> value)
    : binary_value_(value) {
}

CefValueImpl::CefValueImpl(CefRefPtr<CefDictionaryValue> value)
    : dictionary_value_(value) {
}

CefValueImpl::CefValueImpl(CefRefPtr<CefListValue> value)
    : list_value_(value) {
}

CefValueImpl::~CefValueImpl() {
}

void CefValueImpl::SetValue(base::Value* value) {
    base::AutoLock lock_scope(lock_);
    SetValueInternal(value);
}

base::Value* CefValueImpl::CopyOrTransferValue(
    void* new_parent_value,
    bool new_read_only,
    CefValueController* new_controller) {
    base::AutoLock lock_scope(lock_);

    if (binary_value_) {
        base::BinaryValue* value =
            static_cast<CefBinaryValueImpl*>(binary_value_.get())->
            CopyOrDetachValue(new_controller);
        binary_value_ = CefBinaryValueImpl::GetOrCreateRef(
            value, new_parent_value, new_controller);
        return value;
    }

    if (dictionary_value_) {
        base::DictionaryValue* value =
            static_cast<CefDictionaryValueImpl*>(dictionary_value_.get())->
            CopyOrDetachValue(new_controller);
        dictionary_value_ = CefDictionaryValueImpl::GetOrCreateRef(
            value, new_parent_value, new_read_only, new_controller);
        return value;
    }

    if (list_value_) {
        base::ListValue* value =
            static_cast<CefListValueImpl*>(list_value_.get())->
            CopyOrDetachValue(new_controller);
        list_value_ = CefListValueImpl::GetOrCreateRef(
            value, new_parent_value, new_read_only, new_controller);
        return value;
    }

    return value_->DeepCopy();
}

bool CefValueImpl::IsValid() {
    base::AutoLock lock_scope(lock_);

    if (binary_value_)
        return binary_value_->IsValid();
    if (dictionary_value_)
        return dictionary_value_->IsValid();
    if (list_value_)
        return list_value_->IsValid();

    return (value_ != NULL);
}

bool CefValueImpl::IsOwned() {
    base::AutoLock lock_scope(lock_);

    if (binary_value_)
        return binary_value_->IsOwned();
    if (dictionary_value_)
        return dictionary_value_->IsOwned();
    if (list_value_)
        return list_value_->IsOwned();

    return false;
}

bool CefValueImpl::IsReadOnly() {
    base::AutoLock lock_scope(lock_);

    if (binary_value_)
        return true;
    if (dictionary_value_)
        return dictionary_value_->IsReadOnly();
    if (list_value_)
        return list_value_->IsReadOnly();

    return false;
}

bool CefValueImpl::IsSame(CefRefPtr<CefValue> that) {
    if (that.get() == this)
        return true;
    if (!that.get() || that->GetType() != GetType())
        return false;

    CefValueImpl* impl = static_cast<CefValueImpl*>(that.get());

    base::AutoLock lock_scope(lock_);
    base::AutoLock lock_scope2(impl->lock_);

    if (binary_value_)
        return binary_value_->IsSame(impl->binary_value_);
    if (dictionary_value_)
        return dictionary_value_->IsSame(impl->dictionary_value_);
    if (list_value_)
        return list_value_->IsSame(impl->list_value_);

    // Simple types are never the same.
    return false;
}

bool CefValueImpl::IsEqual(CefRefPtr<CefValue> that) {
    if (that.get() == this)
        return true;
    if (!that.get() || that->GetType() != GetType())
        return false;

    CefValueImpl* impl = static_cast<CefValueImpl*>(that.get());

    base::AutoLock lock_scope(lock_);
    base::AutoLock lock_scope2(impl->lock_);

    if (binary_value_)
        return binary_value_->IsEqual(impl->binary_value_);
    if (dictionary_value_)
        return dictionary_value_->IsEqual(impl->dictionary_value_);
    if (list_value_)
        return list_value_->IsEqual(impl->list_value_);

    if (!value_)  // Invalid types are equal.
        return true;

    return value_->Equals(impl->value_.get());
}

CefRefPtr<CefValue> CefValueImpl::Copy() {
    base::AutoLock lock_scope(lock_);

    if (binary_value_)
        return new CefValueImpl(binary_value_->Copy());
    if (dictionary_value_)
        return new CefValueImpl(dictionary_value_->Copy(false));
    if (list_value_)
        return new CefValueImpl(list_value_->Copy());
    if (value_)
        return new CefValueImpl(value_->DeepCopy());

    return new CefValueImpl();
}

CefValueType CefValueImpl::GetType() {
    base::AutoLock lock_scope(lock_);

    if (binary_value_)
        return VTYPE_BINARY;
    if (dictionary_value_)
        return VTYPE_DICTIONARY;
    if (list_value_)
        return VTYPE_LIST;

    if (value_) {
        switch (value_->GetType()) {
        case base::Value::TYPE_NULL:
            return VTYPE_NULL;
        case base::Value::TYPE_BOOLEAN:
            return VTYPE_BOOL;
        case base::Value::TYPE_INTEGER:
            return VTYPE_INT;
        case base::Value::TYPE_DOUBLE:
            return VTYPE_DOUBLE;
        case base::Value::TYPE_STRING:
            return VTYPE_STRING;
        default:
            NOTREACHED();
            break;
        }
    }

    return VTYPE_INVALID;
}

bool CefValueImpl::GetBool() {
    base::AutoLock lock_scope(lock_);

    bool ret_value = false;
    if (value_)
        value_->GetAsBoolean(&ret_value);
    return ret_value;
}

int CefValueImpl::GetInt() {
    base::AutoLock lock_scope(lock_);

    int ret_value = 0;
    if (value_)
        value_->GetAsInteger(&ret_value);
    return ret_value;
}

double CefValueImpl::GetDouble() {
    base::AutoLock lock_scope(lock_);

    double ret_value = 0;
    if (value_)
        value_->GetAsDouble(&ret_value);
    return ret_value;
}

CefString CefValueImpl::GetString() {
    base::AutoLock lock_scope(lock_);

    std::string ret_value;
    if (value_)
        value_->GetAsString(&ret_value);
    return ret_value;
}

CefRefPtr<CefBinaryValue> CefValueImpl::GetBinary() {
    base::AutoLock lock_scope(lock_);
    return binary_value_;
}

CefRefPtr<CefDictionaryValue> CefValueImpl::GetDictionary() {
    base::AutoLock lock_scope(lock_);
    return dictionary_value_;
}

CefRefPtr<CefListValue> CefValueImpl::GetList() {
    base::AutoLock lock_scope(lock_);
    return list_value_;
}

bool CefValueImpl::SetNull() {
    SetValue(base::Value::CreateNullValue()/*.release()*/);
    return true;
}

bool CefValueImpl::SetBool(bool value) {
    SetValue(new base::FundamentalValue(value));
    return true;
}

bool CefValueImpl::SetInt(int value) {
    SetValue(new base::FundamentalValue(value));
    return true;
}

bool CefValueImpl::SetDouble(double value) {
    SetValue(new base::FundamentalValue(value));
    return true;
}

bool CefValueImpl::SetString(const CefString& value) {
    SetValue(new base::StringValue(value.ToString()));
    return true;
}

bool CefValueImpl::SetBinary(CefRefPtr<CefBinaryValue> value) {
    base::AutoLock lock_scope(lock_);
    SetValueInternal(NULL);
    binary_value_ = value;
    return true;
}

bool CefValueImpl::SetDictionary(CefRefPtr<CefDictionaryValue> value) {
    base::AutoLock lock_scope(lock_);
    SetValueInternal(NULL);
    dictionary_value_ = value;
    return true;
}

bool CefValueImpl::SetList(CefRefPtr<CefListValue> value) {
    base::AutoLock lock_scope(lock_);
    SetValueInternal(NULL);
    list_value_ = value;
    return true;
}

void CefValueImpl::SetValueInternal(base::Value* value) {
    lock_.AssertAcquired();

    value_.reset(NULL);
    binary_value_ = NULL;
    dictionary_value_ = NULL;
    list_value_ = NULL;

    if (value) {
        switch (value->GetType()) {
        case base::Value::TYPE_BINARY:
            binary_value_ = new CefBinaryValueImpl(
                static_cast<base::BinaryValue*>(value), true);
            return;
        case base::Value::TYPE_DICTIONARY:
            dictionary_value_ = new CefDictionaryValueImpl(
                static_cast<base::DictionaryValue*>(value), true, false);
            return;
        case base::Value::TYPE_LIST:
            list_value_ = new CefListValueImpl(
                static_cast<base::ListValue*>(value), true, false);
            return;
        default:
            value_.reset(value);
        }
    }
}

CefValueController* CefValueImpl::GetValueController() const {
    lock_.AssertAcquired();

    if (binary_value_) {
        return static_cast<CefBinaryValueImpl*>(binary_value_.get())->controller();
    }
    else if (dictionary_value_) {
        return static_cast<CefDictionaryValueImpl*>(dictionary_value_.get())->
            controller();
    }
    else if (list_value_) {
        return static_cast<CefListValueImpl*>(list_value_.get())->controller();
    }

    return NULL;
}

void CefValueImpl::AcquireLock() {
    lock_.Acquire();

    CefValueController* controller = GetValueController();
    if (controller)
        controller->lock();
}

void CefValueImpl::ReleaseLock() {
    CefValueController* controller = GetValueController();
    if (controller) {
        controller->AssertLockAcquired();
        controller->unlock();
    }

    lock_.Release();
}

base::Value* CefValueImpl::GetValueUnsafe() const {
    lock_.AssertAcquired();

    if (binary_value_) {
        return static_cast<CefBinaryValueImpl*>(binary_value_.get())->
            GetValueUnsafe();
    }
    else if (dictionary_value_) {
        return static_cast<CefDictionaryValueImpl*>(dictionary_value_.get())->
            GetValueUnsafe();
    }
    else if (list_value_) {
        return static_cast<CefListValueImpl*>(list_value_.get())->GetValueUnsafe();
    }

    return value_.get();
}


// CefBinaryValueImpl implementation.

CefRefPtr<CefBinaryValue> CefBinaryValue::Create(const void* data,
    size_t data_size) {
    DCHECK(data);
    DCHECK_GT(data_size, (size_t)0);
    if (!data || data_size == 0)
        return NULL;

    return new CefBinaryValueImpl(static_cast<char*>(const_cast<void*>(data)),
        data_size, true);
}

// static
CefRefPtr<CefBinaryValue> CefBinaryValueImpl::GetOrCreateRef(
    base::BinaryValue* value,
    void* parent_value,
    CefValueController* controller) {
    DCHECK(value);
    DCHECK(parent_value);
    DCHECK(controller);

    CefValueController::Object* object = controller->Get(value);
    if (object)
        return static_cast<CefBinaryValueImpl*>(object);

    return new CefBinaryValueImpl(value, parent_value,
        CefBinaryValueImpl::kReference, controller);
}

CefBinaryValueImpl::CefBinaryValueImpl(base::BinaryValue* value,
    bool will_delete)
    : CefValueBase<CefBinaryValue, base::BinaryValue>(
    value, NULL, will_delete ? kOwnerWillDelete : kOwnerNoDelete,
    true, NULL) {
}

CefBinaryValueImpl::CefBinaryValueImpl(char* data,
    size_t data_size,
    bool copy)
    : CefValueBase<CefBinaryValue, base::BinaryValue>(
    copy ? base::BinaryValue::CreateWithCopiedBuffer(data, data_size) :
    new base::BinaryValue(scoped_ptr<char[]>(data), data_size),
    NULL, kOwnerWillDelete, true, NULL) {
}

base::BinaryValue* CefBinaryValueImpl::CopyValue() {
    CEF_VALUE_VERIFY_RETURN(false, NULL);
    return const_value().DeepCopy();
}

base::BinaryValue* CefBinaryValueImpl::CopyOrDetachValue(
    CefValueController* new_controller) {
    base::BinaryValue* new_value;

    if (!will_delete()) {
        // Copy the value.
        new_value = CopyValue();
    }
    else {
        // Take ownership of the value.
        new_value = Detach(new_controller);
    }

    DCHECK(new_value);
    return new_value;
}

bool CefBinaryValueImpl::IsSameValue(const base::BinaryValue* that) {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return (&const_value() == that);
}

bool CefBinaryValueImpl::IsEqualValue(const base::BinaryValue* that) {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return const_value().Equals(that);
}

base::BinaryValue* CefBinaryValueImpl::GetValueUnsafe() {
    if (!VerifyAttached())
        return NULL;
    controller()->AssertLockAcquired();
    return const_cast<base::BinaryValue*>(&const_value());
}

bool CefBinaryValueImpl::IsValid() {
    return !detached();
}

bool CefBinaryValueImpl::IsOwned() {
    return !will_delete();
}

bool CefBinaryValueImpl::IsSame(CefRefPtr<CefBinaryValue> that) {
    if (!that.get())
        return false;
    if (that.get() == this)
        return true;

    CEF_VALUE_VERIFY_RETURN(false, false);
    return static_cast<CefBinaryValueImpl*>(that.get())->
        IsSameValue(&const_value());
}

bool CefBinaryValueImpl::IsEqual(CefRefPtr<CefBinaryValue> that) {
    if (!that.get())
        return false;
    if (that.get() == this)
        return true;

    CEF_VALUE_VERIFY_RETURN(false, false);
    return static_cast<CefBinaryValueImpl*>(that.get())->
        IsEqualValue(&const_value());
}

CefRefPtr<CefBinaryValue> CefBinaryValueImpl::Copy() {
    CEF_VALUE_VERIFY_RETURN(false, NULL);
    return new CefBinaryValueImpl(const_value().DeepCopy(), NULL,
        CefBinaryValueImpl::kOwnerWillDelete, NULL);
}

size_t CefBinaryValueImpl::GetSize() {
    CEF_VALUE_VERIFY_RETURN(false, 0);
    return const_value().GetSize();
}

size_t CefBinaryValueImpl::GetData(void* buffer,
    size_t buffer_size,
    size_t data_offset) {
    DCHECK(buffer);
    DCHECK_GT(buffer_size, (size_t)0);
    if (!buffer || buffer_size == 0)
        return 0;

    CEF_VALUE_VERIFY_RETURN(false, 0);

    size_t size = const_value().GetSize();
    DCHECK_LT(data_offset, size);
    if (data_offset >= size)
        return 0;

    size = std::min(buffer_size, size - data_offset);
    const char* data = const_value().GetBuffer();
    memcpy(buffer, data + data_offset, size);
    return size;
}

CefBinaryValueImpl::CefBinaryValueImpl(base::BinaryValue* value,
    void* parent_value,
    ValueMode value_mode,
    CefValueController* controller)
    : CefValueBase<CefBinaryValue, base::BinaryValue>(
    value, parent_value, value_mode, true, controller) {
}


// CefDictionaryValueImpl implementation.

// static
CefRefPtr<CefDictionaryValue> CefDictionaryValue::Create() {
    return new CefDictionaryValueImpl(new base::DictionaryValue(), true, false);
}

// static
CefRefPtr<CefDictionaryValue> CefDictionaryValueImpl::GetOrCreateRef(
    base::DictionaryValue* value,
    void* parent_value,
    bool read_only,
    CefValueController* controller) {
    CefValueController::Object* object = controller->Get(value);
    if (object)
        return static_cast<CefDictionaryValueImpl*>(object);

    return new CefDictionaryValueImpl(value, parent_value,
        CefDictionaryValueImpl::kReference, read_only, controller);
}

CefDictionaryValueImpl::CefDictionaryValueImpl(base::DictionaryValue* value,
    bool will_delete,
    bool read_only)
    : CefValueBase<CefDictionaryValue, base::DictionaryValue>(
    value, NULL, will_delete ? kOwnerWillDelete : kOwnerNoDelete,
    read_only, NULL) {
}

base::DictionaryValue* CefDictionaryValueImpl::CopyValue() {
    CEF_VALUE_VERIFY_RETURN(false, NULL);
    return const_value().DeepCopy();
}

base::DictionaryValue* CefDictionaryValueImpl::CopyOrDetachValue(
    CefValueController* new_controller) {
    base::DictionaryValue* new_value;

    if (!will_delete()) {
        // Copy the value.
        new_value = CopyValue();
    }
    else {
        // Take ownership of the value.
        new_value = Detach(new_controller);
    }

    DCHECK(new_value);
    return new_value;
}

bool CefDictionaryValueImpl::IsSameValue(const base::DictionaryValue* that) {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return (&const_value() == that);
}

bool CefDictionaryValueImpl::IsEqualValue(const base::DictionaryValue* that) {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return const_value().Equals(that);
}

base::DictionaryValue* CefDictionaryValueImpl::GetValueUnsafe() {
    if (!VerifyAttached())
        return NULL;
    controller()->AssertLockAcquired();
    return const_cast<base::DictionaryValue*>(&const_value());
}

bool CefDictionaryValueImpl::IsValid() {
    return !detached();
}

bool CefDictionaryValueImpl::IsOwned() {
    return !will_delete();
}

bool CefDictionaryValueImpl::IsReadOnly() {
    return read_only();
}

bool CefDictionaryValueImpl::IsSame(CefRefPtr<CefDictionaryValue> that) {
    if (!that.get())
        return false;
    if (that.get() == this)
        return true;

    CEF_VALUE_VERIFY_RETURN(false, false);
    return static_cast<CefDictionaryValueImpl*>(that.get())->
        IsSameValue(&const_value());
}

bool CefDictionaryValueImpl::IsEqual(CefRefPtr<CefDictionaryValue> that) {
    if (!that.get())
        return false;
    if (that.get() == this)
        return true;

    CEF_VALUE_VERIFY_RETURN(false, false);
    return static_cast<CefDictionaryValueImpl*>(that.get())->
        IsEqualValue(&const_value());
}

CefRefPtr<CefDictionaryValue> CefDictionaryValueImpl::Copy(
    bool exclude_empty_children) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    base::DictionaryValue* value;
    if (exclude_empty_children) {
        value = const_cast<base::DictionaryValue&>(
            const_value()).DeepCopyWithoutEmptyChildren()/*.release()*/;
    }
    else {
        value = const_value().DeepCopy();
    }

    return new CefDictionaryValueImpl(value, NULL,
        CefDictionaryValueImpl::kOwnerWillDelete, false, NULL);
}

size_t CefDictionaryValueImpl::GetSize() {
    CEF_VALUE_VERIFY_RETURN(false, 0);
    return const_value().size();
}

bool CefDictionaryValueImpl::Clear() {
    CEF_VALUE_VERIFY_RETURN(true, false);

    // Detach any dependent values.
    controller()->RemoveDependencies(mutable_value());

    mutable_value()->Clear();
    return true;
}

bool CefDictionaryValueImpl::HasKey(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, 0);
    return const_value().HasKey(key);
}

bool CefDictionaryValueImpl::GetKeys(KeyList& keys) {
    CEF_VALUE_VERIFY_RETURN(false, 0);

    for (base::DictionaryValue::Iterator i(const_value());
        !i.IsAtEnd(); i.Advance()) {
        keys.push_back(i.key());
    }

    return true;
}

bool CefDictionaryValueImpl::Remove(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    return RemoveInternal(key);
}

CefValueType CefDictionaryValueImpl::GetType(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, VTYPE_INVALID);

    const base::Value* out_value = NULL;
    if (const_value().GetWithoutPathExpansion(key, &out_value)) {
        switch (out_value->GetType()) {
        case base::Value::TYPE_NULL:
            return VTYPE_NULL;
        case base::Value::TYPE_BOOLEAN:
            return VTYPE_BOOL;
        case base::Value::TYPE_INTEGER:
            return VTYPE_INT;
        case base::Value::TYPE_DOUBLE:
            return VTYPE_DOUBLE;
        case base::Value::TYPE_STRING:
            return VTYPE_STRING;
        case base::Value::TYPE_BINARY:
            return VTYPE_BINARY;
        case base::Value::TYPE_DICTIONARY:
            return VTYPE_DICTIONARY;
        case base::Value::TYPE_LIST:
            return VTYPE_LIST;
        }
    }

    return VTYPE_INVALID;
}

CefRefPtr<CefValue> CefDictionaryValueImpl::GetValue(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    const base::Value* out_value = NULL;
    if (const_value().GetWithoutPathExpansion(key, &out_value)) {
        return CefValueImpl::GetOrCreateRefOrCopy(
            const_cast<base::Value*>(out_value),
            const_cast<base::DictionaryValue*>(&const_value()),
            read_only(),
            controller());
    }

    return NULL;
}

bool CefDictionaryValueImpl::GetBool(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, false);

    const base::Value* out_value = NULL;
    bool ret_value = false;

    if (const_value().GetWithoutPathExpansion(key, &out_value))
        out_value->GetAsBoolean(&ret_value);

    return ret_value;
}

int CefDictionaryValueImpl::GetInt(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, 0);

    const base::Value* out_value = NULL;
    int ret_value = 0;

    if (const_value().GetWithoutPathExpansion(key, &out_value))
        out_value->GetAsInteger(&ret_value);

    return ret_value;
}

double CefDictionaryValueImpl::GetDouble(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, 0);

    const base::Value* out_value = NULL;
    double ret_value = 0;

    if (const_value().GetWithoutPathExpansion(key, &out_value))
        out_value->GetAsDouble(&ret_value);

    return ret_value;
}

CefString CefDictionaryValueImpl::GetString(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, CefString());

    const base::Value* out_value = NULL;
    std::string ret_value;

    if (const_value().GetWithoutPathExpansion(key, &out_value))
        out_value->GetAsString(&ret_value);

    return ret_value;
}

CefRefPtr<CefBinaryValue> CefDictionaryValueImpl::GetBinary(
    const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    const base::Value* out_value = NULL;

    if (const_value().GetWithoutPathExpansion(key, &out_value) &&
        out_value->IsType(base::Value::TYPE_BINARY)) {
        base::BinaryValue* binary_value =
            static_cast<base::BinaryValue*>(const_cast<base::Value*>(out_value));
        return CefBinaryValueImpl::GetOrCreateRef(binary_value,
            const_cast<base::DictionaryValue*>(&const_value()), controller());
    }

    return NULL;
}

CefRefPtr<CefDictionaryValue> CefDictionaryValueImpl::GetDictionary(
    const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    const base::Value* out_value = NULL;

    if (const_value().GetWithoutPathExpansion(key, &out_value) &&
        out_value->IsType(base::Value::TYPE_DICTIONARY)) {
        base::DictionaryValue* dict_value =
            static_cast<base::DictionaryValue*>(
            const_cast<base::Value*>(out_value));
        return CefDictionaryValueImpl::GetOrCreateRef(
            dict_value,
            const_cast<base::DictionaryValue*>(&const_value()),
            read_only(),
            controller());
    }

    return NULL;
}

CefRefPtr<CefListValue> CefDictionaryValueImpl::GetList(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    const base::Value* out_value = NULL;

    if (const_value().GetWithoutPathExpansion(key, &out_value) &&
        out_value->IsType(base::Value::TYPE_LIST)) {
        base::ListValue* list_value =
            static_cast<base::ListValue*>(const_cast<base::Value*>(out_value));
        return CefListValueImpl::GetOrCreateRef(
            list_value,
            const_cast<base::DictionaryValue*>(&const_value()),
            read_only(),
            controller());
    }

    return NULL;
}

bool CefDictionaryValueImpl::SetValue(const CefString& key,
    CefRefPtr<CefValue> value) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    CefValueImpl* impl = static_cast<CefValueImpl*>(value.get());
    DCHECK(impl);

    base::Value* new_value =
        impl->CopyOrTransferValue(mutable_value(), false, controller());
    SetInternal(key, new_value);
    return true;
}

bool CefDictionaryValueImpl::SetNull(const CefString& key) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(key, base::Value::CreateNullValue()/*.release()*/);
    return true;
}

bool CefDictionaryValueImpl::SetBool(const CefString& key, bool value) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(key, new base::FundamentalValue(value));
    return true;
}

bool CefDictionaryValueImpl::SetInt(const CefString& key, int value) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(key, new base::FundamentalValue(value));
    return true;
}

bool CefDictionaryValueImpl::SetDouble(const CefString& key, double value) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(key, new base::FundamentalValue(value));
    return true;
}

bool CefDictionaryValueImpl::SetString(const CefString& key,
    const CefString& value) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(key, new base::StringValue(value.ToString()));
    return true;
}

bool CefDictionaryValueImpl::SetBinary(const CefString& key,
    CefRefPtr<CefBinaryValue> value) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    CefBinaryValueImpl* impl = static_cast<CefBinaryValueImpl*>(value.get());
    DCHECK(impl);

    SetInternal(key, impl->CopyOrDetachValue(controller()));
    return true;
}

bool CefDictionaryValueImpl::SetDictionary(
    const CefString& key, CefRefPtr<CefDictionaryValue> value) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    CefDictionaryValueImpl* impl =
        static_cast<CefDictionaryValueImpl*>(value.get());
    DCHECK(impl);

    SetInternal(key, impl->CopyOrDetachValue(controller()));
    return true;
}

bool CefDictionaryValueImpl::SetList(const CefString& key,
    CefRefPtr<CefListValue> value) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    CefListValueImpl* impl = static_cast<CefListValueImpl*>(value.get());
    DCHECK(impl);

    SetInternal(key, impl->CopyOrDetachValue(controller()));
    return true;
}

bool CefDictionaryValueImpl::RemoveInternal(const CefString& key) {
    scoped_ptr<base::Value> out_value;
    if (!mutable_value()->RemoveWithoutPathExpansion(key, &out_value))
        return false;

    // Remove the value.
    controller()->Remove(out_value.get(), true);

    // Only list and dictionary types may have dependencies.
    if (out_value->IsType(base::Value::TYPE_LIST) ||
        out_value->IsType(base::Value::TYPE_DICTIONARY)) {
        controller()->RemoveDependencies(out_value.get());
    }

    return true;
}

void CefDictionaryValueImpl::SetInternal(const CefString& key,
    base::Value* value) {
    DCHECK(value);
    RemoveInternal(key);
    mutable_value()->SetWithoutPathExpansion(key, value);
}

CefDictionaryValueImpl::CefDictionaryValueImpl(
    base::DictionaryValue* value,
    void* parent_value,
    ValueMode value_mode,
    bool read_only,
    CefValueController* controller)
    : CefValueBase<CefDictionaryValue, base::DictionaryValue>(
    value, parent_value, value_mode, read_only, controller) {
}


// CefListValueImpl implementation.

// static
CefRefPtr<CefListValue> CefListValue::Create() {
    return new CefListValueImpl(new base::ListValue(), true, false);
}

// static
CefRefPtr<CefListValue> CefListValueImpl::GetOrCreateRef(
    base::ListValue* value,
    void* parent_value,
    bool read_only,
    CefValueController* controller) {
    CefValueController::Object* object = controller->Get(value);
    if (object)
        return static_cast<CefListValueImpl*>(object);

    return new CefListValueImpl(value, parent_value,
        CefListValueImpl::kReference, read_only, controller);
}

CefListValueImpl::CefListValueImpl(base::ListValue* value,
    bool will_delete,
    bool read_only)
    : CefValueBase<CefListValue, base::ListValue>(
    value, NULL, will_delete ? kOwnerWillDelete : kOwnerNoDelete,
    read_only, NULL) {
}

base::ListValue* CefListValueImpl::CopyValue() {
    CEF_VALUE_VERIFY_RETURN(false, NULL);
    return const_value().DeepCopy();
}

base::ListValue* CefListValueImpl::CopyOrDetachValue(
    CefValueController* new_controller) {
    base::ListValue* new_value;

    if (!will_delete()) {
        // Copy the value.
        new_value = CopyValue();
    }
    else {
        // Take ownership of the value.
        new_value = Detach(new_controller);
    }

    DCHECK(new_value);
    return new_value;
}

bool CefListValueImpl::IsSameValue(const base::ListValue* that) {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return (&const_value() == that);
}

bool CefListValueImpl::IsEqualValue(const base::ListValue* that) {
    CEF_VALUE_VERIFY_RETURN(false, false);
    return const_value().Equals(that);
}

base::ListValue* CefListValueImpl::GetValueUnsafe() {
    if (!VerifyAttached())
        return NULL;
    controller()->AssertLockAcquired();
    return const_cast<base::ListValue*>(&const_value());
}

bool CefListValueImpl::IsValid() {
    return !detached();
}

bool CefListValueImpl::IsOwned() {
    return !will_delete();
}

bool CefListValueImpl::IsReadOnly() {
    return read_only();
}

bool CefListValueImpl::IsSame(CefRefPtr<CefListValue> that) {
    if (!that.get())
        return false;
    if (that.get() == this)
        return true;

    CEF_VALUE_VERIFY_RETURN(false, false);
    return static_cast<CefListValueImpl*>(that.get())->
        IsSameValue(&const_value());
}

bool CefListValueImpl::IsEqual(CefRefPtr<CefListValue> that) {
    if (!that.get())
        return false;
    if (that.get() == this)
        return true;

    CEF_VALUE_VERIFY_RETURN(false, false);
    return static_cast<CefListValueImpl*>(that.get())->
        IsEqualValue(&const_value());
}

CefRefPtr<CefListValue> CefListValueImpl::Copy() {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    return new CefListValueImpl(const_value().DeepCopy(), NULL,
        CefListValueImpl::kOwnerWillDelete, false, NULL);
}

bool CefListValueImpl::SetSize(size_t size) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    size_t current_size = const_value().GetSize();
    if (size < current_size) {
        // Clean up any values above the requested size.
        for (size_t i = current_size - 1; i >= size; --i)
            RemoveInternal(i);
    }
    else if (size > 0) {
        // Expand the list size.
        mutable_value()->Set(size - 1, base::Value::CreateNullValue());
    }
    return true;
}

size_t CefListValueImpl::GetSize() {
    CEF_VALUE_VERIFY_RETURN(false, 0);
    return const_value().GetSize();
}

bool CefListValueImpl::Clear() {
    CEF_VALUE_VERIFY_RETURN(true, false);

    // Detach any dependent values.
    controller()->RemoveDependencies(mutable_value());

    mutable_value()->Clear();
    return true;
}

bool CefListValueImpl::Remove(int index) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    return RemoveInternal(index);
}

CefValueType CefListValueImpl::GetType(int index) {
    CEF_VALUE_VERIFY_RETURN(false, VTYPE_INVALID);

    const base::Value* out_value = NULL;
    if (const_value().Get(index, &out_value)) {
        switch (out_value->GetType()) {
        case base::Value::TYPE_NULL:
            return VTYPE_NULL;
        case base::Value::TYPE_BOOLEAN:
            return VTYPE_BOOL;
        case base::Value::TYPE_INTEGER:
            return VTYPE_INT;
        case base::Value::TYPE_DOUBLE:
            return VTYPE_DOUBLE;
        case base::Value::TYPE_STRING:
            return VTYPE_STRING;
        case base::Value::TYPE_BINARY:
            return VTYPE_BINARY;
        case base::Value::TYPE_DICTIONARY:
            return VTYPE_DICTIONARY;
        case base::Value::TYPE_LIST:
            return VTYPE_LIST;
        }
    }

    return VTYPE_INVALID;
}

CefRefPtr<CefValue> CefListValueImpl::GetValue(int index) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    const base::Value* out_value = NULL;
    if (const_value().Get(index, &out_value)) {
        return CefValueImpl::GetOrCreateRefOrCopy(
            const_cast<base::Value*>(out_value),
            const_cast<base::ListValue*>(&const_value()),
            read_only(),
            controller());
    }

    return NULL;
}

bool CefListValueImpl::GetBool(int index) {
    CEF_VALUE_VERIFY_RETURN(false, false);

    const base::Value* out_value = NULL;
    bool ret_value = false;

    if (const_value().Get(index, &out_value))
        out_value->GetAsBoolean(&ret_value);

    return ret_value;
}

int CefListValueImpl::GetInt(int index) {
    CEF_VALUE_VERIFY_RETURN(false, 0);

    const base::Value* out_value = NULL;
    int ret_value = 0;

    if (const_value().Get(index, &out_value))
        out_value->GetAsInteger(&ret_value);

    return ret_value;
}

double CefListValueImpl::GetDouble(int index) {
    CEF_VALUE_VERIFY_RETURN(false, 0);

    const base::Value* out_value = NULL;
    double ret_value = 0;

    if (const_value().Get(index, &out_value))
        out_value->GetAsDouble(&ret_value);

    return ret_value;
}

CefString CefListValueImpl::GetString(int index) {
    CEF_VALUE_VERIFY_RETURN(false, CefString());

    const base::Value* out_value = NULL;
    std::string ret_value;

    if (const_value().Get(index, &out_value))
        out_value->GetAsString(&ret_value);

    return ret_value;
}

CefRefPtr<CefBinaryValue> CefListValueImpl::GetBinary(int index) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    const base::Value* out_value = NULL;

    if (const_value().Get(index, &out_value) &&
        out_value->IsType(base::Value::TYPE_BINARY)) {
        base::BinaryValue* binary_value =
            static_cast<base::BinaryValue*>(const_cast<base::Value*>(out_value));
        return CefBinaryValueImpl::GetOrCreateRef(binary_value,
            const_cast<base::ListValue*>(&const_value()), controller());
    }

    return NULL;
}

CefRefPtr<CefDictionaryValue> CefListValueImpl::GetDictionary(int index) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    const base::Value* out_value = NULL;

    if (const_value().Get(index, &out_value) &&
        out_value->IsType(base::Value::TYPE_DICTIONARY)) {
        base::DictionaryValue* dict_value =
            static_cast<base::DictionaryValue*>(
            const_cast<base::Value*>(out_value));
        return CefDictionaryValueImpl::GetOrCreateRef(
            dict_value,
            const_cast<base::ListValue*>(&const_value()),
            read_only(),
            controller());
    }

    return NULL;
}

CefRefPtr<CefListValue> CefListValueImpl::GetList(int index) {
    CEF_VALUE_VERIFY_RETURN(false, NULL);

    const base::Value* out_value = NULL;

    if (const_value().Get(index, &out_value) &&
        out_value->IsType(base::Value::TYPE_LIST)) {
        base::ListValue* list_value =
            static_cast<base::ListValue*>(const_cast<base::Value*>(out_value));
        return CefListValueImpl::GetOrCreateRef(
            list_value,
            const_cast<base::ListValue*>(&const_value()),
            read_only(),
            controller());
    }

    return NULL;
}

bool CefListValueImpl::SetValue(int index, CefRefPtr<CefValue> value) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    CefValueImpl* impl = static_cast<CefValueImpl*>(value.get());
    DCHECK(impl);

    base::Value* new_value =
        impl->CopyOrTransferValue(mutable_value(), false, controller());
    SetInternal(index, new_value);
    return true;
}

bool CefListValueImpl::SetNull(int index) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(index, base::Value::CreateNullValue()/*.release()*/);
    return true;
}

bool CefListValueImpl::SetBool(int index, bool value) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(index, new base::FundamentalValue(value));
    return true;
}

bool CefListValueImpl::SetInt(int index, int value) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(index, new base::FundamentalValue(value));
    return true;
}

bool CefListValueImpl::SetDouble(int index, double value) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(index, new base::FundamentalValue(value));
    return true;
}

bool CefListValueImpl::SetString(int index, const CefString& value) {
    CEF_VALUE_VERIFY_RETURN(true, false);
    SetInternal(index, new base::StringValue(value.ToString()));
    return true;
}

bool CefListValueImpl::SetBinary(int index, CefRefPtr<CefBinaryValue> value) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    CefBinaryValueImpl* impl = static_cast<CefBinaryValueImpl*>(value.get());
    DCHECK(impl);

    SetInternal(index, impl->CopyOrDetachValue(controller()));
    return true;
}

bool CefListValueImpl::SetDictionary(int index,
    CefRefPtr<CefDictionaryValue> value) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    CefDictionaryValueImpl* impl =
        static_cast<CefDictionaryValueImpl*>(value.get());
    DCHECK(impl);

    SetInternal(index, impl->CopyOrDetachValue(controller()));
    return true;
}

bool CefListValueImpl::SetList(int index, CefRefPtr<CefListValue> value) {
    CEF_VALUE_VERIFY_RETURN(true, false);

    CefListValueImpl* impl = static_cast<CefListValueImpl*>(value.get());
    DCHECK(impl);

    SetInternal(index, impl->CopyOrDetachValue(controller()));
    return true;
}

bool CefListValueImpl::RemoveInternal(int index) {
    scoped_ptr<base::Value> out_value;
    if (!mutable_value()->Remove(index, &out_value))
        return false;

    // Remove the value.
    controller()->Remove(out_value.get(), true);

    // Only list and dictionary types may have dependencies.
    if (out_value->IsType(base::Value::TYPE_LIST) ||
        out_value->IsType(base::Value::TYPE_DICTIONARY)) {
        controller()->RemoveDependencies(out_value.get());
    }

    return true;
}

void CefListValueImpl::SetInternal(int index, base::Value* value) {
    DCHECK(value);
    if (RemoveInternal(index))
        mutable_value()->Insert(index, value);
    else
        mutable_value()->Set(index, value);
}

CefListValueImpl::CefListValueImpl(
    base::ListValue* value,
    void* parent_value,
    ValueMode value_mode,
    bool read_only,
    CefValueController* controller)
    : CefValueBase<CefListValue, base::ListValue>(
    value, parent_value, value_mode, read_only, controller) {
}
#endif
