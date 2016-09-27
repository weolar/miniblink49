{% include 'copyright_block.txt' %}
#include "config.h"
#include "{{cpp_class}}.h"

{% for filename in cpp_includes %}
#include "{{filename}}"
{% endfor %}

namespace blink {

{# Constructor #}
{{cpp_class}}::{{cpp_class}}()
{
    {% for member in members if member.cpp_default_value %}
    {{member.setter_name}}({{member.cpp_default_value}});
    {% endfor %}
}

DEFINE_TRACE({{cpp_class}})
{
    {% for member in members if member.is_traceable %}
    visitor->trace(m_{{member.cpp_name}});
    {% endfor %}
    {% if parent_cpp_class %}
    {{parent_cpp_class}}::trace(visitor);
    {% endif %}
}

} // namespace blink
