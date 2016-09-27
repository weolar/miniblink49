{% from 'conversions.cpp' import declare_enum_validation_variable %}
{% include 'copyright_block.txt' %}
#include "config.h"
#include "{{v8_original_class}}.h"

{% for filename in cpp_includes if filename != '%s.h' % v8_class %}
#include "{{filename}}"
{% endfor %}

namespace blink {

{% from 'conversions.cpp' import v8_value_to_local_cpp_value %}
void {{v8_class}}::toImpl(v8::Isolate* isolate, v8::Local<v8::Value> v8Value, {{cpp_class}}& impl, ExceptionState& exceptionState)
{
    if (isUndefinedOrNull(v8Value))
        return;
    if (!v8Value->IsObject()) {
        {% if use_permissive_dictionary_conversion %}
        // Do nothing.
        return;
        {% else %}
        exceptionState.throwTypeError("cannot convert to dictionary.");
        return;
        {% endif %}
    }

    {% if parent_v8_class %}
    {{parent_v8_class}}::toImpl(isolate, v8Value, impl, exceptionState);
    if (exceptionState.hadException())
        return;

    {% endif %}
    {# Declare local variables only when the dictionary has members to avoid unused variable warnings. #}
    {% if members %}
    v8::TryCatch block;
    v8::Local<v8::Object> v8Object;
    if (!v8Call(v8Value->ToObject(isolate->GetCurrentContext()), v8Object, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return;
    }
    {% endif %}
    {% for member in members %}
    {
        v8::Local<v8::Value> {{member.name}}Value;
        if (!v8Object->Get(isolate->GetCurrentContext(), v8String(isolate, "{{member.name}}")).ToLocal(&{{member.name}}Value)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return;
        }
        if ({{member.name}}Value.IsEmpty() || {{member.name}}Value->IsUndefined()) {
            {% if member.is_required %}
            exceptionState.throwTypeError("required member {{member.name}} is undefined.");
            return;
            {% else %}
            // Do nothing.
            {% endif %}
        {% if member.is_nullable %}
        } else if ({{member.name}}Value->IsNull()) {
            impl.{{member.null_setter_name}}();
        {% endif %}
        } else {
            {% if member.deprecate_as %}
            UseCounter::countDeprecationIfNotPrivateScript(isolate, callingExecutionContext(isolate), UseCounter::{{member.deprecate_as}});
            {% endif %}
            {{v8_value_to_local_cpp_value(member) | indent(12)}}
            {% if member.is_interface_type %}
            if (!{{member.name}} && !{{member.name}}Value->IsNull()) {
                exceptionState.throwTypeError("member {{member.name}} is not of type {{member.idl_type}}.");
                return;
            }
            {% endif %}
            {% if member.enum_values %}
            {{declare_enum_validation_variable(member.enum_values) | indent(12)}}
            if (!isValidEnum({{member.name}}, validValues, WTF_ARRAY_LENGTH(validValues), "{{member.enum_type}}", exceptionState))
                return;
            {% elif member.is_object %}
            if (!{{member.name}}.isObject()) {
                exceptionState.throwTypeError("member {{member.name}} is not an object.");
                return;
            }
            {% endif %}
            impl.{{member.setter_name}}({{member.name}});
        }
    }

    {% endfor %}
}

v8::Local<v8::Value> toV8(const {{cpp_class}}& impl, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    v8::Local<v8::Object> v8Object = v8::Object::New(isolate);
    {% if parent_v8_class %}
    if (!toV8{{parent_cpp_class}}(impl, v8Object, creationContext, isolate))
        return v8::Local<v8::Value>();
    {% endif %}
    if (!toV8{{cpp_class}}(impl, v8Object, creationContext, isolate))
        return v8::Local<v8::Value>();
    return v8Object;
}

bool toV8{{cpp_class}}(const {{cpp_class}}& impl, v8::Local<v8::Object> dictionary, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    {% for member in members %}
    if (impl.{{member.has_method_name}}()) {
        {% if member.is_object %}
        ASSERT(impl.{{member.cpp_name}}().isObject());
        {% endif %}
        if (!v8CallBoolean(dictionary->CreateDataProperty(isolate->GetCurrentContext(), v8String(isolate, "{{member.name}}"), {{member.cpp_value_to_v8_value}})))
            return false;
    {% if member.v8_default_value %}
    } else {
        if (!v8CallBoolean(dictionary->CreateDataProperty(isolate->GetCurrentContext(), v8String(isolate, "{{member.name}}"), {{member.v8_default_value}})))
            return false;
    {% elif member.is_required %}
    } else {
        ASSERT_NOT_REACHED();
    {% endif %}
    }

    {% endfor %}
    return true;
}

{{cpp_class}} NativeValueTraits<{{cpp_class}}>::nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    {{cpp_class}} impl;
    {{v8_class}}::toImpl(isolate, value, impl, exceptionState);
    return impl;
}

} // namespace blink
