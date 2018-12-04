// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_COMMON_KEY_WEAK_MAP_H_
#define ATOM_COMMON_KEY_WEAK_MAP_H_

#include "base/macros.h"
#include "v8.h"

#include <map>
#include <utility>
#include <vector>

namespace atom {

// Like ES6's WeakMap, but the key is Integer and the value is Weak Pointer.
template<typename K>
class KeyWeakMap {
public:
    // Records the key and self, used by SetWeak.
    struct KeyObject {
        KeyObject(const K& otherKey, KeyWeakMap* weakMap)
            : key(otherKey)
            , self(weakMap) {
        }
        KeyObject(const KeyObject& other)
            : key(other.key)
            , self(other.self) {
        }
        KeyObject()
            : key()
            , self(nullptr) {
        }
        K key;
        KeyWeakMap* self;
    };

    KeyWeakMap() {}
    virtual ~KeyWeakMap() {
        for (auto& p : m_map)
            p.second.second.ClearWeak();
    }

    // Sets the object to WeakMap with the given |key|.
    void set(v8::Isolate* isolate, const K& key, v8::Local<v8::Object> object) {
        KeyObject key_object(key, this);
        auto& p = m_map[key] = std::make_pair(key_object, v8::Global<v8::Object>(isolate, object));
        p.second.SetWeak(&(p.first), onObjectGC, v8::WeakCallbackType::kParameter);
    }

    // Gets the object from WeakMap by its |key|.
    v8::MaybeLocal<v8::Object> get(v8::Isolate* isolate, const K& key) {
        auto iter = m_map.find(key);
        if (iter == m_map.end())
            return v8::MaybeLocal<v8::Object>();
        else
            return v8::Local<v8::Object>::New(isolate, iter->second.second);
    }

    // Whethere there is an object with |key| in this WeakMap.
    bool has(const K& key) const {
        return m_map.find(key) != m_map.end();
    }

    // Returns all objects.
    std::vector<v8::Local<v8::Object>> values(v8::Isolate* isolate) const {
        std::vector<v8::Local<v8::Object>> keys;
        keys.reserve(m_map.size());
        for (const auto& it : m_map)
            keys.emplace_back(v8::Local<v8::Object>::New(isolate, it.second.second));
        return keys;
    }

    // Remove object with |key| in the WeakMap.
    void remove(const K& key) {
        auto iter = m_map.find(key);
        if (iter == m_map.end())
            return;

        iter->second.second.ClearWeak();
        m_map.erase(iter);
    }

private:
    static void onObjectGC(
        const v8::WeakCallbackInfo<typename KeyWeakMap<K>::KeyObject>& data) {
        KeyWeakMap<K>::KeyObject* key_object = data.GetParameter();
        key_object->self->remove(key_object->key);
    }
    
    // Map of stored objects.
    std::map<K, std::pair<KeyObject, v8::Global<v8::Object>>> m_map;

    DISALLOW_COPY_AND_ASSIGN(KeyWeakMap);
};

}  // namespace atom

#endif  // ATOM_COMMON_KEY_WEAK_MAP_H_
