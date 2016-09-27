{% include 'copyright_block.txt' %}
#ifndef {{v8_original_class}}_h
#define {{v8_original_class}}_h

{% for filename in header_includes %}
#include "{{filename}}"
{% endfor %}

namespace blink {

class ExceptionState;

class {{v8_class}} {
public:
    {{exported}}static void toImpl(v8::Isolate*, v8::Local<v8::Value>, {{cpp_class}}&, ExceptionState&);
};

v8::Local<v8::Value> toV8(const {{cpp_class}}&, v8::Local<v8::Object>, v8::Isolate*);
{{exported}}bool toV8{{cpp_class}}(const {{cpp_class}}&, v8::Local<v8::Object> dictionary, v8::Local<v8::Object> creationContext, v8::Isolate*);

template<class CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, {{cpp_class}}& impl)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template <>
struct NativeValueTraits<{{cpp_class}}> {
    static {{cpp_class}} nativeValue(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);
};

} // namespace blink

#endif // {{v8_original_class}}_h
