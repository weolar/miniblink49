{% include 'copyright_block.txt' %}
#ifndef {{v8_class}}_h
#define {{v8_class}}_h

{% filter conditional(conditional_string) %}
{% for filename in header_includes %}
#include "{{filename}}"
{% endfor %}

namespace blink {

class {{v8_class}} final : public {{cpp_class}}, public ActiveDOMCallback {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN({{v8_class}});
public:
    static {{v8_class}}* create(v8::Local<v8::Function> callback, ScriptState* scriptState)
    {
        return new {{v8_class}}(callback, scriptState);
    }

    ~{{v8_class}}() override;

    DECLARE_VIRTUAL_TRACE();

{% for method in methods %}
    {{method.cpp_type}} {{method.name}}({{method.argument_declarations | join(', ')}}) override;
{% endfor %}
private:
    {{exported}}{{v8_class}}(v8::Local<v8::Function>, ScriptState*);

    ScopedPersistent<v8::Function> m_callback;
    RefPtr<ScriptState> m_scriptState;
};

}
{% endfilter %}
#endif // {{v8_class}}_h
