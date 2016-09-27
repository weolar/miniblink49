{% include 'copyright_block.txt' %}
#ifndef {{macro_guard}}
#define {{macro_guard}}

{% for filename in header_includes %}
#include "{{filename}}"
{% endfor %}

namespace blink {

{% for decl in header_forward_decls %}
class {{decl}};
{% endfor %}

{% for container in containers %}
class {{exported}}{{container.cpp_class}} final {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    {{container.cpp_class}}();
    bool isNull() const { return m_type == SpecificTypeNone; }

    {% for member in container.members %}
    bool is{{member.type_name}}() const { return m_type == {{member.specific_type_enum}}; }
    {{member.rvalue_cpp_type}} getAs{{member.type_name}}() const;
    void set{{member.type_name}}({{member.rvalue_cpp_type}});
    static {{container.cpp_class}} from{{member.type_name}}({{member.rvalue_cpp_type}});

    {% endfor %}
    {{container.cpp_class}}(const {{container.cpp_class}}&);
    ~{{container.cpp_class}}();
    {{container.cpp_class}}& operator=(const {{container.cpp_class}}&);
    DECLARE_TRACE();

private:
    enum SpecificTypes {
        SpecificTypeNone,
        {% for member in container.members %}
        {{member.specific_type_enum}},
        {% endfor %}
    };
    SpecificTypes m_type;

    {% for member in container.members %}
    {{member.cpp_type}} m_{{member.cpp_name}};
    {% endfor %}

    friend {{exported}}v8::Local<v8::Value> toV8(const {{container.cpp_class}}&, v8::Local<v8::Object>, v8::Isolate*);
};

class V8{{container.cpp_class}} final {
public:
    {{exported}}static void toImpl(v8::Isolate*, v8::Local<v8::Value>, {{container.cpp_class}}&, ExceptionState&);
};

{{exported}}v8::Local<v8::Value> toV8(const {{container.cpp_class}}&, v8::Local<v8::Object>, v8::Isolate*);

template <class CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, {{container.cpp_class}}& impl)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template <>
struct NativeValueTraits<{{container.cpp_class}}> {
    {{exported}}static {{container.cpp_class}} nativeValue(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);
};

{% endfor %}
{% for cpp_type in nullable_cpp_types %}
class V8{{cpp_type}}OrNull final {
public:
    static void toImpl(v8::Isolate* isolate, v8::Local<v8::Value> v8Value, {{cpp_type}}& impl, ExceptionState& exceptionState)
    {
        {# http://heycam.github.io/webidl/#es-union #}
        {# 1. null or undefined #}
        if (isUndefinedOrNull(v8Value))
            return;
        V8{{cpp_type}}::toImpl(isolate, v8Value, impl, exceptionState);
    }
};

{% endfor %}
} // namespace blink

// We need to set canInitializeWithMemset=true because HeapVector supports
// items that can initialize with memset or have a vtable. It is safe to
// set canInitializeWithMemset=true for a union type object in practice.
// See https://codereview.chromium.org/1118993002/#msg5 for more details.
{% for container in containers %}
WTF_ALLOW_MOVE_AND_INIT_WITH_MEM_FUNCTIONS(blink::{{container.cpp_class}});
{% endfor %}

#endif // {{macro_guard}}
