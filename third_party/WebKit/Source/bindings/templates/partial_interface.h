{% include 'copyright_block.txt' %}
#ifndef {{v8_class_or_partial}}_h
#define {{v8_class_or_partial}}_h

{% for filename in header_includes %}
#include "{{filename}}"
{% endfor %}

namespace blink {

class {{v8_class_or_partial}} {
public:
    static void initialize();
    {% for method in methods if method.is_custom %}
    {% filter conditional(method.conditional_string) %}
    static void {{method.name}}MethodCustom(const v8::FunctionCallbackInfo<v8::Value>&);
    {% endfilter %}
    {% endfor %}
    {% for attribute in attributes %}
    {% if attribute.has_custom_getter %}{# FIXME: and not attribute.implemented_by #}
    {% filter conditional(attribute.conditional_string) %}
    static void {{attribute.name}}AttributeGetterCustom(const v8::PropertyCallbackInfo<v8::Value>&);
    {% endfilter %}
    {% endif %}
    {% if attribute.has_custom_setter %}{# FIXME: and not attribute.implemented_by #}
    {% filter conditional(attribute.conditional_string) %}
    static void {{attribute.name}}AttributeSetterCustom(v8::Local<v8::Value>, const v8::PropertyCallbackInfo<void>&);
    {% endfilter %}
    {% endif %}
    {% endfor %}
    {# Custom internal fields #}
    static void preparePrototypeObject(v8::Isolate*, v8::Local<v8::Object>, v8::Local<v8::FunctionTemplate>);
private:
    static void install{{v8_class}}Template(v8::Local<v8::FunctionTemplate>, v8::Isolate*);
};
}
#endif // {{v8_class_or_partial}}_h
