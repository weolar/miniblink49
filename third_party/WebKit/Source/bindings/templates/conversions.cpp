{% macro v8_value_to_local_cpp_value(thing) %}
{# This indirection is just to avoid spurious white-space lines. #}
{{generate_v8_value_to_local_cpp_value(thing) | trim}}
{%- endmacro %}


{% macro generate_v8_value_to_local_cpp_value(thing) %}
{% set item = thing.v8_value_to_local_cpp_value or thing %}
{% if item.error_message %}
/* {{item.error_message}} */
{% else %}
{% if item.declare_variable %}
{% if item.assign_expression %}
{{item.cpp_type}} {{item.cpp_name}} = {{item.assign_expression}};
{% else %}
{{item.cpp_type}} {{item.cpp_name}};
{% endif %}
{% else %}{# item.declare_variable #}
{% if item.assign_expression %}
{{item.cpp_name}} = {{item.assign_expression}};
{% endif %}
{% endif %}{# item.declare_variable #}
{% if item.set_expression %}
{{item.set_expression}};
{% endif %}
{% if item.check_expression %}
if ({{item.check_expression}})
    return{% if item.return_expression %} {{item.return_expression}}{% endif %};
{% endif %}{# item.check_expression #}
{% endif %}{# item.error_message #}
{% endmacro %}


{% macro declare_enum_validation_variable(enum_values) %}
static const char* validValues[] = {
{% for enum_value in enum_values %}
    "{{enum_value}}",
{% endfor %}
};
{%-endmacro %}
