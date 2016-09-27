{% extends 'interface_base.cpp' %}


{##############################################################################}
{% block prepare_prototype_object %}
{% from 'interface.cpp' import install_unscopeables with context %}
{% from 'interface.cpp' import install_conditionally_enabled_attributes_on_prototype with context %}
{% from 'methods.cpp' import install_conditionally_enabled_methods
        with context %}
void {{v8_class_or_partial}}::preparePrototypeObject(v8::Isolate* isolate, v8::Local<v8::Object> prototypeObject, v8::Local<v8::FunctionTemplate> interfaceTemplate)
{
    {{v8_class}}::preparePrototypeObject(isolate, prototypeObject, interfaceTemplate);
{% if unscopeables %}
    {{install_unscopeables() | indent}}
{% endif %}
{% if has_conditional_attributes_on_prototype %}
    {{install_conditionally_enabled_attributes_on_prototype() | indent}}
{% endif %}
{% if conditionally_enabled_methods %}
    {{install_conditionally_enabled_methods() | indent}}
{% endif %}
}

{% endblock %}


{##############################################################################}
{% block partial_interface %}
void {{v8_class_or_partial}}::initialize()
{
    // Should be invoked from initModules.
    {{v8_class}}::updateWrapperTypeInfo(
        &{{v8_class_or_partial}}::install{{v8_class}}Template,
        &{{v8_class_or_partial}}::preparePrototypeObject);
    {% for method in methods %}
    {% if method.overloads and method.overloads.has_partial_overloads %}
    {{v8_class}}::register{{method.name | blink_capitalize}}MethodForPartialInterface(&{{cpp_class_or_partial}}V8Internal::{{method.name}}Method);
    {% endif %}
    {% endfor %}
}

{% endblock %}
