/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef V8DOMConfiguration_h
#define V8DOMConfiguration_h

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8DOMWrapper.h"
#include "core/CoreExport.h"
#include <v8.h>

namespace blink {

class CORE_EXPORT V8DOMConfiguration final {
public:
    // The following Configuration structs and install methods are used for
    // setting multiple properties on ObjectTemplate / FunctionTemplate, used
    // from the generated bindings initialization (ConfigureXXXTemplate).
    // This greatly reduces the binary size by moving from code driven setup to
    // data table driven setup.

    enum ExposeConfiguration {
        ExposedToAllScripts,
        OnlyExposedToPrivateScript,
    };

    // Bitflags to show where the member will be defined.
    enum PropertyLocationConfiguration {
        OnInstance = 1 << 0,
        OnPrototype = 1 << 1,
        OnInterface = 1 << 2,
    };

    enum HolderCheckConfiguration {
        CheckHolder,
        DoNotCheckHolder,
    };

    // AttributeConfiguration translates into calls to SetAccessor() on either
    // the instance or the prototype ObjectTemplate, based on |instanceOrPrototypeConfiguration|.
    struct AttributeConfiguration {
        const char* const name;
        v8::AccessorNameGetterCallback getter;
        v8::AccessorNameSetterCallback setter;
        v8::AccessorNameGetterCallback getterForMainWorld;
        v8::AccessorNameSetterCallback setterForMainWorld;
        const WrapperTypeInfo* data;
        v8::AccessControl settings;
        v8::PropertyAttribute attribute;
        unsigned exposeConfiguration : 1; // ExposeConfiguration
        unsigned propertyLocationConfiguration : 3; // PropertyLocationConfiguration
        unsigned holderCheckConfiguration : 1; // HolderCheckConfiguration
    };

    static void installAttributes(v8::Isolate*, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, const AttributeConfiguration*, size_t attributeCount);

    static void installAttribute(v8::Isolate*, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, const AttributeConfiguration&);

    // AccessorConfiguration translates into calls to SetAccessorProperty()
    // on prototype ObjectTemplate.
    struct AccessorConfiguration {
        const char* const name;
        v8::FunctionCallback getter;
        v8::FunctionCallback setter;
        v8::FunctionCallback getterForMainWorld;
        v8::FunctionCallback setterForMainWorld;
        const WrapperTypeInfo* data;
        v8::AccessControl settings;
        v8::PropertyAttribute attribute;
        unsigned exposeConfiguration : 1; // ExposeConfiguration
        unsigned propertyLocationConfiguration : 3; // PropertyLocationConfiguration
        unsigned holderCheckConfiguration : 1; // HolderCheckConfiguration
    };

    static void installAccessors(v8::Isolate*, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, v8::Local<v8::FunctionTemplate> interfaceTemplate, v8::Local<v8::Signature>, const AccessorConfiguration*, size_t accessorCount);

    static void installAccessor(v8::Isolate*, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, v8::Local<v8::FunctionTemplate> interfaceTemplate, v8::Local<v8::Signature>, const AccessorConfiguration&);

    static void installAccessor(v8::Isolate*, v8::Local<v8::Object> instance, v8::Local<v8::Object> prototype, v8::Local<v8::Function> interface_name, v8::Local<v8::Signature>, const AccessorConfiguration&);

    enum ConstantType {
        ConstantTypeShort,
        ConstantTypeLong,
        ConstantTypeUnsignedShort,
        ConstantTypeUnsignedLong,
        ConstantTypeFloat,
        ConstantTypeDouble,
        ConstantTypeString
    };

    // ConstantConfiguration translates into calls to Set() for setting up an
    // object's constants. It sets the constant on both the FunctionTemplate and
    // the ObjectTemplate. PropertyAttributes is always ReadOnly.
    struct ConstantConfiguration {
        const char* const name;
        int ivalue;
        double dvalue;
        const char* const svalue;
        ConstantType type;
    };

    // Constant installation
    //
    // installConstants and installConstant are used for simple constants. They
    // install constants using v8::Template::Set(), which results in a property
    // that is much faster to access from scripts.
    // installConstantWithGetter is used when some C++ code needs to be executed
    // when the constant is accessed, e.g. to handle deprecation or measuring
    // usage. The property appears the same to scripts, but is slower to access.
    static void installConstants(v8::Isolate*, v8::Local<v8::FunctionTemplate> functionDescriptor, v8::Local<v8::ObjectTemplate> prototypeTemplate, const ConstantConfiguration*, size_t constantCount);

    static void installConstant(v8::Isolate*, v8::Local<v8::FunctionTemplate> functionDescriptor, v8::Local<v8::ObjectTemplate> prototypeTemplate, const ConstantConfiguration&);

    static void installConstantWithGetter(v8::Isolate*, v8::Local<v8::FunctionTemplate> functionDescriptor, v8::Local<v8::ObjectTemplate> prototypeTemplate, const char* name, v8::AccessorNameGetterCallback);

    // MethodConfiguration translates into calls to Set() for setting up an
    // object's callbacks. It sets the method on both the FunctionTemplate or
    // the ObjectTemplate.
    struct MethodConfiguration {
        v8::Local<v8::Name> methodName(v8::Isolate* isolate) const { return v8AtomicString(isolate, name); }
        v8::FunctionCallback callbackForWorld(const DOMWrapperWorld& world) const
        {
            return world.isMainWorld() && callbackForMainWorld ? callbackForMainWorld : callback;
        }

        const char* const name;
        v8::FunctionCallback callback;
        v8::FunctionCallback callbackForMainWorld;
        int length;
        ExposeConfiguration exposeConfiguration;
    };

    struct SymbolKeyedMethodConfiguration {
        v8::Local<v8::Name> methodName(v8::Isolate* isolate) const { return getSymbol(isolate); }
        v8::FunctionCallback callbackForWorld(const DOMWrapperWorld&) const
        {
            return callback;
        }

        v8::Local<v8::Symbol> (*getSymbol)(v8::Isolate*);
        v8::FunctionCallback callback;
        // SymbolKeyedMethodConfiguration doesn't support per-world bindings.
        int length;
        ExposeConfiguration exposeConfiguration;
    };

    static void installMethods(v8::Isolate*, v8::Local<v8::ObjectTemplate> prototypeTemplate, v8::Local<v8::Signature>, v8::PropertyAttribute, const MethodConfiguration*, size_t callbackCount);

    static void installMethod(v8::Isolate*, v8::Local<v8::FunctionTemplate>, v8::Local<v8::Signature>, v8::PropertyAttribute, const MethodConfiguration&);

    static void installMethod(v8::Isolate*, v8::Local<v8::ObjectTemplate>, v8::Local<v8::Signature>, v8::PropertyAttribute, const MethodConfiguration&);

    static void installMethod(v8::Isolate*, v8::Local<v8::ObjectTemplate>, v8::Local<v8::Signature>, v8::PropertyAttribute, const SymbolKeyedMethodConfiguration&);

    static v8::Local<v8::Signature> installDOMClassTemplate(v8::Isolate*, v8::Local<v8::FunctionTemplate>, const char* interfaceName, v8::Local<v8::FunctionTemplate> parentClass, size_t fieldCount,
        const AttributeConfiguration*, size_t attributeCount,
        const AccessorConfiguration*, size_t accessorCount,
        const MethodConfiguration*, size_t callbackCount);

    static v8::Local<v8::FunctionTemplate> domClassTemplate(v8::Isolate*, WrapperTypeInfo*, void (*)(v8::Local<v8::FunctionTemplate>, v8::Isolate*));
};

} // namespace blink

#endif // V8DOMConfiguration_h
