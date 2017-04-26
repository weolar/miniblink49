// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/inspector_protocol/Object.h"

namespace blink {
namespace protocol {

std::unique_ptr<Object> Object::parse(protocol::Value* value, ErrorSupport* errors)
{
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    if (!object) {
        errors->addError("object expected");
        return nullptr;
    }
    return wrapUnique(new Object(wrapUnique(static_cast<DictionaryValue*>(object->clone().release()))));
}

std::unique_ptr<protocol::DictionaryValue> Object::serialize() const
{
    return DictionaryValue::cast(m_object->clone());
}

std::unique_ptr<Object> Object::clone() const
{
    return wrapUnique(new Object(DictionaryValue::cast(m_object->clone())));
}

Object::Object(std::unique_ptr<protocol::DictionaryValue> object) : m_object(std::move(object)) { }

Object::~Object() { }

} // namespace protocol
} // namespace blink
