{% include 'copyright_block.txt' %}
#include "config.h"
{% filter conditional(conditional_string) %}
#include "{{v8_class_or_partial}}.h"

{% for filename in cpp_includes if filename != '%s.h' % cpp_class_or_partial %}
#include "{{filename}}"
{% endfor %}

namespace blink {
{% set to_active_dom_object = '%s::toActiveDOMObject' % v8_class
                              if is_active_dom_object else '0' %}
{% set visit_dom_wrapper = '%s::visitDOMWrapper' % v8_class
                           if has_visit_dom_wrapper else '0' %}
{% set parent_wrapper_type_info = '&V8%s::wrapperTypeInfo' % parent_interface
                                  if parent_interface else '0' %}
{% set wrapper_type_prototype = 'WrapperTypeExceptionPrototype' if is_exception else
                                'WrapperTypeObjectPrototype' %}
{% set dom_template = '%s::domTemplate' % v8_class if not is_array_buffer_or_view else '0' %}

{% set wrapper_type_info_const = '' if has_partial_interface else 'const ' %}
{% if not is_partial %}
// Suppress warning: global constructors, because struct WrapperTypeInfo is trivial
// and does not depend on another global objects.
#if defined(COMPONENT_BUILD) && defined(WIN32) && COMPILER(CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif
{{wrapper_type_info_const}}WrapperTypeInfo {{v8_class}}::wrapperTypeInfo = { gin::kEmbedderBlink, {{dom_template}}, {{v8_class}}::refObject, {{v8_class}}::derefObject, {{v8_class}}::trace, {{to_active_dom_object}}, {{visit_dom_wrapper}}, {{v8_class}}::preparePrototypeObject, {{v8_class}}::installConditionallyEnabledProperties, "{{interface_name}}", {{parent_wrapper_type_info}}, WrapperTypeInfo::{{wrapper_type_prototype}}, WrapperTypeInfo::{{wrapper_class_id}}, WrapperTypeInfo::{{event_target_inheritance}}, WrapperTypeInfo::{{lifetime}}, WrapperTypeInfo::{{gc_type}} };
#if defined(COMPONENT_BUILD) && defined(WIN32) && COMPILER(CLANG)
#pragma clang diagnostic pop
#endif

// This static member must be declared by DEFINE_WRAPPERTYPEINFO in {{cpp_class}}.h.
// For details, see the comment of DEFINE_WRAPPERTYPEINFO in
// bindings/core/v8/ScriptWrappable.h.
{% if not is_typed_array_type %}
const WrapperTypeInfo& {{cpp_class}}::s_wrapperTypeInfo = {{v8_class}}::wrapperTypeInfo;
{% endif %}

{% endif %}
{% if not is_array_buffer_or_view %}
namespace {{cpp_class_or_partial}}V8Internal {
{% if has_partial_interface %}
{% for method in methods if method.overloads and method.overloads.has_partial_overloads %}
static void (*{{method.name}}MethodForPartialInterface)(const v8::FunctionCallbackInfo<v8::Value>&) = 0;
{% endfor %}
{% endif %}

{# Constants #}
{% from 'constants.cpp' import constant_getter_callback
       with context %}
{% for constant in special_getter_constants %}
{{constant_getter_callback(constant)}}
{% endfor %}
{# Attributes #}
{% block replaceable_attribute_setter_and_callback %}
{% if has_replaceable_attributes or has_constructor_attributes %}
template<class CallbackInfo>
static bool {{cpp_class}}CreateDataProperty(v8::Local<v8::Name> name, v8::Local<v8::Value> v8Value, const CallbackInfo& info)
{
    {% if is_check_security %}
    {{cpp_class}}* impl = {{v8_class}}::toImpl(info.Holder());
    v8::String::Utf8Value attributeName(name);
    ExceptionState exceptionState(ExceptionState::SetterContext, *attributeName, "{{interface_name}}", info.Holder(), info.GetIsolate());
    if (!BindingSecurity::shouldAllowAccessToFrame(info.GetIsolate(), impl->frame(), exceptionState)) {
        exceptionState.throwIfNeeded();
        return false;
    }
    {% endif %}
    ASSERT(info.This()->IsObject());
    return v8CallBoolean(v8::Local<v8::Object>::Cast(info.This())->CreateDataProperty(info.GetIsolate()->GetCurrentContext(), name, v8Value));
}

{% if has_constructor_attributes %}
static void {{cpp_class}}ConstructorAttributeSetterCallback(v8::Local<v8::Name>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("blink", "DOMSetter");
    do {
        v8::Local<v8::Value> data = info.Data();
        ASSERT(data->IsExternal());
        V8PerContextData* perContextData = V8PerContextData::from(info.Holder()->CreationContext());
        if (!perContextData)
            break;
        const WrapperTypeInfo* wrapperTypeInfo = WrapperTypeInfo::unwrap(data);
        if (!wrapperTypeInfo)
            break;
        {{cpp_class}}CreateDataProperty(v8String(info.GetIsolate(), wrapperTypeInfo->interfaceName), v8Value, info);
    } while (false); // do ... while (false) just for use of break
    TRACE_EVENT_SET_SAMPLING_STATE("v8", "V8Execution");
}

{% endif %}
{% endif %}
{% endblock %}
{##############################################################################}
{% from 'attributes.cpp' import constructor_getter_callback,
       attribute_getter, attribute_getter_callback,
       attribute_setter, attribute_setter_callback,
       attribute_getter_implemented_in_private_script,
       attribute_setter_implemented_in_private_script
       with context %}
{% for attribute in attributes if attribute.should_be_exposed_to_script %}
{% for world_suffix in attribute.world_suffixes %}
{% if not attribute.constructor_type %}
{% if not attribute.has_custom_getter %}
{{attribute_getter(attribute, world_suffix)}}
{% endif %}
{{attribute_getter_callback(attribute, world_suffix)}}
{% endif %}
{% if attribute.has_setter %}
{% if not attribute.has_custom_setter and
       (not attribute.constructor_type or attribute.needs_constructor_setter_callback) %}
{{attribute_setter(attribute, world_suffix)}}
{% endif %}
{% if not attribute.constructor_type or attribute.needs_constructor_setter_callback %}
{{attribute_setter_callback(attribute, world_suffix)}}
{% endif %}
{% endif %}
{% endfor %}
{% endfor %}
{##############################################################################}
{% for attribute in attributes if attribute.needs_constructor_getter_callback %}
{% for world_suffix in attribute.world_suffixes %}
{{constructor_getter_callback(attribute, world_suffix)}}
{% endfor %}
{% endfor %}
{##############################################################################}
{% block security_check_functions %}
{% if has_access_check_callbacks %}
bool indexedSecurityCheck(v8::Local<v8::Object> host, uint32_t index, v8::AccessType type, v8::Local<v8::Value>)
{
    {{cpp_class}}* impl = {{v8_class}}::toImpl(host);
    return BindingSecurity::shouldAllowAccessToFrame(v8::Isolate::GetCurrent(), impl->frame(), DoNotReportSecurityError);
}

bool namedSecurityCheck(v8::Local<v8::Object> host, v8::Local<v8::Value> key, v8::AccessType type, v8::Local<v8::Value>)
{
    {{cpp_class}}* impl = {{v8_class}}::toImpl(host);
    return BindingSecurity::shouldAllowAccessToFrame(v8::Isolate::GetCurrent(), impl->frame(), DoNotReportSecurityError);
}

{% endif %}
{% endblock %}
{##############################################################################}
{# Methods #}
{% from 'methods.cpp' import generate_method, overload_resolution_method,
       method_callback, origin_safe_method_getter, generate_constructor,
       method_implemented_in_private_script, generate_post_message_impl,
       runtime_determined_length_method, runtime_determined_maxarg_method
       with context %}
{% for method in methods %}
{% if method.should_be_exposed_to_script %}
{% for world_suffix in method.world_suffixes %}
{% if not method.is_custom and not method.is_post_message and method.visible %}
{{generate_method(method, world_suffix)}}
{% endif %}
{% if method.is_post_message %}
{{generate_post_message_impl()}}
{% endif %}
{% if method.overloads and method.overloads.visible %}
{% if method.overloads.runtime_determined_lengths %}
{{runtime_determined_length_method(method.overloads)}}
{% endif %}
{% if method.overloads.runtime_determined_maxargs %}
{{runtime_determined_maxarg_method(method.overloads)}}
{% endif %}
{{overload_resolution_method(method.overloads, world_suffix)}}
{% endif %}
{% if not method.overload_index or method.overloads %}
{# Document about the following condition: #}
{# https://docs.google.com/document/d/1qBC7Therp437Jbt_QYAtNYMZs6zQ_7_tnMkNUG_ACqs/edit?usp=sharing #}
{% if (method.overloads and method.overloads.visible and
        (not method.overloads.has_partial_overloads or not is_partial)) or
      (not method.overloads and method.visible) %}
{# A single callback is generated for overloaded methods #}
{# with considering partial overloads #}
{{method_callback(method, world_suffix)}}
{% endif %}
{% endif %}
{% if method.is_do_not_check_security and method.visible %}
{{origin_safe_method_getter(method, world_suffix)}}
{% endif %}
{% endfor %}
{% endif %}
{% endfor %}
{% if iterator_method %}
{{generate_method(iterator_method)}}
{{method_callback(iterator_method)}}
{% endif %}
{% block origin_safe_method_setter %}{% endblock %}
{# Constructors #}
{% for constructor in constructors %}
{{generate_constructor(constructor)}}
{% endfor %}
{% block overloaded_constructor %}{% endblock %}
{% block event_constructor %}{% endblock %}
{# Special operations (methods) #}
{% block indexed_property_getter %}{% endblock %}
{% block indexed_property_getter_callback %}{% endblock %}
{% block indexed_property_setter %}{% endblock %}
{% block indexed_property_setter_callback %}{% endblock %}
{% block indexed_property_deleter %}{% endblock %}
{% block indexed_property_deleter_callback %}{% endblock %}
{% block named_property_getter %}{% endblock %}
{% block named_property_getter_callback %}{% endblock %}
{% block named_property_setter %}{% endblock %}
{% block named_property_setter_callback %}{% endblock %}
{% block named_property_query %}{% endblock %}
{% block named_property_query_callback %}{% endblock %}
{% block named_property_deleter %}{% endblock %}
{% block named_property_deleter_callback %}{% endblock %}
{% block named_property_enumerator %}{% endblock %}
{% block named_property_enumerator_callback %}{% endblock %}
} // namespace {{cpp_class_or_partial}}V8Internal

{% block visit_dom_wrapper %}{% endblock %}
{% block shadow_attributes %}{% endblock %}
{##############################################################################}
{% block install_attributes %}
{% from 'attributes.cpp' import attribute_configuration with context %}
{% if has_attribute_configuration %}
// Suppress warning: global constructors, because AttributeConfiguration is trivial
// and does not depend on another global objects.
#if defined(COMPONENT_BUILD) && defined(WIN32) && COMPILER(CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif
static const V8DOMConfiguration::AttributeConfiguration {{v8_class}}Attributes[] = {
    {% for attribute in attributes
       if not (attribute.is_expose_js_accessors or
               attribute.is_static or
               attribute.runtime_enabled_function or
               attribute.exposed_test or
               (interface_name == 'Window' and attribute.is_unforgeable))
           and attribute.should_be_exposed_to_script %}
    {% filter conditional(attribute.conditional_string) %}
    {{attribute_configuration(attribute)}},
    {% endfilter %}
    {% endfor %}
};
#if defined(COMPONENT_BUILD) && defined(WIN32) && COMPILER(CLANG)
#pragma clang diagnostic pop
#endif

{% endif %}
{% endblock %}
{##############################################################################}
{% block install_accessors %}
{% from 'attributes.cpp' import attribute_configuration with context %}
{% if has_accessor_configuration %}
static const V8DOMConfiguration::AccessorConfiguration {{v8_class}}Accessors[] = {
    {% for attribute in attributes
       if (attribute.is_expose_js_accessors and
           not (attribute.is_static or
                attribute.runtime_enabled_function or
                attribute.exposed_test) and
           attribute.should_be_exposed_to_script) %}
    {% filter conditional(attribute.conditional_string) %}
    {{attribute_configuration(attribute)}},
    {% endfilter %}
    {% endfor %}
};

{% endif %}
{% endblock %}
{##############################################################################}
{% block install_methods %}
{% from 'methods.cpp' import method_configuration with context %}
{% if method_configuration_methods %}
static const V8DOMConfiguration::MethodConfiguration {{v8_class}}Methods[] = {
    {% for method in method_configuration_methods %}
    {% filter conditional(method.conditional_string) %}
    {{method_configuration(method)}},
    {% endfilter %}
    {% endfor %}
};

{% endif %}
{% endblock %}
{% endif %}{# not is_array_buffer_or_view #}
{##############################################################################}
{% block named_constructor %}{% endblock %}
{% block constructor_callback %}{% endblock %}
{% block configure_shadow_object_template %}{% endblock %}
{##############################################################################}
{% block install_dom_template %}
{% if not is_array_buffer_or_view %}
{% from 'methods.cpp' import install_custom_signature with context %}
{% from 'attributes.cpp' import attribute_configuration with context %}
{% from 'constants.cpp' import install_constants with context %}
{% if has_partial_interface or is_partial %}
void {{v8_class_or_partial}}::install{{v8_class}}Template(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate)
{% else %}
static void install{{v8_class}}Template(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate)
{% endif %}
{
    {% if is_partial %}
    {{v8_class}}::install{{v8_class}}Template(functionTemplate, isolate);
    {% else %}
    functionTemplate->ReadOnlyPrototype();
    {% endif %}

    v8::Local<v8::Signature> defaultSignature;
    {% set parent_template =
           'V8%s::domTemplate(isolate)' % parent_interface
           if parent_interface else 'v8::Local<v8::FunctionTemplate>()' %}
    {% if runtime_enabled_function %}
    if (!{{runtime_enabled_function}}())
        defaultSignature = V8DOMConfiguration::installDOMClassTemplate(isolate, functionTemplate, "{{interface_name}}", {{parent_template}}, {{v8_class}}::internalFieldCount, 0, 0, 0, 0, 0, 0);
    else
    {% endif %}
    {% set runtime_enabled_indent = 4 if runtime_enabled_function else 0 %}
    {% filter indent(runtime_enabled_indent, true) %}
    defaultSignature = V8DOMConfiguration::installDOMClassTemplate(isolate, functionTemplate, "{{interface_name}}", {{parent_template}}, {{v8_class}}::internalFieldCount,
        {# Test needed as size 0 arrays definitions are not allowed per standard
           (so objects have distinct addresses), which is enforced by MSVC.
           8.5.1 Aggregates [dcl.init.aggr]
           An array of unknown size initialized with a brace-enclosed
           initializer-list containing n initializer-clauses, where n shall be
           greater than zero, is defined as having n elements (8.3.4). #}
        {% set attributes_name, attributes_length =
               ('%sAttributes' % v8_class,
                'WTF_ARRAY_LENGTH(%sAttributes)' % v8_class)
           if has_attribute_configuration else (0, 0) %}
        {% set accessors_name, accessors_length =
               ('%sAccessors' % v8_class,
                'WTF_ARRAY_LENGTH(%sAccessors)' % v8_class)
           if has_accessor_configuration else (0, 0) %}
        {% set methods_name, methods_length =
               ('%sMethods' % v8_class,
                'WTF_ARRAY_LENGTH(%sMethods)' % v8_class)
           if method_configuration_methods else (0, 0) %}
        {{attributes_name}}, {{attributes_length}},
        {{accessors_name}}, {{accessors_length}},
        {{methods_name}}, {{methods_length}});
    {% endfilter %}

    {% if constructors or has_custom_constructor or has_event_constructor %}
    functionTemplate->SetCallHandler({{v8_class}}::constructorCallback);
    functionTemplate->SetLength({{interface_length}});
    {% endif %}
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    ALLOW_UNUSED_LOCAL(instanceTemplate);
    v8::Local<v8::ObjectTemplate> prototypeTemplate = functionTemplate->PrototypeTemplate();
    ALLOW_UNUSED_LOCAL(prototypeTemplate);
    {% if has_access_check_callbacks %}
    instanceTemplate->SetAccessCheckCallbacks({{cpp_class}}V8Internal::namedSecurityCheck, {{cpp_class}}V8Internal::indexedSecurityCheck, v8::External::New(isolate, const_cast<WrapperTypeInfo*>(&{{v8_class}}::wrapperTypeInfo)));
    {% endif %}
    {% for attribute in attributes
       if attribute.runtime_enabled_function and
          not attribute.exposed_test and
          not attribute.is_static %}
    {% filter conditional(attribute.conditional_string) %}
    if ({{attribute.runtime_enabled_function}}()) {
        {% if attribute.is_expose_js_accessors %}
        static const V8DOMConfiguration::AccessorConfiguration accessorConfiguration =\
        {{attribute_configuration(attribute)}};
        V8DOMConfiguration::installAccessor(isolate, instanceTemplate, prototypeTemplate, functionTemplate, defaultSignature, accessorConfiguration);
        {% else %}
        static const V8DOMConfiguration::AttributeConfiguration attributeConfiguration =\
        {{attribute_configuration(attribute)}};
        V8DOMConfiguration::installAttribute(isolate, instanceTemplate, prototypeTemplate, attributeConfiguration);
        {% endif %}
    }
    {% endfilter %}
    {% endfor %}
    {% if constants %}
    {{install_constants() | indent}}
    {% endif %}
    {# Special operations #}
    {# V8 has access-check callback API and it\'s used on Window instead of
       deleters or enumerators; see ObjectTemplate::SetAccessCheckCallbacks.
       In addition, the getter should be set on the prototype template, to get
       the implementation straight out of the Window prototype, regardless of
       what prototype is actually set on the object. #}
    {% set set_on_template = 'PrototypeTemplate' if interface_name == 'Window'
                        else 'InstanceTemplate' %}
    {% if indexed_property_getter %}
    {# if have indexed properties, MUST have an indexed property getter #}
    {% set indexed_property_getter_callback =
           '%sV8Internal::indexedPropertyGetterCallback' % cpp_class %}
    {% set indexed_property_setter_callback =
           '%sV8Internal::indexedPropertySetterCallback' % cpp_class
           if indexed_property_setter else '0' %}
    {% set indexed_property_query_callback = '0' %}{# Unused #}
    {% set indexed_property_deleter_callback =
           '%sV8Internal::indexedPropertyDeleterCallback' % cpp_class
           if indexed_property_deleter else '0' %}
    {% set indexed_property_enumerator_callback =
           'indexedPropertyEnumerator<%s>' % cpp_class
           if indexed_property_getter.is_enumerable else '0' %}
    {
        v8::IndexedPropertyHandlerConfiguration config({{indexed_property_getter_callback}}, {{indexed_property_setter_callback}}, {{indexed_property_query_callback}}, {{indexed_property_deleter_callback}}, {{indexed_property_enumerator_callback}});
        {% if indexed_property_getter.do_not_check_security %}
        config.flags = v8::PropertyHandlerFlags::kAllCanRead;
        {% endif %}
        functionTemplate->{{set_on_template}}()->SetHandler(config);
    }
    {% endif %}
    {% if named_property_getter %}
    {# if have named properties, MUST have a named property getter #}
    {% set named_property_getter_callback =
           '%sV8Internal::namedPropertyGetterCallback' % cpp_class %}
    {% set named_property_setter_callback =
           '%sV8Internal::namedPropertySetterCallback' % cpp_class
           if named_property_setter else '0' %}
    {% set named_property_query_callback =
           '%sV8Internal::namedPropertyQueryCallback' % cpp_class
           if named_property_getter.is_enumerable else '0' %}
    {% set named_property_deleter_callback =
           '%sV8Internal::namedPropertyDeleterCallback' % cpp_class
           if named_property_deleter else '0' %}
    {% set named_property_enumerator_callback =
           '%sV8Internal::namedPropertyEnumeratorCallback' % cpp_class
           if named_property_getter.is_enumerable else '0' %}
    {
        v8::NamedPropertyHandlerConfiguration config({{named_property_getter_callback}}, {{named_property_setter_callback}}, {{named_property_query_callback}}, {{named_property_deleter_callback}}, {{named_property_enumerator_callback}});
        {# TODO(yukishiino): Determine how to treat Window interface. #}
        {% if interface_name != 'Window' %}
        config.flags = static_cast<v8::PropertyHandlerFlags>(static_cast<int>(config.flags) | static_cast<int>(v8::PropertyHandlerFlags::kOnlyInterceptStrings));
        {% endif %}
        {% if named_property_getter.do_not_check_security %}
        config.flags = v8::PropertyHandlerFlags::kAllCanRead;
        {% endif %}
        {# TODO(yukishiino): Determine how to treat Window interface. #}
        {% if not is_override_builtins and interface_name != 'Window' %}
        config.flags = static_cast<v8::PropertyHandlerFlags>(static_cast<int>(config.flags) | static_cast<int>(v8::PropertyHandlerFlags::kNonMasking));
        {% endif %}
        functionTemplate->{{set_on_template}}()->SetHandler(config);
    }
    {% endif %}
    {% if iterator_method %}
    {% filter exposed(iterator_method.exposed_test) %}
    {% filter runtime_enabled(iterator_method.runtime_enabled_function) %}
    static const V8DOMConfiguration::SymbolKeyedMethodConfiguration symbolKeyedIteratorConfiguration = { v8::Symbol::GetIterator, {{cpp_class_or_partial}}V8Internal::iteratorMethodCallback, 0, V8DOMConfiguration::ExposedToAllScripts };
    V8DOMConfiguration::installMethod(isolate, prototypeTemplate, defaultSignature, v8::DontDelete, symbolKeyedIteratorConfiguration);
    {% endfilter %}{# runtime_enabled() #}
    {% endfilter %}{# exposed() #}
    {% endif %}
    {# End special operations #}
    {% if has_custom_legacy_call_as_function %}
    functionTemplate->InstanceTemplate()->SetCallAsFunctionHandler({{v8_class}}::legacyCallCustom);
    {% endif %}
    {% if interface_name == 'HTMLAllCollection' %}
    {# Needed for legacy support of document.all #}
    functionTemplate->InstanceTemplate()->MarkAsUndetectable();
    {% endif %}
    {% for method in custom_registration_methods %}
    {# install_custom_signature #}
    {% filter conditional(method.conditional_string) %}
    {% filter exposed(method.overloads.exposed_test_all
                      if method.overloads else
                      method.exposed_test) %}
    {% filter runtime_enabled(method.overloads.runtime_enabled_function_all
                              if method.overloads else
                              method.runtime_enabled_function) %}
    {% if method.is_do_not_check_security %}
    {{install_do_not_check_security_signature(method) | indent}}
    {% else %}{# is_do_not_check_security #}
    {{install_custom_signature(method) | indent}}
    {% endif %}{# is_do_not_check_security #}
    {% endfilter %}{# runtime_enabled() #}
    {% endfilter %}{# exposed() #}
    {% endfilter %}{# conditional() #}
    {% endfor %}
    {% for attribute in attributes if attribute.is_static %}
    {% set getter_callback = '%sV8Internal::%sAttributeGetterCallback' %
               (cpp_class, attribute.name) %}
    {% set setter_callback = '%sV8Internal::%sAttributeSetterCallback' %
               (cpp_class, attribute.name)
           if attribute.has_setter else '0' %}
    {% filter conditional(attribute.conditional_string) %}
    functionTemplate->SetNativeDataProperty(v8AtomicString(isolate, "{{attribute.name}}"), {{getter_callback}}, {{setter_callback}}, v8::External::New(isolate, 0), static_cast<v8::PropertyAttribute>(v8::None), v8::Local<v8::AccessorSignature>(), static_cast<v8::AccessControl>(v8::DEFAULT));
    {% endfilter %}
    {% endfor %}
    {# Special interfaces #}
    {% if not is_partial %}
    {% if interface_name == 'Window' %}

    prototypeTemplate->SetInternalFieldCount(V8Window::internalFieldCount);
    functionTemplate->SetHiddenPrototype(true);
    instanceTemplate->SetInternalFieldCount(V8Window::internalFieldCount);
    {% elif interface_name in [
           'HTMLDocument', 'DedicatedWorkerGlobalScope', 'CompositorWorkerGlobalScope',
           'SharedWorkerGlobalScope', 'ServiceWorkerGlobalScope'] %}
    functionTemplate->SetHiddenPrototype(true);
    {% endif %}

    // Custom toString template
    functionTemplate->Set(v8AtomicString(isolate, "toString"), V8PerIsolateData::from(isolate)->toStringTemplate());
    {% endif %}
}

{% endif %}{# not is_array_buffer_or_view #}
{% endblock %}
{##############################################################################}
{% block get_dom_template %}{% endblock %}
{% block has_instance %}{% endblock %}
{% block to_impl %}{% endblock %}
{% block to_impl_with_type_check %}{% endblock %}
{% block install_conditional_attributes %}{% endblock %}
{##############################################################################}
{% block prepare_prototype_object %}{% endblock %}
{##############################################################################}
{% block to_active_dom_object %}{% endblock %}
{% block get_shadow_object_template %}{% endblock %}
{% block ref_object_and_deref_object %}{% endblock %}
{% for method in methods if method.is_implemented_in_private_script and method.visible %}
{{method_implemented_in_private_script(method)}}
{% endfor %}
{% for attribute in attributes if attribute.is_implemented_in_private_script %}
{{attribute_getter_implemented_in_private_script(attribute)}}
{% if attribute.has_setter %}
{{attribute_setter_implemented_in_private_script(attribute)}}
{% endif %}
{% endfor %}
{% block partial_interface %}{% endblock %}
} // namespace blink
{% endfilter %}
