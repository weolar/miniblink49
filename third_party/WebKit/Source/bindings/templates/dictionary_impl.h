{% include 'copyright_block.txt' %}
#ifndef {{cpp_class}}_h
#define {{cpp_class}}_h

{% for filename in header_includes %}
#include "{{filename}}"
{% endfor %}

namespace blink {

{# FIXME: Add "final" if this class doesn't have subclasses #}
class {{exported}}{{cpp_class}}{% if parent_cpp_class %} : public {{parent_cpp_class}}{% endif %} {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    {{cpp_class}}();

    {% for member in members %}
    bool {{member.has_method_name}}() const { return {{member.has_method_expression}}; }
    {{member.rvalue_cpp_type}} {{member.cpp_name}}() const { return {{member.getter_expression}}; }
    void {{member.setter_name}}({{member.rvalue_cpp_type}} value) { m_{{member.cpp_name}} = value; }
    {% if member.null_setter_name %}
    void {{member.null_setter_name}}() { m_{{member.cpp_name}} = {{member.member_cpp_type}}(); }
    {% endif %}

    {% endfor %}
    DECLARE_VIRTUAL_TRACE();

private:
    {% for member in members %}
    {{member.member_cpp_type}} m_{{member.cpp_name}};
    {% endfor %}

    friend class V8{{cpp_class}};
};

} // namespace blink

#endif // {{cpp_class}}_h
